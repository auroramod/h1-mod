EnableGlobals()

local height = 64
local width = 64
local f0_local2 = 10 -- idk
local f0_local3 = 10
local f0_local4 = width
local f0_local5 = 40
local f0_local6 = 300
local f0_local7 = 4
local f0_local8 = 3
local twar_flag_hud_table = {}

local custom_friendly_color = Colors.s1.team1_mlg
custom_friendly_color.r = 0

local function get_omnvar_wrapper(omnvar, is_float)
    is_float = is_float or false
    if is_float then
        return Engine.GetDvarFloat(omnvar)
    end
    return Engine.GetDvarInt(omnvar)
end

local render_twar_flag_hud = function(element)
    local flag_index = 0
    local twar_flag_element = element:getFirstDescendentById("twar_flag_0")
    while twar_flag_element do
        element:removeElement(twar_flag_element)
        flag_index = flag_index + 1
        twar_flag_element = element:getFirstDescendentById("twar_flag_" .. flag_index)
    end
    twar_flag_hud_table = {}
    local friendly_flag = RegisterMaterial("hud_esports_flag_blue")
    local enemy_flag = RegisterMaterial("hud_esports_flag_red")
    local neutral_flag = RegisterMaterial("hud_esports_flag")
    local flag_count_value = get_omnvar_wrapper("ui_twar_flag_count")
    local f1_local6 = flag_count_value * width + (flag_count_value - 1) * f0_local2
    for index = 0, flag_count_value - 1, 1 do
        local self = LUI.UIImage.new()
        self.id = "twar_flag_" .. index
        local top_offset = 10
        local right_offset = -1 * f1_local6 / 2 + index * (width + f0_local2) + width
        twar_flag_hud_table[index + 1] = {
            top = top_offset,
            midX = right_offset - (width * 0.5)
        }
        self:registerAnimationState("neutral", {
            topAnchor = true,
            top = top_offset,
            right = right_offset,
            height = height,
            width = width,
            alpha = 0
        })
        self:registerAnimationState("enemy", {
            material = enemy_flag,
            alpha = 1
        })
        self:registerAnimationState("friendly", {
            material = friendly_flag,
            alpha = 1
        })
        self:animateToState("neutral", 0)
        element:addElement(self)
    end
end

local ui_create_bar = function(f2_arg0, f2_arg1, f2_arg2, f2_arg3, f2_arg4, f2_arg5, f2_arg6, f2_arg7)
    local self = LUI.UIElement.new(CoD.CreateState(f2_arg1, f2_arg2, f2_arg1 + f2_arg3, f2_arg2 + f2_arg4,
        CoD.AnchorTypes.TopLeft))
    f2_arg0:addElement(self)
    local f2_local1 = CoD.CreateState(0, 0, 0, 0, CoD.AnchorTypes.All)
    f2_local1.color = f2_arg5
    f2_local1.alpha = f2_arg5.a or 0.5
    self:addElement(LUI.UIImage.new(f2_local1))
    local f2_local2 = CoD.CreateState(0, 0, 0, 0, CoD.AnchorTypes.TopBottomLeft)
    f2_local2.color = f2_arg6
    local f2_local3 = LUI.UIImage.new(f2_local2)
    if not f2_local3.properties then
        f2_local3.properties = {}
    end
    f2_local3.properties.totalWidth = f2_arg3
    self:addElement(f2_local3)
    if f2_arg7 then
        local f2_local4 = 16
        local f2_local5 = LUI.UIImage.new({
            topAnchor = true,
            rightAnchor = true,
            top = -f2_local4 - 2,
            right = f2_local4 * 0.5,
            width = f2_local4,
            height = f2_local4,
            material = RegisterMaterial("hud_momentum_arrow"),
            alpha = 1
        })
        f2_local3:addElement(f2_local5)
        f2_local5:registerAnimationState("hidden", {
            alpha = 0
        })
        f2_local5:registerEventHandler("momentum_bar_marker_visible", function(element, event)
            if event.visible then
                element:animateToState("default")
            else
                element:animateToState("hidden")
            end
        end)
        f2_local5:registerEventHandler("momentum_bar_marker_color_update", function(element, event)
            if event.color then
                element:setRGB(event.color.r, event.color.g, event.color.b)
            end
        end)
    end
    f2_local3:registerEventHandler("momentumFillBarProgressUpdate", function(element, event)
        local f5_local0 = event.percent or 1
        if f5_local0 < 0 then
            f5_local0 = 0
        elseif f5_local0 > 1 then
            f5_local0 = 1
        end
        local f5_local1 = CoD.CreateState(0, 0, element.properties.totalWidth * event.percent, 0,
            CoD.AnchorTypes.TopBottomLeft)
        f5_local1.color = event.fillColor
        local f5_local2 = event.duration or 0
        element:registerAnimationState("fill", f5_local1)
        element:animateToState("fill", f5_local2)
    end)
    f2_local3:registerEventHandler("momentumFillBarColorUpdate", function(element, event)
        element:setRGB(event.color.r, event.color.g, event.color.b)
    end)
    return self
end

local function update_current_flag(element)
    local f7_local0 = 80
    local f7_local1 = width
    local f7_local2 = 3
    local f7_local3 = f7_local0 * 0.8
    local f7_local4 = math.ceil(#twar_flag_hud_table * 0.5)
    local self = LUI.UIElement.new(CoD.CreateState(twar_flag_hud_table[f7_local4].midX,
        twar_flag_hud_table[f7_local4].top, 0, 0, CoD.AnchorTypes.Top))

    element.m_neutralFlagWidget = self
    element:addElement(self)

    self:registerEventHandler("momentum_neutral_flag_change", function(element, event)
        local f8_local0 = twar_flag_hud_table[event.newFlagIndex + 1]
        element:registerAnimationState("default",
            CoD.CreateState(f8_local0.midX, f8_local0.top, 0, 0, CoD.AnchorTypes.Top))
        element:animateToState("default")
    end)

    element.m_progressBar = ui_create_bar(self, -f7_local3 * 0.5, 8, f7_local3, f7_local2, {
        r = 0.6,
        g = 0.6,
        b = 0.6
    }, Colors.s1.team1)

    local f7_local6 = 4
    local neutral_flag_elem = CoD.CreateState(-f7_local1 * 0.5 + f7_local6, 0, f7_local1 * 0.5 + f7_local6, f7_local1,
        CoD.AnchorTypes.TopLeft)

    neutral_flag_elem.material = RegisterMaterial("hud_esports_flag")
    self:addElement(LUI.UIImage.new(neutral_flag_elem))
    local tiny_font = CoD.TextSettings.BodyFontTiny
    local f7_local9 = 18
    local f7_local10 = 17
    local f7_local11 = -16
    local f7_local12 = f7_local11 - f7_local0 * 0.5
    local friendly_text_state = CoD.CreateState(f7_local12, f7_local10, f7_local11, f7_local10 + f7_local9,
        CoD.AnchorTypes.TopLeft)
    friendly_text_state.font = tiny_font.Font
    friendly_text_state.color = custom_friendly_color
    friendly_text_state.alignment = LUI.Alignment.Right

    local m_teamCountFriendly = LUI.UIText.new(friendly_text_state)
    m_teamCountFriendly:setTextStyle(CoD.TextStyle.Outlined)
    m_teamCountFriendly:setText("0")
    self:addElement(m_teamCountFriendly)
    element.m_teamCountFriendly = m_teamCountFriendly

    local enemy_text_state = CoD.CreateState(f7_local12, f7_local10 + f7_local9, f7_local11, f7_local10 + f7_local9 * 2,
        CoD.AnchorTypes.TopLeft)
    enemy_text_state.font = tiny_font.Font
    enemy_text_state.color = Colors.s1.overload_red
    enemy_text_state.alignment = LUI.Alignment.Right

    local m_teamCountEnemy = LUI.UIText.new(enemy_text_state)
    m_teamCountEnemy:setTextStyle(CoD.TextStyle.Outlined)
    m_teamCountEnemy:setText("0")
    self:addElement(m_teamCountEnemy)
    element.m_teamCountEnemy = m_teamCountEnemy

end

local f0_local13 = function(f9_arg0, f9_arg1)
    local f9_local0 = f9_arg0:getParent()
    if f9_local0 ~= nil then
        f9_local0:removeElement(f9_arg0)
    end
    f9_arg1:addElement(f9_arg0)
end

local update_flag_count = function(element)
    -- blank function def
end
update_flag_count = function(element)
    local team = Game.GetPlayerTeam()
    if team ~= Teams.allies and team ~= Teams.axis then
        if element.teamInitTimer == nil then
            element.teamInitTimer = LUI.UITimer.new(500, "uplink_init_flags_owner")
            element:addElement(element.teamInitTimer)
            element:registerEventHandler("uplink_init_flags_owner", function(element, event)
                update_flag_count(element)
            end)
        end
        return
    elseif element.teamInitTimer then
        LUI.UITimer.Stop(element.teamInitTimer)
        element:removeElement(element.teamInitTimer)
        element.teamInitTimer = nil
    end
    local flag_count = get_omnvar_wrapper("ui_twar_flag_count") -- 5
    local ally_flag_count = get_omnvar_wrapper("ui_twar_ally_flag_count") -- 2
    local enemy_flag_count = flag_count - ally_flag_count - 1

    local neutral_flag_index = 0

    for index = 0, flag_count - 1, 1 do
        local flag_element = element:getFirstDescendentById("twar_flag_" .. index)
        if flag_element ~= nil then
            if not (team ~= Teams.allies or index ~= ally_flag_count) or team == Teams.axis and index ==
                enemy_flag_count then
                neutral_flag_index = index

                flag_element:animateToState("neutral", 0)
                element.m_neutralFlagWidget:processEvent({
                    name = "momentum_neutral_flag_change",
                    newFlagIndex = index
                })
            end
            if not (team ~= Teams.allies or index >= ally_flag_count) or team == Teams.axis and index < enemy_flag_count then
                flag_element:animateToState("friendly", 0)
            else
                -- weird hack, but needed to render LUI huds properly
                if (neutral_flag_index ~= index) then
                    flag_element:animateToState("enemy", 0)
                end
            end
        end
    end
end

local ui_twar_touching_internal = function(element)
    print("ui_twar_touching_internal start")
    local f12_local0 = Game.GetPlayerTeam()
    local f12_local1 = get_omnvar_wrapper("ui_twar_touching_allies")
    local f12_local2 = get_omnvar_wrapper("ui_twar_touching_axis")
    local f12_local3 = f12_local1
    local f12_local4 = f12_local2
    if f12_local0 == Teams.axis then
        f12_local3 = f12_local2
        f12_local4 = f12_local1
    end
    element.m_teamCountFriendly:setText("" .. f12_local3)
    element.m_teamCountEnemy:setText("" .. f12_local4)
    print("ui_twar_touching_internal end")
end

local saved_capture_team = 0
local ui_twar_capture_team_progress = function(element)
    local capture_team = get_omnvar_wrapper("ui_twar_capture_team")

    -- fix for progress bar possibly disappearing for a frame
    if (capture_team ~= saved_capture_team) then
        saved_capture_team = capture_team
        return
    end

    if capture_team == 0 then
        element.m_progressBar:processEvent({
            name = "momentumFillBarProgressUpdate",
            dispatchChildren = true,
            percent = 0
        })
    elseif capture_team == Game.GetPlayerTeam() then
        element.m_progressBar:processEvent({
            name = "momentumFillBarColorUpdate",
            dispatchChildren = true,
            color = custom_friendly_color
        })
    else
        element.m_progressBar:processEvent({
            name = "momentumFillBarColorUpdate",
            dispatchChildren = true,
            color = Colors.s1.overload_red
        })
    end

    saved_capture_team = capture_team
end

local ui_twar_capture_team_callback = function(element, f14_arg1)
    ui_twar_capture_team_progress(element)
end

local f0_local18 = function(element)
    element.m_progressBar:processEvent({
        name = "momentumFillBarProgressUpdate",
        dispatchChildren = true,
        percent = get_omnvar_wrapper("ui_twar_capture_progress", true)
    })
end

local ui_twar_capture_progress_callback = function(f16_arg0, f16_arg1)
    f0_local18(f16_arg0)
end

local ui_twar_flag_count_callback = function(element, dvar)
    render_twar_flag_hud(element)
    update_flag_count(element)
end

local ui_twar_ally_flag_count_callback = function(element, f18_arg1)
    update_flag_count(element)
end

local ui_team_selected_callback = function(element, f19_arg1)
    update_flag_count(element)
    f0_local18(element)
    ui_twar_capture_team_progress(element)
    ui_twar_touching_internal(element)
end

local ui_twar_touching_callback = function(element, f20_arg1)
    ui_twar_touching_internal(element)
end

local function twar_flags_hud_def()
    local ui_element_settings = {
        topAnchor = true,
        bottomAnchor = true,
        leftAnchor = true,
        rightAnchor = true,
        top = 0,
        bottom = 0,
        left = 0,
        right = 0,
        alpha = 1
    }
    local self = LUI.UIElement.new(ui_element_settings)
    self.id = "twar_flags_hud"
    self:registerAnimationState("default", {
        topAnchor = true,
        leftAnchor = true,
        bottomAnchor = true,
        rightAnchor = true,
        top = 0,
        left = 0,
        bottom = 0,
        right = 0,
        alpha = 1
    })
    self:animateToState("default", 0)
    self:registerDvarHandler("ui_twar_flag_count", ui_twar_flag_count_callback)
    self:registerDvarHandler("ui_twar_ally_flag_count", ui_twar_ally_flag_count_callback)
    self:registerDvarHandler("ui_twar_capture_progress", ui_twar_capture_progress_callback)
    self:registerDvarHandler("ui_twar_capture_team", ui_twar_capture_team_callback)
    self:registerOmnvarHandler("ui_team_selected", ui_team_selected_callback)
    self:registerDvarHandler("ui_twar_touching_allies", ui_twar_touching_callback)
    self:registerDvarHandler("ui_twar_touching_axis", ui_twar_touching_callback)
    render_twar_flag_hud(self)
    update_current_flag(self)
    update_flag_count(self)
    return self
end

local f0_local25 = function(f22_arg0)
    if get_omnvar_wrapper("ui_twar_momentum_bar_visible") == 0 then
        f22_arg0:animateToState("hidden")
    else
        f22_arg0:animateToState("default")
    end
end

local activate_multipliers = function(f23_arg0, f23_arg1)
    f0_local25(f23_arg0)
    if f23_arg1 == 2 then
        f23_arg0.m_multiplier2x:animateToState("active")
        f23_arg0.m_multiplier3x:animateToState("default")
    elseif f23_arg1 == 3 then
        f23_arg0.m_multiplier2x:animateToState("default")
        f23_arg0.m_multiplier3x:animateToState("active")
    else
        f23_arg0.m_multiplier2x:animateToState("default")
        f23_arg0.m_multiplier3x:animateToState("default")
    end
end

local update_momentum_progress_bar = function(f24_arg0, is_enemy, momentum, momentum_scale, momentum_end_time)
    f0_local25(f24_arg0)
    local friendly_bar = f24_arg0.m_friendlyBar1
    local friendly_bar_2 = f24_arg0.m_friendlyBar2
    if not is_enemy then
        friendly_bar = f24_arg0.m_enemyBar1
        friendly_bar_2 = f24_arg0.m_enemyBar2
    end
    if momentum_scale < 2 then
        friendly_bar:processEvent({
            name = "momentumFillBarProgressUpdate",
            dispatchChildren = true,
            percent = momentum,
            duration = momentum_end_time
        })
        friendly_bar_2:processEvent({
            name = "momentumFillBarProgressUpdate",
            dispatchChildren = true,
            percent = 0,
            fillColor = Colors.orange
        })
        if is_enemy then
            friendly_bar:processEvent({
                name = "momentum_bar_marker_visible",
                dispatchChildren = true,
                visible = true
            })
            friendly_bar_2:processEvent({
                name = "momentum_bar_marker_visible",
                dispatchChildren = true,
                visible = false
            })
            friendly_bar_2:processEvent({
                name = "momentum_bar_marker_color_update",
                dispatchChildren = true,
                color = Colors.white
            })
        end
    elseif momentum_scale < 3 then
        friendly_bar:processEvent({
            name = "momentumFillBarProgressUpdate",
            dispatchChildren = true,
            percent = 1
        })
        friendly_bar_2:processEvent({
            name = "momentumFillBarProgressUpdate",
            dispatchChildren = true,
            percent = momentum,
            fillColor = Colors.orange,
            duration = momentum_end_time
        })
        if is_enemy then
            friendly_bar:processEvent({
                name = "momentum_bar_marker_visible",
                dispatchChildren = true,
                visible = false
            })
            friendly_bar_2:processEvent({
                name = "momentum_bar_marker_visible",
                dispatchChildren = true,
                visible = true
            })
            friendly_bar_2:processEvent({
                name = "momentum_bar_marker_color_update",
                dispatchChildren = true,
                color = Colors.white
            })
        end
    else
        local f24_local2 = 1
        if momentum_end_time > 0 then
            f24_local2 = momentum_end_time / get_omnvar_wrapper("ui_twar_momentum_maxed_time", true) * 1000
        end
        friendly_bar:processEvent({
            name = "momentumFillBarProgressUpdate",
            dispatchChildren = true,
            percent = 1
        })
        friendly_bar_2:processEvent({
            name = "momentumFillBarProgressUpdate",
            dispatchChildren = true,
            percent = f24_local2
        })
        friendly_bar_2:processEvent({
            name = "momentumFillBarProgressUpdate",
            dispatchChildren = true,
            percent = momentum,
            fillColor = Colors.red,
            duration = momentum_end_time
        })
        if is_enemy then
            friendly_bar:processEvent({
                name = "momentum_bar_marker_visible",
                dispatchChildren = true,
                visible = false
            })
            friendly_bar_2:processEvent({
                name = "momentum_bar_marker_visible",
                dispatchChildren = true,
                visible = true
            })
            friendly_bar_2:processEvent({
                name = "momentum_bar_marker_color_update",
                dispatchChildren = true,
                color = Colors.red
            })
        end
    end
end

local update_allies_team_bar = function(element, momentum, momentum_scale, momentum_end_time)
    activate_multipliers(element, momentum_scale)
    update_momentum_progress_bar(element, true, momentum, momentum_scale, momentum_end_time)
end

local update_axis_team_bar = function(element, momentum, momentum_scale, momentum_end_time)
    update_momentum_progress_bar(element, false, momentum, momentum_scale, momentum_end_time)
end

local ui_twar_momentum_allies_stuff_internal = function(element)
    local f27_local0 = Game.GetPlayerTeam()
    local momentum_allies = get_omnvar_wrapper("ui_twar_momentum_allies", true)
    local momentum_scale_allies = get_omnvar_wrapper("ui_twar_momentum_scale_allies")
    local momentum_end_time_allies = get_omnvar_wrapper("ui_twar_momentum_end_time_allies")
    local momentum_end_time_new = 100
    if momentum_end_time_allies > 0 then
        momentum_end_time_new = momentum_end_time_allies - Game.GetTime()
    end
    if f27_local0 == Teams.allies then
        update_allies_team_bar(element, momentum_allies, momentum_scale_allies, momentum_end_time_new)
    elseif f27_local0 == Teams.axis then
        update_axis_team_bar(element, momentum_allies, momentum_scale_allies, momentum_end_time_new)
    end
end

local ui_twar_momentum_allies_stuff = function(element, f28_arg1)
    ui_twar_momentum_allies_stuff_internal(element)
end

local ui_twar_momentum_axis_stuff_internal = function(element)
    local f29_local0 = Game.GetPlayerTeam()
    local f29_local1 = get_omnvar_wrapper("ui_twar_momentum_axis", true)
    local f29_local2 = get_omnvar_wrapper("ui_twar_momentum_scale_axis")
    local f29_local3 = get_omnvar_wrapper("ui_twar_momentum_end_time_axis")
    local f29_local4 = 100
    if f29_local3 > 0 then
        f29_local4 = f29_local3 - Game.GetTime()
    end
    if f29_local0 == Teams.axis then
        update_allies_team_bar(element, f29_local1, f29_local2, f29_local4)
    elseif f29_local0 == Teams.allies then
        update_axis_team_bar(element, f29_local1, f29_local2, f29_local4)
    end
end

local f0_local33 = function(f30_arg0, f30_arg1)
    ui_twar_momentum_axis_stuff_internal(f30_arg0)
end

local ui_twar_momentum_bar_visible = function(element, f31_arg1)
    ui_twar_momentum_allies_stuff(element)
    ui_twar_momentum_axis_stuff_internal(element)
end

local f0_local35 = function(f32_arg0)
    local TOP_OFFSET_AMOUNT = 32 -- custom for H1 offsets

    local f32_local0 = 12
    local f32_local1 = 14
    local f32_local2 = 6
    local f32_local3 = 460
    local momentum_bar_height = 48 + TOP_OFFSET_AMOUNT
    local f32_local5 = 34
    local f32_local6 = 194
    local f32_local7 = 10
    local f32_local8 = 4
    local f32_local9 = {
        r = 0.8,
        g = 0.37,
        b = 0.2,
        a = 0.3
    }
    local text_settings = CoD.TextSettings.BodyFontTiny
    if CoD.IsUsingAspectRatio(1.33) then
        local self = 0.75
        f32_local0 = f32_local0 * self
        f32_local2 = f32_local2 * self
        f32_local3 = f32_local3 * self
        f32_local5 = f32_local5 * self
        f32_local6 = f32_local6 * self
    end
    local self = LUI.UIElement.new({
        bottomAnchor = true,
        width = f32_local3,
        height = momentum_bar_height,
        top = -momentum_bar_height - 15
    })
    f32_arg0:addElement(self)
    local momentum_meter_bracket = CoD.CreateState(0, 0, 0, -TOP_OFFSET_AMOUNT, CoD.AnchorTypes.All)
    momentum_meter_bracket.material = RegisterMaterial("hud_momentum_meter_bracket")
    self:addElement(LUI.UIImage.new(momentum_meter_bracket))
    self:addElement(LUI.UIImage.new({
        topAnchor = true,
        leftAnchor = true,
        top = f32_local1,
        left = f32_local0,
        width = f32_local5,
        height = f32_local7,
        color = custom_friendly_color
    }))
    self:addElement(LUI.UIImage.new({
        topAnchor = true,
        leftAnchor = true,
        top = f32_local1 + f32_local7 + f32_local2,
        left = f32_local0,
        width = f32_local5,
        height = f32_local8,
        color = Colors.s1.overload_red
    }))
    f32_arg0.m_friendlyBar1 = ui_create_bar(self, f32_local0 + f32_local5 + f32_local2, f32_local1, f32_local6,
        f32_local7, f32_local9, Colors.orange, true)
    f32_arg0.m_friendlyBar2 = ui_create_bar(self, f32_local0 + f32_local5 + f32_local2 + f32_local6 + f32_local2,
        f32_local1, f32_local6, f32_local7, f32_local9, Colors.orange, true)
    f32_arg0.m_enemyBar1 = ui_create_bar(self, f32_local0 + f32_local5 + f32_local2,
        f32_local1 + f32_local7 + f32_local2, f32_local6, f32_local8, f32_local9, Colors.orange)
    f32_arg0.m_enemyBar2 = ui_create_bar(self, f32_local0 + f32_local5 + f32_local2 + f32_local6 + f32_local2,
        f32_local1 + f32_local7 + f32_local2, f32_local6, f32_local8, f32_local9, Colors.orange)
    local f32_local13 = Engine.Localize("@MENU_2X")
    local f32_local14 = Engine.Localize("@MENU_3X")
    local f32_local15 = -8
    local f32_local16, f32_local17, f32_local18, f32_local19 =
        GetTextDimensions(f32_local13, text_settings.Font, text_settings.Height)

    local MULTIPLIER_TOP_OFFSET = text_settings.Height + f32_local15 - (TOP_OFFSET_AMOUNT + TOP_OFFSET_AMOUNT - 4)

    local m_multiplier2x = LUI.UIText.new({
        topAnchor = false,
        leftAnchor = true,
        left = f32_local0 + f32_local5 + f32_local2 + f32_local6 + f32_local2 * 0.5 - f32_local18 * 0.5,
        top = MULTIPLIER_TOP_OFFSET,
        height = text_settings.Height,
        font = text_settings.Font,
        color = Colors.white,
        alpha = 0.3
    })
    m_multiplier2x:setText(f32_local13)
    m_multiplier2x:registerAnimationState("active", {
        alpha = 1
    })
    m_multiplier2x:setGlow(Colors.white, 0.25, 0.25)
    self:addElement(m_multiplier2x)
    f32_arg0.m_multiplier2x = m_multiplier2x

    local m_multiplier3x = LUI.UIText.new({
        topAnchor = false,
        leftAnchor = true,
        left = f32_local0 + f32_local5 + f32_local2 + f32_local6 + f32_local2 + f32_local6 + f32_local2 * 0.5 -
            f32_local18 * 0.5,
        top = MULTIPLIER_TOP_OFFSET,
        height = text_settings.Height,
        font = text_settings.Font,
        color = Colors.red,
        alpha = 0.3
    })
    m_multiplier3x:setText(f32_local14)
    m_multiplier3x:registerAnimationState("active", {
        alpha = 1
    })
    m_multiplier3x:setGlow(Colors.red, 0.25, 0.25)
    self:addElement(m_multiplier3x)
    f32_arg0.m_multiplier3x = m_multiplier3x

    local momentum_text = LUI.UIText.new({
        topAnchor = true,
        rightAnchor = true,
        right = -f32_local3,
        top = 16,
        width = 500,
        height = text_settings.Height,
        font = text_settings.Font,
        alignment = LUI.Alignment.Right
    })
    momentum_text:setText(Engine.Localize("@MPUI_TWAR_CAPS"))
    self:addElement(momentum_text)
    update_momentum_progress_bar(f32_arg0, true, 0, 1, 0)
    update_momentum_progress_bar(f32_arg0, false, 0, 1, 0)
    return self
end

local twar_momentum_hud_def = function()
    local f33_local0 = CoD.CreateState(0, 0, 0, 0, CoD.AnchorTypes.All)
    f33_local0.alpha = 1
    local self = LUI.UIElement.new(f33_local0)
    self.id = "twar_momentum_hud"
    self:registerAnimationState("hidden", {
        alpha = 0
    })
    self:registerDvarHandler("ui_twar_momentum_allies", ui_twar_momentum_allies_stuff)
    self:registerDvarHandler("ui_twar_momentum_axis", f0_local33)
    self:registerDvarHandler("ui_twar_momentum_scale_allies", ui_twar_momentum_allies_stuff)
    self:registerDvarHandler("ui_twar_momentum_scale_axis", f0_local33)
    self:registerDvarHandler("ui_twar_momentum_bar_visible", ui_twar_momentum_bar_visible)
    self:registerOmnvarHandler("ui_team_selected", ui_twar_momentum_bar_visible)
    f0_local35(self)
    return self
end

LUI.MenuBuilder.registerType("twarFlagsHudDef", twar_flags_hud_def)
LUI.MenuBuilder.registerType("twarMomentumHudDef", twar_momentum_hud_def)
