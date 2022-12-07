if not Engine.InFrontend() then
    -- No vignette in 'Advanced video' menu
    LUI.MenuTemplate.AddVignette = function(f37_arg0)
        if not LUI.FlowManager.IsMenuTopmost(Engine.GetLuiRoot(), "advanced_video") then
            local f37_local0 = CoD.CreateState(0, 0, 0, 0, CoD.AnchorTypes.All)
            f37_local0.material = RegisterMaterial("h1_ui_bg_vignette")
            local self = LUI.UIImage.new(f37_local0)
            self:setupFullWindowElement()
            f37_arg0:addElement(self)
        end
    end
    -- Less background blur in SP, no blur in MP
    LUI.MenuTemplate.InitInGameBkg = function(f39_arg0, f39_arg1)
        LUI.MenuTemplate.AddDarken(f39_arg0, f39_arg1)
        if game:issingleplayer() and not LUI.FlowManager.IsMenuTopmost(Engine.GetLuiRoot(), "advanced_video") then
            LUI.MenuTemplate.AddWorldBlur(f39_arg0, f39_arg1)
        end
    end
end
