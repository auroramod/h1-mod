require("language")
require("background_effects")

if game:issingleplayer() then
    require("sp_unlockall")
    return
end

require("disable_useless_things")

if Engine.InFrontend() then
    require("shader_dialog")
    require("gamemodes")
    require("no_mode_switch")
else
    require("scoreboard")
end

-- defined in mp_hud/hudutils.lua
function GetGameModeName()
    return Engine.Localize(Engine.TableLookup(GameTypesTable.File, GameTypesTable.Cols.Ref, GameX.GetGameMode(),
        GameTypesTable.Cols.Name))
end

function NeverAllowChangeTeams()
    return false
end
