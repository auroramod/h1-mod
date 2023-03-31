if (Engine.InFrontend()) then
    return
end

if game:issingleplayer() and Engine.GetDvarString("mapname") == "coup" then
    LUI.onmenuopen("sp_pause_menu", function(element)
        local menu = element:getFirstChild()
        menu:AddButton("@MENU_SP_SKIP_MISSION", function()
            Engine.Exec("map blackout")
        end)
    end)
end

if game:issingleplayer() then
    LUI.onmenuopen("sp_pause_menu", function(element)
        local menu = element:getFirstChild()
        menu:AddButton("@MENU_QUIT_TO_DESKTOP", function()
            LUI.FlowManager.RequestAddMenu(nil, "quit_popmenu")
        end)
    end)
end

if not game:issingleplayer() then
    local quitToDesktop = function()
        LUI.FlowManager.RequestAddMenu(nil, "quit_popmenu")
    end
    local addQuitButton = function(element)
        local menu = element
        menu:AddButton("@MENU_QUIT_TO_DESKTOP", quitToDesktop)
    end
    LUI.onmenuopen("mp_pause_menu", addQuitButton)
end
