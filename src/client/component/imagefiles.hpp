#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

class imagefiles final : public component_interface
{
public:
	void post_unpack() override;

	static void close_custom_handles();
	static void close_handle(const std::string& fastfile);

private:
	static std::string get_image_file_name();
	static void* get_image_file_unk_mp(unsigned int index);
	static void* get_image_file_unk_sp(unsigned int index);
	static game::DB_IFileSysFile* get_image_file_handle(unsigned int index);
	static void db_create_gfx_image_stream_stub(utils::hook::assembler& a);
	static void* pakfile_open_stub(void* handles, unsigned int count, int is_imagefile, unsigned int index, short is_localized);
	static int com_sprintf_stub(char* buffer, const int len, const char* fmt, unsigned int index);
};
