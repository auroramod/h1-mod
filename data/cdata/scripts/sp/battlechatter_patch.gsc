main()
{
    replacefunc(animscripts\battlechatter::init_battlechatter, ::init_battlechatter);
}

init_battlechatter()
{
    if ( isdefined( anim.chatinitialized ) && anim.chatinitialized )
        return;

    if ( getdvar( "bcs_enable" ) == "" )
        setdvar( "bcs_enable", "on" );

    if ( getdvar( "bcs_enable" ) == "off" )
    {
        anim.chatinitialized = 0;
        anim.player.chatinitialized = 0;
        return;
    }

    anim.chatinitialized = 1;
    anim.player.chatinitialized = 0;

    if ( getdvar( "bcs_filterThreat" ) == "" )
        setdvar( "bcs_filterThreat", "off" );

    if ( getdvar( "bcs_filterInform" ) == "" )
        setdvar( "bcs_filterInform", "off" );

    if ( getdvar( "bcs_filterOrder" ) == "" )
        setdvar( "bcs_filterOrder", "off" );

    if ( getdvar( "bcs_filterReaction" ) == "" )
        setdvar( "bcs_filterReaction", "off" );

    if ( getdvar( "bcs_filterResponse" ) == "" )
        setdvar( "bcs_filterResponse", "off" );

    if ( getdvar( "bcs_threatLimitTargettedBySelf" ) == "" )
        setdvar( "bcs_threatLimitTargettedBySelf", "off" );

    if ( getdvar( "bcs_threatLimitTargetingPlayer" ) == "" )
        setdvar( "bcs_threatLimitTargetingPlayer", "off" );

    if ( getdvar( "bcs_threatLimitInPlayerFOV" ) == "" )
        setdvar( "bcs_threatLimitInPlayerFOV", "on" );

    if ( getdvar( "bcs_threatLimitInLocation" ) == "" )
        setdvar( "bcs_threatLimitInLocation", "on" );

    if ( getdvar( "bcs_threatLimitSpeakerDist" ) == "" )
        setdvar( "bcs_threatLimitSpeakerDist", "512" );

    if ( getdvar( "bcs_threatLimitThreatDist" ) == "" )
        setdvar( "bcs_threatLimitThreatDist", "2048" );

    if ( getdvar( "bcs_threatPlayerRelative" ) == "" )
        setdvar( "bcs_threatPlayerRelative", "off" );

    if ( getdvar( "debug_bcprint" ) == "" )
        setdvar( "debug_bcprint", "off" );

    if ( getdvar( "debug_bcshowqueue" ) == "" )
        setdvar( "debug_bcshowqueue", "off" );

    if ( getdvar( "debug_bcprintdump" ) == "" )
        setdvar( "debug_bcprintdump", "off" );

    anim.countryids["british"] = "UK";
    anim.countryids["american"] = "US";
    anim.countryids["russian"] = "RU";
    anim.countryids["arab"] = "AB";
    anim.usedids = [];
    anim.usedids["russian"] = [];
    anim.usedids["russian"][0] = spawnstruct();
    anim.usedids["russian"][0].count = 0;
    anim.usedids["russian"][0].npcid = "1";
    anim.usedids["russian"][1] = spawnstruct();
    anim.usedids["russian"][1].count = 0;
    anim.usedids["russian"][1].npcid = "2";
    anim.usedids["russian"][2] = spawnstruct();
    anim.usedids["russian"][2].count = 0;
    anim.usedids["russian"][2].npcid = "3";
    anim.usedids["british"] = [];
    anim.usedids["british"][0] = spawnstruct();
    anim.usedids["british"][0].count = 0;
    anim.usedids["british"][0].npcid = "0";
    anim.usedids["british"][1] = spawnstruct();
    anim.usedids["british"][1].count = 0;
    anim.usedids["british"][1].npcid = "1";
    anim.usedids["british"][2] = spawnstruct();
    anim.usedids["british"][2].count = 0;
    anim.usedids["british"][2].npcid = "2";
    anim.usedids["british"][3] = spawnstruct();
    anim.usedids["british"][3].count = 0;
    anim.usedids["british"][3].npcid = "3";
    anim.usedids["american"] = [];
    anim.usedids["american"][0] = spawnstruct();
    anim.usedids["american"][0].count = 0;
    anim.usedids["american"][0].npcid = "0"; //0 - US_7
    anim.usedids["american"][1] = spawnstruct();
    anim.usedids["american"][1].count = 0;
    anim.usedids["american"][1].npcid = "1";
    anim.usedids["american"][2] = spawnstruct();
    anim.usedids["american"][2].count = 0;
    anim.usedids["american"][2].npcid = "2";
    anim.usedids["american"][3] = spawnstruct();
    anim.usedids["american"][3].count = 0;
    anim.usedids["american"][3].npcid = "3";
    anim.usedids["american"][4] = spawnstruct();
    anim.usedids["american"][4].count = 0;
    anim.usedids["american"][4].npcid = "4"; //4 - US_6
    anim.usedids["arab"] = [];
    anim.usedids["arab"][0] = spawnstruct();
    anim.usedids["arab"][0].count = 0;
    anim.usedids["arab"][0].npcid = "0";
    anim.usedids["arab"][1] = spawnstruct();
    anim.usedids["arab"][1].count = 0;
    anim.usedids["arab"][1].npcid = "1";
    anim.usedids["arab"][2] = spawnstruct();
    anim.usedids["arab"][2].count = 0;
    anim.usedids["arab"][2].npcid = "2";
    anim.usedids["arab"][3] = spawnstruct();
    anim.usedids["arab"][3].count = 0;
    anim.usedids["arab"][3].npcid = "3";
    anim.eventtypeminwait = [];
    anim.eventtypeminwait["threat"] = [];
    anim.eventtypeminwait["response"] = [];
    anim.eventtypeminwait["reaction"] = [];
    anim.eventtypeminwait["order"] = [];
    anim.eventtypeminwait["inform"] = [];
    anim.eventtypeminwait["custom"] = [];
    anim.eventtypeminwait["direction"] = [];

    if ( isdefined( level._stealth ) )
    {
        anim.eventactionminwait["threat"]["self"] = 20000;
        anim.eventactionminwait["threat"]["squad"] = 30000;
    }
    else
    {
        anim.eventactionminwait["threat"]["self"] = 12000;
        anim.eventactionminwait["threat"]["squad"] = 8000;
    }

    anim.eventactionminwait["response"]["self"] = 1000;
    anim.eventactionminwait["response"]["squad"] = 1000;
    anim.eventactionminwait["reaction"]["self"] = 1000;
    anim.eventactionminwait["reaction"]["squad"] = 1000;
    anim.eventactionminwait["order"]["self"] = 8000;
    anim.eventactionminwait["order"]["squad"] = 40000;
    anim.eventactionminwait["inform"]["self"] = 6000;
    anim.eventactionminwait["inform"]["squad"] = 8000;
    anim.eventactionminwait["custom"]["self"] = 0;
    anim.eventactionminwait["custom"]["squad"] = 5000;
    anim.eventtypeminwait["reaction"]["casualty"] = 20000;
    anim.eventtypeminwait["reaction"]["taunt"] = 200000;
    anim.eventtypeminwait["inform"]["reloading"] = 20000;
    anim.eventpriority["threat"]["infantry"] = 0.5;
    anim.eventpriority["threat"]["vehicle"] = 0.7;
    anim.eventpriority["response"]["killfirm"] = 0.8;
    anim.eventpriority["response"]["ack"] = 0.9;
    anim.eventpriority["reaction"]["casualty"] = 0.5;
    anim.eventpriority["reaction"]["taunt"] = 0.9;
    anim.eventpriority["order"]["cover"] = 0.9;
    anim.eventpriority["order"]["action"] = 0.5;
    anim.eventpriority["order"]["move"] = 0.9;
    anim.eventpriority["order"]["displace"] = 0.5;
    anim.eventpriority["inform"]["killfirm"] = 0.6;
    anim.eventpriority["inform"]["attack"] = 0.9;
    anim.eventpriority["inform"]["incoming"] = 0.8;
    anim.eventpriority["inform"]["reloading"] = 0.2;
    anim.eventpriority["inform"]["suppressed"] = 0.2;
    anim.eventpriority["custom"]["generic"] = 1.0;
    anim.eventduration["threat"]["infantry"] = 1000;
    anim.eventduration["threat"]["vehicle"] = 1000;
    anim.eventduration["response"]["killfirm"] = 3000;
    anim.eventduration["response"]["ack"] = 2000;
    anim.eventduration["reaction"]["casualty"] = 2000;
    anim.eventduration["reaction"]["taunt"] = 2000;
    anim.eventduration["order"]["cover"] = 3000;
    anim.eventduration["order"]["action"] = 3000;
    anim.eventduration["order"]["move"] = 3000;
    anim.eventduration["order"]["displace"] = 3000;
    anim.eventduration["inform"]["killfirm"] = 1000;
    anim.eventduration["inform"]["attack"] = 1000;
    anim.eventduration["inform"]["incoming"] = 1000;
    anim.eventduration["inform"]["reloading"] = 1000;
    anim.eventduration["inform"]["suppressed"] = 2000;
    anim.eventduration["custom"]["generic"] = 1000;
    anim.chatcount = 0;
    anim.moveorigin = spawn( "script_origin", ( 0.0, 0.0, 0.0 ) );
    anim.areas = getentarray( "trigger_location", "targetname" );
    anim.locations = getentarray( "trigger_location", "targetname" );
    anim.landmarks = getentarray( "trigger_landmark", "targetname" );
    anim.squadcreatefuncs[anim.squadcreatefuncs.size] = animscripts\battlechatter::init_squadbattlechatter;
    anim.squadcreatestrings[anim.squadcreatestrings.size] = "::init_squadBattleChatter";
    anim.isteamspeaking["allies"] = 0;
    anim.isteamsaying["allies"]["threat"] = 0;
    anim.isteamsaying["allies"]["order"] = 0;
    anim.isteamsaying["allies"]["reaction"] = 0;
    anim.isteamsaying["allies"]["response"] = 0;
    anim.isteamsaying["allies"]["inform"] = 0;
    anim.isteamsaying["allies"]["custom"] = 0;
    anim.isteamspeaking["axis"] = 0;
    anim.isteamsaying["axis"]["threat"] = 0;
    anim.isteamsaying["axis"]["order"] = 0;
    anim.isteamsaying["axis"]["reaction"] = 0;
    anim.isteamsaying["axis"]["response"] = 0;
    anim.isteamsaying["axis"]["inform"] = 0;
    anim.isteamsaying["axis"]["custom"] = 0;
    anim.isteamspeaking["neutral"] = 0;
    anim.isteamsaying["neutral"]["threat"] = 0;
    anim.isteamsaying["neutral"]["order"] = 0;
    anim.isteamsaying["neutral"]["reaction"] = 0;
    anim.isteamsaying["neutral"]["response"] = 0;
    anim.isteamsaying["neutral"]["inform"] = 0;
    anim.isteamsaying["neutral"]["custom"] = 0;

    if ( !isdefined( level.battlechatter ) )
    {
        level.battlechatter = [];
        level.battlechatter["allies"] = 1;
        level.battlechatter["axis"] = 1;
        level.battlechatter["neutral"] = 1;
    }

    anim.lastteamspeaktime = [];
    anim.lastteamspeaktime["allies"] = -5000;
    anim.lastteamspeaktime["axis"] = -5000;

    for ( var_0 = 0; var_0 < anim.squadindex.size; var_0++ )
    {
        if ( isdefined( anim.squadindex[var_0].chatinitialized ) && anim.squadindex[var_0].chatinitialized )
            continue;

        anim.squadindex[var_0] animscripts\battlechatter::init_squadbattlechatter();
    }

    level notify( "battlechatter initialized" );
    anim notify( "battlechatter initialized" );
}