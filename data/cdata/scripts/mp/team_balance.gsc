main()
{
    precachestring(&"MP_AUTOBALANCE_NOW");

    // use player's score to balance instead player's time on team
    replacefunc(maps\mp\gametypes\_teams::balanceteams, ::balance_teams_stub);
}

balance_teams_stub()
{
    iprintlnbold(&"MP_AUTOBALANCE_NOW");

    allied_players = get_valid_team_array("allies");
    axis_players = get_valid_team_array("axis");
    while (is_team_bigger_than(allied_players, axis_players) || is_team_bigger_than(axis_players, allied_players))
    {
        if (is_team_bigger_than(allied_players, axis_players))
        {
            handle_lowest_score_player(allied_players, "axis");
        }
        else if (is_team_bigger_than(axis_players, allied_players))
        {
            handle_lowest_score_player(axis_players, "allies");
        }

        // refresh array for loop
        allied_players = get_valid_team_array("allies");
        axis_players = get_valid_team_array("axis");
    }
}

get_valid_team_array(team)
{
    team_array = [];
    players = level.players;
    for (i = 0; i < players.size; i++)
    {
        if (!isdefined(players[i].pers["score"])) // was teamTime
            continue;

        if (isdefined(players[i].pers["team"]) && players[i].pers["team"] == team)
            team_array[team_array.size] = players[i];
    }
    return team_array;
}

is_team_bigger_than(team_one, team_two)
{
    return (team_one.size > (team_two.size + 1));
}

handle_lowest_score_player(team, new_team)
{
    lowest_score_player = undefined;

    // move the player that has the lowest score (highest teamTime value)
    for (i = 0; i < team.size; i++)
    {
        if (isdefined(team[i].dont_auto_balance))
            continue;

        if (!isdefined(lowest_score_player))
            lowest_score_player = team[i];
        else if (team[i].pers["score"] < lowest_score_player.pers["score"])
            lowest_score_player = team[i];
    }

    lowest_score_player set_team(new_team);
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

    self maps\mp\gametypes\_menus::addtoteam(team);
    self maps\mp\gametypes\_menus::endrespawnnotify();
}
