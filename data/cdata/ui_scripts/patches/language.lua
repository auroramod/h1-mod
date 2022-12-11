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
    text = "@MENU_LANGUAGE",
    description = Engine.Localize("@LUA_MENU_CHOOSE_LANGUAGE"),
    callback = function()
        LUI.FlowManager.RequestAddMenu(nil, "choose_language_menu")
    end
})

LUI.MenuBuilder.registerType("choose_language_menu", function(a1)
    local menu = LUI.MenuTemplate.new(a1, {
        menu_title = "@LUA_MENU_CHOOSE_LANGUAGE",
        menu_list_divider_top_offset = -(LUI.H1MenuTab.tabChangeHoldingElementHeight + H1MenuDims.spacing),
        uppercase_title = true
    })

    function createdivider(menu, text)
        local element = LUI.UIElement.new({
            leftAnchor = true,
            rightAnchor = false,
            left = 10,
            right = 0,
            topAnchor = true,
            bottomAnchor = false,
            top = 0,
            bottom = 33.33
        })

        element.scrollingToNext = true
        element:addElement(LUI.MenuBuilder.BuildRegisteredType("h1_option_menu_titlebar", {
            title_bar_text = text
        }))

        menu.list:addElement(element)
    end

    createdivider(menu, "^2" .. Engine.Localize("@LUA_MENU_DOWNLOAD") .. ": ^7https://docs.h1.gg/languages")

    for i = 1, #available_languages do
        if does_zone_folder_exists(available_languages[i]) then
            menu:AddButton(Engine.Localize(string.format("MENU_%s", available_languages[i])), function()
                LUI.yesnopopup({
                    title = Engine.Localize("@MENU_NOTICE"),
                    text = Engine.Localize("MENU_" .. current_language) .. " → " ..
                        Engine.Localize("MENU_" .. available_languages[i]) .. "\n\n" ..
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
    menu:AddBackButton()

    return menu
end)
