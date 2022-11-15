if (not Engine.InFrontend()) then
    return
end

local levelselectmenu = LUI.sp_menus.LevelSelectMenu
levelselectmenu.SetupInfoBoxRightForArcadeMode = function(f44_arg0, f44_arg1)
    return false
end

local addlevellistbuttons = LUI.LevelSelect.AddLevelListButtons
LUI.LevelSelect.AddLevelListButtons = function(self, map_info_list)
    addlevellistbuttons()

    if Engine.GetDvarBool("arcademode") then
        return
    end

    if Engine.GetDvarBool("profileMenuOption_hasUnlockedAll_SP") then
        self:AddHelp({
            name = "add_button_helper_text",
            button_ref = "button_alt2",
            helper_text = Engine.Localize("@LUA_MENU_CANCEL_UNLOCK_CAPS"),
            side = "right",
            clickable = true
        }, levelselectmenu.SetUnlockAll)
    else
        self:AddHelp({
            name = "add_button_helper_text",
            button_ref = "button_alt2",
            helper_text = Engine.Localize("@LUA_MENU_CAMPAIGN_UNLOCKED_ALL_TITLE"),
            side = "right",
            clickable = true
        }, levelselectmenu.UnlockAllPopup)
    end
end

levelselectmenu.UnlockAllPopup = function(f56_arg0, f56_arg1)
    LUI.FlowManager.RequestAddMenu(nil, "request_yesno_popup_generic", true, f56_arg1.controller, nil, {
        popup_title = Engine.Localize("@LUA_MENU_CAMPAIGN_UNLOCKED_ALL_TITLE"),
        yes_action = levelselectmenu.SetUnlockAll,
        message_text = Engine.Localize("@MENU_COMPLETED_CHEAT") .. ". " ..
            Engine.Localize("@LUA_MENU_CAMPAIGN_UNLOCKED_ALL_DESC")
    })
end

local setunlockall = levelselectmenu.SetUnlockAll
levelselectmenu.SetUnlockAll = function(f57_arg0, f57_arg1)
    Engine.SetDvarBool("profileMenuOption_hasUnlockedAll_SP",
        not Engine.GetDvarBool("profileMenuOption_hasUnlockedAll_SP"))
    Engine.SetDvarBool("mis_cheat", not Engine.GetDvarBool("mis_cheat"))

    setunlockall()
end
