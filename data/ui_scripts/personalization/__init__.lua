local s1MPPlayercardMenu = LUI.mp_menus.s1MPPlayercardMenu

game:addlocalizedstring("LUA_MENU_CHANGE_NAME", "Name")


playercard_menu = function(f17_arg0, f17_arg1)
    if not f17_arg1 then
        f17_arg1 = {}
    end

    local exclusiveController = f17_arg1.exclusiveController
    local personalizationMenu = LUI.MenuTemplate.new(f17_arg0, {
        menu_title = "@LUA_MENU_PERSONALIZATION_CAPS",
        persistentBackground = PersistentBackground.Variants.VirtualLobby
    })

    Cac.SetSelectedControllerIndex(exclusiveController)

    local f17_local2 = {}
    if s1MPPlayercardMenu.IsPublicMatch() and LUI.InventoryUtils.GetCategoryNewStickerState(exclusiveController, "CallingCard") then
        f17_local2.showNewSticker = true
    end
    local f17_local3 = {}
    if s1MPPlayercardMenu.IsPublicMatch() and LUI.InventoryUtils.GetCategoryNewStickerState(exclusiveController, "Emblem") then
        f17_local3.showNewSticker = true
    end

    if Engine.IsDepotEnabled() then
        personalizationMenu:AddButton("@LUA_MENU_GEARTYPE_SINGLE_HEAD", "PersonalizeCharacter")
        personalizationMenu:AddButton("@MENU_CAMO_CAPS", "PersonalizeCamo")
        personalizationMenu:AddButton("@MENU_CALLING_CARD", "CallingCardSelectMenu", nil, nil, nil, f17_local2)
        personalizationMenu:AddButton("@MENU_PLAYERCARD_ICONS", "EmblemSelectMenu", nil, nil, nil, f17_local3)
    else
        personalizationMenu:AddButton("@MENU_CALLING_CARD", "CallingCardMenu", nil, nil, nil, f17_local2)
        personalizationMenu:AddButton("@MENU_PLAYERCARD_ICONS", "EmblemMenu", nil, nil, nil, f17_local3)
    end

    local f17_local4 = false
    local f17_local5 = Engine.IsXbox360()
    if not f17_local5 then
        f17_local5 = Engine.IsXB3()
    end
    if f17_local5 and not Engine.IsUserSignedInToLive(exclusiveController) then
        f17_local4 = true
    end

    local f17_local6, f17_local7, f17_local8 = nil

    if Engine.GetOnlineGame() and not Engine.IsChatRestricted() then
        f17_local6, f17_local7, f17_local8 = Cac.GetItemLockState(exclusiveController, nil, "clantag")
        if not f17_local4 then
            f17_local4 = f17_local6 ~= "Unlocked"
        end
        local f17_local9 = {}
        if f17_local4 then
            f17_local9.desc_text = f17_local8
            f17_local9.showLockOnDisable = true
        end
        local personalizationMenu0 = personalizationMenu:AddButton("LUA_MP_FRONTEND_TAG", "TagEditPopup", f17_local4,
            nil, nil, f17_local9)
        personalizationMenu0:clearActionSFX()

        personalizationMenu:AddButton("LUA_MENU_CHANGE_NAME", function(a1, a2)
            LUI.FlowManager.RequestAddMenu(a1, "client_set_name", true, nil)
        end)
    end

    personalizationMenu:AddSpacing(20)
    personalizationMenu:AddHeader("@MENU_EMBLEM_OPTIONS")

    local f17_local9 = nil
    local personalizationMenu0 = {
        variant = GenericButtonSettings.Variants.Checkbox,
        content_margin = 0,
        content_width = 30,
        canShowLocked = true,
        showLockOnDisable = true
    }

    local personalizationMenu1 = {}

    LUI.InheritCopy(personalizationMenu1, personalizationMenu0)
    local personalizationMenu2, personalizationMenu3, personalizationMenu4 =
        Cac.GetItemLockState(exclusiveController, nil, "feature_character_emblem")
    f17_local8 = personalizationMenu4
    f17_local7 = personalizationMenu3
    personalizationMenu2 = personalizationMenu2 == "Unlocked"
    if not personalizationMenu2 then
        personalizationMenu1.desc_text = f17_local8
    end
 
    s1MPPlayercardMenu.UpdateCheckbox(personalizationMenu:AddButton("MENU_OPTION_CHARACTER_PATCH",
    s1MPPlayercardMenu.OnToggleCharacterPatch, not personalizationMenu2, nil, nil, personalizationMenu1),
    s1MPPlayercardMenu.GetCharacterPatchOption(exclusiveController))

    personalizationMenu3 = {}
    LUI.InheritCopy(personalizationMenu3, personalizationMenu0)
    local personalizationMenu4, self, personalizationMenu6 =
        Cac.GetItemLockState(exclusiveController, nil, "feature_weapon_emblem")
    f17_local8 = personalizationMenu6
    f17_local7 = self
    personalizationMenu4 = personalizationMenu4 == "Unlocked"
    if not personalizationMenu4 then
        personalizationMenu3.desc_text = f17_local8
    end
    s1MPPlayercardMenu.UpdateCheckbox(
        personalizationMenu:AddButton("MENU_OPTION_WEAPON_STICKER", s1MPPlayercardMenu.OnToggleWeaponSticker, not personalizationMenu4,
            nil, nil, personalizationMenu3), s1MPPlayercardMenu.GetWeaponStickerOption(exclusiveController))  

    personalizationMenu:AddMenuDescription(3)
    local VerticalList = LUI.UIVerticalList.new({
        topAnchor = true,
        bottomAnchor = true,
        leftAnchor = true,
        rightAnchor = false,
        top = 162,
        left = LUI.Playercard.Width + 7,
        width = LUI.Playercard.Width,
        spacing = 7
    })
    personalizationMenu6 = personalizationMenu.list
    personalizationMenu.list = VerticalList
    VerticalList.buttonCount = 0
    personalizationMenu:addElement(VerticalList)
    s1MPPlayercardMenu.AddPlayerCard(personalizationMenu, exclusiveController)
   if true == Engine.IsDepotEnabled() then
        s1MPPlayercardMenu.AddMiscCard(personalizationMenu, exclusiveController, "character")
        s1MPPlayercardMenu.AddMiscCard(personalizationMenu, exclusiveController, "char_camo")
    end
    personalizationMenu.list = personalizationMenu6
    personalizationMenu:AddBackButton(function(f18_arg0, f18_arg1)
        Engine.ExecNow("upload_playercard", f18_arg1.controller)
        LUI.FlowManager.RequestLeaveMenu(f18_arg0)
    end)
    personalizationMenu:addElement(LUI.CacFactionWidget.new(personalizationMenu,
        personalizationMenu.exclusiveController, true))
    Cac.NotifyVirtualLobby("cao", exclusiveController)
    personalizationMenu:AddRotateHelp()
    personalizationMenu:AddCurrencyInfoPanel()

    return personalizationMenu
end

LUI.MenuBuilder.m_types_build["PlayercardMenu"] = playercard_menu

function ConfirmChangeName( f3_arg0, f3_arg1 )
	local name = Engine.GetDvarString("change_name")
	if  name ~= nil and name ~= "" then
		Engine.SetDvarFromString("name", name)
		LUI.FlowManager.RequestLeaveMenu( f3_arg0 )
	end
end

ClientRenamePopupButtons = function ()
	local keyboard = LUI.FlowManager.GetMenuScopedDataByMenuName( "virtual_keyboard" )
	local buttons = {
		[1] = {
			type = "generic_input_field",
			id = "edit_name_field",
			max_length = keyboard.maxLength,
			properties = {
				help_value = Engine.GetDvarString("name"),
				keyboard_type = CoD.KeyboardInputTypes.Normal,
				field_edited_func = function ( f4_arg0, f4_arg1 )
					local keyboard = LUI.FlowManager.GetMenuScopedDataFromElement( f4_arg0 )
					keyboard.input = f4_arg1.text or ""
					Engine.SetDvarFromString("change_name", keyboard.input)
					if keyboard.exiting then
						return 
					else
						f4_arg1.controller = keyboard.exclusiveController
					end
				end
			}
		},
		[2] = {
			type = "UIGenericButton",
			id = "confirm_edit_name",
			properties = {
				style = GenericButtonSettings.Styles.GlassButton,
				substyle = GenericButtonSettings.Styles.GlassButton.SubStyles.Popup,
				button_text = "Change",
				index = 2,
				button_action_func = ConfirmChangeName
			},
		},
	}
	return buttons
end

ClientRenameField = function ( f50_arg0, f50_arg1 )
	return LUI.MenuBuilder.BuildRegisteredType( "generic_selectionList_popup", {
		popup_childfeeder = ClientRenamePopupButtons,
		popup_title = "Change name"
	} )
end

LUI.MenuBuilder.registerPopupType( "client_set_name", ClientRenameField )
