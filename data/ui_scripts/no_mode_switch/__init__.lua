LUI.MenuBuilder.m_definitions["main_choose_exe_popup_menu"] = function()
    return {
        type = "generic_yesno_popup",
        id = "main_choose_exe_popup_menu_id",
        properties = {
            popup_title = Engine.Localize("@MENU_NOTICE"),
            message_text = Engine.Localize("@MENU_QUIT_WARNING"),
            yes_action = function()
                Engine.Quit()
            end
        }
    }
end
