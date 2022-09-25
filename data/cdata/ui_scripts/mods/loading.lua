function createdivider(menu, text)
    local element = LUI.UIElement.new({
        leftAnchor = true,
        rightAnchor = true,
        left = 0,
        right = 0,
        topAnchor = true,
        bottomAnchor = false,
        top = 0,
        bottom = 33.33
    })

    element.scrollingToNext = true
    element:addElement(LUI.MenuBuilder.BuildRegisteredType("h1_option_menu_titlebar", {
        title_bar_text = Engine.ToUpperCase(text)
    }))

    element.text = element:getFirstChild():getFirstChild():getNextSibling()

    menu.list:addElement(element)
    return element
end

function string:truncate(length)
    if (#self <= length) then
        return self
    end

    return self:sub(1, length - 3) .. "..."
end

if (game:issingleplayer()) then
    LUI.addmenubutton("main_campaign", {
        index = 6,
        text = "@MENU_MODS",
        description = Engine.Localize("@MENU_MODS_DESC"),
        callback = function()
            LUI.FlowManager.RequestAddMenu(nil, "mods_menu")
        end
    })
end

function getmodname(path)
    local name = path
    game:addlocalizedstring(name, name)
    local desc = Engine.Localize("LUA_MENU_MOD_DESC_DEFAULT", name)
    local infofile = path .. "/info.json"

    if (io.fileexists(infofile)) then
        pcall(function()
            local data = json.decode(io.readfile(infofile))
            game:addlocalizedstring(data.description, data.description)
            game:addlocalizedstring(data.author, data.author)
            game:addlocalizedstring(data.version, data.version)
            desc = Engine.Localize("@LUA_MENU_MOD_DESC", data.description, data.author, data.version)
            name = data.name
        end)
    end

    return name, desc
end

LUI.MenuBuilder.registerType("mods_menu", function(a1)
    local menu = LUI.MenuTemplate.new(a1, {
        menu_title = "@MENU_MODS",
        exclusiveController = 0,
        menu_width = 400,
        menu_top_indent = LUI.MenuTemplate.spMenuOffset,
        showTopRightSmallBar = true,
        uppercase_title = true
    })

    local modfolder = game:getloadedmod()
    if (modfolder ~= "") then
        local name = getmodname(modfolder)
        createdivider(menu, Engine.Localize("@LUA_MENU_LOADED_MOD", name:truncate(24)))

        menu:AddButton("@LUA_MENU_UNLOAD", function()
            Engine.Exec("unloadmod")
        end, nil, true, nil, {
            desc_text = Engine.Localize("@LUA_MENU_UNLOAD_DESC")
        })
    end

    createdivider(menu, Engine.Localize("@LUA_MENU_AVAILABLE_MODS"))

    if (io.directoryexists("mods")) then
        local mods = io.listfiles("mods/")
        for i = 1, #mods do
            if (io.directoryexists(mods[i]) and not io.directoryisempty(mods[i])) then
                local name, desc = getmodname(mods[i])

                if (mods[i] ~= modfolder) then
                    game:addlocalizedstring(name, name)
                    menu:AddButton(name, function()
                        Engine.Exec("loadmod " .. mods[i])
                    end, nil, true, nil, {
                        desc_text = desc
                    })
                end
            end
        end
    end

    menu:AddBackButton(function(a1)
        Engine.PlaySound(CoD.SFX.MenuBack)
        LUI.FlowManager.RequestLeaveMenu(a1)
    end)

    LUI.Options.InitScrollingList(menu.list, nil)
    menu:CreateBottomDivider()
    menu.optionTextInfo = LUI.Options.AddOptionTextInfo(menu)

    return menu
end)
