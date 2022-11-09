Cac.GameModes.Data = {
    Standard = {
        Label = Engine.Localize("@MPUI_STANDARD_CAPS"),
        Image = "h1_ui_icon_playlist_standard",
        List = 
        {
            "dm", "war", "sd", "dom", "conf", "sab", "koth", "hp", "gun", 
            "dd", "ctf" -- missing gamemodes from UI
        }
    }
}

if (Engine.InFrontend()) then
    return
end

local gametypes = {
	["dm"] = true,
	["dom"] = true,
	["sd"] = true,
	["war"] = true,
	["conf"] = true,
	["vlobby"] = true,
	["koth"] = true,
	["sab"] = true,
	["ctf"] = true,
	["dd"] = true,
	["hp"] = true,
	["gun"] = true,
}

local func = LUI.mp_hud.Scoreboard.DetermineIfSingleTeamGameType
LUI.mp_hud.Scoreboard.DetermineIfSingleTeamGameType = function()
    local gametype = Engine.GetDvarString("ui_gametype")
    if (gametypes[gametype]) then
        return func()
    end

    return Game.GetPlayerTeam() == Teams.free
end
