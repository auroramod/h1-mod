#include <std_include.hpp>

#ifdef DEBUG
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/fastfiles.hpp"
#include "../gui.hpp"
#include "../asset_list.hpp"

#include <utils/string.hpp>
#include <utils/compression.hpp>
#include <utils/hook.hpp>

#include <shader-tool/shader.hpp>

#pragma comment(lib, "dxguid.lib")

namespace gui::asset_list::shader
{
	namespace
	{
		constexpr GUID guid_shader_bytecode = {0x8B07816E, 0x844C, 0x4F5C, {0xA8, 0x83, 0x1C, 0x35, 0x56, 0xDC, 0x54, 0x20}};

		utils::hook::detour create_pixel_shader_hook;
		utils::hook::detour create_vertex_shader_hook;
		utils::hook::detour create_domain_shader_hook;
		utils::hook::detour create_hull_shader_hook;
		utils::hook::detour create_compute_shader_hook;

		void create_pixel_shader_stub(game::GfxPixelShaderLoadDef* load_def, game::MaterialPixelShader* shader)
		{
			create_pixel_shader_hook.invoke<void>(load_def, shader);
			shader->prog.ps->SetPrivateData(guid_shader_bytecode, load_def->programSize, load_def->program);
			shader->prog.ps->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::strlen(shader->name)), shader->name);
		}

		void create_vertex_shader_stub(game::GfxVertexShaderLoadDef* load_def, game::MaterialVertexShader* shader)
		{
			create_vertex_shader_hook.invoke<void>(load_def, shader);
			shader->prog.vs->SetPrivateData(guid_shader_bytecode, load_def->programSize, load_def->program);
			shader->prog.vs->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::strlen(shader->name)), shader->name);
		}

		void create_domain_shader_stub(game::GfxDomainShaderLoadDef* load_def, game::MaterialDomainShader* shader)
		{
			create_domain_shader_hook.invoke<void>(load_def, shader);
			shader->prog.ds->SetPrivateData(guid_shader_bytecode, load_def->programSize, load_def->program);
			shader->prog.ds->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::strlen(shader->name)), shader->name);
		}

		void create_hull_shader_stub(game::GfxHullShaderLoadDef* load_def, game::MaterialHullShader* shader)
		{
			create_hull_shader_hook.invoke<void>(load_def, shader);
			shader->prog.hs->SetPrivateData(guid_shader_bytecode, load_def->programSize, load_def->program);
			shader->prog.hs->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::strlen(shader->name)), shader->name);
		}

		void create_compute_shader_stub(game::GfxComputeShaderLoadDef* load_def, game::ComputeShader* shader)
		{
			create_compute_shader_hook.invoke<void>(load_def, shader);
			shader->prog.cs->SetPrivateData(guid_shader_bytecode, load_def->programSize, load_def->program);
			shader->prog.cs->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(std::strlen(shader->name)), shader->name);
		}

		std::string disassemble_shader(ID3D11DeviceChild* shader)
		{
			std::string data;
			UINT data_size{};
			shader->GetPrivateData(guid_shader_bytecode, &data_size, data.data());
			if (data_size == 0u)
			{
				return "failed to get shader bytecode";
			}

			data.resize(data_size);
			shader->GetPrivateData(guid_shader_bytecode, &data_size, data.data());

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
		std::string& get_disassembled_shader(T* asset, bool force = false)
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
		bool draw_asset(T* asset)
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
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			const auto size = ImVec2(900, 700);

			create_pixel_shader_hook.create(SELECT_VALUE(0x56DB40_b, 0x6910A0_b), create_pixel_shader_stub);
			create_vertex_shader_hook.create(SELECT_VALUE(0x56DBF0_b, 0x691150_b), create_vertex_shader_stub);
			create_domain_shader_hook.create(SELECT_VALUE(0x56DA20_b, 0x690F80_b), create_domain_shader_stub);
			create_hull_shader_hook.create(SELECT_VALUE(0x56DAB0_b, 0x691010_b), create_hull_shader_stub);
			create_compute_shader_hook.create(SELECT_VALUE(0x56D8F0_b, 0x690E50_b), create_compute_shader_stub);

			gui::asset_list::add_asset_view<game::MaterialPixelShader>(game::ASSET_TYPE_PIXELSHADER, draw_asset<game::MaterialPixelShader>, size);
			gui::asset_list::add_asset_view<game::MaterialVertexShader>(game::ASSET_TYPE_VERTEXSHADER, draw_asset<game::MaterialVertexShader>, size);
			gui::asset_list::add_asset_view<game::MaterialDomainShader>(game::ASSET_TYPE_DOMAINSHADER, draw_asset<game::MaterialDomainShader>, size);
			gui::asset_list::add_asset_view<game::MaterialHullShader>(game::ASSET_TYPE_HULLSHADER, draw_asset<game::MaterialHullShader>, size);
			gui::asset_list::add_asset_view<game::ComputeShader>(game::ASSET_TYPE_COMPUTESHADER, draw_asset<game::ComputeShader>, size);
		}
	};
}

REGISTER_COMPONENT(gui::asset_list::shader::component)
#endif
