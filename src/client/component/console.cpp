#include <std_include.hpp>
#include "console.hpp"
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "command.hpp"

#include <utils/thread.hpp>
#include <utils/flags.hpp>
#include <utils/concurrency.hpp>
#include <utils/hook.hpp>

namespace game_console
{
	void print(int type, const std::string& data);
}

namespace console
{
	namespace
	{
		using message_queue = std::queue<std::string>;
		utils::concurrency::container<message_queue> messages;

		void hide_console()
		{
			auto* const con_window = GetConsoleWindow();

			DWORD process;
			GetWindowThreadProcessId(con_window, &process);

			if (process == GetCurrentProcessId() || IsDebuggerPresent())
			{
				ShowWindow(con_window, SW_HIDE);
			}
		}

		std::string format(va_list* ap, const char* message)
		{
			static thread_local char buffer[0x1000];

			const auto count = _vsnprintf_s(buffer, sizeof(buffer), sizeof(buffer), message, *ap);

			if (count < 0) return {};
			return { buffer, static_cast<size_t>(count) };
		}

		void dispatch_message(const int type, const std::string& message)
		{
			game_console::print(type, message);
			messages.access([&message](message_queue& msgs)
				{
					msgs.emplace(message);
				});
		}

		void append_text(const char* text)
		{
			dispatch_message(con_type_info, text);
		}
	}

	class component final : public component_interface
	{
	public:
		component()
		{
			hide_console();

			(void)_pipe(this->handles_, 1024, _O_TEXT);
			(void)_dup2(this->handles_[1], 1);
			(void)_dup2(this->handles_[1], 2);

			//setvbuf(stdout, nullptr, _IONBF, 0);
			//setvbuf(stderr, nullptr, _IONBF, 0);
		}

		void post_start() override
		{
			this->terminate_runner_ = false;

			this->console_runner_ = utils::thread::create_named_thread("Console IO", [this]
				{
					this->runner();
				});
		}

		void pre_destroy() override
		{
			this->terminate_runner_ = true;

			printf("\r\n");
			_flushall();

			if (this->console_runner_.joinable())
			{
				this->console_runner_.join();
			}

			if (this->console_thread_.joinable())
			{
				this->console_thread_.join();
			}

			_close(this->handles_[0]);
			_close(this->handles_[1]);

			messages.access([&](message_queue& msgs)
				{
					msgs = {};
				});
		}

		void post_unpack() override
		{
			// Redirect input (]command)
			utils::hook::jump(SELECT_VALUE(0x1403E34C0, 0x1405141E0), append_text); // H1(1.4)

			this->initialize();
		}

	private:
		volatile bool console_initialized_ = false;
		volatile bool terminate_runner_ = false;

		std::thread console_runner_;
		std::thread console_thread_;

		int handles_[2]{};

		void initialize()
		{
			this->console_thread_ = utils::thread::create_named_thread("Console", [this]()
				{
					if (game::environment::is_dedi() || !utils::flags::has_flag("noconsole"))
					{
						game::Sys_ShowConsole();
					}

					if (!game::environment::is_dedi())
					{
						// Hide that shit
						ShowWindow(console::get_window(), SW_MINIMIZE);
					}

					{
						messages.access([&](message_queue&)
							{
								this->console_initialized_ = true;
							});
					}

					MSG msg;
					while (!this->terminate_runner_)
					{
						if (PeekMessageA(&msg, nullptr, NULL, NULL, PM_REMOVE))
						{
							if (msg.message == WM_QUIT)
							{
								command::execute("quit", false);
								break;
							}

							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
						else
						{
							this->log_messages();
							std::this_thread::sleep_for(1ms);
						}
					}
				});
		}

		void log_messages()
		{
			/*while*/
			if (this->console_initialized_ && !messages.get_raw().empty())
			{
				std::queue<std::string> message_queue_copy;

				{
					messages.access([&](message_queue& msgs)
						{
							message_queue_copy = std::move(msgs);
							msgs = {};
						});
				}

				while (!message_queue_copy.empty())
				{
					log_message(message_queue_copy.front());
					message_queue_copy.pop();
				}
			}

			fflush(stdout);
			fflush(stderr);
		}

		static void log_message(const std::string& message)
		{
			OutputDebugStringA(message.data());
			game::Conbuf_AppendText(message.data());
		}

		void runner()
		{
			char buffer[1024];

			while (!this->terminate_runner_ && this->handles_[0])
			{
				const auto len = _read(this->handles_[0], buffer, sizeof(buffer));
				if (len > 0)
				{
					dispatch_message(con_type_info, std::string(buffer, len));
				}
				else
				{
					std::this_thread::sleep_for(1ms);
				}
			}

			std::this_thread::yield();
		}
	};

	HWND get_window()
	{
		return *reinterpret_cast<HWND*>((SELECT_VALUE(0x14CF56C00, 0x14DDFC2D0))); // H1(1.4)
	}

	void set_title(std::string title)
	{
		SetWindowText(get_window(), title.data());
	}

	void set_size(const int width, const int height)
	{
		RECT rect;
		GetWindowRect(get_window(), &rect);

		SetWindowPos(get_window(), nullptr, rect.left, rect.top, width, height, 0);

		auto* const logo_window = *reinterpret_cast<HWND*>(SELECT_VALUE(0x14CF56C10, 0x14DDFC2E0)); // H1(1.4)
		SetWindowPos(logo_window, nullptr, 5, 5, width - 25, 60, 0);
	}

	void print(const int type, const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		const auto result = format(&ap, fmt);
		va_end(ap);

		dispatch_message(type, result);
	}
}

REGISTER_COMPONENT(console::component)