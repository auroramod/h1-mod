local Lobby = luiglobals.Lobby
local MPLobbyOnline = LUI.mp_menus.MPLobbyOnline

function LeaveLobby()
    LeaveXboxLive()
    if Lobby.IsInPrivateParty() == false or Lobby.IsPrivatePartyHost() then
        LUI.FlowManager.RequestLeaveMenuByName("menu_xboxlive")
        Engine.ExecNow("clearcontrollermap")
    end
end

function menu_xboxlive(f16_arg0)
    local menu = LUI.MPLobbyBase.new(f16_arg0, {
        menu_title = "@PLATFORM_UI_HEADER_PLAY_MP_CAPS",
        memberListState = Lobby.MemberListStates.Prelobby
    })

    menu:setClass(LUI.MPLobbyOnline)

    local serverListButton = menu:AddButton("@LUA_MENU_SERVERLIST", function(a1)
        LUI.FlowManager.RequestAddMenu(a1, "menu_systemlink_join", true, nil)
    end)
    serverListButton:setDisabledRefreshRate(500)
    if Engine.IsCoreMode() then
        menu:AddCACButton()
        menu:AddBarracksButton()
        menu:AddPersonalizationButton()
        menu:AddDepotButton()

        menu:AddButton("@MENU_MODS", function(a1)
            LUI.FlowManager.RequestAddMenu(a1, "mods_menu", true, nil)
        end)
    end

    local privateMatchButton = menu:AddButton("@MENU_PRIVATE_MATCH", MPLobbyOnline.OnPrivateMatch,
        MPLobbyOnline.disablePrivateMatchButton)
    privateMatchButton:rename("menu_xboxlive_private_match")
    privateMatchButton:setDisabledRefreshRate(500)
    if not Engine.IsCoreMode() then
        local leaderboardButton = menu:AddButton("@LUA_MENU_LEADERBOARD", "OpLeaderboardMain")
        leaderboardButton:rename("OperatorMenu_leaderboard")
    end

    menu:AddOptionsButton()
    local natType = Lobby.GetNATType()
    if natType then
        local natTypeText = Engine.Localize("NETWORK_YOURNATTYPE", natType)
        local properties = CoD.CreateState(nil, nil, 2, -62, CoD.AnchorTypes.BottomRight)
        properties.width = 250
        properties.height = CoD.TextSettings.BodyFontVeryTiny.Height
        properties.font = CoD.TextSettings.BodyFontVeryTiny.Font
        properties.color = luiglobals.Colors.white
        properties.alpha = 0.25
        local self = LUI.UIText.new(properties)
        self:setText(natTypeText)
        menu:addElement(self)
    end

    menu:AddMenuDescription(1)
    menu:AddMarketingPanel(LUI.MarketingLocation.Featured, LUI.ComScore.ScreenID.PlayOnline)
    menu.isSignInMenu = true
    menu:registerEventHandler("gain_focus", LUI.MPLobbyOnline.OnGainFocus)
    menu:registerEventHandler("player_joined", luiglobals.Cac.PlayerJoinedEvent)
    menu:registerEventHandler("exit_live_lobby", LeaveLobby)

    if Engine.IsCoreMode() then
        Engine.ExecNow("eliteclan_refresh", Engine.GetFirstActiveController())
    end

    local root = Engine.GetLuiRoot()
    if (root.vltimer) then
        root.vltimer:close()
    end

    root.vltimer = LUI.UITimer.new(4000, "vl")
    root:addElement(root.vltimer)
    root:registerEventHandler("vl", function()
        if (Engine.GetDvarBool("virtualLobbyReady")) then
            root.vltimer:close()
            game:virtuallobbypresentable()
        end
    end)

    menu:AddHelp({
        name = "add_button_helper_text",
        button_ref = "",
        helper_text = "                                                                                                ",
        side = "left",
        priority = -9000,
        clickable = false
    })

    return menu
end

LUI.MenuBuilder.m_types_build["menu_xboxlive"] = menu_xboxlive