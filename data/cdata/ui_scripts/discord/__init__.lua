if (game:issingleplayer() or Engine.InFrontend()) then
    return
end

local container = LUI.UIElement.new({
    topAnchor = true,
    rightAnchor = true,
    top = 20,
    right = 200,
    width = 200,
    spacing = 5
})

function truncatename(name, length)
    if (#name <= length - 3) then
        return name
    end

    return name:sub(1, length - 3) .. "..."
end

local requestlist = {}
local requestcount = 0

function addrequest(request)
    for i = 1, #requestlist do
        if (requestlist[i].userid == request.userid or #requestlist > 5) then
            return
        end
    end

    request.id = requestcount
    requestcount = requestcount + 1
    local yoffset = #requestlist * (75 + 5)

    local invite = LUI.UIElement.new({
        leftAnchor = true,
        rightAnchor = true,
        height = 75,
        top = yoffset
    })

    local getcurrentindex = function()
        for i = 1, #requestlist do
            if (requestlist[i].id == request.id) then
                return i
            end
        end

        return 0
    end

    invite:registerEventHandler("update_position", function()
        yoffset = (getcurrentindex() - 1) * (75 + 5)
        local state = {
            leftAnchor = true,
            height = 75,
            width = 200,
            left = -220,
            top = yoffset
        }

        invite:registerAnimationState("default", state)
        invite:animateToState("default", 50)
    end)

    invite:registerAnimationState("move_in", {
        leftAnchor = true,
        height = 75,
        width = 200,
        left = -220,
        top = yoffset
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
        material = avatarmaterial
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

    local requesttext = nil
    if (request.discriminator == "0") then
        requesttext = Engine.Localize("LUA_MENU_DISCORD_REQUEST", truncatename(request.username, 18))
    else
        requesttext = Engine.Localize("LUA_MENU_DISCORD_REQUEST_DISCRIMINATOR", truncatename(request.username, 18), request.discriminator)
    end

    username:setText(requesttext)

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

    local accepttext = Engine.Localize("LUA_MENU_DISCORD_ACCEPT", Engine.GetBinding("discord_accept"))
    local denytext = Engine.Localize("LUA_MENU_DISCORD_DENY", Engine.GetBinding("discord_deny"))

    buttons:addElement(createbutton(accepttext, true))
    buttons:addElement(createbutton(denytext))

    local fadeouttime = 50
    local timeout = 10 * 1000 - fadeouttime

    local function close()
        table.remove(requestlist, getcurrentindex())

        invite:registerAnimationState("fade_out", {
            leftAnchor = true,
            rightAnchor = true,
            height = 75,
            alpha = 0,
            left = 0,
            top = yoffset
        })

        invite:animateToState("fade_out", fadeouttime)
        invite:addElement(LUI.UITimer.new(fadeouttime + 50, "remove"))

        invite:registerEventHandler("remove", function()
            container:removeElement(invite)
            container:processEvent({
                name = "update_position",
                dispatchToChildren = true
            })
        end)
    end

    local closed = false
    request.handleresponse = function(event)
        if (closed) then
            return
        end

        if (event.accept) then
            discord.respond(request.userid, discord.reply.yes)
        else
            discord.respond(request.userid, discord.reply.no)
        end

        closed = true
        close()
    end

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
        avatar:setImage(avatarmaterial)
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

    table.insert(requestlist, request)
end

LUI.roots.UIRoot0:registerEventHandler("discord_response", function(element, event)
    if (#requestlist <= 0) then
        return
    end

    local request = requestlist[1]
    request.handleresponse(event)
end)

LUI.roots.UIRoot0:registerEventHandler("discord_join_request", function(element, event)
    addrequest(event.request)
end)

LUI.roots.UIRoot0:addElement(container)
