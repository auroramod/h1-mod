local mphud = luiglobals.require("LUI.mp_hud.MPHud")

function createinfobar()
    local infobar = LUI.UIElement.new({
        left = luiglobals.GameX.IsHardcoreMode() and 160 or 228,
        top = luiglobals.GameX.IsHardcoreMode() and 5 or 9,
        height = 15,
        width = 70,
        leftAnchor = true,
        topAnchor = true
    })

    infobar:registerAnimationState("hud_on", {
        alpha = 1
    })

    infobar:registerAnimationState("hud_off", {
        alpha = 0
    })

    return infobar
end

function populateinfobar(infobar)
    elementoffset = 0

    if (Engine.GetDvarBool("cg_infobar_fps")) then
        infobar:addElement(infoelement({
            label = "FPS: ",
            getvalue = function()
                return game:getfps()
            end,
            width = 70,
            interval = 100
        }))
    end

    if (Engine.GetDvarBool("cg_infobar_ping")) then
        infobar:addElement(infoelement({
            label = "Latency: ",
            getvalue = function()
                return game:getping() .. " ms"
            end,
            width = 115,
            interval = 100
        }))
    end
end

function infoelement(data)
    local container = LUI.UIElement.new({
        bottomAnchor = true,
        leftAnchor = true,
        topAnchor = true,
        width = data.width,
        left = elementoffset
    })

    elementoffset = elementoffset + data.width + 10

    local background = LUI.UIImage.new({
        bottomAnchor = true,
        leftAnchor = true,
        topAnchor = true,
        rightAnchor = true,
        material = luiglobals.RegisterMaterial("white"),
        color = luiglobals.Colors.black,
        alpha = 0.5
    })

    local labelfont = CoD.TextSettings.FontBold110
    local textheight = 13

    local label = LUI.UIText.new({
        left = 5,
        top = 1,
        font = labelfont.Font,
        height = textheight,
        leftAnchor = true,
        topAnchor = true,
        color = {
            r = 0.8,
            g = 0.8,
            b = 0.8,
        }
    })

    label:setText(data.label)

    local _, _, left = luiglobals.GetTextDimensions(data.label, labelfont.Font, textheight)
    local value = LUI.UIText.new({
        left = left + 5,
        top = 1,
        height = textheight,
        leftAnchor = true,
        topAnchor = true,
        color = {
            r = 0.6,
            g = 0.6,
            b = 0.6,
        }
    })
    
    value:addElement(LUI.UITimer.new(data.interval, "update"))
    value:setText(data.getvalue())
    value:addEventHandler("update", function()
        value:setText(data.getvalue())
    end)
    
    container:addElement(background)
    container:addElement(label)
    container:addElement(value)
    
    return container
end

local updatehudvisibility = mphud.updateHudVisibility
mphud.updateHudVisibility = function(a1, a2)
    updatehudvisibility(a1, a2)

    local root = Engine.GetLuiRoot()
    local menus = root:AnyActiveMenusInStack()
    local infobar = root.infobar

    if (menus) then
        infobar:animateToState("hud_off")
    else
        infobar:animateToState("hud_on")
    end
end

LUI.onmenuopen("mp_hud", function(hud)
    if (Engine.InFrontend()) then
        return
    end

    local infobar = createinfobar()
    local root = Engine.GetLuiRoot()
    root.infobar = infobar
    populateinfobar(infobar)

    root:registerEventHandler("update_hud_infobar_settings", function()
        infobar:removeAllChildren()
        populateinfobar(infobar)
    end)

    root:processEvent({
        name = "update_hud_infobar_settings"
    })
    
    hud.static:addElement(infobar)
end)