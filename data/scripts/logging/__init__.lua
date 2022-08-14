-- modified version of https://github.com/Joelrau/S1x-IW6x-g_log-script (permission to use by author)

if (game:getdvar("gamemode") ~= "mp") then
    return
end

-- setup dvars
game:setdvarifuninitialized("logfile", 1)
if (tonumber(game:getdvar("logfile")) < 1) then
    return
end
game:setdvarifuninitialized("g_log", "logs/games_mp.log")

start_time = 0

function get_time()
    local seconds = math.floor((game:gettime() - start_time) / 1000)
    local minutes = math.floor(seconds / 60)
    time = string.format("%d:%02d", minutes, seconds - minutes * 60)
    while (string.len(time) < 6) do
        time = " " .. time
    end
    time = time .. " "
    return time
end

function create_path(path)
    local dir = path:gsub("%/", "\\"):match("(.*[\\])")
    os.execute("if not exist " .. dir .. " mkdir " .. dir)
end

function log_print(message)
    local path = game:getdvar("g_log")
    local file = io.open(path, "a")
    if (file == nil) then
        create_path(path)
        file = assert(io.open(path, "a"))
    end
    file:write(get_time() .. message .. "\n")
    file:close()
end

function init()
    start_time = game:gettime()

    log_print("------------------------------------------------------------")
    log_print("InitGame")

    -- player callbacks
    level:onnotify("connected", function(player)
        player:player_connected()
    end)
    level:onnotify("say", function(player, message, hidden)
        player:say(message)
    end)
    level:onnotify("say_team", function(player, message, hidden)
        player:say(message, "say_team")
    end)

    -- damage/killed hooks
    game:onplayerdamage(player_damage)
    game:onplayerkilled(player_killed)

    -- other level notifies for log
    level:onnotify("exitLevel_called", function()
        log_print("ExitLevel: executed")
    end)
    level:onnotify("shutdownGame_called", function()
        log_print("ShutdownGame:")
        log_print("------------------------------------------------------------")
    end)
end

function entity:player_connected()
    log_print(string.format("J;%s;%i;%s", self:getguid(), self:getentitynumber(), self.name))

    self:onnotifyonce("disconnect", function()
        self:disconnect()
    end)
end

function entity:disconnect()
    log_print(string.format("Q;%s;%i;%s", self:getguid(), self:getentitynumber(), self.name))
end

function player_damage(self_, inflictor, attacker, damage, dflags, mod, weapon, vPoint, vDir, hitLoc)
    if (game:isplayer(attacker) == 1) then
        log_print(string.format("D;%s;%i;%s;%s;%s;%i;%s;%s;%s;%i;%s;%s", self_:getguid(), self_:getentitynumber(),
            self_.team, self_.name, attacker:getguid(), attacker:getentitynumber(), attacker.team, attacker.name,
            weapon, damage, mod, hitLoc))
    else
        log_print(string.format("D;%s;%i;%s;%s;%s;%i;%s;%s;%s;%i;%s;%s", self_:getguid(), self_:getentitynumber(),
            self_.team, self_.name, "", "-1", "world", "", weapon, damage, mod, hitLoc))
    end
end

function player_killed(self_, inflictor, attacker, damage, mod, weapon, vDir, hitLoc, psTimeOffset, deathAnimDuration)
    if (game:isplayer(attacker) == 1) then
        log_print(string.format("K;%s;%i;%s;%s;%s;%i;%s;%s;%s;%i;%s;%s", self_:getguid(), self_:getentitynumber(),
            self_.team, self_.name, attacker:getguid(), attacker:getentitynumber(), attacker.team, attacker.name,
            weapon, damage, mod, hitLoc))
    else
        log_print(string.format("K;%s;%i;%s;%s;%s;%i;%s;%s;%s;%i;%s;%s", self_:getguid(), self_:getentitynumber(),
            self_.team, self_.name, "", "-1", "world", "", weapon, damage, mod, hitLoc))
    end
end

-- this function handles 'say' and 'say_team'
function entity:say(message, mode)
    if (not mode) then
        mode = "say"
    end

    log_print(string.format("%s;%s;%i;%s;%s", mode, self:getguid(), self:getentitynumber(), self.name, message))
end

init()
