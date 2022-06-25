LUI.MenuBuilder.registerPopupType("ShaderCacheDialog_original", LUI.ShaderCacheDialog.new)

game:addlocalizedstring("PLATFORM_SHADER_PRECACHE_ASK", "Would you like to populate the shader cache? It may cause crashes with certain GPUs (e.g. RTX cards) but will improve performance if successful.")
game:addlocalizedstring("MENU_NO_DONT_ASK", "No, don't ask me again")

local function dialog(...)
	if (game:sharedget("has_accepted_shader_caching") == "1") then
		return LUI.ShaderCacheDialog.new(...)
	end

	return LUI.MenuBuilder.BuildRegisteredType("generic_yesno_popup", {
		popup_title = Engine.Localize("@MENU_WARNING"),
		message_text = Engine.Localize("@PLATFORM_SHADER_PRECACHE_ASK"),
		yes_action = function()
			game:sharedset("has_accepted_shader_caching", "1")
			LUI.FlowManager.RequestAddMenu(nil, "ShaderCacheDialog_original")
		end,
		yes_text = Engine.Localize("@MENU_YES"),
		no_text = Engine.Localize("@MENU_NO_DONT_ASK"),
		no_action = function()
			Engine.SetDvarInt("r_preloadShadersFrontendAllow", 0)
		end,
		default_focus_index = 2,
		cancel_will_close = false
	})
end

LUI.MenuBuilder.m_types_build["ShaderCacheDialog"] = dialog
