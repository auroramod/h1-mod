#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "console.hpp"
#include "map_rotation.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace map_rotation
{
	namespace
	{
		rotation_data dedicated_rotation;

		const game::dvar_t* sv_map_rotation;
		const game::dvar_t* sv_map_rotation_current;
		const game::dvar_t* sv_random_map_rotation;

		void set_gametype(const std::string& gametype)
		{
			assert(!gametype.empty());

			game::Dvar_SetFromStringByNameFromSource("g_gametype", gametype.data(), game::DVAR_SOURCE_INTERNAL);
		}

		void launch_map(const std::string& mapname)
		{
			assert(!mapname.empty());

			command::execute(utils::string::va("map %s", mapname.data()), false);
		}

		void launch_default_map()
		{
			auto* mapname = game::Dvar_FindVar("mapname");
			if (mapname && mapname->current.string && strlen(mapname->current.string) && mapname->current.string !=
				"mp_vlobby_room"s)
			{
				launch_map(mapname->current.string);
			}
			else
			{
				launch_map("mp_crash");
			}
		}

		void apply_rotation(rotation_data& rotation)
		{
			assert(!rotation.empty());

			std::size_t i = 0;
			while (i < rotation.get_entries_size())
			{
				const auto& entry = rotation.get_next_entry();
				if (entry.first == "map"s)
				{
					console::info("Loading new map: '%s'\n", entry.second.data());
					if (!game::SV_MapExists(entry.second.data()))
					{
						console::info("map_rotation: '%s' map doesn't exist!\n", entry.second.data());
						launch_default_map();
						return;
					}

					launch_map(entry.second);

					break;
				}

				if (entry.first == "gametype"s)
				{
					console::info("Applying new gametype: '%s'\n", entry.second.data());
					set_gametype(entry.second);
				}

				++i;
			}
		}

		void load_rotation(const std::string& data)
		{
			static auto loaded = false;
			if (loaded)
			{
				return;
			}

			loaded = true;
			try
			{
				dedicated_rotation.parse(data);
			}
			catch (const std::exception& ex)
			{
				console::error("%s: sv_map_rotation contains invalid data!\n", ex.what());
			}

			console::debug("dedicated_rotation size after parsing is '%llu'", dedicated_rotation.get_entries_size());
		}

		void load_map_rotation()
		{
			const std::string map_rotation = sv_map_rotation->current.string;
			if (!map_rotation.empty())
			{
				console::debug("sv_map_rotation is not empty. Parsing...\n");
				load_rotation(map_rotation);
			}
		}

		void apply_map_rotation_current(const std::string& data)
		{
			assert(!data.empty());

			rotation_data rotation_current;

			try
			{
				rotation_current.parse(data);
			}
			catch (const std::exception& ex)
			{
				console::error("%s: sv_map_rotation_current contains invalid data!\n", ex.what());
			}

			game::Dvar_SetFromStringByNameFromSource("sv_map_rotation_current", "", game::DVAR_SOURCE_INTERNAL);

			if (rotation_current.empty())
			{
				console::warn("sv_map_rotation_current is empty or contains invalid data\n");
				launch_default_map();
				return;
			}

			apply_rotation(rotation_current);
		}

		void randomize_map_rotation()
		{
			if (sv_random_map_rotation->current.enabled)
			{
				console::info("Randomizing map rotation\n");
				dedicated_rotation.randomize();
			}
		}

		void perform_map_rotation()
		{
			if (game::Live_SyncOnlineDataFlags(0) != 0)
			{
				scheduler::on_game_initialized(perform_map_rotation, scheduler::pipeline::main, 1s);
				return;
			}

			console::info("Rotating map...\n");

			// This takes priority because of backwards compatibility
			const std::string map_rotation_current = sv_map_rotation_current->current.string;
			if (!map_rotation_current.empty())
			{
				console::debug("Applying sv_map_rotation_current\n");
				apply_map_rotation_current(map_rotation_current);
				return;
			}

			load_map_rotation();
			if (dedicated_rotation.empty())
			{
				console::warn("sv_map_rotation is empty or contains invalid data. Restarting map\n");
				launch_default_map();
				return;
			}

			randomize_map_rotation();

			apply_rotation(dedicated_rotation);
		}

		void trigger_map_rotation()
		{
			scheduler::schedule([]
			{
				if (game::CL_IsCgameInitialized())
				{
					return scheduler::cond_continue;
				}

				command::execute("map_rotate", false);
				return scheduler::cond_end;
			}, scheduler::pipeline::main, 1s);
		}
	}

	rotation_data::rotation_data()
		: index_(0)
	{
	}

	void rotation_data::randomize()
	{
		std::random_device rd;
		std::mt19937 gen(rd());

		std::ranges::shuffle(this->rotation_entries_, gen);
	}

	void rotation_data::add_entry(const std::string& key, const std::string& value)
	{
		this->rotation_entries_.emplace_back(std::make_pair(key, value));
	}

	bool rotation_data::contains(const std::string& key, const std::string& value) const
	{
		return std::ranges::any_of(this->rotation_entries_, [&](const auto& entry)
		{
			return entry.first == key && entry.second == value;
		});
	}

	bool rotation_data::empty() const noexcept
	{
		return this->rotation_entries_.empty();
	}

	std::size_t rotation_data::get_entries_size() const noexcept
	{
		return this->rotation_entries_.size();
	}

	rotation_data::rotation_entry& rotation_data::get_next_entry()
	{
		const auto index = this->index_;
		++this->index_ %= this->rotation_entries_.size();
		return this->rotation_entries_.at(index);
	}

	void rotation_data::parse(const std::string& data)
	{
		const auto tokens = utils::string::split(data, ' ');
		for (std::size_t i = 0; !tokens.empty() && i < (tokens.size() - 1); i += 2)
		{
			const auto& key = tokens[i];
			const auto& value = tokens[i + 1];

			if (key == "map"s || key == "gametype"s)
			{
				this->add_entry(key, value);
			}
			else
			{
				throw parse_rotation_error();
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_dedi())
			{
				return;
			}

			scheduler::once([]
			{
				sv_map_rotation = dvars::register_string("sv_mapRotation", "", game::DVAR_FLAG_NONE, "");
				sv_map_rotation_current = dvars::register_string("sv_mapRotationCurrent", "", game::DVAR_FLAG_NONE, "");
			}, scheduler::pipeline::main);

			sv_random_map_rotation = dvars::register_bool("sv_randomMapRotation", false, game::DVAR_FLAG_NONE, "Randomize map rotation");

			command::add("map_rotate", &perform_map_rotation);

			// Hook GScr_ExitLevel 
			utils::hook::jump(0xE2670_b, &trigger_map_rotation, true); // not sure if working
		}
	};
}

REGISTER_COMPONENT(map_rotation::component)
