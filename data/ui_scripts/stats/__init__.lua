if (game:issingleplayer()) then
	return
end

game:addlocalizedstring("LUA_MENU_STATS", "Stats")
game:addlocalizedstring("LUA_MENU_STATS_DESC", "Edit player stats settings.")

game:addlocalizedstring("LUA_MENU_UNLOCKALL_ITEMS", "Unlock all items")
game:addlocalizedstring("LUA_MENU_UNLOCKALL_ITEMS_DESC", "Whether items should be locked based on the player's stats or always unlocked.")

game:addlocalizedstring("LUA_MENU_UNLOCKALL_CLASSES", "Unlock all classes")
game:addlocalizedstring("LUA_MENU_UNLOCKALL_CLASSES_DESC", "Whether classes should be locked based on the player's stats or always unlocked.")

game:addlocalizedstring("LUA_MENU_PRESTIGE", "Prestige")
game:addlocalizedstring("LUA_MENU_PRESTIGE_DESC", "Edit prestige level.")
game:addlocalizedstring("LUA_MENU_RANK", "Rank")
game:addlocalizedstring("LUA_MENU_RANK_DESC", "Edit rank.")

game:addlocalizedstring("LUA_MENU_UNSAVED_CHANGES", "You have unsaved changes, are you sure you want to exit?")
game:addlocalizedstring("LUA_MENU_SAVE", "Save changes")
game:addlocalizedstring("LUA_MENU_SAVE_DESC", "Save changes.")
game:addlocalizedstring("LUA_MENU_SETTINGS", "Settings")
game:addlocalizedstring("LUA_MENU_EDIT_STATS", "Edit Stats")

function createdivider(menu, text)
	local element = LUI.UIElement.new( {
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
		menu_width = luiglobals.GenericMenuDims.OptionMenuWidth,
	})

	createdivider(menu, "@LUA_MENU_SETTINGS")

	LUI.Options.CreateOptionButton(
		menu, 
		"cg_unlockall_items", 
		"@LUA_MENU_UNLOCKALL_ITEMS", 
		"@LUA_MENU_UNLOCKALL_ITEMS_DESC", 
		{
			{
				text = "@LUA_MENU_ENABLED",
				value = true
			},
			{
				text = "@LUA_MENU_DISABLED",
				value = false
			}
		},
		nil,
		nil
	)

	LUI.Options.CreateOptionButton(
		menu, 
		"cg_unlockall_classes", 
		"@LUA_MENU_UNLOCKALL_CLASSES", 
		"@LUA_MENU_UNLOCKALL_CLASSES_DESC", 
		{
			{
				text = "@LUA_MENU_ENABLED",
				value = true
			},
			{
				text = "@LUA_MENU_DISABLED",
				value = false
			}
		},
		nil,
		nil
	)

	createdivider(menu, "@LUA_MENU_EDIT_STATS")

	local prestige = Engine.GetPlayerData(0, CoD.StatsGroup.Ranked, "prestige") or 0
	local experience = Engine.GetPlayerData(0, CoD.StatsGroup.Ranked, "experience") or 0
	local rank = luiglobals.Lobby.GetRankForXP(experience, prestige)

	local saved = true
	local prestigevalue = prestige
	local rankvalue = rank
	local rankbutton = nil

	prestigeeditbutton(menu, function(value)
		prestigevalue = value
		saved = false
	end)

	rankbutton = rankeditbutton(menu, function(value)
		rankvalue = value
		saved = false
	end)

	local savebutton = menu:AddButton("@LUA_MENU_SAVE", function()
		Engine.SetPlayerData(0, CoD.StatsGroup.Ranked, "prestige", tonumber(prestigevalue))

		local rank = tonumber(rankvalue)
		local prestige = Engine.GetPlayerData(0, CoD.StatsGroup.Ranked, "prestige") or 0
		local experience = rank == 0 and 0 or luiglobals.Rank.GetRankMaxXP(tonumber(rankvalue) - 1, prestige)

		Engine.SetPlayerData(0, CoD.StatsGroup.Ranked, "experience", experience)

		saved = true
	end, nil, nil, nil, {
		desc_text = Engine.Localize("LUA_MENU_SAVE_DESC")
	})

	LUI.Options.InitScrollingList(menu.list, nil)
	LUI.Options.AddOptionTextInfo(menu)

	menu:AddBackButton(function()
		if (saved) then
			LUI.FlowManager.RequestLeaveMenu(menu)
			return
		end

		LUI.yesnopopup({
			title = Engine.Localize("@MENU_NOTICE"),
			text = Engine.Localize("@LUA_MENU_UNSAVED_CHANGES"),
			callback = function(result)
				if (result) then
					LUI.FlowManager.RequestLeaveMenu(menu)
				end
			end
		})
	end)

	return menu
end)

function prestigeeditbutton(menu, callback)
	local options = {}
	local max = luiglobals.Lobby.GetMaxPrestigeLevel()
	local prestige = Engine.GetPlayerData(0, CoD.StatsGroup.Ranked, "prestige") or 0

	for i = 0, max do
		game:addlocalizedstring("LUA_MENU_" .. i, i .. "")

		table.insert(options, {
			text = "@" .. i,
			value = i .. ""
		})
	end

	Engine.SetDvarFromString("ui_prestige_level", prestige .. "")

	LUI.Options.CreateOptionButton(
		menu, 
		"ui_prestige_level", 
		"@LUA_MENU_PRESTIGE", 
		"@LUA_MENU_PRESTIGE_DESC", 
		options,
		nil,
		nil,
		callback
	)
end

function rankeditbutton(menu, callback)
	local options = {}
	local prestige = Engine.GetPlayerData(0, CoD.StatsGroup.Ranked, "prestige") or 0
	local experience = Engine.GetPlayerData(0, CoD.StatsGroup.Ranked, "experience") or 0

	local rank = luiglobals.Lobby.GetRankForXP(experience, prestige)
	local max = luiglobals.Rank.GetMaxRank(prestige)
	local maxprestige = luiglobals.Lobby.GetMaxPrestigeLevel()

	for i = 0, max do
		game:addlocalizedstring("LUA_MENU_" .. i, i .. "")

		table.insert(options, {
			text = "@" .. (i + 1),
			value = i .. ""
		})
	end

	Engine.SetDvarFromString("ui_rank_level_", rank .. "")

	return LUI.Options.CreateOptionButton(
		menu, 
		"ui_rank_level_", 
		"@LUA_MENU_RANK", 
		"@LUA_MENU_RANK_DESC", 
		options,
		nil,
		nil,
		callback
	)
end

local isclasslocked = luiglobals.Cac.IsCustomClassLocked
luiglobals.Cac.IsCustomClassLocked = function(...)
	if (Engine.GetDvarBool("cg_unlockall_classes")) then
		return false
	end

	return isclasslocked(table.unpack({...}))
end
