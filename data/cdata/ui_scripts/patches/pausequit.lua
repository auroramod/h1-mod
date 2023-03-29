if (Engine.InFrontend()) then
    return
end

if game:issingleplayer() and Engine.GetDvarString("mapname") == "coup" then
    LUI.onmenuopen("sp_pause_menu", function(element)
        local menu = element:getFirstChild()
        menu:AddButton("@MENU_SP_SKIP_MISSION", function()
            Engine.Exec( "map blackout" )
        end)
    end)
end

LUI.onmenuopen("sp_pause_menu", function(element)
    local menu = element:getFirstChild()
    menu:AddButton("@MENU_QUIT_TO_DESKTOP", function()
        LUI.FlowManager.RequestAddMenu(nil, "quit_popmenu")
    end)
end)
