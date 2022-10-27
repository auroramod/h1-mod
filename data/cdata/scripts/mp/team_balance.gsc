init()
{
    // define the auto balance string in the game array (referenced in gsc dump, but not defined past IW6?)
    precachestring(&"MP_AUTOBALANCE_NOW");
    game["strings"]["autobalance"] = &"MP_AUTOBALANCE_NOW";

    // define onteamselection callback function used in balanceteams()
    level.onteamselection = ::set_team;
}

set_team(team)
{
    if (team != self.pers["team"])
    {
        self.switching_teams = true;
        self.joining_team = team;
        self.leaving_team = self.pers["team"];
    }

    if (self.sessionstate == "playing")
    {
        self suicide();
    }

    maps\mp\gametypes\_menus::addtoteam(team);
    maps\mp\gametypes\_menus::endrespawnnotify();
}
