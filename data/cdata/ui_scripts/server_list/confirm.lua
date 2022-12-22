LUI.MenuBuilder.m_types_build["popup_confirmdownload"] = function()
    return LUI.MenuBuilder.BuildRegisteredType("generic_yesno_popup", {
		popup_title = Engine.Localize("@MENU_NOTICE"),
		message_text = Engine.Localize("@LUA_MENU_3RD_PARTY_CONTENT_DESC", download.getwwwurl()),
		yes_action = function()
			download.userdownloadresponse(true)
		end,
		no_action = function()
			download.userdownloadresponse(false)
		end
	})
end
