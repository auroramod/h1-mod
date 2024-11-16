local lookcontrols = luiglobals.require("LUI.LookControls")

local function get_sensitivity()
    return (Engine.GetDvarFloat("sensitivity") - SliderBounds.MouseSensitivity.Min) /
               (SliderBounds.MouseSensitivity.Max - SliderBounds.MouseSensitivity.Min)
end

local function change_sens(value)
    Engine.SetDvarFloat("sensitivity", math.min(SliderBounds.MouseSensitivity.Max, math.max(
        SliderBounds.MouseSensitivity.Min, Engine.GetDvarFloat("sensitivity") + value)))
end

local create_options_og = lookcontrols.CreateOptions
lookcontrols.CreateOptions = function(menu)
    LUI.Options.AddButtonOptionVariant(menu, GenericButtonSettings.Variants.Slider, "@MENU_MOUSE_SENSITIVITY",
        "@MENU_MOUSE_SENSITIVITY_DESC", get_sensitivity, function() -- less
            change_sens(-SliderBounds.MouseSensitivity.Step)
        end, function() -- more
            change_sens(SliderBounds.MouseSensitivity.Step)
        end)

    -- not much code to even reimplement, so just paste it all
    LUI.Options.CreateOptionButton(menu, "ui_mousePitch", "@MENU_INVERT_MOUSE", "@MENU_INVERT_MOUSE_DESC",
        lookcontrols.GetPitchValues(), lookcontrols.UpdateMousePitch)
    LUI.Options.CreateOptionButton(menu, "m_filter", "@MENU_SMOOTH_MOUSE", "@MENU_SMOOTH_MOUSE_DESC", {{
        text = "@LUA_MENU_YES",
        value = true
    }, {
        text = "@LUA_MENU_NO",
        value = false
    }})
    LUI.Options.CreateOptionButton(menu, "cl_freelook", "@MENU_FREE_LOOK", "@MENU_FREE_LOOK_DESC", {{
        text = "@LUA_MENU_YES",
        value = true
    }, {
        text = "@LUA_MENU_NO",
        value = false
    }})
    LUI.Options.CreateControlBindButton(menu, "@MENU_LEAN_LEFT", "@MENU_LEAN_LEFT_DESC", "+leanleft")
    LUI.Options.CreateControlBindButton(menu, "@MENU_LEAN_RIGHT", "@MENU_LEAN_RIGHT_DESC", "+leanright")
    LUI.Options.CreateControlBindButton(menu, "@MENU_LOOK_UP", "@MENU_LOOK_UP_DESC", "+lookup")
    LUI.Options.CreateControlBindButton(menu, "@MENU_LOOK_DOWN", "@MENU_LOOK_DOWN_DESC", "+lookdown")
    LUI.Options.CreateControlBindButton(menu, "@MENU_HOLD_MOUSE_LOOK", "@MENU_HOLD_MOUSE_LOOK_DESC", "+mlook")
    LUI.Options.CreateControlBindButton(menu, "@MENU_CENTER_VIEW", "@MENU_CENTER_VIEW_DESC", "centerview")
    LUI.Options.InitScrollingList(menu.list, nil)
end
