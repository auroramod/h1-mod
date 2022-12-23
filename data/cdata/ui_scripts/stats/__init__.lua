if (game:issingleplayer() or not Engine.InFrontend()) then
    return
end

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
        title_bar_text = Engine.ToUpperCase(Engine.Localize(text))
    }))

    menu.list:addElement(element)
end

local personalizationbutton = LUI.MPLobbyBase.AddPersonalizationButton
LUI.MPLobbyBase.AddPersonalizationButton = function(menu)
    personalizationbutton(menu)
    menu:AddButton("@LUA_MENU_STATS", function()
        LUI.FlowManager.RequestAddMenu(nil, "stats_menu")
    end)
end

LUI.MenuBuilder.registerType("stats_menu", function(a1)
    local menu = LUI.MenuTemplate.new(a1, {
        menu_title = Engine.ToUpperCase(Engine.Localize("@LUA_MENU_STATS")),
        menu_width = luiglobals.GenericMenuDims.OptionMenuWidth
    })

    createdivider(menu, "LUA_MENU_SETTINGS")

    LUI.Options.CreateOptionButton(menu, "cg_unlockall_items", "@LUA_MENU_UNLOCKALL_ITEMS",
        "@LUA_MENU_UNLOCKALL_ITEMS_DESC", {{
            text = "@LUA_MENU_ENABLED",
            value = true
        }, {
            text = "@LUA_MENU_DISABLED",
            value = false
        }}, nil, nil)

    LUI.Options.CreateOptionButton(menu, "cg_unlockall_loot", "@LUA_MENU_UNLOCKALL_LOOT",
        "@LUA_MENU_UNLOCKALL_LOOT_DESC", {{
            text = "@LUA_MENU_ENABLED",
            value = true
        }, {
            text = "@LUA_MENU_DISABLED",
            value = false
        }}, nil, nil)

    LUI.Options.CreateOptionButton(menu, "cg_unlockall_classes", "@LUA_MENU_UNLOCKALL_CLASSES",
        "@LUA_MENU_UNLOCKALL_CLASSES_DESC", {{
            text = "@LUA_MENU_ENABLED",
            value = true
        }, {
            text = "@LUA_MENU_DISABLED",
            value = false
        }}, nil, nil)

    createdivider(menu, "LUA_MENU_EDIT_STATS")

    local prestige = Engine.GetPlayerData(0, CoD.StatsGroup.Ranked, "prestige") or 0
    local experience = Engine.GetPlayerData(0, CoD.StatsGroup.Ranked, "experience") or 0
    local rank = Lobby.GetRankForXP(experience, prestige)

    prestigeeditbutton(menu, function(value)
        Engine.SetPlayerData(0, CoD.StatsGroup.Ranked, "prestige", tonumber(value))
    end)

    rankeditbutton(menu, function(value)
        local rank = tonumber(value)
        local prestige = Engine.GetPlayerData(0, CoD.StatsGroup.Ranked, "prestige") or 0
        local experience = rank == 0 and 0 or Rank.GetRankMaxXP(tonumber(value) - 1, prestige)

        Engine.SetPlayerData(0, CoD.StatsGroup.Ranked, "experience", experience)
    end)

    LUI.Options.InitScrollingList(menu.list, nil)
    LUI.Options.AddOptionTextInfo(menu)

    menu:AddBackButton()

    return menu
end)

function prestigeeditbutton(menu, callback)
    local options = {}
    local max = Lobby.GetMaxPrestigeLevel()
    local prestige = Engine.GetPlayerData(0, CoD.StatsGroup.Ranked, "prestige") or 0

    for i = 0, max do
        game:addlocalizedstring("LUA_MENU_" .. i, i .. "")

        table.insert(options, {
            text = "@" .. i,
            value = i .. ""
        })
    end

    Engine.SetDvarFromString("ui_prestige_level", prestige .. "")

    LUI.Options.CreateOptionButton(menu, "ui_prestige_level", "@LUA_MENU_PRESTIGE", "@LUA_MENU_PRESTIGE_DESC", options,
        nil, nil, callback)
end

function rankeditbutton(menu, callback)
    local options = {}
    local prestige = Engine.GetPlayerData(0, CoD.StatsGroup.Ranked, "prestige") or 0
    local experience = Engine.GetPlayerData(0, CoD.StatsGroup.Ranked, "experience") or 0

    local rank = Lobby.GetRankForXP(experience, prestige)
    local max = Rank.GetMaxRank(prestige)
    local maxprestige = Lobby.GetMaxPrestigeLevel()

    for i = 0, max do
        game:addlocalizedstring("LUA_MENU_" .. i, i .. "")

        table.insert(options, {
            text = "@" .. (i + 1),
            value = i .. ""
        })
    end

    Engine.SetDvarFromString("ui_rank_level_", rank .. "")

    return LUI.Options.CreateOptionButton(menu, "ui_rank_level_", "@LUA_MENU_RANK", "@LUA_MENU_RANK_DESC", options, nil,
        nil, callback)
end

local isclasslocked = Cac.IsCustomClassLocked
Cac.IsCustomClassLocked = function(...)
    if (Engine.GetDvarBool("cg_unlockall_classes")) then
        return false
    end

    return isclasslocked(...)
end
