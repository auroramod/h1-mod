local available_languages = {"english", "english_safe", "french", "german", "italian", "polish", "portuguese",
                             "russian", "spanish", "simplified_chinese", "traditional_chinese", "japanese_partial",
                             "korean"}
local current_language = "LANGUAGE"

LUI.UIButtonText.IsOffsetedLanguage = function()
    return false
end

function get_user_language()
    user_language = game:getcurrentgamelanguage()
end

function set_language(value)
    local file_path = "players2/default/language"
    local file = io.open(file_path, "w")
    file:write(value)
    file:close()
end

function does_zone_folder_exists(language)
    return io.directoryexists("zone/" .. language)
end

get_user_language()

if user_language ~= "" and does_zone_folder_exists(user_language) then
    current_language = user_language
end

LUI.addmenubutton("pc_controls", {
    index = 4,
    text = "LUA_MENU_CHOOSE_LANGUAGE",
    description = Engine.Localize("LUA_MENU_CHOOSE_LANGUAGE_DESC"),
    callback = function()
        LUI.FlowManager.RequestAddMenu(nil, "choose_language_menu")
    end
})

LUI.MenuBuilder.registerType("choose_language_menu", function(a1)
    local menu = LUI.MenuTemplate.new(a1, {
        menu_title = "LUA_MENU_CHOOSE_LANGUAGE",
        menu_list_divider_top_offset = -(LUI.H1MenuTab.tabChangeHoldingElementHeight + H1MenuDims.spacing),
        uppercase_title = true
    })

    for i = 1, #available_languages do
        if does_zone_folder_exists(available_languages[i]) then
            menu:AddButton(Engine.Localize(string.format("MENU_%s", available_languages[i])), function()
                LUI.yesnopopup({
                    title = Engine.Localize("@MENU_NOTICE"),
                    text = "^2" .. Engine.Localize("MENU_" .. current_language) .. "^7 → ^5" ..
                        Engine.Localize("MENU_" .. available_languages[i]) .. "\n\n^7" ..
                        Engine.Localize("@LUA_MENU_CONFIRM_LANGUAGE") .. " " ..
                        Engine.Localize("@MENU_APPLY_LANGUAGE_SETTINGS"),
                    callback = function(result)
                        if (result) then
                            set_language(available_languages[i])
                            updater.relaunch()
                        else
                            LUI.FlowManager.RequestLeaveMenu(popup)
                        end
                    end
                })
            end, available_languages[i] == current_language, true, nil, {
                desc_text = Engine.Localize("LOCALE_" .. (available_languages[i]))
            })
        end
    end

    LUI.Options.InitScrollingList(menu.list, nil, {
        rows = 12
    })

    LUI.Options.AddOptionTextInfo(menu)

    menu:AddHelp({
        name = "add_button_helper_text",
        button_ref = "",
        helper_text = "^2" .. Engine.Localize("@LUA_MENU_DOWNLOAD") .. ": ^7https://docs.h1.gg/languages",
        side = "left",
        priority = -9001,
        clickable = false
    })

    menu:AddBackButton()

    return menu
end)

-- fix for ammo zeros
if not Engine.InFrontend() then
    local weaponinfodef = LUI.MenuBuilder.m_definitions["WeaponInfoHudDef"]
    LUI.MenuBuilder.m_definitions["WeaponInfoHudDef"] = function(...)
        Engine.GetCurrentLanguage = function()
            return 0
        end
        local res = weaponinfodef(...)
        Engine.GetCurrentLanguage = function()
            lang = Engine.GetDvarString("loc_language")
            return lang
        end
        return res
    end
end

--[[
-- H2 title font
if game:issingleplayer() or
    (user_language == "simplified_chinese" or user_language == "traditional_chinese" or user_language ==
        "japanese_partial" or user_language == "korean") then
    return
end

local scale = function(size)
    return size * 720 / 1080
end

CoD.TextSettings.H1TitleFont = {
    Font = RegisterFont("fonts/bank_h1.ttf", 50),
    Height = scale(50)
}
]]--
