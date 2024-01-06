EnableGlobals()

if not Engine.InFrontend() then
    local gametype = Engine.GetDvarString("ui_gametype")

    if gametype == "twar" then
        require("twar_hud") -- doesn't exist in H1
        -- require("capturing_hud") -- TODO
    end
end

-- hook mphud to use twar hud
local mphud = require("LUI.mp_hud.MPHud")
local init_widgets_og = mphud.initWidgets
mphud.initWidgets = function(hud_widget)
    init_widgets_og(hud_widget) -- call og because we're gonna override it

    if Engine.InFrontend() then
        return
    end

    local gametype = Engine.GetDvarString("ui_gametype")
    if gametype == "twar" then
        --[[
        hud_widget.capture = nil
        hud_widget.capture = mphud.addWidget(hud_widget, LUI.MenuBuilder.buildItems({
            type = "captureHudTwar" -- doesn't exist anymore, need to use new system or patch tf out of it
        }), {
            shakeable = true,
            scalable = true
        })
        ]] --

        hud_widget.twarFlags = mphud.addWidget(hud_widget, LUI.MenuBuilder.buildItems({
            type = "twarFlagsHudDef"
        }), {
            shakeable = true,
            scalable = true
        })
        hud_widget.twarMomentum = mphud.addWidget(hud_widget, LUI.MenuBuilder.buildItems({
            type = "twarMomentumHudDef"
        }), {
            shakeable = true,
            scalable = true
        })
    end
end
