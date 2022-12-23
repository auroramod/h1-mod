if (Engine.InFrontend()) then
    return
end

-- less background blur in SP, no blur in MP
LUI.MenuTemplate.InitInGameBkg = function(f39_arg0, f39_arg1)
    LUI.MenuTemplate.AddDarken(f39_arg0, f39_arg1)
    if game:issingleplayer() and not LUI.FlowManager.IsMenuTopmost(Engine.GetLuiRoot(), "advanced_video") then
        LUI.MenuTemplate.AddWorldBlur(f39_arg0, f39_arg1)
    end
end
