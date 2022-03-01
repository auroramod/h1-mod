#include <std_include.hpp>
#include "../services.hpp"

#include <utils/nt.hpp>
#include <utils/io.hpp>
#include <utils/cryptography.hpp>

#include "game/game.hpp"

namespace demonware
{
	bdStorage::bdStorage() : service(10, "bdStorage")
	{
		this->register_task(20, &bdStorage::list_publisher_files);
		this->register_task(21, &bdStorage::get_publisher_file);
		this->register_task(24, &bdStorage::set_user_file);
		this->register_task(16, &bdStorage::get_user_file);
		this->register_task(12, &bdStorage::unk12);

		this->map_publisher_resource("motd-.*\\.txt", DW_MOTD);
		this->map_publisher_resource("ffotd-.*\\.ff", DW_FASTFILE);
		this->map_publisher_resource("playlists(_.+)?\\.aggr", DW_PLAYLISTS);
	}

	void bdStorage::map_publisher_resource(const std::string& expression, const INT id)
	{
		auto data = utils::nt::load_resource(id);
		this->map_publisher_resource_variant(expression, std::move(data));
	}

	void bdStorage::map_publisher_resource_variant(const std::string& expression, resource_variant resource)
	{
		if (resource.valueless_by_exception())
		{
			throw std::runtime_error("Publisher resource variant is empty!");
		}

		this->publisher_resources_.emplace_back(std::regex{expression}, std::move(resource));
	}

	bool bdStorage::load_publisher_resource(const std::string& name, std::string& buffer)
	{
		for (const auto& resource : this->publisher_resources_)
		{
			if (std::regex_match(name, resource.first))
			{
				if (std::holds_alternative<std::string>(resource.second))
				{
					buffer = std::get<std::string>(resource.second);
				}
				else
				{
					buffer = std::get<callback>(resource.second)();
				}

				return true;
			}
		}

#ifdef DEBUG
		printf("[DW]: [bdStorage]: missing publisher file: %s\n", name.data());
#endif

		return false;
	}

	void bdStorage::list_publisher_files(service_server* server, byte_buffer* buffer)
	{
#ifdef DEBUG
		utils::io::write_file("demonware/bdStorage/list_publisher_files", buffer->get_buffer());
#endif

		uint32_t date;
		uint16_t num_results, offset;
		std::string unk, filename, data;

		buffer->read_string(&unk);
		buffer->read_uint32(&date);
		buffer->read_uint16(&num_results);
		buffer->read_uint16(&offset);
		buffer->read_string(&filename);

#ifdef DEBUG
		printf("[DW]: [bdStorage]: list publisher files: %s\n", filename.data());
#endif

		auto reply = server->create_reply(this->task_id());

		if (this->load_publisher_resource(filename, data))
		{
			auto* info = new bdFileInfo;

			info->file_id = *reinterpret_cast<const uint64_t*>(utils::cryptography::sha1::compute(filename).data());
			info->filename = filename;
			info->create_time = 0;
			info->modified_time = info->create_time;
			info->file_size = uint32_t(data.size());
			info->owner_id = 0;
			info->priv = false;

			reply->add(info);
		}

		reply->send();
	}

	void bdStorage::get_publisher_file(service_server* server, byte_buffer* buffer)
	{
#ifdef DEBUG
		utils::io::write_file("demonware/bdStorage/get_publisher_file", buffer->get_buffer());
#endif

		std::string unk, filename;
		buffer->read_string(&unk);
		buffer->read_string(&filename);

#ifdef DEBUG
		printf("[DW]: [bdStorage]: loading publisher file: %s\n", filename.data());
#endif

		std::string data;

		if (this->load_publisher_resource(filename, data))
		{
#ifdef DEBUG
			printf("[DW]: [bdStorage]: sending publisher file: %s, size: %lld\n", filename.data(), data.size());
#endif

			auto reply = server->create_reply(this->task_id());
			reply->add(new bdFileData(data));
			reply->send();
		}
		else
		{
			server->create_reply(this->task_id(), game::BD_NO_FILE)->send();
		}
	}

	std::string bdStorage::get_user_file_path(const std::string& name)
	{
		return "players2/user/" + name;
	}

	void bdStorage::set_user_file(service_server* server, byte_buffer* buffer) const
	{
#ifdef DEBUG
		utils::io::write_file("demonware/bdStorage/set_user_file", buffer->get_buffer());
#endif

		auto reply = server->create_reply(this->task_id());
		uint64_t owner;

		{
			bool priv;
			uint32_t unk32_0;
			uint32_t unk32_1;
			std::string game, platform, filename, data;

			buffer->read_string(&game);
			buffer->read_uint64(&owner);
			buffer->read_string(&platform);
			buffer->read_uint32(&unk32_0);

			buffer->read_string(&filename);
			buffer->read_blob(&data);
			buffer->read_uint32(&unk32_1);
			buffer->read_bool(&priv);

			const auto path = get_user_file_path(filename);
			utils::io::write_file(path, data);

#ifdef DEBUG
			printf("[DW]: [bdStorage]: set user file: %s\n", filename.data());
#endif

			auto* info = new bdFileInfo;

			info->file_id = *reinterpret_cast<const uint64_t*>(utils::cryptography::sha1::compute(filename).data());
			info->filename = filename;
			info->create_time = uint32_t(time(nullptr));
			info->modified_time = info->create_time;
			info->file_size = uint32_t(data.size());
			info->owner_id = uint64_t(owner);
			info->priv = priv;

			reply->add(info);
		}

		{
			bool priv;
			uint32_t unk32_0;
			std::string filename, data;

			buffer->read_string(&filename);
			buffer->read_blob(&data);
			buffer->read_uint32(&unk32_0);
			buffer->read_bool(&priv);

			const auto path = get_user_file_path(filename);
			utils::io::write_file(path, data);

#ifdef DEBUG
			printf("[DW]: [bdStorage]: set user file: %s\n", filename.data());
#endif

			auto* info = new bdFileInfo;

			info->file_id = *reinterpret_cast<const uint64_t*>(utils::cryptography::sha1::compute(filename).data());
			info->filename = filename;
			info->create_time = uint32_t(time(nullptr));
			info->modified_time = info->create_time;
			info->file_size = uint32_t(data.size());
			info->owner_id = uint64_t(owner);
			info->priv = priv;

			reply->add(info);
		}

		reply->send();
	}

	void bdStorage::get_user_file(service_server* server, byte_buffer* buffer) const
	{
#ifdef DEBUG
		utils::io::write_file("demonware/bdStorage/get_user_file", buffer->get_buffer());
#endif

		uint32_t unk32_0;
		uint32_t unk32_1;
		uint64_t owner;
		std::string game, filename, filename2, 
			platform, data, data2;

		buffer->read_string(&game);
		buffer->read_uint32(&unk32_0);
		buffer->read_uint64(&owner);
		buffer->read_string(&platform);
		buffer->read_uint64(&owner);
		buffer->read_string(&platform);
		buffer->read_uint32(&unk32_1);
		buffer->read_string(&filename);
		buffer->read_string(&filename2);

#ifdef DEBUG
		printf("[DW]: [bdStorage]: get user file: %s, %s, %s\n", game.data(), filename.data(), platform.data());
#endif

#ifdef DEBUG
		printf("[DW]: [bdStorage]: get user file: %s, %s, %s\n", game.data(), filename2.data(), platform.data());
#endif

		const auto path = get_user_file_path(filename);
		const auto path2 = get_user_file_path(filename2);

		if (utils::io::read_file(path, &data) && utils::io::read_file(path2, &data2))
		{
			// TODO: find out what the response should be for 2 files
			// auto reply = server->create_reply(this->task_id());
			// reply->add(new bdFileData(data));
			// reply->add(new bdFileData(data2));
			// reply->send();

			server->create_reply(this->task_id(), game::BD_NO_FILE)->send();
		}
		else
		{
			server->create_reply(this->task_id(), game::BD_NO_FILE)->send();
		}
	}

	void bdStorage::unk12(service_server* server, byte_buffer* buffer) const
	{
#ifdef DEBUG
		utils::io::write_file("demonware/bdStorage/unk12", buffer->get_buffer());
#endif

		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}
}
