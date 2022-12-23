if Engine.InFrontend() then
    -- Disable CP
    Engine.SetDvarInt("ui_enable_cp", 0)

    -- Disable CP store
    Engine.SetDvarInt("ui_show_store", 0)

    -- Remove CoD account button
    if Engine.IsMultiplayer() and CoD.IsCoDAccountRegistrationAvailableInMyRegion() then
        LUI.removemenubutton("pc_controls", 4)
    end
end

-- Remove social button
LUI.MenuBuilder.m_definitions["online_friends_widget"] = function()
    return {
        type = "UIElement"
    }
end
