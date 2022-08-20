if (game:issingleplayer()) then
    require("loading")
    return
end

if (Engine.InFrontend()) then
    require("download")
end
