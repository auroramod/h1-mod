if Engine.InFrontend() then
    local levelselectmenu = LUI.sp_menus.LevelSelectMenu
    levelselectmenu.SetupInfoBoxRightForArcadeMode = function(f44_arg0, f44_arg1)
        return false
    end
    LUI.LevelSelect.AddLevelListButtons = function(f50_arg0, f50_arg1)
        for f50_local0 = 1, #f50_arg1, 1 do
            if not Engine.GetDvarBool("arcademode") or not f50_arg1[f50_local0].narativeLevel then
                f50_arg0:AddLevelListEntry(f50_local0, f50_arg1[f50_local0])
            end
        end
        if not Engine.GetDvarBool("arcademode") then
            if Engine.GetDvarBool("profileMenuOption_hasUnlockedAll_SP") then
                f50_arg0:AddHelp({
                    name = "add_button_helper_text",
                    button_ref = "button_alt2",
                    helper_text = Engine.Localize("@LUA_MENU_CANCEL_UNLOCK_CAPS"),
                    side = "right",
                    clickable = true
                }, levelselectmenu.SetUnlockAll)
            else
                f50_arg0:AddHelp({
                    name = "add_button_helper_text",
                    button_ref = "button_alt2",
                    helper_text = Engine.Localize("@LUA_MENU_CAMPAIGN_UNLOCKED_ALL_TITLE"),
                    side = "right",
                    clickable = true
                }, levelselectmenu.UnlockAllPopup)
            end
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
    levelselectmenu.SetUnlockAll = function(f57_arg0, f57_arg1)
        Engine.SetDvarBool("profileMenuOption_hasUnlockedAll_SP",
            not Engine.GetDvarBool("profileMenuOption_hasUnlockedAll_SP"))
        Engine.SetDvarBool("mis_cheat", not Engine.GetDvarBool("mis_cheat"))
        Engine.ExecNow("profile_menuDvarsFinish")
        Engine.Exec("updategamerprofile")
        local f57_local0 = LUI.FlowManager.GetTopOpenAndVisibleMenuName()
        LUI.FlowManager.RequestAddMenu(nil, f57_local0, true, f57_arg1.controller, true,
            LUI.LevelSelect.FindActData(f57_local0), {
                reload = true
            })
    end
end

