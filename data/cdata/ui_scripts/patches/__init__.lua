require("language")

if (game:issingleplayer()) then
	return
end

if (Engine.InFrontend()) then
	require("shader_dialog")
	require("gamemodes")
	require("no_mode_switch")
	require("disable_useless_things")
end

-- defined in mp_hud/hudutils.lua
function GetGameModeName()
	return Engine.Localize(Engine.TableLookup(GameTypesTable.File, 
		GameTypesTable.Cols.Ref, GameX.GetGameMode(), GameTypesTable.Cols.Name))
end

function NeverAllowChangeTeams()
	return false
end
