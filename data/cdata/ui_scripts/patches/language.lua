game:addlocalizedstring("MENU_english", "English")
game:addlocalizedstring("MENU_french", "Français")
game:addlocalizedstring("MENU_german", "Deutsch")
game:addlocalizedstring("MENU_italian", "Italiano")
game:addlocalizedstring("MENU_spanish", "Español")
game:addlocalizedstring("MENU_russian", "Русский")
game:addlocalizedstring("MENU_polish", "Polski")
game:addlocalizedstring("MENU_portuguese", "Português")
game:addlocalizedstring("MENU_japanese_full", "日本語")
game:addlocalizedstring("MENU_japanese_partial", "日本語(一部)")
game:addlocalizedstring("MENU_traditional_chinese", "繁體中文")
game:addlocalizedstring("MENU_simplified_chinese", "简体中文")
game:addlocalizedstring("MENU_arabic", "العربية")
game:addlocalizedstring("MENU_czech", "České")
game:addlocalizedstring("MENU_spanishna", "Español (América Latina)") -- text only (English dubbing)
game:addlocalizedstring("MENU_korean", "한국어")
game:addlocalizedstring("MENU_english_safe", "English (Safe)")

LUI.addmenubutton("pc_controls", {
	index = 4,
	text = "@LUA_MENU_CHOOSE_LANGUAGE",
	description = Engine.Localize("@LUA_MENU_CHOOSE_LANGUAGE_DESC"),
	callback = function()
		LUI.FlowManager.RequestAddMenu(nil, "choose_language_menu")
	end
})

LUI.MenuBuilder.registerType("choose_language_menu", function(a1)
	local menu = LUI.MenuTemplate.new(a1, {
		menu_title = "@LUA_MENU_CHOOSE_LANGUAGE",
		menu_list_divider_top_offset = -(LUI.H1MenuTab.tabChangeHoldingElementHeight + H1MenuDims.spacing),
		menu_width = 240,
		uppercase_title = true
	})

	local languages = Engine.GetSupportedLanguages()

	for i = 1, #languages do
		local id = languages[i].id

		local lang = Engine.GetCurrentLanguage()
	
		local button = menu:AddButton("", function()
			LUI.FlowManager.RequestAddMenu(nil, "choose_language_confirm_popup", false, nil, true, {
				language = languages[i].id
			})
		end, languages[i].id == Engine.GetCurrentLanguage(), true, nil, {
			desc_text = Engine.Localize("LOCALE_" .. (languages[i].id))
		})

		overrideyoffset = nil

		local label = button:getFirstDescendentById("text_label")
		label:setText(Engine.ToUpperCase(languages[i].name))
	end

	LUI.Options.InitScrollingList(menu.list, nil, {
		rows = 10
	})

	LUI.Options.AddOptionTextInfo(menu)
	menu:AddBackButton()

	return menu
end)

LUI.UIButtonText.IsOffsetedLanguage = function()
	return false
end
