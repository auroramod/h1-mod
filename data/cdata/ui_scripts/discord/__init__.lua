if (game:issingleplayer() or Engine.InFrontend()) then
    return
end

local container = LUI.UIVerticalList.new({
    topAnchor = true,
    rightAnchor = true,
    top = 20,
    right = 200,
    width = 200,
    spacing = 5
})

function canasktojoin(userid)
    history = history or {}
    if (history[userid] ~= nil) then
        return false
    end

    history[userid] = true
    game:ontimeout(function()
        history[userid] = nil
    end, 15000)

    return true
end

function truncatename(name, length)
    if (#name <= length - 3) then
        return name
    end

    return name:sub(1, length - 3) .. "..."
end

function addrequest(request)
    if (not canasktojoin(request.userid)) then
        return
    end

    if (container.temp) then
        container:removeElement(container.temp)
        container.temp = nil
    end

    local invite = LUI.UIElement.new({
        leftAnchor = true,
        rightAnchor = true,
        height = 75
    })

    invite:registerAnimationState("move_in", {
        leftAnchor = true,
        height = 75,
        width = 200,
        left = -220
    })

    invite:animateToState("move_in", 100)

    local background = LUI.UIImage.new({
        topAnchor = true,
        leftAnchor = true,
        rightAnchor = true,
        bottomAnchor = true,
        top = 1,
        left = 1,
        bottom = -1,
        right = -1,
        material = RegisterMaterial("white"),
        color = {
            r = 0,
            b = 0,
            g = 0
        },
        alpha = 0.6
    })

    local border = LUI.UIImage.new({
        topAnchor = true,
        leftAnchor = true,
        rightAnchor = true,
        bottomAnchor = true,
        material = RegisterMaterial("btn_focused_rect_innerglow")
    })

    border:setup9SliceImage(10, 5, 0.25, 0.12)

    local paddingvalue = 10
    local padding = LUI.UIElement.new({
        topAnchor = true,
        leftAnchor = true,
        rightAnchor = true,
        bottomAnchor = true,
        top = paddingvalue,
        left = paddingvalue,
        right = -paddingvalue,
        bottom = -paddingvalue
    })

    local avatarmaterial = discord.getavatarmaterial(request.userid)
    local avatar = LUI.UIImage.new({
        leftAnchor = true,
        topAnchor = true,
        width = 32,
        height = 32,
        left = 1,
        material = RegisterMaterial(avatarmaterial)
    })

    local username = LUI.UIText.new({
        leftAnchor = true,
        topAnchor = true,
        height = 12,
        left = 32 + paddingvalue,
        color = Colors.white,
        alignment = LUI.Alignment.Left,
        rightAnchor = true,
        font = CoD.TextSettings.BodyFontBold.Font
    })

    username:setText(string.format("%s^7#%s requested to join your game!", truncatename(request.username, 18),
        request.discriminator))

    local buttons = LUI.UIElement.new({
        leftAnchor = true,
        rightAnchor = true,
        topAnchor = true,
        top = 37,
        height = 18
    })

    local createbutton = function(text, left)
        local button = LUI.UIElement.new({
            leftAnchor = left,
            rightAnchor = not left,
            topAnchor = true,
            height = 18,
            width = 85,
            material = RegisterMaterial("btn_focused_rect_innerglow")
        })

        local center = LUI.UIText.new({
            rightAnchor = true,
            height = 12,
            width = 85,
            top = -6.5,
            alignment = LUI.Alignment.Center,
            font = CoD.TextSettings.BodyFontBold.Font
        })

        button:setup9SliceImage(10, 5, 0.25, 0.12)
        center:setText(text)
        button:addElement(center)

        return button
    end

    buttons:addElement(createbutton("[F1] Accept", true))
    buttons:addElement(createbutton("[F2] Deny"))

    local fadeouttime = 50
    local timeout = 10 * 1000 - fadeouttime

    local function close()
        container:processEvent({
            name = "update_navigation",
            dispatchToChildren = true
        })
        invite:animateToState("fade_out", fadeouttime)
        invite:addElement(LUI.UITimer.new(fadeouttime + 50, "remove"))

        invite:registerEventHandler("remove", function()
            container:removeElement(invite)
            if (container.temp) then
                container:removeElement(container.temp)
                container.temp = nil
            end
            local temp = LUI.UIElement.new({})
            container.temp = temp
            container:addElement(temp)
        end)
    end

    buttons:registerEventHandler("keydown_", function(element, event)
        if (event.key == "F1") then
            close()
            discord.respond(request.userid, discord.reply.yes)
        end

        if (event.key == "F2") then
            close()
            discord.respond(request.userid, discord.reply.no)
        end
    end)

    invite:registerAnimationState("fade_out", {
        leftAnchor = true,
        rightAnchor = true,
        height = 75,
        alpha = 0,
        left = 0
    })

    invite:addElement(LUI.UITimer.new(timeout, "end_invite"))
    invite:registerEventHandler("end_invite", function()
        close()
        discord.respond(request.userid, discord.reply.ignore)
    end)

    local bar = LUI.UIImage.new({
        bottomAnchor = true,
        leftAnchor = true,
        bottom = -3,
        left = 3,
        width = 200 - 6,
        material = RegisterMaterial("white"),
        height = 2,
        color = {
            r = 92 / 255,
            g = 206 / 255,
            b = 113 / 255
        }
    })

    bar:registerAnimationState("closing", {
        bottomAnchor = true,
        leftAnchor = true,
        bottom = -3,
        left = 3,
        width = 0,
        height = 2
    })

    bar:animateToState("closing", timeout)

    avatar:registerEventHandler("update", function()
        local avatarmaterial = discord.getavatarmaterial(request.userid)
        avatar:setImage(RegisterMaterial(avatarmaterial))
    end)

    avatar:addElement(LUI.UITimer.new(100, "update"))

    invite:addElement(background)
    invite:addElement(bar)
    invite:addElement(border)
    invite:addElement(padding)
    padding:addElement(username)
    padding:addElement(avatar)
    padding:addElement(buttons)

    container:addElement(invite)
end

container:registerEventHandler("keydown", function(element, event)
    local first = container:getFirstChild()

    if (not first) then
        return
    end

    first:processEvent({
        name = "keydown_",
        key = event.key
    })
end)

LUI.roots.UIRoot0:registerEventHandler("discord_join_request", function(element, event)
    addrequest(event.request)
end)

LUI.roots.UIRoot0:addElement(container)
