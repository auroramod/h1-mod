if (game:issingleplayer()) then
	return
end

require("shaderdialog")

-- defined in mp_hud/hudutils.lua
function GetGameModeName()
	return Engine.Localize(Engine.TableLookup(GameTypesTable.File, 
		GameTypesTable.Cols.Ref, GameX.GetGameMode(), GameTypesTable.Cols.Name))
end

function NeverAllowChangeTeams()
	return false
end
