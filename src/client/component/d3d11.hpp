#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class d3d11 final : public component_interface
{
public:
	void* load_import(const std::string& library, const std::string& function) override;
	void post_unpack() override;

	static constexpr GUID guid_shader_bytecode = {0x8B07816E, 0x844C, 0x4F5C, {0xA8, 0x83, 0x1C, 0x35, 0x56, 0xDC, 0x54, 0x20}};

private:
	static std::string get_shader_name(ID3D11DeviceChild* shader);
	static void print_current_shaders(int print_type);
	static HRESULT __stdcall info_queue_add_message_stub(ID3D11InfoQueue* this_,
		D3D11_MESSAGE_CATEGORY category, D3D11_MESSAGE_SEVERITY severity,
		D3D11_MESSAGE_ID id, LPCSTR description);
	static void hook_debug_info(ID3D11Device* device);
	static HRESULT d3d11_create_device_stub(IDXGIAdapter* p_adapter, D3D_DRIVER_TYPE driver_type, HMODULE software,
		UINT flags, const D3D_FEATURE_LEVEL* p_feature_levels, UINT feature_levels, UINT sdk_version,
		ID3D11Device** pp_device, D3D_FEATURE_LEVEL* p_feature_level, ID3D11DeviceContext** pp_immediate_context);
	static void create_pixel_shader_stub(game::GfxPixelShaderLoadDef* load_def, game::MaterialPixelShader* shader);
	static void create_vertex_shader_stub(game::GfxVertexShaderLoadDef* load_def, game::MaterialVertexShader* shader);
	static void create_domain_shader_stub(game::GfxDomainShaderLoadDef* load_def, game::MaterialDomainShader* shader);
	static void create_hull_shader_stub(game::GfxHullShaderLoadDef* load_def, game::MaterialHullShader* shader);
	static void create_compute_shader_stub(game::GfxComputeShaderLoadDef* load_def, game::ComputeShader* shader);
};
