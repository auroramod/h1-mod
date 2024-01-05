/*

    Momentum from AW ported to MWR
    - MWR maps do not have Momentum points at the moment
    - "Minion" AI doesn't exist at all (used for lobbies not full?)

    * Requires yoyo's AW patches

*/

main()
{
    if (getdvar("mapname") == "mp_background")
        return;

    maps\mp\gametypes\_globallogic::init();
    maps\mp\gametypes\_callbacksetup::setupcallbacks();
    maps\mp\gametypes\_globallogic::setupcallbacks();

    if (isusingmatchrulesdata())
    {
        level.initializematchrules = ::initializematchrules;
        [[ level.initializematchrules ]]();
        level thread maps\mp\_utility::reinitializematchrulesonmigration();
    }
    else
    {
        maps\mp\_utility::registerroundswitchdvar(level.gametype, 0, 0, 9);
        maps\mp\_utility::registertimelimitdvar(level.gametype, 10);
        maps\mp\_utility::registerscorelimitdvar(level.gametype, 75);
        maps\mp\_utility::registerroundlimitdvar(level.gametype, 1);
        maps\mp\_utility::registerwinlimitdvar(level.gametype, 1);
        maps\mp\_utility::registernumlivesdvar(level.gametype, 0);
        maps\mp\_utility::registerhalftimedvar(level.gametype, 0);
        level.matchrules_damagemultiplier = 0;
        level.matchrules_vampirism = 0;
    }

    maps\mp\_utility::setovertimelimitdvar(3);
    level.teambased = 1;
    level.classicgamemode = 0; // for classic sounds?
    level.onstartgametype = ::onstartgametype;
    level.getspawnpoint = ::getspawnpoint;
    level.onnormaldeath = ::onnormaldeath;
    //level.onspawnplayer = ::onspawnplayer;
    level.ontimelimit = ::ontimelimit;
    level.onplayerkilled = ::onplayerkilled;
    //level.onplayerscore = ::onplayerscore; // not used?
    level.allowboostingabovetriggerradius = 1;
    //level.ai_game_mode = 1;
    //level.modifyplayerdamage = ::minion_damage;
    //level.on_agent_player_killed = ::on_minion_killed;
    level.spawn_version = 3;

    // TODO: do these effects exist?
    //level.flagfxid = loadfx("vfx/unique/vfx_ctf_usmc_blue_grnd"); // was vfx_flag_project_neutral 
    level.boarderfxid = loadfx("vfx/unique/vfx_marker_dom_white"); // was vfx_marker_dom_white

    /*
    if (getdvarint("party_maxPlayers", 18) == 2)
        level.xpgamemodescale = 2;
    */

    if (level.matchrules_damagemultiplier || level.matchrules_vampirism)
        level.modifyplayerdamage = maps\mp\gametypes\_damage::gamemodemodifyplayerdamage;

    setteammode("ffa");
    
    game["dialog"]["gametype"] = "mom_intro";

    if (getdvarint("g_hardcore"))
        game["dialog"]["gametype"] = "hc_" + game["dialog"]["gametype"];
    else if (getdvarint("camera_thirdPerson"))
        game["dialog"]["gametype"] = "thirdp_" + game["dialog"]["gametype"];
    else if (getdvarint("scr_diehard"))
        game["dialog"]["gametype"] = "dh_" + game["dialog"]["gametype"];

    game["dialog"]["defense_obj"] = "mtm_alert";
    game["dialog"]["offense_obj"] = "mtm_alert";
    game["dialog"]["mtm_taking"] = "mtm_taking";
    game["dialog"]["mtm_etaking"] = "mtm_etaking";
    game["dialog"]["mtm_lastflg"] = "mtm_lastflg";
    game["dialog"]["mtm_elastflg"] = "mtm_elastflg";
    game["dialog"]["mtm_secured"] = "mtm_secured";
    game["dialog"]["mtm_captured"] = "mtm_captured";
    game["dialog"]["mtm_max"] = "mtm_max";
    game["dialog"]["mtm_gain"] = "mtm_gain";
    game["dialog"]["mtm_reset"] = "mtm_reset";
    game["dialog"]["mtm_clrd"] = "mtm_clrd";
    game["dialog"]["mtm_lost"] = "mtm_lost";

    // we should add grapping hooks hehe
    /*
    if (maps\mp\_utility::isgrapplinghookgamemode())
        game["dialog"]["gametype"] = "grap_" + game["dialog"]["gametype"];
    */

    if (!isdefined(game["shut_out"]))
    {
        game["shut_out"]["axis"] = 1;
        game["shut_out"]["allies"] = 1;
        game["max_meter"]["axis"] = 0;
        game["max_meter"]["allies"] = 0;
    }

    //setdvar("r_hudOutlineWidth", 1);
}

initializematchrules()
{
    maps\mp\_utility::setcommonrulesfrommatchrulesdata(true);
    setdynamicdvar("scr_twar_roundswitch", 0);
    maps\mp\_utility::registerroundswitchdvar("twar", 0, 0, 9);
    setdynamicdvar("scr_twar_roundlimit", 1);
    maps\mp\_utility::registerroundlimitdvar("twar", 1);
    setdynamicdvar("scr_twar_winlimit", 1);
    maps\mp\_utility::registerwinlimitdvar("twar", 1);
    setdynamicdvar("scr_twar_halftime", 0);
    maps\mp\_utility::registerhalftimedvar("twar", 0);
    setdynamicdvar("scr_twar_halftime", 0);
    setdynamicdvar("scr_twar_minionsmax", getmatchrulesdata("twarData", "numMinions"));
    setdynamicdvar("scr_twar_capture_time", getmatchrulesdata("twarData", "captureTime"));
    setdynamicdvar("scr_twar_zone_count", getmatchrulesdata("twarData", "numFlags"));
    setdynamicdvar("scr_twar_ot_zone_count", getmatchrulesdata("twarData", "numOTFlags"));
    setdynamicdvar("scr_twar_min_capture_players", 1);
    setdynamicdvar("scr_twar_hud_momentum_bar", !getmatchrulesdata("twarData", "hideMomentumBar"));
}

onstartgametype()
{
    setclientnamemode("auto_change");

    if (!isdefined(game["switchedsides"]))
        game["switchedsides"] = 0;

    if (game["switchedsides"])
    {
        attackers_team = game["attackers"];
        defenders_team = game["defenders"];
        game["attackers"] = attackers_team;
        game["defenders"] = defenders_team;
    }

    maps\mp\_utility::setobjectivetext("allies", &"OBJECTIVES_TWAR");
    maps\mp\_utility::setobjectivetext("axis", &"OBJECTIVES_TWAR");

    if (level.splitscreen)
    {
        maps\mp\_utility::setobjectivescoretext("allies", &"OBJECTIVES_TWAR");
        maps\mp\_utility::setobjectivescoretext("axis", &"OBJECTIVES_TWAR");
    }
    else
    {
        maps\mp\_utility::setobjectivescoretext("allies", &"OBJECTIVES_TWAR_SCORE");
        maps\mp\_utility::setobjectivescoretext("axis", &"OBJECTIVES_TWAR_SCORE");
    }

    maps\mp\_utility::setobjectivehinttext("allies", &"OBJECTIVES_TWAR_HINT");
    maps\mp\_utility::setobjectivehinttext("axis", &"OBJECTIVES_TWAR_HINT");
    game["dialog"]["lockouts"]["mtm_taking"] = 5;
    game["dialog"]["lockouts"]["mtm_etaking"] = 5;

    initspawns(); // _bots_gametype_twar needed

    allowed[0] = level.gametype;
    maps\mp\gametypes\_gameobjects::main(allowed);

    // this is needed for bot movement i think?
    level.zone_radius = getdvarint("scr_twar_zone_radius", 150);

    if (!isdefined(level.zone_height))
        level.zone_height = 60;

    level.momentum_multiplier_max = 3;

    find_zones();
    assign_spawns();
    create_active_zone();

    init_momentum("allies");
    init_momentum("axis");

    /*
    thread watch_for_joined_team();
    thread updateminions();
    */
    thread update_lua_hud();
}

/*
watch_for_joined_team()
{
    for (;;)
    {
        level waittill("joined_team");
        level notify("update_flag_outline");
        update_minion_hud_outlines();
    }
}
*/

ontimelimit()
{
    level.finalkillcam_winner = "none";
    if (game["status"] == "overtime")
        winner = "tie";
    else if (game["teamScores"]["allies"] == game["teamScores"]["axis"])
    {
        winner = "overtime";
        game["owned_flags"] = [];
        game["owned_flags"]["allies"] = 0;
        game["owned_flags"]["axis"] = 0;

        foreach (twar_zones in level.twar_zones)
        {
            if (twar_zones.owner == "allies")
            {
                game["owned_flags"]["allies"]++;
                continue;
            }

            if (twar_zones.owner == "axis")
                game["owned_flags"]["axis"]++;
        }
    }
    else if (game["teamScores"]["axis"] > game["teamScores"]["allies"])
    {
        level.finalkillcam_winner = "axis";
        winner = "axis";
    }
    else
    {
        level.finalkillcam_winner = "allies";
        winner = "allies";
    }

    // practice rounds??
    /*
    if (maps\mp\_utility::practiceroundgame())
        winner = "none";
    */

    thread maps\mp\gametypes\_gamelogic::endgame(winner, game["end_reason"]["time_limit_reached"]);
}

spawn_flag_projector(model)
{
    script_model = spawn("script_model", model);
    script_model setmodel("h1_flag_mp_domination_usmc_blue"); // TODO: model exists in H1? flag_holo_base_ground
    return script_model;
}

create_active_zone()
{
    flag_projector[0] = spawn_flag_projector((0, 0, 0));
    trigger_radius = spawn("trigger_radius", (0, 0, 0), 0, level.zone_radius, level.zone_height);
    trigger_radius.radius = level.zone_radius;
    capture_time = getdvarfloat("scr_twar_capture_time", 20.0);
    use_object = maps\mp\gametypes\_gameobjects::createuseobject("neutral", trigger_radius, flag_projector);
    use_object maps\mp\gametypes\_gameobjects::allowuse("any");
    use_object maps\mp\gametypes\_gameobjects::setusetime(capture_time);
    use_object maps\mp\gametypes\_gameobjects::setvisibleteam("any");
    use_object.keepprogress = 1;
    use_object.nousebar = 1;
    use_object.id = "twarZone";
    use_object.onbeginuse = ::onbeginuse;
    use_object.onuse = ::onuse;
    use_object.onenduse = ::onenduse;
    use_object.onupdateuserate = ::onupdateuserate;
    level.twar_use_obj = use_object;
    reset_zone_owners();
}

zone_set_waiting()
{
    maps\mp\gametypes\_gameobjects::setownerteam("neutral");
    maps\mp\gametypes\_gameobjects::allowuse("none");
    waypoint_neutral = "waypoint_waitfor_flag_neutral";
    maps\mp\gametypes\_gameobjects::set2dicon("friendly", waypoint_neutral);
    maps\mp\gametypes\_gameobjects::set3dicon("friendly", waypoint_neutral);
    maps\mp\gametypes\_gameobjects::set2dicon("enemy", waypoint_neutral);
    maps\mp\gametypes\_gameobjects::set3dicon("enemy", waypoint_neutral);
    maps\mp\_utility::setmlgicons(self, waypoint_neutral);
    self.waiting = 1;
    set_omnvar_wrapper("ui_twar_capture_team", 3);
}

update_icons(friendlies_touching, enemies_touching)
{
    if (isdefined(self.waiting))
        return;

    if (friendlies_touching > 0 && enemies_touching > 0)
    {
        waypoint_contested = "waypoint_contested";
        maps\mp\gametypes\_gameobjects::set2dicon("friendly", waypoint_contested);
        maps\mp\gametypes\_gameobjects::set3dicon("friendly", waypoint_contested);
        maps\mp\gametypes\_gameobjects::set2dicon("enemy", waypoint_contested);
        maps\mp\gametypes\_gameobjects::set3dicon("enemy", waypoint_contested);
        maps\mp\_utility::setmlgicons(self, waypoint_contested);
    }
    else if (friendlies_touching == 0 && enemies_touching == 0)
    {
        waypoint_captureneutral = "waypoint_captureneutral";
        maps\mp\gametypes\_gameobjects::set2dicon("friendly", waypoint_captureneutral);
        maps\mp\gametypes\_gameobjects::set3dicon("friendly", waypoint_captureneutral);
        maps\mp\gametypes\_gameobjects::set2dicon("enemy", waypoint_captureneutral);
        maps\mp\gametypes\_gameobjects::set3dicon("enemy", waypoint_captureneutral);
        maps\mp\_utility::setmlgicons(self, waypoint_captureneutral);
    }
    else
    {
        maps\mp\gametypes\_gameobjects::set2dicon("enemy", "waypoint_losing");
        maps\mp\gametypes\_gameobjects::set3dicon("enemy", "waypoint_losing");
        maps\mp\gametypes\_gameobjects::set2dicon("friendly", "waypoint_taking");
        maps\mp\gametypes\_gameobjects::set3dicon("friendly", "waypoint_taking");

        if (self.claimteam == "allies")
            maps\mp\_utility::setmlgicons(self, "waypoint_esports_taking_blue");
        else
            maps\mp\_utility::setmlgicons(self, "waypoint_esports_taking_red");
    }
}

zone_set_neutral()
{
    maps\mp\gametypes\_gameobjects::setownerteam("neutral");
    maps\mp\gametypes\_gameobjects::allowuse("any");
    self.waiting = undefined;
    set_omnvar_wrapper("ui_twar_capture_team", 0);
    update_icons(0, 0);
}

/*
zone_flag_effect()
{
    zone_flag_effect_stop();
    obj_visuals = level.twar_use_obj.visuals[0];

    // TODO: don't use spawnlinkedfx???
    self.flagfx = spawnlinkedfx(level.flagfxid, obj_visuals, "tag_fx_flag");
    //self.flagfx = maps\mp\_utility::spawnfxshowtoteam(level.flagfxid, "any", obj_visuals.origin, anglestoforward(obj_visuals.origin));

    setfxkillondelete(self.flagfx, 1);
    triggerfx(self.flagfx);
}

zone_flag_effect_stop()
{
    if (isdefined(self.flagfx))
        self.flagfx delete();
}
*/

zone_boarder_effect()
{
    zone_boarder_effect_stop();
    obj_visuals = level.twar_use_obj.visuals[0];
    self.boarderfx = spawnfx(level.boarderfxid, obj_visuals.origin, anglestoup(obj_visuals.angles));
    setfxkillondelete(self.boarderfx, 1);
    triggerfx(self.boarderfx);
}

zone_boarder_effect_stop()
{
    if (isdefined(self.boarderfx))
        self.boarderfx delete();
}

zone_set_team(owner_team)
{
    maps\mp\gametypes\_gameobjects::setownerteam(owner_team);
    maps\mp\gametypes\_gameobjects::allowuse("any");
}

/*
update_flag_outline()
{
    for (;;)
    {
        level waittill("update_flag_outline");
        friendly_color = getdvarint("scr_twar_flag_outline_color_friendly", -1);
        enemy_color = getdvarint("scr_twar_flag_outline_color_enemy", -1);
        neutral_color = getdvarint("scr_twar_flag_outline_color_neutral", -1);
        outline_depth = getdvarint("scr_twar_flag_outline_depth", 0);
        //self hudoutlinedisableforclients(level.players); // whats this???
        zone_index = level.twar_use_obj maps\mp\gametypes\_gameobjects::getclaimteam();
        claim_team = [];
        momentum_bar = [];
        capture_progress = [];

        foreach (number_touching in level.players)
        {
            if ((zone_index == "allies" || zone_index == "axis") && (number_touching.team == "allies" || number_touching.team == "axis"))
            {
                if (zone_index == number_touching.team)
                    claim_team[claim_team.size] = number_touching;
                else
                    momentum_bar[momentum_bar.size] = number_touching;

                continue;
            }

            capture_progress[capture_progress.size] = number_touching;
        }

        if (claim_team.size && friendly_color >= 0)
            self hudoutlineenableforclients(claim_team, friendly_color, outline_depth);

        if (momentum_bar.size && enemy_color >= 0)
            self hudoutlineenableforclients(momentum_bar, enemy_color, outline_depth);

        if (capture_progress.size && neutral_color >= 0)
            self hudoutlineenableforclients(capture_progress, neutral_color, outline_depth);
    }
}
*/

reset_zone_owners()
{
    twar_zone_index = int(level.twar_zones.size / 2);

    // no valid zones to work with (H1 maps don't have them lol)
    if (level.twar_zones.size < 1)
    {
        return;
    }

    foreach (index, zone in level.twar_zones)
    {
        if (index < twar_zone_index)
        {
            zone.owner = "allies";
            continue;
        }

        if (index > twar_zone_index)
        {
            zone.owner = "axis";
            continue;
        }

        zone.owner = "none";
    }

    set_contested_zone(level.twar_zones[twar_zone_index]);
}

onbeginuse(player)
{
    team = player.team;
    other_team = maps\mp\_utility::getotherteam(team);
    zone_set_team(team);
    maps\mp\_utility::leaderdialog("mtm_taking", team);
    maps\mp\_utility::leaderdialog("mtm_etaking", other_team);
    //level notify("update_flag_outline");
}

onuse(player)
{
    team = player.team;
    other_team = maps\mp\_utility::getotherteam(team);
    zone = self.zone;
    zone.owner = team;
    zone_index = zone.index;

    if (team == "allies")
        zone_index++;
    else if (team == "axis")
        zone_index--;

    game["shut_out"][other_team] = 0;

    if (getdvarint("scr_twar_momentum_clear_friendly_on_capture", 0))
        clear_momentum(team);
    else
        add_capture_friendly_momentum(team);

    if (getdvarint("scr_twar_momentum_clear_enemy_on_capture", 0))
        clear_momentum(other_team);
    else
        add_capture_enemy_momentum(other_team);

    player thread maps\mp\_audio::snd_play_team_splash("mp_obj_notify_pos_lrg", "mp_obj_notify_neg_lrg");
    thread givezonecapturexp(self.touchlist[team]);

    if (zone_index < 0 || zone_index >= level.num_zones)
    {
        //zone_flag_effect_stop();
        zone_boarder_effect_stop();
        level maps\mp\gametypes\_gamescore::giveteamscoreforobjective(team, 1);
        maps\mp\_utility::leaderdialog("mtm_secured", team);

        if (level.gameended)
            maps\mp\gametypes\_gameobjects::disableobject();
        else
            reset_zone_owners();
    }
    else
    {
        set_contested_zone(level.twar_zones[zone_index]);

        if (zone_index == 0 || zone_index == level.num_zones - 1)
        {
            leaderdialogwait("mtm_lastflg", other_team);
            leaderdialogwait("mtm_elastflg", team);
        }
        else
            leaderdialogwait("mtm_secured", team);
    }

    self.nextusetime = gettime() + 50;
}

leaderdialogwait(dialong_name, team)
{
    thread _leaderdialogwait(dialong_name, team);
}

_leaderdialogwait(dialong_name, team)
{
    waitframe();
    maps\mp\_utility::leaderdialog(dialong_name, team);
}

givezonecapturexp(players_touching)
{
    level endon("game_ended");
    player = maps\mp\gametypes\_gameobjects::getearliestclaimplayer();

    if (isdefined(player.owner))
        player = player.owner;

    if (isplayer(player))
        level thread maps\mp\_utility::teamplayercardsplash("callout_securedposition", player);

    players_touching_arr = getarraykeys(players_touching);

    for (index = 0; index < players_touching_arr.size; index++)
    {
        player = players_touching[players_touching_arr[index]].player; // lots of player

        if (isdefined(player.owner))
            player = player.owner;

        if (!isplayer(player))
            continue;

        player thread maps\mp\_events::domcaptureevent(0);
        wait 0.05;
    }
}

onenduse(team, player, result)
{
    zone_set_neutral();
    //level notify("update_flag_outline");
}

onupdateuserate()
{
    friendlies_touching_count = 0;
    enemies_touching_count = 0;
    other_team = maps\mp\_utility::getotherteam(self.claimteam);

    foreach (claim_team in self.touchlist[other_team])
    {
        momentum_bar = claim_team.player;

        if (!isdefined(momentum_bar))
            continue;

        if (momentum_bar.pers["team"] != other_team)
            continue;

        enemies_touching_count++;
    }

    var_8 = getdvarint("scr_twar_capture_players_max", 3);

    foreach (claim_team in self.touchlist[self.claimteam])
    {
        momentum_bar = claim_team.player;

        if (!isdefined(momentum_bar))
            continue;

        if (momentum_bar.pers["team"] != self.claimteam)
            continue;

        friendlies_touching_count++;

        if (friendlies_touching_count >= var_8)
            break;
    }

    self.userate = 0;
    self.stalemate = friendlies_touching_count && enemies_touching_count;
    min_players_capture = getdvarint("scr_twar_min_capture_players", 1);

    if (friendlies_touching_count && !enemies_touching_count 
        && friendlies_touching_count >= min_players_capture)
    {
        var_12 = level.twar_team_multiplier[self.claimteam];
        self.userate = friendlies_touching_count * var_12;
    }

    capture_rate_max = getdvarint("scr_twar_capture_rate_max", 9.0);
    self.userate = min(self.userate, capture_rate_max);

    if (self.keepprogress && self.lastclaimteam != self.claimteam)
        self.userate *= -1;

    update_icons(friendlies_touching_count, enemies_touching_count);
}

set_contested_zone(zone)
{
    if (!isdefined(zone))
    {
        print("zone isn't defined, does this map support Momentum?");
        return;
    }

    zone.owner = "none";
    level.twar_use_obj.zone = zone;
    level.twar_use_obj maps\mp\gametypes\_gameobjects::move_use_object(zone.origin, (0, 0, 100));

    foreach (twar_zone in level.twar_zones)
    {
        if (twar_zone != zone)
        {
            twar_zone.projector show();
            continue;
        }

        twar_zone.projector hide();
    }

    if (level.twar_use_obj.keepprogress)
        level.twar_use_obj.lastclaimteam = "none";

    level thread set_contested_zone_wait(5);
}

set_contested_zone_wait(waittime)
{
    waittillframeend;
    //level.twar_use_obj zone_flag_effect_stop();
    level.twar_use_obj zone_boarder_effect();
    level.twar_use_obj zone_set_waiting();
    wait(waittime);
    //level.twar_use_obj zone_flag_effect();
    level.twar_use_obj zone_set_neutral();
}

update_lua_hud()
{
    level endon("game_ended");

    momentum_bar = getdvarint("scr_twar_hud_momentum_bar", 1);
    set_omnvar_wrapper("ui_twar_momentum_bar_visible", momentum_bar);

    for(;;)
    {
        ally_flag_count = 0;
        if (isdefined(level.twar_zones))
        {
            foreach (var_2 in level.twar_zones)
            {
                if (var_2.owner == "allies")
                    ally_flag_count++;
            }
        }
        set_omnvar_wrapper("ui_twar_ally_flag_count", ally_flag_count);

        claim_team = "";
        if (isdefined(level.twar_use_obj.keepprogress) && level.twar_use_obj.keepprogress)
            claim_team = level.twar_use_obj.lastclaimteam;
        else
            claim_team = level.twar_use_obj.claimteam;

        // lastclaimteam or claimteam can be undefined i think?
        if (!isdefined(claim_team))
            claim_team = "";

        capture_team_index = 0; // progress update
        if (claim_team == "axis")
            capture_team_index = 1;
        else if (claim_team == "allies")
            capture_team_index = 2;

        capture_team_check = get_omnvar_wrapper("ui_twar_capture_team");
        //mikey_debug(va("--- capture_team_check: %s", capture_team_check));
        if (capture_team_check != 3) // 3 == neutral
            set_omnvar_wrapper("ui_twar_capture_team", capture_team_index);

        // capture progress (float)
        capture_progress = 0.0;
        if (claim_team != "none")
            capture_progress = level.twar_use_obj.curprogress / level.twar_use_obj.usetime;
        //mikey_debug(va("--- capture_progress: %s", capture_progress));
        set_omnvar_wrapper("ui_twar_capture_progress", capture_progress);
        
        if (isdefined(level.teamnamelist))
        {
            foreach (capture_team_name in level.teamnamelist)
            {
                number_touching = 0;
                if (level.twar_use_obj.interactteam == "any")
                    number_touching = level.twar_use_obj.numtouching[capture_team_name];
                //mikey_debug(va("--- number_touching: %s [%s]", number_touching, capture_team_name));
                set_omnvar_wrapper("ui_twar_touching_" + capture_team_name, number_touching);
            }
        }

        //mikey_debug("--- LOOP END ---\n");

        wait 0.05; // another wait
    }
}

is_maxed_momentum(var_0) // TODO
{
    return level.twar_team_multiplier[var_0] == level.momentum_multiplier_max;
}

set_maxed_momentum(var_0, var_1)
{
    thread clear_max_momentum_delayed(var_0, var_1);
    set_momentum(var_0, 0.0);
    set_omnvar_wrapper("ui_twar_momentum_maxed_time", var_1);
    set_omnvar_wrapper("ui_twar_momentum_end_time_" + var_0, gettime() + int(1000 * var_1));
}

clear_max_momentum_delayed(var_0, var_1)
{
    level endon("clear_max_momentum_" + var_0);
    wait(var_1);
    thread clear_maxed_momentum(var_0);
}

clear_maxed_momentum(var_0)
{
    level notify("clear_max_momentum_" + var_0);

    if (!is_maxed_momentum(var_0))
        return;

    set_momentum(var_0, 0.0);
    set_momentum_multiplier(var_0, level.momentum_multiplier_max - 1);
    set_omnvar_wrapper("ui_twar_momentum_end_time_" + var_0, 0);
}

clear_momentum(var_0)
{
    if (level.twar_team_multiplier[var_0] == 1)
        maps\mp\_utility::leaderdialog("mtm_clrd", var_0, "momentum_down");
    else
        maps\mp\_utility::leaderdialog("mtm_reset", var_0, "momentum_down");

    clear_maxed_momentum(var_0);
    var_1 = level.twar_team_momentum[var_0] + level.twar_team_multiplier[var_0] - 1;
    add_momentum(var_0, -1 * var_1);
}

add_momentum(var_0, var_1, var_2)
{
    if (var_1 == 0)
        return;

    if (level.momentum_multiplier_max <= 1)
        return;

    if (!isdefined(var_2))
        var_2 = 0;

    if (!var_2 && is_maxed_momentum(var_0))
        return;

    var_3 = level.twar_team_momentum[var_0];
    var_4 = level.twar_team_multiplier[var_0];
    var_5 = 0;

    for (var_3 += var_1; var_3 <= 0.0 && var_4 > 1; var_5 = 1)
    {
        var_3 += 1.0;
        var_4--;
    }

    if (!var_5)
    {
        while (var_3 >= 1.0 && var_4 < level.momentum_multiplier_max)
        {
            var_3 -= 1.0;
            var_4++;
            var_5 = 1;
        }
    }

    set_momentum(var_0, var_3);

    if (var_5)
        set_momentum_multiplier(var_0, var_4);

    if (is_maxed_momentum(var_0))
    {
        var_6 = getdvarfloat("scr_twar_maxed_time", 20.0);
        set_maxed_momentum(var_0, var_6);
    }
    else
        set_omnvar_wrapper("ui_twar_momentum_" + var_0, level.twar_team_momentum[var_0]);
}

set_momentum(var_0, var_1)
{
    var_1 = clamp(var_1, 0.0, 1.0);
    level.twar_team_momentum[var_0] = var_1;
    set_omnvar_wrapper("ui_twar_momentum_" + var_0, level.twar_team_momentum[var_0]);
}

set_momentum_multiplier(var_0, var_1)
{
    var_2 = level.twar_team_multiplier[var_0];
    level.twar_team_multiplier[var_0] = var_1;

    if (var_2 != var_1)
    {
        set_omnvar_wrapper("ui_twar_momentum_scale_" + var_0, var_1);
        level.twar_use_obj maps\mp\gametypes\_gameobjects::updateuserate();

        if (var_2 > var_1)
        {
            if (var_2 != level.momentum_multiplier_max)
                maps\mp\_utility::leaderdialog("mtm_lost", var_0, "momentum_down");
        }
        else if (is_maxed_momentum(var_0))
        {
            maps\mp\_utility::leaderdialog("mtm_max", var_0);

            if (!game["max_meter"][var_0])
            {
                game["max_meter"][var_0] = 1;

                foreach (var_4 in level.players)
                {
                    if (var_4.team != var_0)
                        continue;

                    var_4 maps\mp\gametypes\_missions::processchallenge("ch_twar_blitzkrieg");
                }
            }
        }
        else
            maps\mp\_utility::leaderdialog("mtm_gain", var_0);
    }
}

add_kill_enemy_momentum(var_0)
{
    var_1 = getdvarfloat("scr_twar_momentum_kill_enemy", 0.1);
    add_momentum(var_0, var_1);
}

add_kill_friendly_momentum(var_0)
{
    var_1 = getdvarfloat("scr_twar_momentum_kill_friendly", -0.1);
    add_momentum(var_0, var_1);
}

add_capture_friendly_momentum(var_0)
{
    var_1 = getdvarfloat("scr_twar_momentum_capture_friendly", 0.2);
    add_momentum(var_0, var_1);
}

add_capture_enemy_momentum(var_0)
{
    var_1 = getdvarfloat("scr_twar_momentum_capture_enemy", -0.2);
    add_momentum(var_0, var_1);
}

init_momentum(var_0)
{
    level.twar_team_multiplier[var_0] = 1;
    level.twar_team_momentum[var_0] = 0;
    set_omnvar_wrapper("ui_twar_momentum_end_time_" + var_0, 0);
    set_omnvar_wrapper("ui_twar_momentum_" + var_0, level.twar_team_momentum[var_0]);
    set_omnvar_wrapper("ui_twar_momentum_scale_" + var_0, level.twar_team_multiplier[var_0]);

    if (level.momentum_multiplier_max <= 1)
        return;

    level thread init_overtime_momentum(var_0);
}

init_overtime_momentum(var_0)
{
    maps\mp\_utility::gameflagwait("prematch_done");

    if (game["status"] == "overtime")
    {
        var_1 = 0;
        var_2 = game["owned_flags"][var_0];

        if (var_2 == 4)
            var_1 = 2.0;
        else if (var_2 == 3)
            var_1 = 1.0;

        add_momentum(var_0, var_1);
    }
}

initspawns()
{
    level.spawnmins = (0, 0, 0);
    level.spawnmaxs = (0, 0, 0);
    level.start_spawn_prefix = "mp_twar_spawn_";
    level.start_spawn_allies = "mp_twar_spawn_allies_start";
    level.start_spawn_axis = "mp_twar_spawn_axis_start";

    if (!getspawnarray(level.start_spawn_allies).size)
    {
        level.start_spawn_prefix = "mp_tdm_spawn_";
        level.start_spawn_allies = "mp_tdm_spawn_allies_start";
        level.start_spawn_axis = "mp_tdm_spawn_axis_start";
    }

    maps\mp\gametypes\_spawnlogic::addstartspawnpoints(level.start_spawn_allies);
    maps\mp\gametypes\_spawnlogic::addstartspawnpoints(level.start_spawn_axis);
    level.spawn_name = "mp_twar_spawn";

    if (!getspawnarray(level.spawn_name).size)
        level.spawn_name = "mp_tdm_spawn";

    maps\mp\gametypes\_spawnlogic::addspawnpoints("allies", level.spawn_name);
    maps\mp\gametypes\_spawnlogic::addspawnpoints("axis", level.spawn_name);
    level.mapcenter = maps\mp\gametypes\_spawnlogic::findboxcenter(level.spawnmins, level.spawnmaxs);
    setmapcenter(level.mapcenter);
}

getspawnpoint()
{
    var_0 = self.pers["team"];

    if (game["switchedsides"])
        var_0 = maps\mp\_utility::getotherteam(var_0);

    if (level.usestartspawns && level.ingraceperiod)
    {
        var_1 = maps\mp\gametypes\_spawnlogic::getspawnpointarray(level.start_spawn_prefix + var_0 + "_start");
        var_2 = maps\mp\gametypes\_spawnlogic::getspawnpoint_startspawn(var_1);
    }
    else
    {
        var_1 = [];
        var_3 = [];

        if (level.twar_zones.size == 1 && level.spawn_version != 3)
            var_1 = level.single_zone_spawns[var_0];
        else if (level.spawn_version == 1)
        {
            var_4 = maps\mp\gametypes\_spawnlogic::getteamspawnpoints(var_0);

            foreach (var_2 in var_4)
            {
                if (var_2.nearestzone.owner == var_0)
                {
                    var_1[var_1.size] = var_2;
                    continue;
                }

                if (var_2.nearestzone.owner == "none")
                    var_3[var_3.size] = var_2;
            }
        }
        else if (level.spawn_version == 2)
        {
            for (var_7 = 0; var_7 < level.twar_zones.size; var_7++)
            {
                var_8 = var_7;

                if (var_0 == "allies")
                    var_8 = level.twar_zones.size - 1 - var_7;

                var_9 = level.twar_zones[var_8];

                if (var_9.owner == var_0)
                {
                    var_1 = var_9.nearspawns;
                    break;
                }
                else if (var_9.owner == "none")
                    var_3 = var_9.nearspawns;
            }
        }
        else if (level.spawn_version == 3)
        {
            var_10 = level.twar_use_obj.zone;
            var_1 = var_10.nearspawns[var_0];
        }

        if (var_1.size == 0)
            var_1 = var_3;

        var_2 = maps\mp\gametypes\_spawnscoring::getspawnpoint_twar(var_1, level.twar_use_obj.zone);
    }

    maps\mp\gametypes\_spawnlogic::recon_set_spawnpoint(var_2);
    return var_2;
}

onnormaldeath(var_0, var_1, var_2)
{
    if (isdefined(var_1))
        add_kill_enemy_momentum(var_1.team);

    if (isdefined(var_0.team))
        add_kill_friendly_momentum(var_0.team);
}

get_start_spawn_centers(var_0)
{
    var_1 = [];
    var_2 = [ "allies", "axis" ];
    var_3 = [];

    foreach (var_5 in var_2)
        var_3[var_5] = maps\mp\gametypes\_spawnlogic::getspawnpointarray(level.start_spawn_prefix + var_5 + "_start");

    foreach (var_5 in var_2)
    {
        var_8 = (0, 0, 0);

        foreach (var_10 in var_3[var_5])
            var_8 += var_10.origin;

        var_8 /= var_3[var_5].size;
        var_1[var_5] = var_8;
    }

    if (var_0)
    {
        var_13 = getallnodes();

        foreach (var_5, var_8 in var_1)
        {
            var_15 = 0;

            for (var_16 = 0; var_16 < 10 && var_16 < var_13.size; var_16++)
            {
                var_17 = getpathdist(var_8, var_13[var_16].origin, 99999, 1);

                if (var_17 > 0)
                {
                    var_15 = 1;
                    break;
                }
            }

            if (!var_15)
                var_1[var_5] = var_3[var_5][0].origin;
        }
    }

    return var_1;
}

find_zones()
{
    if (!isdefined(game["zone_origins"]) || game["status"] == "overtime")
        game["zone_origins"] = get_zone_origins();

    var_0 = 5;
    level.num_zones = getdvarint("scr_twar_zone_count", var_0);

    if (level.num_zones <= 0)
        level.num_zones = var_0;

    if (game["status"] == "overtime")
        level.num_zones = getdvarint("scr_twar_ot_zone_count", 3);

    if (game["zone_origins"].size > level.num_zones)
    {
        var_1 = int((game["zone_origins"].size - level.num_zones) / 2);
        var_2 = [];

        for (var_3 = var_1; var_3 <= game["zone_origins"].size - 1 - var_1; var_3++)
            var_2[var_2.size] = game["zone_origins"][var_3];

        game["zone_origins"] = var_2;
    }

    // TODO
    set_omnvar_wrapper("ui_twar_flag_count", level.num_zones);
    mikey_debug(va("twar flag count: %s", level.num_zones));
    level.twar_zones = [];
    mikey_debug("twar_zones created");

    foreach (index, zone_spawns in game["zone_origins"])
    {
        zone = twar_zone(index, zone_spawns.origin, zone_spawns.angles, color_from_index(index));
        level.twar_zones[index] = zone;
        mikey_debug(va("adding twar zone %s", index));
    }

    mikey_debug(va("twar_zones size is %s", level.twar_zones.size));
}

get_zone_origins()
{
    var_0 = [];
    var_1 = common_scripts\utility::getstructarray("twar_zone", "targetname");

    foreach (var_3 in var_1)
    {
        if (!isdefined(var_3.script_index))
            return var_0;
    }

    if (maps\mp\_utility::isaugmentedgamemode())
    {
        var_5 = common_scripts\utility::getstructarray("twar_zone_augmented", "targetname");

        foreach (var_7 in var_5)
        {
            if (!isdefined(var_7.script_index))
                continue;

            foreach (var_9, var_3 in var_1)
            {
                if (var_3.script_index == var_7.script_index)
                    var_1[var_9] = var_7;
            }
        }
    }

    if (var_1.size < 3 || var_1.size > 7)
        return var_0;

    var_1 = maps\mp\_utility::quicksort(var_1, ::quicksort_flag_compare);

    foreach (var_3 in var_1)
    {
        var_12 = twarzoneangleoverride(var_3);
        var_13 = spawnstruct();
        var_13.origin = var_3.origin;

        if (isdefined(var_12))
            var_13.angles = var_12;
        else
            var_13.angles = var_3.script_angles;

        var_0[var_0.size] = var_13;
    }

    level.num_zones = var_0.size;
    return var_0;
}

twarzoneangleoverride(var_0)
{
    var_1 = maps\mp\_utility::getmapname();
    var_2 = undefined;
    var_3 = var_0.script_index;

    switch (var_1)
    {
        case "mp_detroit":
            if (var_3 == 5)
                var_2 = (0, 215, 0);

            if (var_3 == 1)
                var_2 = (0, 245, 0);

            break;
        case "mp_instinct":
            if (var_3 == 1)
                var_2 = (0, 190, 0);

            if (var_3 == 2)
                var_2 = (0, 285, 0);

            if (var_3 == 4)
                var_2 = (0, 340, 0);

            if (var_3 == 5)
                var_2 = (0, 350, 0);

            break;
        default:
            break;
    }

    return var_2;
}

quicksort_flag_compare(var_0, var_1)
{
    return var_0.script_index <= var_1.script_index;
}

get_zone_origins_auto()
{
    level.num_zones = getdvarint("scr_twar_zone_count", 5);
    var_0 = get_start_spawn_centers(1);
    var_1 = [ "allies", "axis" ];
    var_2 = getallnodes();
    var_3 = level.num_zones;
    var_4 = getdvarfloat("scr_twar_auto_zone_spacing", 0.15);
    var_5 = getdvarfloat("scr_twar_auto_zone_zig_zag", 0.1);
    var_6 = getdvarint("scr_twar_auto_zone_allow_traversals", 0);
    var_7 = getdvarint("scr_twar_auto_zone_sky_only", 1);
    var_8 = [];

    for (var_9 = 0; var_9 < level.num_zones; var_9++)
    {
        var_10 = (var_9 + 1) / (level.num_zones + 1 - var_9 + 1);
        var_11 = (2 * (var_9 + 1) - 1) / (2 * (level.num_zones + 1 - var_9 + 1) + 1);
        var_12 = (2 * (var_9 + 1) + 1) / (2 * (level.num_zones + 1 - var_9 + 1) - 1);
        var_8[var_9]["min"] = var_10 - var_4 * (var_10 - var_11);
        var_8[var_9]["max"] = var_10 + var_4 * (var_12 - var_10);
    }

    var_13 = [];

    for (var_9 = 0; var_9 < var_8.size; var_9++)
        var_13[var_9] = [];

    foreach (var_15 in var_2)
    {
        if (var_7 && !nodeexposedtosky(var_15, 1))
            continue;

        var_16 = [];

        foreach (var_18 in var_1)
            var_16[var_18] = getpathdist(var_0[var_18], var_15.origin, 99999, var_6);

        if (var_16["allies"] <= 0 || var_16["axis"] <= 0)
            continue;

        var_20 = var_16["allies"] / var_16["axis"];

        for (var_9 = 0; var_9 < var_8.size; var_9++)
        {
            if (var_20 > var_8[var_9]["min"] && var_20 < var_8[var_9]["max"])
                var_13[var_9][var_13[var_9].size] = var_15;
        }
    }

    var_22 = [];
    var_23 = var_0["allies"];

    foreach (var_9, var_25 in var_13)
    {
        var_25 = sortbydistance(var_25, var_23);
        var_26 = int(clamp(2 * var_25.size * var_5 - var_25.size, 0, var_25.size));
        var_27 = int(clamp(2 * var_25.size * var_5, 0, var_25.size));

        if (var_26 < var_27)
            var_28 = randomintrange(var_26, var_27);
        else
            var_28 = int(clamp(var_26, 0, var_25.size - 1));

        var_29 = spawnstruct();
        var_29.origin = var_25[var_28].origin;
        var_22[var_9] = var_29;
        var_23 = var_25[var_28].origin;
    }

    return var_22;
}

color_from_index(var_0)
{
    return (var_0 & 4, var_0 & 2, var_0 & 1);
}

twar_zone(var_0, var_1, var_2, var_3)
{
    twar_zone_struct = spawnstruct();
    var_5 = var_1 + (0, 0, 32);
    var_6 = var_1 + (0, 0, -64);
    var_7 = bullettrace(var_5, var_6, 0, undefined);
    twar_zone_struct.origin = var_7["position"];
    twar_zone_struct.owner = "none";
    twar_zone_struct.index = var_0;
    twar_zone_struct.angles = var_2;
    twar_zone_struct.debug_color = var_3;
    twar_zone_struct.projector = spawn_flag_projector(twar_zone_struct.origin);
    return twar_zone_struct;
}

draw_spawn_until_notify(var_0, var_1, var_2)
{
    level endon(var_2);
    var_3 = anglestoforward(var_0.angles);
    level thread draw_line_until_notify(var_0.origin, var_0.origin + var_3 * 50, var_1, var_2);
    var_0.debug_draw = 1;

    for (;;)
        wait 0.05;
}

draw_line_until_notify(var_0, var_1, var_2, var_3)
{
    level endon(var_3);

    for (;;)
        wait 0.05;
}

draw_circle_until_notify(var_0, var_1, var_2, var_3, var_4)
{
    if (isdefined(var_4))
        var_5 = var_4;
    else
        var_5 = 16;

    var_6 = 360 / var_5;
    var_7 = [];

    for (var_8 = 0; var_8 < var_5; var_8++)
    {
        var_9 = var_6 * var_8;
        var_10 = cos(var_9) * var_1;
        var_11 = sin(var_9) * var_1;
        var_12 = var_0[0] + var_10;
        var_13 = var_0[1] + var_11;
        var_14 = var_0[2];
        var_7[var_7.size] = (var_12, var_13, var_14);
    }

    for (var_8 = 0; var_8 < var_7.size; var_8++)
    {
        var_15 = var_7[var_8];

        if (var_8 + 1 >= var_7.size)
            var_16 = var_7[0];
        else
            var_16 = var_7[var_8 + 1];

        thread draw_line_until_notify(var_15, var_16, var_2, var_3);
    }
}

assign_spawns()
{
    if (level.spawn_version == 2)
        assign_spawns_version_2();
    else if (level.spawn_version == 3)
        assign_spawns_version_3();
}

assign_spawns_version_2()
{
    if (level.twar_zones.size == 1)
    {
        level.single_zone_spawns = [];
        level.single_zone_spawns["allies"] = [];
        level.single_zone_spawns["axis"] = [];
        var_0 = getnearestspawns(level.twar_zones[0], 12);

        foreach (var_2 in var_0)
        {
            var_3 = maps\mp\gametypes\_spawnlogic::getspawnpointarray(level.start_spawn_prefix + "allies_start")[0];
            var_4 = twar_dist(var_2.origin, var_3.origin);
            var_3 = maps\mp\gametypes\_spawnlogic::getspawnpointarray(level.start_spawn_prefix + "axis_start")[0];
            var_5 = twar_dist(var_2.origin, var_3.origin);
            var_6 = common_scripts\utility::ter_op(var_4 < var_5, "allies", "axis");
            var_7 = level.single_zone_spawns[var_6].size;
            level.single_zone_spawns[var_6][var_7] = var_2;
        }
    }
    else
    {
        foreach (var_10 in level.twar_zones)
            var_10.nearspawns = getnearestspawns(var_10, 6);

        var_12 = maps\mp\gametypes\_spawnlogic::getteamspawnpoints("axis");

        foreach (var_14 in var_12)
        {
            var_14.nearestzone = getnearestzonepoint(var_14);

            if (!common_scripts\utility::array_contains(var_14.nearestzone.nearspawns, var_14))
            {
                var_7 = var_14.nearestzone.nearspawns.size;
                var_14.nearestzone.nearspawns[var_7] = var_14;
            }
        }
    }
}

get_zone_dir(var_0)
{
    if (isdefined(level.twar_zones[var_0].angles))
        return anglestoforward(level.twar_zones[var_0].angles);
    else
    {
        var_3 = get_start_spawn_centers(0);
        var_4 = var_3["axis"] - var_3["allies"];
        var_4 = (var_4[0], var_4[1], 0);
        return vectornormalize(var_4);
    }
}

assign_spawns_version_3()
{
    var_0 = maps\mp\gametypes\_spawnlogic::getteamspawnpoints("axis");
    var_1 = var_0.size;

    foreach (var_14, var_3 in level.twar_zones)
    {
        var_4 = 9;
        var_3.nearspawns["all"] = getnearestspawns(var_3, 24, level.zone_radius * 3, 0.0);
        var_3.dir = get_zone_dir(var_14);

        foreach (var_6 in var_3.nearspawns["all"])
        {
            var_7 = vectornormalize(var_3.origin - var_6.origin);
            var_6.dot = vectordot(var_7, var_3.dir);
        }

        var_3.nearspawns["all"] = maps\mp\_utility::quicksort(var_3.nearspawns["all"], ::twar_spawn_dot);
        var_3.nearspawns["allies"] = [];
        var_3.nearspawns["axis"] = [];

        for (var_9 = 0; var_9 < var_3.nearspawns["all"].size; var_9++)
        {
            var_10 = int(var_9 / 2);
            var_11 = "axis";

            if (var_9 % 2 == 1)
            {
                var_10 = var_3.nearspawns["all"].size - int((var_9 + 1) / 2);
                var_11 = "allies";
            }

            var_12 = undefined;
            var_6 = var_3.nearspawns["all"][var_10];

            if (var_3.nearspawns[var_11].size < var_4)
                var_12 = var_11;
            else if (var_6.dot > 0)
                var_12 = "allies";
            else
                var_12 = "axis";

            if (isdefined(var_12))
            {
                var_13 = var_3.nearspawns[var_12].size;
                var_3.nearspawns[var_12][var_13] = var_6;
            }
        }
    }
}

twar_dist(var_0, var_1)
{
    var_2 = maps\mp\gametypes\_spawnlogic::ispathdataavailable();
    var_3 = -1;

    if (var_2)
        var_3 = getpathdist(var_0, var_1, 999999);

    if (var_3 == -1)
        var_3 = distance(var_0, var_1);

    return var_3;
}

getnearestspawns(var_0, var_1, var_2, var_3)
{
    if (!isdefined(var_2))
        var_2 = 0;

    var_4 = maps\mp\gametypes\_spawnlogic::ispathdataavailable();
    var_5 = maps\mp\gametypes\_spawnlogic::getteamspawnpoints("axis");

    foreach (var_7 in var_5)
    {
        var_7.dist = -1;

        if (var_4)
            var_7.dist = getpathdist(var_7.origin, var_0.origin, 999999);

        if (var_7.dist == -1)
            var_7.dist = distance(var_0.origin, var_7.origin);
    }

    var_5 = maps\mp\_utility::quicksort(var_5, ::twar_spawn_dist);
    var_9 = maps\mp\_utility::getmapname();
    var_10 = game["status"] == "overtime";
    var_11 = var_0.index;

    if (var_10)
        var_11 += 1;

    var_12 = [];

    for (var_13 = 0; var_13 < var_5.size && var_12.size < var_1; var_13++)
    {
        var_7 = var_5[var_13];

        if (var_7.dist < var_2)
            continue;

        switch (var_9)
        {
            case "mp_instinct":
                if (var_11 == 0)
                {
                    if (var_7.index == 1)
                        continue;
                }
                else if (var_11 == 1)
                {
                    if (var_7.index == 14)
                        continue;
                }
                else if (var_11 == 3)
                {
                    if (var_7.index == 16)
                        continue;
                }
                else if (var_11 == 4)
                {
                    if (var_7.index == 16)
                        continue;
                }

                break;
        }

        var_12[var_12.size] = var_7;
    }

    return var_12;
}

twar_spawn_dot(var_0, var_1)
{
    return var_0.dot <= var_1.dot;
}

twar_spawn_dist(var_0, var_1)
{
    return var_0.dist <= var_1.dist;
}

getnearestzonepoint(var_0)
{
    var_1 = maps\mp\gametypes\_spawnlogic::ispathdataavailable();
    var_2 = undefined;
    var_3 = undefined;

    foreach (var_5 in level.twar_zones)
    {
        var_6 = undefined;

        if (var_1)
            var_6 = getpathdist(var_0.origin, var_5.origin, 999999);

        if (!isdefined(var_6) || var_6 == -1)
            var_6 = distance(var_5.origin, var_0.origin);

        if (!isdefined(var_2) || var_6 < var_3)
        {
            var_2 = var_5;
            var_3 = var_6;
        }
    }

    return var_2;
}

/*
onspawnplayer()
{
    self.minionstreak = 0;
}

updateminions()
{
    level endon("game_ended");

    while (!isdefined(level.agentarray))
        waitframe();

    maps\mp\_utility::gameflagwait("prematch_done");
    var_0 = getdvarint("scr_twar_minionsmax", 18);

    if (var_0 <= 0)
        return;

    update_minion_hud_outlines();
    var_1 = [ "allies", "axis" ];
    var_2 = getdvarfloat("scr_twar_minionspawndelay", 10.0);
    var_3 = [];
    var_4 = undefined;
    var_5 = undefined;
    var_6 = getdvarint("scr_twar_minionspawnhud", 0);

    if (var_6 > 0)
    {
        var_4 = minion_spawn_timer_hud();
        var_4 hud_set_visible();

        if (var_6 > 1)
        {
            var_5 = minion_max_hud();

            foreach (var_8 in var_1)
            {
                foreach (var_10 in var_1)
                    var_3[var_8][var_10] = minion_count_hud(var_8, var_10);
            }
        }
    }

    while (!level.gameended)
    {
        if (isdefined(var_4))
            var_4 settimer(var_2);

        wait(var_2);
        maps\mp\gametypes\_hostmigration::waittillhostmigrationdone();
        var_13 = getminioncount();
        var_14 = level.num_zones - 1;

        if (var_13 + var_14 < var_0)
        {
            var_15 = [];

            foreach (var_17 in var_1)
                var_15[var_17] = 0;

            foreach (var_20 in level.twar_zones)
            {
                var_17 = var_20.owner;

                if (var_17 != "allies" && var_17 != "axis")
                    continue;

                var_21 = undefined;
                var_22 = undefined;
                var_23 = maps\mp\agents\_agents::add_humanoid_agent("player", var_17, "minion", var_21, var_22, undefined, 0, 0, "recruit");

                if (isdefined(var_23))
                {
                    var_23 takeallweapons();
                    var_24 = getdvarint("scr_twar_minionweapon", 0);
                    var_25 = "";

                    switch (var_24)
                    {
                        case 2:
                            var_25 = "h1_p90_mp";
                            break;
                        case 1:
                            var_25 = "h1_m21_mp";
                            break;
                        case 0:
                        default:
                            var_25 = "h1_usp_mp";
                            break;
                    }

                    var_23 giveweapon(var_25);
                    var_23 switchtoweaponimmediate(var_25);
                    var_23 maps\mp\_utility::giveperk("specialty_minion", 0);
                    var_23.movespeedscaler = getdvarfloat("scr_twar_minionmovespeedscale", 0.85);
                    var_23.damage_scale = getdvarfloat("scr_twar_miniondamagescale", 0.5);
                    var_23.agentname = &"MP_MINION";
                    var_23.nonkillstreakagent = 1;
                    var_23 thread minion_ai();
                    update_minion_hud_outlines();
                    var_23 detachall();
                    var_23 setmodel("kva_hazmat_body_infected_mp");
                    var_23 attach("kva_hazmat_head_infected");
                    var_23 setclothtype("cloth");
                    var_26 = getdvarfloat("scr_twar_minionhealthscale", 0.75);
                    var_23 maps\mp\agents\_agent_common::set_agent_health(int(var_23.health * var_26));
                    var_15[var_17]++;
                }
            }

            foreach (var_8, var_29 in var_3)
            {
                foreach (var_10, var_31 in var_29)
                {
                    var_31 hud_set_visible();
                    var_31 setvalue(var_15[var_10]);
                    var_31 maps\mp\_utility::delaythread(3.0, ::hud_set_invisible);
                }
            }

            continue;
        }

        if (isdefined(var_5))
        {
            var_5 hud_set_visible();
            var_5 maps\mp\_utility::delaythread(3.0, ::hud_set_invisible);
        }
    }
}

is_minion()
{
    return self hasperk("specialty_minion", 1);
}

hud_set_visible()
{
    self.alpha = 1.0;
}

hud_set_invisible()
{
    self.alpha = 0.0;
}

minion_max_hud()
{
    var_0 = maps\mp\gametypes\_hud_util::createserverfontstring("hudbig", 1.0);
    var_0 maps\mp\gametypes\_hud_util::setpoint("BOTTOM", undefined, 0, -20);
    var_0.label = &"MP_DOMAI_MINIONS_SPAWNED_MAX";
    var_0.color = (1, 0, 0);
    var_0.archived = 1;
    var_0.showinkillcam = 1;
    var_0.alpha = 0.0;
    return var_0;
}

minion_count_hud(var_0, var_1)
{
    var_2 = maps\mp\gametypes\_hud_util::createserverfontstring("hudbig", 1.0, var_0);
    var_2 maps\mp\gametypes\_hud_util::setpoint("BOTTOM", undefined, 0, common_scripts\utility::ter_op(var_0 == var_1, -40, -20));
    var_2.label = common_scripts\utility::ter_op(var_0 == var_1, &"MP_DOMAI_MINIONS_SPAWNED_FRIENDLY", &"MP_DOMAI_MINIONS_SPAWNED_ENEMY");
    var_2.color = common_scripts\utility::ter_op(var_0 == var_1, (0.157, 0.392, 0.784), (0.784, 0.49, 0.157));
    var_2.archived = 1;
    var_2.showinkillcam = 1;
    var_2.alpha = 0.0;
    return var_2;
}

minion_spawn_timer_hud()
{
    var_0 = maps\mp\gametypes\_hud_util::createservertimer("hudbig", 1.0);
    var_0 maps\mp\gametypes\_hud_util::setpoint("BOTTOM", undefined, 0, -60);
    var_0.label = &"MP_DOMAI_MINIONS_SPAWN_TIMER";
    var_0.color = (1, 1, 1);
    var_0.archived = 1;
    var_0.showinkillcam = 1;
    return var_0;
}

update_minion_hud_outlines()
{
    var_0 = [];
    var_1 = [];
    var_2 = getdvarint("scr_twar_minionoutline", 0);

    foreach (var_4 in level.players)
    {
        if (var_4.team == "allies")
        {
            var_0[var_0.size] = var_4;
            continue;
        }

        var_1[var_1.size] = var_4;
    }

    foreach (var_7 in level.agentarray)
    {
        if (var_7 is_minion())
        {
            if (level.players.size > 0)
                var_7 hudoutlinedisableforclients(level.players);

            if (var_2)
            {
                if (var_0.size > 0)
                    var_7 hudoutlineenableforclients(var_0, common_scripts\utility::ter_op(var_7.team == "allies", 2, 3), 1);

                if (var_1.size > 0)
                    var_7 hudoutlineenableforclients(var_1, common_scripts\utility::ter_op(var_7.team == "axis", 2, 3), 1);
            }
        }
    }
}

minion_ai()
{
    self endon("death");

    for (;;)
    {
        if (isdefined(level.twar_use_obj))
        {
            var_0 = level.twar_use_obj.trigger.origin;
            self botsetscriptgoal(var_0, level.zone_radius * 0.9, "objective");
        }

        wait 0.1;
    }
}

getminioncount()
{
    var_0 = 0;

    foreach (var_2 in level.agentarray)
    {
        if (isdefined(var_2.isactive) && var_2.isactive && var_2.agent_type == "player" && var_2 is_minion())
            var_0++;
    }

    return var_0;
}

// in AW, var_0 from _agents::modifyplayerdamage function isn't passed through here???
//(victim, unk, eattacker, idamage, smeansofdeath, sweapon, vpoint, vdir, shitloc)
minion_damage(victim, eattacker, idamage, var_3, var_4, var_5, var_6, var_7)
{
    if (isdefined(eattacker) && isdefined(eattacker.damage_scale))
        idamage = int(idamage * eattacker.damage_scale);

    return idamage;
}

on_minion_killed(var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8)
{
    if (isplayer(var_1) && is_minion() && var_1.team != self.team)
    {
        var_9 = getdvarfloat("scr_twar_score_kill_minion_multipler", 0);

        if (var_9 > 0)
        {
            var_10 = getdvarfloat("scr_twar_score_kill_minion_base", 10);
            var_11 = int(var_1.minionstreak * var_9 + var_10);
            var_11 = min(var_11, getdvarint("scr_twar_score_kill_minion_max", 150));
            setdvar("scr_twar_score_kill_minion", var_11);
        }

        var_1.minionstreak++;
        level thread maps\mp\gametypes\_rank::awardgameevent("kill_minion", var_1, var_4, self, var_3);

        if (isdefined(var_1))
            add_kill_enemy_minion_momentum(var_1.team);

        if (isdefined(self.team))
            add_kill_friendly_minion_momentum(self.team);
    }
}

add_kill_enemy_minion_momentum(var_0)
{
    var_1 = getdvarfloat("scr_twar_momentum_kill_enemy_minion", 0.1);
    add_momentum(var_0, var_1);
}

add_kill_friendly_minion_momentum(var_0)
{
    var_1 = getdvarfloat("scr_twar_momentum_kill_friendly_minion", -0.1);
    add_momentum(var_0, var_1);
}
*/

onplayerkilled(var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9)
{
    if (!isplayer(var_1))
        return;

    if (maps\mp\gametypes\_damage::isfriendlyfire(self, var_1))
        return;

    if (var_1 == self)
        return;

    if (isdefined(var_4) && maps\mp\_utility::iskillstreakweapon(var_4))
        return;

    var_10 = 0;
    var_11 = self;

    foreach (var_13 in var_1.touchtriggers)
    {
        if (var_13 != level.twar_use_obj.trigger)
            continue;

        var_1 thread maps\mp\_events::killwhilecapture(var_11, var_9);
        var_10 = 1;
        break;
    }

    if (!var_10)
    {
        foreach (var_13 in var_11.touchtriggers)
        {
            if (var_13 != level.twar_use_obj.trigger)
                continue;

            var_1 thread maps\mp\_events::assaultobjectiveevent(self, var_9);
        }
    }
}

// utils
mikey_debug(string)
{
    print(va("[mikey_debug] %s", string));
}

set_omnvar_wrapper(name, value)
{
    //setomnvar(name, value);
    setdvar(name, value);
}

get_omnvar_wrapper(name)
{
    //return getomnvar(name, value);
    value = getdvarint(name);
    return value;
}
