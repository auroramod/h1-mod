#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "sound.hpp"
#include "filesystem.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "command.hpp"

#include <utils/io.hpp>
#include <utils/memory.hpp>
#include <utils/hook.hpp>
#include <utils/concurrency.hpp>
#include <utils/string.hpp>

// https://github.com/skkuull/h1-zonetool/blob/main/src/client/zonetool/assets/sound.cpp
// https://github.com/skkuull/h1-zonetool/blob/main/src/client/zonetool/assets/loadedsound.cpp

namespace sound
{
	namespace
	{
		utils::memory::allocator sound_allocator;
		using loaded_sound_map = std::unordered_map<size_t, game::snd_alias_list_t*>;
		utils::concurrency::container<loaded_sound_map, std::recursive_mutex> loaded_sounds;
		std::hash<std::string_view> hasher;

#define FATAL(...) \
		throw std::runtime_error(utils::string::va(__VA_ARGS__)); \

#define SOUND_STRING(entry, optional) \
		if (j.HasMember(#entry) && j[#entry].IsString()) \
		{ \
			asset->entry = sound_allocator.duplicate_string(j[#entry].GetString()); \
		} \
		else if (!optional) \
		{ \
			FATAL("member '%s' does not exist or isn't of type 'String'\n", #entry); \
		} \

#define SOUND_FLOAT(entry, optional) \
		if (j.HasMember(#entry) && (j[#entry].IsFloat() || j[#entry].IsInt())) \
		{ \
			asset->entry = j[#entry].GetFloat(); \
		} \
		else if (!optional) \
		{ \
			FATAL("member '%s' does not exist or isn't of type 'Float'\n", #entry); \
		} \


#define SOUND_INT(entry, optional) \
		if (j.HasMember(#entry) && j[#entry].IsInt()) \
		{ \
			asset->entry = j[#entry].GetInt(); \
		} \
		else if (!optional) \
		{ \
			FATAL("member '%s' does not exist or isn't of type 'Int'\n", #entry); \
		} \

#define SOUND_CHAR(entry, optional) \
		if (j.HasMember(#entry) && j[#entry].IsInt()) \
		{ \
			asset->entry = static_cast<char>(j[#entry].GetInt()); \
		} \
		else if (!optional) \
		{ \
			FATAL("member '%s' does not exist or isn't of type 'Char'\n", #entry); \
		} \

#define JSON_GET(parent, name, type, parent_name) \
		(parent.HasMember(name) && parent[name].Is##type()) \
			? parent[name].Get##type() \
			: FATAL("'%s' member '%s' does not exist or it isn't of type '%s'", parent_name, name, #type) \

#define JSON_GET_OPTIONAL(parent, name, type, default_value) \
		(parent.HasMember(name) && parent[name].Is##type()) \
			? parent[name].Get##type() \
			: default_value \

#define JSON_GET_CAST(parent, name, type, cast_type, parent_name) \
		(parent.HasMember(name) && parent[name].Is##type()) \
			? static_cast<cast_type>(parent[name].Get##type()) \
			: FATAL("'%s' member '%s' does not exist or it isn't of type '%s'", parent_name, name, #type) \

#define JSON_CHECK(parent, name, type, parent_name) \
		if (!parent.HasMember(name) || !parent[name].Is##type()) \
		{ \
			FATAL("'%s' member '%s' does not exist or it isn't of type '%s'", parent_name, name, #type) \
		} \

		std::string rapidjson_get_object_bytes(const rapidjson::Value& value)
		{
			std::string buffer{};
			
			if (!value.IsArray())
			{
				return buffer;
			}

			for (auto i = 0; i < static_cast<int>(value.Size()); i++)
			{
				buffer += static_cast<char>(value[i].GetInt());
			}
			return buffer;
		}

		rapidjson::Value rapidjson_bytes_to_object(const std::vector<std::uint8_t>& bytes, rapidjson::Document& j)
		{
			rapidjson::Value arr{rapidjson::kArrayType};

			for (const auto& byte : bytes)
			{
				arr.PushBack(byte, j.GetAllocator());
			}

			return arr;
		}

		game::LoadedSound* parse_flac(const std::string& name)
		{
			const auto path = "loaded_sound/"s + name + ".flac";
			std::string data{};
			if (!filesystem::read_file(path, &data))
			{
				return nullptr;
			}

			console::info("[Sound] Parsing flac %s\n", path.data());

			auto* result = sound_allocator.allocate<game::LoadedSound>();
			result->name = sound_allocator.duplicate_string(name);
			result->info.loadedSize = static_cast<int>(data.size());
			result->info.data = sound_allocator.allocate_array<char>(result->info.loadedSize);
			std::memcpy(result->info.data, data.data(), data.size());

			return result;
		}

		game::LoadedSound* parse_wav(const std::string& name)
		{
			const auto path = "loaded_sound/"s + name + ".wav";
			std::string full_path{};
			if (!filesystem::find_file(path, &full_path))
			{
				return nullptr;
			}

			console::info("[Sound] Parsing wav %s\n", path.data());

			std::ifstream file;
			file.open(full_path, std::ios::binary);

			if (!file.is_open())
			{
				FATAL("failed to open loaded sound file: %s\n", name.data());
			}

			const auto result = sound_allocator.allocate<game::LoadedSound>();

			unsigned int chunk_id_buffer{};
			unsigned int chunk_size{};

			file.read(reinterpret_cast<char*>(&chunk_id_buffer), 4);
			if (chunk_id_buffer != 0x46464952) // RIFF
			{
				FATAL("%s: Invalid RIFF Header 0x%lX.", name.data(), chunk_id_buffer);
			}

			file.read(reinterpret_cast<char*>(&chunk_size), 4);
			file.read(reinterpret_cast<char*>(&chunk_id_buffer), 4);

			if (chunk_id_buffer != 0x45564157) // WAVE
			{
				FATAL("%s: Invalid WAVE Header 0x%lX.", name.data(), chunk_id_buffer);
			}

			while (!result->info.data && !file.eof())
			{
				file.read(reinterpret_cast<char*>(&chunk_id_buffer), 4);
				file.read(reinterpret_cast<char*>(&chunk_size), 4);

				switch (chunk_id_buffer)
				{
				case 0x20746D66: // fmt
					if (chunk_size >= 16)
					{
						short format{};
						file.read(reinterpret_cast<char*>(&format), 2);
						if (format != 1 && format != 17)
						{
							FATAL("%s: Invalid wave format %i.", name.data(), format);
						}
						result->info.format = format;

						short num_channels{};
						file.read(reinterpret_cast<char*>(&num_channels), 2);
						result->info.channels = static_cast<char>(num_channels);

						int sample_rate{};
						file.read(reinterpret_cast<char*>(&sample_rate), 4);
						result->info.sampleRate = sample_rate;

						int byte_rate{};
						file.read(reinterpret_cast<char*>(&byte_rate), 4);

						short block_align{};
						file.read(reinterpret_cast<char*>(&block_align), 2);
						result->info.blockAlign = static_cast<char>(block_align);

						short bit_per_sample{};
						file.read(reinterpret_cast<char*>(&bit_per_sample), 2);
						result->info.numBits = static_cast<char>(bit_per_sample);

						if (chunk_size > 16)
						{
							file.seekg(chunk_size - 16, std::ios::cur);
						}
					}
					break;

				case 0x61746164: // data
					result->info.data = sound_allocator.allocate_array<char>(chunk_size);
					file.read(result->info.data, chunk_size);

					result->info.loadedSize = chunk_size;
					result->info.dataByteCount = result->info.loadedSize;

					result->info.numSamples = result->info.dataByteCount / (result->info.channels * result->info.numBits / 8);
					break;

				default:
					if (chunk_size > 0)
					{
						file.seekg(chunk_size, std::ios::cur);
					}
					break;
				}
			}

			if (!result->info.data)
			{
				FATAL("%s: Could not read sounddata.", name.data());
				return nullptr;
			}

			result->name = sound_allocator.duplicate_string(name);
			return result;
		}

		game::LoadedSound* parse_loaded_sound(const std::string& name)
		{
			std::string full_path{};
			auto path = "loaded_sound/"s + name;

			if (filesystem::find_file(path + ".wav", &full_path))
			{
				return parse_wav(name);
			}

			if (filesystem::find_file(path + ".flac", &full_path))
			{
				return parse_flac(name);
			}

			console::warn("Sound %s not found, falling back to default sound\n", path.data());
			return game::DB_FindXAssetHeader(game::ASSET_TYPE_LOADED_SOUND, name.data(), true).loaded_sound;
		}

		void parse_sound_alias(const rapidjson::Value& j, game::snd_alias_t* asset)
		{
			SOUND_STRING(aliasName, false);
			SOUND_STRING(secondaryAliasName, true);
			SOUND_STRING(chainAliasName, true);
			SOUND_STRING(subtitle, true);
			SOUND_STRING(mixerGroup, true);

			if (!j.HasMember("soundfile") || !j["soundfile"].IsObject())
			{
				FATAL("missing 'soundfile' object");
			}

			const auto& sound_file = j["soundfile"];
			const auto sound_file_type = JSON_GET(sound_file, "type", Int, "soundfile");

			asset->soundFile = sound_allocator.allocate<game::SoundFile>();
			asset->soundFile->type = static_cast<game::snd_alias_type_t>(sound_file_type);
			asset->soundFile->exists = true;

			if (asset->soundFile->type == game::SAT_LOADED)
			{
				const auto name = JSON_GET(sound_file, "name", String, "soundfile is missing 'name'");
				asset->soundFile->u.loadSnd = parse_loaded_sound(name);
			}
			else if (asset->soundFile->type == game::SAT_STREAMED)
			{
				asset->soundFile->u.streamSnd.totalMsec = 
					JSON_GET(sound_file, "totalMsec", Uint, "soundfile");
				asset->soundFile->u.streamSnd.filename.isLocalized = 
					JSON_GET(sound_file, "isLocalized", Bool, "soundfile");
				asset->soundFile->u.streamSnd.filename.isStreamed = 
					JSON_GET(sound_file, "isStreamed", Bool, "soundfile");
				asset->soundFile->u.streamSnd.filename.fileIndex = 
					JSON_GET_CAST(sound_file, "fileIndex", Int, unsigned short, "soundfile");

				if (asset->soundFile->u.streamSnd.filename.fileIndex)
				{
					JSON_CHECK(sound_file, "packed", Object, "soundfile");

					asset->soundFile->u.streamSnd.filename.info.packed.offset = 
						JSON_GET(sound_file["packed"], "offset", Uint64, "soundfile.raw");
					asset->soundFile->u.streamSnd.filename.info.packed.length = 
						JSON_GET(sound_file["packed"], "length", Uint64, "soundfile.raw");
				}
				else
				{
					JSON_CHECK(sound_file, "raw", Object, "soundfile");

					const auto dir = JSON_GET(sound_file["raw"], "dir", String, "soundfile.raw");
					const auto name = JSON_GET(sound_file["raw"], "name", String, "soundfile.raw");

					asset->soundFile->u.streamSnd.filename.info.raw.dir = sound_allocator.duplicate_string(dir);
					asset->soundFile->u.streamSnd.filename.info.raw.name = sound_allocator.duplicate_string(name);
				}
			}
			else
			{
				FATAL("Sound alias has invalid soundFile type %i", asset->soundFile->type);
			}

			SOUND_INT(flags, false);
			SOUND_INT(sequence, false);
			SOUND_FLOAT(volMin, false);
			SOUND_FLOAT(volMax, false);
			SOUND_INT(volModIndex, false);
			SOUND_FLOAT(pitchMin, false);
			SOUND_FLOAT(pitchMax, false);
			SOUND_FLOAT(distMin, false);
			SOUND_FLOAT(distMax, false);
			SOUND_FLOAT(velocityMin, false);
			SOUND_CHAR(masterPriority, false);
			SOUND_FLOAT(masterPercentage, false);
			SOUND_FLOAT(slavePercentage, false);
			SOUND_FLOAT(probability, false);
			SOUND_INT(startDelay, false);

			if (j.HasMember("sndContext") && j["sndContext"].IsString())
			{
				asset->sndContext = game::DB_FindXAssetHeader(game::ASSET_TYPE_SNDCONTEXT, 
					j["sndContext"].GetString(), false).snd_context;
			}

			if (j.HasMember("sndCurve") && j["sndCurve"].IsString())
			{
				asset->sndCurve = game::DB_FindXAssetHeader(game::ASSET_TYPE_SNDCURVE,
					j["sndCurve"].GetString(), false).snd_curve;
			}

			if (j.HasMember("lpfCurve") && j["lpfCurve"].IsString())
			{
				asset->lpfCurve = game::DB_FindXAssetHeader(game::ASSET_TYPE_LPFCURVE,
					j["lpfCurve"].GetString(), false).snd_curve;
			}

			if (j.HasMember("hpfCurve") && j["hpfCurve"].IsString())
			{
				asset->hpfCurve = game::DB_FindXAssetHeader(game::ASSET_TYPE_LPFCURVE,
					j["hpfCurve"].GetString(), false).snd_curve;
			}

			if (j.HasMember("reverbSendCurve") && j["reverbSendCurve"].IsString())
			{
				asset->reverbSendCurve = game::DB_FindXAssetHeader(game::ASSET_TYPE_REVERBSENDCURVE,
					j["reverbSendCurve"].GetString(), false).snd_curve;
			}

			if (j.HasMember("speakerMap") && j["speakerMap"].IsObject())
			{
				asset->speakerMap = sound_allocator.allocate<game::SpeakerMap>();
				const auto& speaker_map = j["speakerMap"];

				const auto speaker_map_name = JSON_GET(speaker_map, "name", String, "speakerMap");
				const auto is_default = JSON_GET(speaker_map, "isDefault", Bool, "speakerMap");

				asset->speakerMap->name = sound_allocator.duplicate_string(speaker_map_name);
				asset->speakerMap->isDefault = is_default;

				if (speaker_map.HasMember("channelMaps") && speaker_map["channelMaps"].IsArray())
				{
					const auto& channel_maps = speaker_map["channelMaps"];
					for (char x = 0; x < 2; x++)
					{
						for (char y = 0; y < 2; y++)
						{
							const auto index = static_cast<unsigned int>((x & 0x01) << 1 | y & 0x01);
							if (index >= channel_maps.Size() || !channel_maps[index].IsObject())
							{
								FATAL("channelMaps at index %i does not exist", index);
							}

							const auto& channel_map = channel_maps[index];
							asset->speakerMap->channelMaps[x][y].speakerCount = JSON_GET(channel_map, 
								"speakerCount", Int, "speakerMap.channelMaps[]");

							if (!channel_map.HasMember("speakers") || !channel_map["speakers"].IsArray())
							{
								FATAL("channelMap does not have a 'speakers' member or it isn't an array");
							}

							const auto& speakers = channel_map["speakers"];

							for (auto speaker = 0; speaker < asset->speakerMap->channelMaps[x][y].speakerCount;
								speaker++)
							{
								if (static_cast<unsigned int>(speaker) < speakers.Size() && speakers[speaker].IsObject())
								{
									const auto& jspeaker = speakers[speaker];
									asset->speakerMap->channelMaps[x][y].speakers[speaker].speaker =
										JSON_GET_CAST(jspeaker, "speaker", Int, char, "speakerMap.channelMaps.speakers[]");
									asset->speakerMap->channelMaps[x][y].speakers[speaker].numLevels =
										JSON_GET_CAST(jspeaker, "numLevels", Int, char, "speakerMap.channelMaps.speakers[]");
									asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[0] =
										JSON_GET(jspeaker, "levels0", Float, "speakerMap.channelMaps.speakers[]");
									asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[1] =
										JSON_GET(jspeaker, "levels1", Float, "speakerMap.channelMaps.speakers[]");
								}
								else
								{
									FATAL("speaker at index %i does not exist or is not an object", speaker);
								}
							}
						}
					}
				}
			}

			/*SOUND_CHAR(allowDoppler);
			if (j.HasMember("dopplerPreset") && !j["dopplerPreset"].IsNull())
			{
				asset->dopplerPreset = game::DB_FindXAssetHeader(game::ASSET_TYPE_DOPPLERPRESET, 
					j["dopplerPreset"].GetString(), false).doppler_preset;
			}*/

			if (j.HasMember("unknown") && j["unknown"].IsObject())
			{
				const auto& snd_unknown = j["unknown"];

				const auto& pad0 = rapidjson_get_object_bytes(snd_unknown["pad"][0]);
				const auto& pad1 = rapidjson_get_object_bytes(snd_unknown["pad"][1]);
				const auto& pad2 = rapidjson_get_object_bytes(snd_unknown["pad"][2]);
				const auto& pad3 = rapidjson_get_object_bytes(snd_unknown["pad"][3]);

				std::memcpy(asset->__pad0, pad0.data(), pad0.size());
				std::memcpy(asset->__pad1, pad1.data(), pad1.size());
				std::memcpy(asset->__pad2, pad2.data(), pad2.size());
				std::memcpy(asset->__pad3, pad3.data(), pad3.size());

				asset->u4 = JSON_GET_OPTIONAL(snd_unknown, "u4", Int, 0);
				asset->u5 = JSON_GET_OPTIONAL(snd_unknown, "u5", Int, 0);
				asset->u18 = static_cast<char>(JSON_GET_OPTIONAL(snd_unknown, "u18", Int, 0));
				asset->u20 = static_cast<char>(JSON_GET_OPTIONAL(snd_unknown, "u20", Int, 0));
				asset->u34 = JSON_GET_OPTIONAL(snd_unknown, "u34", Float, 0.f);
			}
		}

		game::snd_alias_list_t* parse_sound_alias_list(const rapidjson::Document& j)
		{
			const auto asset = sound_allocator.allocate<game::snd_alias_list_t>();

			SOUND_STRING(aliasName, false);

			asset->count = JSON_GET_CAST(j, "count", Int, char, "sound");
			asset->head = sound_allocator.allocate_array<game::snd_alias_t>(asset->count);

			JSON_CHECK(j, "head", Array, "sound");

			const auto head = j["head"].GetArray();
			for (auto i = 0; i < static_cast<int>(asset->count) && head.Size(); i++)
			{
				parse_sound_alias(head[i], &asset->head[i]);
			}

			if (j.HasMember("unknownArray") && j["unknownArray"].IsArray())
			{
				const auto& unk = j["unknownArray"];
				asset->unkCount = static_cast<unsigned char>(unk.Size());
				asset->unk = sound_allocator.allocate_array<short>(asset->unkCount);

				for (unsigned char i = 0; i < asset->unkCount && unk.Size(); i++)
				{
					asset->unk[i] = static_cast<short>(unk[i].GetInt());
				}
			}

			return asset;
		}

		bool sound_exists(const char* name)
		{
			return sound::find_sound(name).sound != nullptr;
		}

		utils::hook::detour db_is_xasset_default_hook;
		int db_is_xasset_default_stub(game::XAssetType type, const char* name)
		{
			if (type != game::ASSET_TYPE_SOUND)
			{
				return db_is_xasset_default_hook.invoke<bool>(type, name);
			}

			const auto res = db_is_xasset_default_hook.invoke<bool>(type, name);
			if (!res)
			{
				return res;
			}

			return !sound_exists(name);
		}

		utils::hook::detour db_xasset_exists_hook;
		int db_xasset_exists_stub(game::XAssetType type, const char* name)
		{
			const auto res = utils::hook::invoke<bool>(0x39B7B0_b, type, name);
			if (res)
			{
				return true;
			}

			return sound_exists(name);
		}

		utils::hook::detour scr_table_lookup_hook;
		void scr_table_lookup_stub()
		{
			const auto table = game::Scr_GetString(0);
			const auto search_column = game::Scr_GetInt(1);
			const auto search_value = game::Scr_GetString(2);
			const auto return_row = game::Scr_GetInt(3);

			if (table != "mp/sound/soundlength.csv"s || search_column != 0 || return_row != 1)
			{
				return scr_table_lookup_hook.invoke<void>();
			}

			std::optional<int> new_value{};
			loaded_sounds.access([&](loaded_sound_map& map)
			{
				const auto i = map.find(hasher(search_value));
				if (i == map.end())
				{
					return;
				}

				const auto sound_list = i->second;
				if (sound_list->count)
				{
					const auto sound = &sound_list->head[0];
					if (sound->soundFile && sound->soundFile->type == game::SAT_STREAMED)
					{
						new_value = sound->soundFile->u.streamSnd.totalMsec;
					}
				}
			});

			if (new_value.has_value())
			{
				game::Scr_AddString(utils::string::va("%i\n", new_value.value()));
			}
			else
			{
				scr_table_lookup_hook.invoke<void>();
			}
		}

		void com_sprintf_raw_sound_localized_stub(char* buffer, int size, const char* fmt,
			const char* lang, const char* name, const char* extension)
		{
			sprintf_s(buffer, size, "%s%s", name, extension);
		}

		void com_sprintf_raw_sound_stub(char* buffer, int size, const char* fmt, 
			const char* name, const char* extension)
		{
			sprintf_s(buffer, size, "%s%s", name, extension);
		}

		utils::hook::detour snd_is_music_playing_hook;
		bool snd_is_music_playing_stub(void* a1)
		{
			if (a1 == nullptr)
			{
				return true; // dont play music
			}

			return snd_is_music_playing_hook.invoke<bool>(a1);
		}

		void load_sound(const std::string& name, const std::string& path)
		{
			try
			{
				const auto data = utils::io::read_file(path);

				rapidjson::Document j;
				j.Parse(data.data());

				console::info("[Sound] Loading sound %s\n", name.data());
				const auto sound = parse_sound_alias_list(j);

				const auto h = hasher(name.data());
				loaded_sounds.access([&](loaded_sound_map& map)
				{
					map[h] = sound;
				});
			}
			catch (const std::exception& e)
			{
				console::error("[Sound] Error loading sound %s: %s\n", name.data(), e.what());
			}
		}

		void load_sounds()
		{
			const auto paths = filesystem::get_search_paths();
			for (const auto& path : paths)
			{
				const auto dir = path + "/sounds";
				if (!utils::io::directory_exists(dir))
				{
					continue;
				}

				const auto sound_files = utils::io::list_files(dir);
				for (const auto& file : sound_files)
				{
					const auto last = file.find_last_of("\\/");
					std::string name = file;
					if (last != std::string::npos)
					{
						name = file.substr(last + 1);
					}

					load_sound(name, file);
				}
			}
		}
	}

	bool dump_sound(game::snd_alias_list_t* asset)
	{
		if (asset == nullptr)
		{
			return false;
		}

		rapidjson::Document j;
		j.SetObject();

		j.AddMember("aliasName", rapidjson::StringRef(asset->aliasName), j.GetAllocator());
		j.AddMember("count", asset->count, j.GetAllocator());

		rapidjson::Value head{rapidjson::kArrayType};

		for (auto i = 0; i < asset->count; i++)
		{
			const auto snd_head = &asset->head[i];
			rapidjson::Value entry{rapidjson::kObjectType};
			entry.AddMember("aliasName", rapidjson::StringRef(snd_head->aliasName), j.GetAllocator());

			if (snd_head->secondaryAliasName)
			{
				entry.AddMember("secondaryAliasName", rapidjson::StringRef(snd_head->secondaryAliasName), j.GetAllocator());
			}

			else
			{
				entry.AddMember("secondaryAliasName", rapidjson::Value{rapidjson::kNullType}, j.GetAllocator());
			}

			if (snd_head->chainAliasName)
			{
				entry.AddMember("chainAliasName", rapidjson::StringRef(snd_head->chainAliasName), j.GetAllocator());
			}
			else
			{
				entry.AddMember("chainAliasName", rapidjson::Value{rapidjson::kNullType}, j.GetAllocator());
			}

			if (snd_head->subtitle)
			{
				entry.AddMember("subtitle", rapidjson::StringRef(snd_head->subtitle), j.GetAllocator());
			}
			else
			{
				entry.AddMember("subtitle", rapidjson::Value{rapidjson::kNullType}, j.GetAllocator());
			}

			if (snd_head->mixerGroup)
			{
				entry.AddMember("mixerGroup", rapidjson::StringRef(snd_head->mixerGroup), j.GetAllocator());
			}
			else
			{
				entry.AddMember("mixerGroup", rapidjson::Value{rapidjson::kNullType}, j.GetAllocator());
			}

			if (snd_head->soundFile)
			{
				rapidjson::Value sound_file{rapidjson::kObjectType};
				sound_file.AddMember("type", snd_head->soundFile->type, j.GetAllocator());

				if (snd_head->soundFile->exists)
				{
					if (snd_head->soundFile->type == game::SAT_LOADED)
					{
						sound_file.AddMember("name", rapidjson::StringRef(snd_head->soundFile->u.loadSnd->name), j.GetAllocator());
					}
					else if (snd_head->soundFile->type == game::SAT_STREAMED)
					{
						sound_file.AddMember("totalMsec", snd_head->soundFile->u.streamSnd.totalMsec, j.GetAllocator());
						sound_file.AddMember("isLocalized", snd_head->soundFile->u.streamSnd.filename.isLocalized, j.GetAllocator());
						sound_file.AddMember("isStreamed", snd_head->soundFile->u.streamSnd.filename.isStreamed, j.GetAllocator());
						sound_file.AddMember("fileIndex", snd_head->soundFile->u.streamSnd.filename.fileIndex, j.GetAllocator());

						if (snd_head->soundFile->u.streamSnd.filename.fileIndex)
						{
							rapidjson::Value packed{rapidjson::kObjectType};
							packed.AddMember("offset", snd_head->soundFile->u.streamSnd.filename.info.packed.offset, j.GetAllocator());
							packed.AddMember("length", snd_head->soundFile->u.streamSnd.filename.info.packed.length, j.GetAllocator());
							sound_file.AddMember("packed", packed, j.GetAllocator());
						}
						else
						{
							rapidjson::Value raw{rapidjson::kObjectType};
							raw.AddMember("dir", rapidjson::StringRef(snd_head->soundFile->u.streamSnd.filename.info.raw.dir), j.GetAllocator());
							raw.AddMember("name", rapidjson::StringRef(snd_head->soundFile->u.streamSnd.filename.info.raw.name), j.GetAllocator());
							sound_file.AddMember("packed", raw, j.GetAllocator());
						}
					}
				}

				entry.AddMember("soundfile", sound_file, j.GetAllocator());
			}

			entry.AddMember("flags", snd_head->flags, j.GetAllocator());
			entry.AddMember("sequence", snd_head->sequence, j.GetAllocator());
			entry.AddMember("volMin", snd_head->volMin, j.GetAllocator());
			entry.AddMember("volMax", snd_head->volMax, j.GetAllocator());
			entry.AddMember("volModIndex", snd_head->volModIndex, j.GetAllocator());
			entry.AddMember("pitchMin", snd_head->pitchMin, j.GetAllocator());
			entry.AddMember("pitchMax", snd_head->pitchMax, j.GetAllocator());
			entry.AddMember("distMin", snd_head->distMin, j.GetAllocator());
			entry.AddMember("distMax", snd_head->distMax, j.GetAllocator());
			entry.AddMember("velocityMin", snd_head->velocityMin, j.GetAllocator());
			entry.AddMember("masterPriority", snd_head->masterPriority, j.GetAllocator());
			entry.AddMember("masterPercentage", snd_head->masterPercentage, j.GetAllocator());
			entry.AddMember("slavePercentage", snd_head->slavePercentage, j.GetAllocator());
			entry.AddMember("probability", snd_head->probability, j.GetAllocator());
			entry.AddMember("startDelay", snd_head->startDelay, j.GetAllocator());

			if (snd_head->sndContext)
			{
				entry.AddMember("sndContext", rapidjson::StringRef(snd_head->sndContext->name), j.GetAllocator());
			}
			else
			{
				entry.AddMember("sndContext", rapidjson::Value{rapidjson::kNullType}, j.GetAllocator());
			}

			if (snd_head->sndCurve)
			{
				entry.AddMember("sndCurve", rapidjson::StringRef(snd_head->sndCurve->name), j.GetAllocator());
			}
			else
			{
				entry.AddMember("sndCurve", rapidjson::Value{rapidjson::kNullType}, j.GetAllocator());
			}

			if (snd_head->lpfCurve)
			{
				entry.AddMember("lpfCurve", rapidjson::StringRef(snd_head->lpfCurve->name), j.GetAllocator());
			}
			else
			{
				entry.AddMember("lpfCurve", rapidjson::Value{rapidjson::kNullType}, j.GetAllocator());
			}

			if (snd_head->hpfCurve)
			{
				entry.AddMember("hpfCurve", rapidjson::StringRef(snd_head->hpfCurve->name), j.GetAllocator());
			}
			else
			{
				entry.AddMember("hpfCurve", rapidjson::Value{rapidjson::kNullType}, j.GetAllocator());
			}

			if (snd_head->reverbSendCurve)
			{
				entry.AddMember("reverbSendCurve", rapidjson::StringRef(snd_head->reverbSendCurve->name), j.GetAllocator());
			}
			else
			{
				entry.AddMember("reverbSendCurve", rapidjson::Value{rapidjson::kNullType}, j.GetAllocator());
			}

			if (snd_head->speakerMap)
			{
				rapidjson::Value speaker_map{rapidjson::kObjectType};
				rapidjson::Value channel_maps{rapidjson::kArrayType};

				for (char x = 0; x < 2; x++)
				{
					for (char y = 0; y < 2; y++)
					{
						rapidjson::Value channel_map{rapidjson::kObjectType};

						channel_map.AddMember("speakerCount", snd_head->speakerMap->channelMaps[x][y].speakerCount, j.GetAllocator());

						rapidjson::Value speakers{rapidjson::kArrayType};
						for (int speaker = 0; speaker < snd_head->speakerMap->channelMaps[x][y].speakerCount; speaker++)
						{
							rapidjson::Value jspeaker{rapidjson::kObjectType};

							jspeaker.AddMember("speaker", snd_head->speakerMap->channelMaps[x][y].speakers[speaker].speaker, j.GetAllocator());
							jspeaker.AddMember("numLevels", snd_head->speakerMap->channelMaps[x][y].speakers[speaker].numLevels, j.GetAllocator());
							jspeaker.AddMember("levels0", snd_head->speakerMap->channelMaps[x][y].speakers[speaker].levels[0], j.GetAllocator());
							jspeaker.AddMember("levels1", snd_head->speakerMap->channelMaps[x][y].speakers[speaker].levels[1], j.GetAllocator());

							speakers.PushBack(jspeaker, j.GetAllocator());
						}

						channel_map.AddMember("speakers", speakers, j.GetAllocator());
						channel_maps.PushBack(channel_map, j.GetAllocator());
					}
				}

				speaker_map.AddMember("name", rapidjson::StringRef(snd_head->speakerMap->name), j.GetAllocator());
				speaker_map.AddMember("isDefault", snd_head->speakerMap->isDefault, j.GetAllocator());

				speaker_map.AddMember("channelMaps", channel_maps, j.GetAllocator());
				entry.AddMember("speakerMap", speaker_map, j.GetAllocator());
			}
			else
			{
				j.AddMember("speakerMap", rapidjson::Value{rapidjson::kNullType}, j.GetAllocator());
			}

			/*entry.AddMember("allowDoppler", snd_head->allowDoppler, j.GetAllocator());
			if (snd_head->dopplerPreset)
			{
				entry.AddMember("dopplerPreset", rapidjson::StringRef(snd_head->dopplerPreset->name), j.GetAllocator());
			}
			else
			{
				entry.AddMember("dopplerPreset", rapidjson::Value{rapidjson::kNullType}, j.GetAllocator());
			}*/

			rapidjson::Value unknown{rapidjson::kObjectType};
			rapidjson::Value pad{rapidjson::kArrayType};

			const auto pad0 = std::vector<std::uint8_t>(snd_head->__pad0, snd_head->__pad0 + sizeof(snd_head->__pad0));
			const auto pad1 = std::vector<std::uint8_t>(snd_head->__pad1, snd_head->__pad1 + sizeof(snd_head->__pad1));
			const auto pad2 = std::vector<std::uint8_t>(snd_head->__pad2, snd_head->__pad2 + sizeof(snd_head->__pad2));
			const auto pad3 = std::vector<std::uint8_t>(snd_head->__pad3, snd_head->__pad3 + sizeof(snd_head->__pad3));

			auto rpad0 = rapidjson_bytes_to_object(pad0, j);
			auto rpad1 = rapidjson_bytes_to_object(pad1, j);
			auto rpad2 = rapidjson_bytes_to_object(pad2, j);
			auto rpad3 = rapidjson_bytes_to_object(pad3, j);

			pad.PushBack(rpad0, j.GetAllocator());
			pad.PushBack(rpad1, j.GetAllocator());
			pad.PushBack(rpad2, j.GetAllocator());
			pad.PushBack(rpad3, j.GetAllocator());

			unknown.AddMember("pad", pad, j.GetAllocator());
			unknown.AddMember("u4", snd_head->u4, j.GetAllocator());
			unknown.AddMember("u5", snd_head->u5, j.GetAllocator());
			unknown.AddMember("u18", snd_head->u18, j.GetAllocator());
			unknown.AddMember("u20", snd_head->u20, j.GetAllocator());
			unknown.AddMember("u34", snd_head->u34, j.GetAllocator());

			entry.AddMember("unknown", unknown, j.GetAllocator());

			head.PushBack(entry, j.GetAllocator());
		}

		j.AddMember("head", head, j.GetAllocator());


		rapidjson::Value unknown_array{rapidjson::kArrayType};
		for (unsigned char i = 0; i < asset->unkCount; i++)
		{
			unknown_array.PushBack(asset->unk[i], j.GetAllocator());
		}

		j.AddMember("unknownArray", unknown_array, j.GetAllocator());

		std::string path = "dumps/sounds/"s + asset->aliasName;

		rapidjson::StringBuffer buffer{};
		rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::Document::EncodingType, rapidjson::ASCII<>>
			writer(buffer);
		writer.SetIndent(' ', 4);
		j.Accept(writer);

		utils::io::write_file(path, std::string{buffer.GetString(), buffer.GetLength()}, false);
		return true;
	}

	game::XAssetHeader find_sound(const char* name)
	{
		const auto hash = hasher(name);
		return loaded_sounds.access<game::XAssetHeader>([&](loaded_sound_map& map)
		{
			const auto i = map.find(hash);
			if (i != map.end())
			{
				return static_cast<game::XAssetHeader>(i->second);
			}

			return static_cast<game::XAssetHeader>(nullptr);
		});
	}

	void clear()
	{
		sound_allocator.clear();
		loaded_sounds.access([](loaded_sound_map& map)
		{
			map.clear();
		});

		load_sounds();
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			db_is_xasset_default_hook.create(0x3968C0_b, db_is_xasset_default_stub);
			utils::hook::call(0x5A0471_b, db_xasset_exists_stub);
			scr_table_lookup_hook.create(0x439A10_b, scr_table_lookup_stub);

			// remove raw/sound or raw/language/sound prefix when loading raw sounds
			utils::hook::call(0x5B219F_b, com_sprintf_raw_sound_localized_stub);
			utils::hook::call(0x5B221C_b, com_sprintf_raw_sound_stub);

			// fix playing non-existing music crashing
			snd_is_music_playing_hook.create(0x7024A0_b, snd_is_music_playing_stub);

			scheduler::once(clear, scheduler::pipeline::main);

			command::add("dumpSoundAlias", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					console::info("Usage: dumpSoundAlias <name>\n");
					return;
				}

				const auto name = params.get(1);
				const auto sound = game::DB_FindXAssetHeader(game::ASSET_TYPE_SOUND, name, false).sound;
				if (sound == nullptr)
				{
					console::error("Sound %s does not exist\n", name);
					return;
				}

				if (dump_sound(sound))
				{
					console::info("Sound dumped to dumps/sound/%s\n", name);
				}
				else
				{
					console::error("Failed to dump sound %s\n", name);
				}
			});
		}
	};
}

REGISTER_COMPONENT(sound::component)