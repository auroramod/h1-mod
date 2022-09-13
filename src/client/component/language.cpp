#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "language.hpp"
#include "localized_strings.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>

#define LANGUAGE_FILE "players2/default/language"

namespace language
{
	namespace
	{
		const char* get_loc_language_string()
		{
			if (!utils::io::file_exists(LANGUAGE_FILE))
			{
				return nullptr;
			}

			static char language[0x200] = {0};
			const auto data = utils::io::read_file(LANGUAGE_FILE);
			strcpy_s(language, data.data());
			return language;
		}
	}

	void set(const std::string& lang)
	{
		utils::io::write_file(LANGUAGE_FILE, lang, false);
	}

	void set_from_index(const int index)
	{
		if (index < 0 || index > 16)
		{
			return;
		}

		const auto language = game::languages[index];
		set(language.name);
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// 0x14060AFFB on h2, cannot find on h1
			// utils::hook::call(SELECT_VALUE(0x0_b, 0x0_b), get_loc_language_string);
		}
	};
}

REGISTER_COMPONENT(language::component)
