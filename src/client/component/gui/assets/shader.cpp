#include <std_include.hpp>

#ifdef _DEBUG
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/fastfiles.hpp"
#include "component/d3d11.hpp"
#include "../gui.hpp"
#include "../asset_list.hpp"
#include "shader.hpp"

#include <utils/string.hpp>
#include <utils/compression.hpp>
#include <utils/hook.hpp>

#include <shader-tool/shader.hpp>

#pragma comment(lib, "dxguid.lib")

void asset_shader::post_unpack()
{
	const auto size = ImVec2(900, 700);

	asset_list::add_asset_view<game::MaterialPixelShader>(game::ASSET_TYPE_PIXELSHADER, draw_asset<game::MaterialPixelShader>, size);
	asset_list::add_asset_view<game::MaterialVertexShader>(game::ASSET_TYPE_VERTEXSHADER, draw_asset<game::MaterialVertexShader>, size);
	asset_list::add_asset_view<game::MaterialDomainShader>(game::ASSET_TYPE_DOMAINSHADER, draw_asset<game::MaterialDomainShader>, size);
	asset_list::add_asset_view<game::MaterialHullShader>(game::ASSET_TYPE_HULLSHADER, draw_asset<game::MaterialHullShader>, size);
	asset_list::add_asset_view<game::ComputeShader>(game::ASSET_TYPE_COMPUTESHADER, draw_asset<game::ComputeShader>, size);
}

std::string asset_shader::disassemble_shader(ID3D11DeviceChild* shader)
{
	std::string data;
	UINT data_size{};
	shader->GetPrivateData(d3d11::guid_shader_bytecode, &data_size, data.data());
	if (data_size == 0u)
	{
		return "failed to get shader bytecode";
	}

	data.resize(data_size);
	shader->GetPrivateData(d3d11::guid_shader_bytecode, &data_size, data.data());

	try
	{
		auto obj = alys::shader::shader_object::parse(data);
		return obj.dump();
	}
	catch (const std::exception& e)
	{
		return e.what();
	}
}

template <typename T>
std::string& asset_shader::get_disassembled_shader(T* asset, bool force)
{
	static std::unordered_map<std::string, std::string> buffers;

	if (!force)
	{
		if (const auto iter = buffers.find(asset->name); iter != buffers.end())
		{
			return iter->second;
		}
	}

	buffers[asset->name] = disassemble_shader(*reinterpret_cast<ID3D11DeviceChild**>(&asset->prog));
	
	return get_disassembled_shader(asset);
}

template <typename T>
bool asset_shader::draw_asset(T* asset)
{
	const auto reset_buffer = ImGui::Button("reset");
	auto& buffer = get_disassembled_shader(asset, reset_buffer);

	ImGui::SameLine();

	if (ImGui::Button("copy"))
	{
		gui::copy_to_clipboard(buffer);
	}

	ImGui::PushItemWidth(-1);
	ImGui::InputTextMultiline("buffer", &buffer, ImGui::GetContentRegionAvail(), ImGuiInputTextFlags_ReadOnly);
	ImGui::PopItemWidth();

	return true;
}

REGISTER_COMPONENT(asset_shader)
#endif
