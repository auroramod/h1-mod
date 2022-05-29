#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "scheduler.hpp"
#include "gui.hpp"
#include "renderer.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/concurrency.hpp>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace gui
{
	std::unordered_map<std::string, bool> enabled_menus;

	namespace
	{
		struct frame_callback
		{
			std::function<void()> callback;
			bool always;
		};

		struct event
		{
			HWND hWnd;
			UINT msg;
			WPARAM wParam;
			LPARAM lParam;
		};

		utils::concurrency::container<std::vector<frame_callback>> on_frame_callbacks;
		utils::concurrency::container<std::deque<notification_t>> notifications;
		utils::concurrency::container<std::vector<event>> event_queue;

		ID3D11Device* device;
		ID3D11DeviceContext* device_context;
		bool initialized = false;
		bool toggled = false;

		void initialize_gui_context()
		{
			ImGui::CreateContext();
			ImGui::StyleColorsDark();

			ImGui_ImplWin32_Init(*game::hWnd);
			ImGui_ImplDX11_Init(device, device_context);

			initialized = true;
		}

		void run_event_queue()
		{
			event_queue.access([](std::vector<event>& queue)
			{
				for (const auto& event : queue)
				{
					ImGui_ImplWin32_WndProcHandler(event.hWnd, event.msg, event.wParam, event.lParam);
				}

				queue.clear();
			});
		}

		void gui_style()
		{
			auto* style = &ImGui::GetStyle();
			auto colors = style->Colors;

			style->WindowRounding = 5.3f;
			style->FrameRounding = 2.3f;
			style->ScrollbarRounding = 0;

			//const auto color = ImColor(0, 0, 0, 240);
			auto alpha = 0.92f;

			colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, alpha);
			colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, alpha);
			colors[ImGuiCol_WindowBg] = ImVec4(0.180f, 0.180f, 0.180f, alpha);
			colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
			colors[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, alpha);
			colors[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, alpha);
			colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
			colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, alpha);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, alpha);
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, alpha);
			colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, alpha);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, alpha);
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, alpha);
			colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, alpha);
			colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, alpha);
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, alpha);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, alpha);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, alpha);
			colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, alpha);
			colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, alpha);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, alpha);
			colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
			colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
			colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, alpha);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.469f, 0.469f, 0.469f, alpha);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.469f, 0.469f, 0.469f, alpha);
			colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
			colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, alpha);
			colors[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.391f, 0.000f, alpha);
			colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
			colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, alpha);
			colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, alpha);
			colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, alpha);
			colors[ImGuiCol_TabActive] = ImVec4(0.195f, 0.195f, 0.195f, alpha);
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, alpha);
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, alpha);
			colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, alpha);
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, alpha);
			colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, alpha);
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, alpha);
			colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
			colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, alpha);
			colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, alpha);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, alpha);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
		}

		void new_gui_frame()
		{
			ImGui::GetIO().MouseDrawCursor = toggled;
			if (toggled)
			{
				*game::keyCatchers |= 0x10;
			}
			else
			{
				*game::keyCatchers &= ~0x10;
			}

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			run_event_queue();

			ImGui::NewFrame();
			gui_style();
		}

		void end_gui_frame()
		{
			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		void toggle_menu(const std::string& name)
		{
			enabled_menus[name] = !enabled_menus[name];
		}

		void menu_checkbox(const std::string& name, const std::string& menu)
		{
			ImGui::Checkbox(name.data(), &enabled_menus[menu]);
		}

		void run_frame_callbacks()
		{
			on_frame_callbacks.access([](std::vector<frame_callback>& callbacks)
			{
				for (const auto& callback : callbacks)
				{
					if (callback.always || toggled)
					{
						callback.callback();
					}
				}
			});
		}

		void draw_window()
		{
			ImGui::Begin("hello world!", nullptr);

			static int value = 0;
			const auto technique = ImGui::SliderInt("lighting technique", &value, 0, 25);

			const auto should_apply = ImGui::Button("apply", ImVec2(125, 125));
			if (should_apply)
			{
				printf("new technique! value is %d\n", value);
				renderer::update_tech(value);
			}

			ImGui::End();
		}

		void gui_on_frame()
		{
			if (!initialized)
			{
				printf("initialzed gui context\n");
				initialize_gui_context();
			}
			else
			{
				new_gui_frame();
				run_frame_callbacks();
				end_gui_frame();
			}
		}

		HRESULT d3d11_create_device_stub(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software,
			UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion,
			ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext)
		{
			const auto result = D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels,
				FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);

			if (ppDevice != nullptr && ppImmediateContext != nullptr)
			{
				device = *ppDevice;
				device_context = *ppImmediateContext;
			}

			return result;
		}

		void dxgi_swap_chain_present_stub(utils::hook::assembler& a)
		{
			a.pushad64();
			a.call_aligned(gui_on_frame);
			a.popad64();

			a.mov(r8d, esi);
			a.mov(edx, r15d);
			a.mov(rcx, rdi);
			a.call_aligned(rbx);
			a.mov(dword_ptr(rsp, 0x20), eax);

			a.jmp(0x6CB185_b);
		}

		utils::hook::detour wnd_proc_hook;
		LRESULT wnd_proc_stub(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			if (wParam != VK_ESCAPE && toggled)
			{
				event_queue.access([hWnd, msg, wParam, lParam](std::vector<event>& queue)
				{
					queue.push_back({hWnd, msg, wParam, lParam});
				});
			}

			return wnd_proc_hook.invoke<LRESULT>(hWnd, msg, wParam, lParam);
		}
	}

	bool gui_key_event(const int local_client_num, const int key, const int down)
	{
		printf("gui key event\n");

		if (key == game::K_INS && down)
		{
			toggled = !toggled;
			return false;
		}

		if (key == game::K_ESCAPE && down && toggled)
		{
			toggled = false;
			return false;
		}

		return !toggled;
	}

	bool gui_char_event(const int local_client_num, const int key)
	{
		return !toggled;
	}

	bool gui_mouse_event(const int local_client_num, int x, int y)
	{
		return !toggled;
	}

	void on_frame(const std::function<void()>& callback, bool always)
	{
		on_frame_callbacks.access([always, callback](std::vector<frame_callback>& callbacks)
		{
			callbacks.push_back({callback, always});
		});
	}

	bool is_menu_open(const std::string& name)
	{
		return enabled_menus[name];
	}

	void notification(const std::string& title, const std::string& text, const std::chrono::milliseconds duration)
	{
		notification_t notification{};
		notification.title = title;
		notification.text = text;
		notification.duration = duration;
		notification.creation_time = std::chrono::high_resolution_clock::now();

		notifications.access([notification](std::deque<notification_t>& notifications_)
		{
			notifications_.push_front(notification);
		});
	}

	void copy_to_clipboard(const std::string& text)
	{
		utils::string::set_clipboard_data(text);
		gui::notification("Text copied to clipboard", utils::string::va("\"%s\"", text.data()));
	}

	class component final : public component_interface
	{
	public:
		void* load_import(const std::string& library, const std::string& function) override
		{
			if (function == "D3D11CreateDevice")
			{
				return d3d11_create_device_stub;
			}

			return nullptr;
		}

		void post_unpack() override
		{
			utils::hook::jump(0x6CB176_b, utils::hook::assemble(dxgi_swap_chain_present_stub), true);

			wnd_proc_hook.create(0x5BFF60_b, wnd_proc_stub);

			on_frame([]()
			{
				draw_window();
			});
		}

		void pre_destroy() override
		{
			if (initialized)
			{
				ImGui_ImplWin32_Shutdown();
				ImGui::DestroyContext();
			}
		}
	};
}

REGISTER_COMPONENT(gui::component)
