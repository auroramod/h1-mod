if (game:issingleplayer()) then
	if (Engine.InFrontend()) then
		require("options")
	end

	return
end

if (Engine.InFrontend()) then
	require("shaderdialog")
	require("gamemodes")
end

-- defined in mp_hud/hudutils.lua
function GetGameModeName()
	return Engine.Localize(Engine.TableLookup(GameTypesTable.File, 
		GameTypesTable.Cols.Ref, GameX.GetGameMode(), GameTypesTable.Cols.Name))
end

function NeverAllowChangeTeams()
	return false
end
