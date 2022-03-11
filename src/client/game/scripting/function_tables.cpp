#include <std_include.hpp>

// This file has been generated.
// Do not touch!

// https://github.com/xensik/gsc-tool/blob/dev/src/h1/xsk/resolver.cpp :)

namespace scripting
{
	std::unordered_map<std::string, unsigned> function_map =
	{
		{"precacheturret", 0x001}, // SP 0x14029D940 MP 0x140368EA0
		{"getweaponarray", 0x002}, // SP 0x14028F750 MP 0x140357B80
		{"createprintchannel", 0x003}, // SP 0x1405D92F0 MP 0x1403668A0 nullsub
		{"updategamerprofileall", 0x004}, // SP 0x1402ACD70 MP 0x000000000
		{"clearlocalizedstrings", 0x005}, // SP 0x1402ADB70 MP 0x000000000
		{"setphysicsgravitydir", 0x006}, // SP 0x1402A8E20 MP 0x000000000
		{"gettimescale", 0x007}, // SP 0x1402A8B20 MP 0x000000000
		{"settimescale", 0x008}, // SP 0x1402A8B40 MP 0x000000000
		{"setslowmotionview", 0x009}, // SP 0x1402A8BC0 MP 0x000000000
		{"teleportscene", 0x00A}, // SP 0x1402A8B60 MP 0x000000000
		{"forcesharedammo", 0x00B}, // SP 0x1402A8960 MP 0x000000000
		{"refreshhudcompass", 0x00C}, // SP 0x1402ADBE0 MP 0x000000000
		{"refreshhudammocounter", 0x00D}, // SP 0x1402ADC60 MP 0x000000000
		{"notifyoncommand", 0x00E}, // SP 0x1402ADC90 MP 0x000000000
		{"setprintchannel", 0x00F}, // SP 0x1405D92F0 MP 0x1403669E0 nullsub
		{"print", 0x010}, // SP 0x14029AF60 MP 0x1403669F0 ^
		{"println", 0x011}, // SP 0x14029AF60 MP 0x140366D40 ^
		{"print3d", 0x012}, // SP 0x1405D92F0 MP 0x140367400 ^
		{"line", 0x013}, // SP 0x1405D92F0 MP 0x140367520 ^
		{"box", 0x014}, // SP 0x1405D92F0 MP 0x140367530 ^
		{"orientedbox", 0x015}, // SP 0x1405D92F0 MP 0x1403675E0 ^
		{"sphere", 0x016}, // SP 0x1405D92F0 MP 0x140367660 ^
		{"cylinder", 0x017}, // SP 0x1405D92F0 MP 0x140367890 ^
		{"spawnturret", 0x018}, // SP 0x14029D650 MP 0x140368B10
		{"canspawnturret", 0x019}, // SP 0x14029D870 MP 0x140368E80
		{"assert", 0x01A}, // SP 0x14029C9B0 MP 0x1403678A0
		{"pausecinematicingame", 0x01B}, // SP 0x1402AB640 MP 0x000000000
		{"drawcompassfriendlies", 0x01C}, // SP 0x1402AB670 MP 0x000000000
		{"bulletspread", 0x01D}, // SP 0x1402AB7A0 MP 0x000000000
		{"bullettracer", 0x01E}, // SP 0x1402AB960 MP 0x000000000
		{"badplace_delete", 0x01F}, // SP 0x14028F340 MP 0x140358600
		{"badplace_cylinder", 0x020}, // SP 0x14028F4B0 MP 0x140358C50
		{"badplace_arc", 0x021}, // SP 0x14028F830 MP 0x140359110
		{"badplace_brush", 0x022}, // SP 0x14028FF10 MP 0x140359CB0
		{"clearallcorpses", 0x023}, // SP 0x1402AC9D0 MP 0x000000000
		{"setturretnode", 0x024}, // SP 0x1402CC640 MP 0x000000000
		{"unsetturretnode", 0x025}, // SP 0x1402CC6E0 MP 0x000000000
		{"setnodepriority", 0x026}, // SP 0x1402CC570 MP 0x000000000
		{"_func_027", 0x027},
		{"_func_028", 0x028},
		{"isnodeoccupied", 0x029}, // SP 0x1402CC430 MP 0x000000000
		{"setdebugorigin", 0x02A}, // SP 0x1405D92F0 MP 0x000000000
		{"setdebugangles", 0x02B}, // SP 0x1405D92F0 MP 0x000000000
		{"updategamerprofile", 0x02C}, // SP 0x1402ACD10 MP 0x000000000
		{"assertex", 0x02D}, // SP 0x14029C9B0 MP 0x140367950
		{"assertmsg", 0x02E}, // SP 0x14029CAC0 MP 0x140367980
		{"isdefined", 0x02F}, // SP 0x14029D000 MP 0x140368520
		{"isvalidmissile", 0x030}, // SP 0x14029D3E0 MP 0x140368A70
		{"isstring", 0x031}, // SP 0x14029D630 MP 0x140368D30
		{"setomnvar", 0x032}, // SP 0x14029E0A0 MP 0x14036A580
		{"getomnvar", 0x033}, // SP 0x14029E220 MP 0x14036A7C0
		{"setdvar", 0x034}, // SP 0x14029EB20 MP 0x14036AE70
		{"setdynamicdvar", 0x035}, // SP 0x000000000 MP 0x14036B600
		{"setdvarifuninitialized", 0x036}, // SP 0x14029F8F0 MP 0x14036BB60
		{"setdevdvar", 0x037}, // SP 0x14029F1C0 MP 0x14036B7F0
		{"setdevdvarifuninitialized", 0x038}, // SP 0x14029F8F0 MP 0x14036C070
		{"getdvar", 0x039}, // SP 0x14029FE30 MP 0x14036C350
		{"getdvarint", 0x03A}, // SP 0x1402A0270 MP 0x14036C7B0
		{"getdvarfloat", 0x03B}, // SP 0x1402A0810 MP 0x14036CDD0
		{"getdvarvector", 0x03C}, // SP 0x1402A10E0 MP 0x14036D190
		{"gettime", 0x03D}, // SP 0x1402A1A50 MP 0x14036DB40
		{"gettimeutc", 0x03E}, // SP 0x1402A1A60 MP 0x14036DD80
		{"getradiometricunit", 0x03F}, // SP 0x1402A1AC0 MP 0x14036DDA0
		{"getentbynum", 0x040}, // SP 0x1402A1C50 MP 0x14036DF50
		{"getscreenwidth", 0x041}, // SP 0x1402A1D10 MP 0x000000000
		{"getscreenheight", 0x042}, // SP 0x1402A1D20 MP 0x000000000
		{"getweaponmodel", 0x043}, // SP 0x1402A1D30 MP 0x14036E110
		{"getculldist", 0x044}, // SP 0x1402ABBE0 MP 0x000000000
		{"sethalfresparticles", 0x045}, // SP 0x1402ABCF0 MP 0x000000000
		{"getmapsunlight", 0x046}, // SP 0x1402ABD30 MP 0x000000000
		{"setsunlight", 0x047}, // SP 0x1402ABD90 MP 0x1403775E0
		{"resetsunlight", 0x048}, // SP 0x1402ABE60 MP 0x140377840
		{"getmapsundirection", 0x049}, // SP 0x1402ABF90 MP 0x000000000
		{"getmapsunangles", 0x04A}, // SP 0x1402ABFC0 MP 0x000000000
		{"setsundirection", 0x04B}, // SP 0x1402AC0E0 MP 0x000000000
		{"lerpsundirection", 0x04C}, // SP 0x1402AC210 MP 0x000000000
		{"lerpsunangles", 0x04D}, // SP 0x1402AC520 MP 0x000000000
		{"resetsundirection", 0x04E}, // SP 0x1402AC6F0 MP 0x000000000
		{"enableforcedsunshadows", 0x04F}, // SP 0x1402AC720 MP 0x000000000
		{"enableforcednosunshadows", 0x050}, // SP 0x1402AC740 MP 0x000000000
		{"disableforcedsunshadows", 0x051}, // SP 0x1402AC760 MP 0x000000000
		{"enableouterspacemodellighting", 0x052}, // SP 0x1402AC780 MP 0x000000000
		{"disableouterspacemodellighting", 0x053}, // SP 0x1402AC830 MP 0x000000000
		{"remapstage", 0x054}, // SP 0x1402AD6E0 MP 0x000000000
		{"changelevel", 0x055}, // SP 0x1402AAEC0 MP 0x000000000
		{"missionsuccess", 0x056}, // SP 0x1402AAF60 MP 0x000000000
		{"missionfailed", 0x057}, // SP 0x1402AB040 MP 0x000000000
		{"cinematic", 0x058}, // SP 0x1402AB060 MP 0x000000000
		{"cinematicingame", 0x059}, // SP 0x1402AB140 MP 0x000000000
		{"cinematicingamesync", 0x05A}, // SP 0x1402AB1E0 MP 0x000000000
		{"cinematicingameloop", 0x05B}, // SP 0x1402AB2A0 MP 0x000000000
		{"cinematicingameloopresident", 0x05C}, // SP 0x1402AB420 MP 0x000000000
		{"iscinematicplaying", 0x05D}, // SP 0x1402AB4A0 MP 0x000000000
		{"stopcinematicingame", 0x05E}, // SP 0x1402AB620 MP 0x000000000
		{"getweapondisplayname", 0x05F}, // SP 0x1402A2410 MP 0x14036E740
		{"getweaponbasename", 0x060}, // SP 0x1402A2910 MP 0x14036ED60
		{"getweaponattachments", 0x061}, // SP 0x1402A3170 MP 0x14036F3E0
		{"getweaponattachmentdisplaynames", 0x062}, // SP 0x1402A34D0 MP 0x140357CA0
		{"getweaponcamoname", 0x063}, // SP 0x14028DEA0 MP 0x140358890
		{"getweaponreticlename", 0x064}, // SP 0x14028E0B0 MP 0x140358B80
		{"getanimlength", 0x065}, // SP 0x14028FB80 MP 0x14035AB80
		{"animhasnotetrack", 0x066}, // SP 0x14028FD60 MP 0x14035B110
		{"getnotetracktimes", 0x067}, // SP 0x14028FDC0 MP 0x14035B220
		{"spawn", 0x068}, // SP 0x14029D0C0 MP 0x140368410
		{"spawnloopsound", 0x069}, // SP 0x14029D0C0 MP 0x140368410
		{"spawnloopingsound", 0x06A}, // SP 0x140444BB0 MP 0x14036AF90
		{"bullettrace", 0x06B}, // SP 0x14029E950 MP 0x140366EF0
		{"target_setmaxsize", 0x06C}, // SP 0x1402BBCA0 MP 0x000000000
		{"target_setcolor", 0x06D}, // SP 0x1402BB830 MP 0x000000000
		{"target_setdelay", 0x06E}, // SP 0x1402BB990 MP 0x000000000
		{"getstartorigin", 0x06F}, // SP 0x14025D860 MP 0x140375D40
		{"getstartangles", 0x070}, // SP 0x14025D770 MP 0x140375F10
		{"getcycleoriginoffset", 0x071}, // SP 0x14025D630 MP 0x000000000
		{"magicgrenade", 0x072}, // SP 0x1402AB510 MP 0x000000000
		{"magicgrenademanual", 0x073}, // SP 0x1402981B0 MP 0x140364BC0
		{"setblur", 0x074}, // SP 0x1402AFD80 MP 0x000000000
		{"musicplay", 0x075}, // SP 0x1402AFEA0 MP 0x000000000
		{"musicstop", 0x076}, // SP 0x1402A78A0 MP 0x000000000
		{"soundfade", 0x077}, // SP 0x1402A79D0 MP 0x000000000
		{"addsoundsubmix", 0x078}, // SP 0x14029E170 MP 0x140367970
		{"clearsoundsubmix", 0x079}, // SP 0x14029E210 MP 0x140367990
		{"clearallsubmixes", 0x07A}, // SP 0x14029E2D0 MP 0x140367D70
		{"blendsoundsubmix", 0x07B}, // SP 0x14029E320 MP 0x140367F30
		{"makesoundsubmixsticky", 0x07C}, // SP 0x14029E330 MP 0x140367F40
		{"makesoundsubmixunsticky", 0x07D}, // SP 0x14029E730 MP 0x140367FA0
		{"soundsettimescalefactor", 0x07E}, // SP 0x1402A7BF0 MP 0x000000000
		{"soundresettimescale", 0x07F}, // SP 0x1402A8BB0 MP 0x000000000
		{"levelsoundfade", 0x080}, // SP 0x1402A7A80 MP 0x000000000
		{"precachenightvisioncodeassets", 0x081}, // SP 0x1402AAE30 MP 0x000000000
		{"_func_082", 0x082},
		{"precachedigitaldistortcodeassets", 0x083}, // SP 0x1402AAE50 MP 0x000000000
		{"precacheminimapsentrycodeassets", 0x084}, // SP 0x1402AAEA0 MP 0x000000000
		{"savegame", 0x085}, // SP 0x1402A8CB0 MP 0x000000000
		{"issavesuccessful", 0x086}, // SP 0x1402A9990 MP 0x000000000
		{"issaverecentlyloaded", 0x087}, // SP 0x1402A99B0 MP 0x000000000
		{"savegamenocommit", 0x088}, // SP 0x1402A9100 MP 0x000000000
		{"commitsave", 0x089}, // SP 0x1402A99D0 MP 0x000000000
		{"commitwouldbevalid", 0x08A}, // SP 0x1402A9A70 MP 0x000000000
		{"getfxvisibility", 0x08B}, // SP 0x1402AB690 MP 0x000000000
		{"setculldist", 0x08C}, // SP 0x1402ABB90 MP 0x000000000
		{"bullettracepassed", 0x08D}, // SP 0x14029F480 MP 0x140367410
		{"sighttracepassed", 0x08E}, // SP 0x14029F7A0 MP 0x140367670
		{"physicstrace", 0x08F}, // SP 0x14029FF90 MP 0x140367E20
		{"playerphysicstrace", 0x090}, // SP 0x1402A0360 MP 0x140368180
		{"getgroundposition", 0x091}, // SP 0x1402A2670 MP 0x14036A300
		{"getmovedelta", 0x092}, // SP 0x14028F3D0 MP 0x14036E210
		{"getangledelta", 0x093}, // SP 0x14028FA60 MP 0x14036E910
		{"getnorthyaw", 0x094}, // SP 0x1402903E0 MP 0x140357C50
		{"getcommandfromkey", 0x095}, // SP 0x1402A7970 MP 0x000000000
		{"getsticksconfig", 0x096}, // SP 0x1402A7A50 MP 0x000000000
		{"weaponfightdist", 0x097}, // SP 0x1402A9B20 MP 0x000000000
		{"weaponmaxdist", 0x098}, // SP 0x1402A9B70 MP 0x000000000
		{"isturretactive", 0x099}, // SP 0x1402A9AE0 MP 0x000000000
		{"getstarttime", 0x09A}, // SP 0x1402AE4D0 MP 0x000000000
		{"target_alloc", 0x09B}, // SP 0x1402BB040 MP 0x000000000
		{"target_flush", 0x09C}, // SP 0x1402BB410 MP 0x000000000
		{"target_set", 0x09D}, // SP 0x1402BB6F0 MP 0x000000000
		{"target_remove", 0x09E}, // SP 0x1402BB6B0 MP 0x000000000
		{"target_setshader", 0x09F}, // SP 0x1402BC260 MP 0x000000000
		{"target_setoffscreenshader", 0x0A0}, // SP 0x1402BBF00 MP 0x000000000
		{"target_isinrect", 0x0A1}, // SP 0x1402BB590 MP 0x000000000
		{"target_isincircle", 0x0A2}, // SP 0x1402BB520 MP 0x000000000
		{"target_startreticlelockon", 0x0A3}, // SP 0x1402BC390 MP 0x000000000
		{"target_clearreticlelockon", 0x0A4}, // SP 0x1402BB060 MP 0x000000000
		{"target_getarray", 0x0A5}, // SP 0x1402BB4A0 MP 0x000000000
		{"target_istarget", 0x0A6}, // SP 0x1402BB630 MP 0x000000000
		{"target_setattackmode", 0x0A7}, // SP 0x1402BB710 MP 0x000000000
		{"target_setjavelinonly", 0x0A8}, // SP 0x1402BBB80 MP 0x000000000
		{"target_hidefromplayer", 0x0A9}, // SP 0x1402BB510 MP 0x000000000
		{"target_showtoplayer", 0x0AA}, // SP 0x1402BC380 MP 0x000000000
		{"target_setscaledrendermode", 0x0AB}, // SP 0x1402BC140 MP 0x000000000
		{"target_drawcornersonly", 0x0AC}, // SP 0x1402BB090 MP 0x000000000
		{"target_drawsquare", 0x0AD}, // SP 0x1402BB2C0 MP 0x000000000
		{"target_drawsingle", 0x0AE}, // SP 0x1402BB1C0 MP 0x000000000
		{"target_setminsize", 0x0AF}, // SP 0x1402BBDB0 MP 0x000000000
		{"setnorthyaw", 0x0B0}, // SP 0x140290420 MP 0x140357E90
		{"setslowmotion", 0x0B1}, // SP 0x14029BC60 MP 0x140365480
		{"randomint", 0x0B2}, // SP 0x1402A2ED0 MP 0x14036A770
		{"randomfloat", 0x0B3}, // SP 0x1402A3140 MP 0x14036A9E0
		{"randomintrange", 0x0B4}, // SP 0x1402A3240 MP 0x14036ACD0
		{"randomfloatrange", 0x0B5}, // SP 0x1402A3310 MP 0x14036AE10
		{"sin", 0x0B6}, // SP 0x1402A3440 MP 0x14036B240
		{"cos", 0x0B7}, // SP 0x1402A34A0 MP 0x14036B550
		{"tan", 0x0B8}, // SP 0x1402A3610 MP 0x14036B5A0
		{"asin", 0x0B9}, // SP 0x1402A37D0 MP 0x14036B720
		{"acos", 0x0BA}, // SP 0x1402A3A60 MP 0x14036B7A0
		{"atan", 0x0BB}, // SP 0x14028D680 MP 0x14036BAE0
		{"int", 0x0BC}, // SP 0x14028DAE0 MP 0x14036BBF0
		{"float", 0x0BD}, // SP 0x14028DE30 MP 0x14036C000
		{"abs", 0x0BE}, // SP 0x14028DFC0 MP 0x14036C320
		{"min", 0x0BF}, // SP 0x14028DFF0 MP 0x14036C580
		{"objective_additionalcurrent", 0x0C0}, // SP 0x1402B7A30 MP 0x000000000
		{"objective_ring", 0x0C1}, // SP 0x1402B7B50 MP 0x000000000
		{"objective_setpointertextoverride", 0x0C2}, // SP 0x1402B7BF0 MP 0x000000000
		{"getnode", 0x0C3}, // SP 0x1402CBED0 MP 0x14031FA00
		{"getnodearray", 0x0C4}, // SP 0x1402CBFE0 MP 0x14031FB10
		{"getallnodes", 0x0C5}, // SP 0x1402CBCD0 MP 0x14031F7E0
		{"getnodesinradius", 0x0C6}, // SP 0x1402CC0F0 MP 0x14031FCC0
		{"getnodesinradiussorted", 0x0C7}, // SP 0x1402CC100 MP 0x14031FCD0
		{"getclosestnodeinsight", 0x0C8}, // SP 0x1402CBD20 MP 0x14031F830
		{"getreflectionlocs", 0x0C9}, // SP 0x1405D92F0 MP 0x000000000
		{"getreflectionreferencelocs", 0x0CA}, // SP 0x1405D92F0 MP 0x000000000
		{"getvehicletracksegment", 0x0CB}, // SP 0x140468CF0 MP 0x000000000
		{"getvehicletracksegmentarray", 0x0CC}, // SP 0x140468DD0 MP 0x000000000
		{"getallvehicletracksegments", 0x0CD}, // SP 0x140468CA0 MP 0x000000000
		{"isarray", 0x0CE}, // SP 0x140291BF0 MP 0x14035E620
		{"isai", 0x0CF}, // SP 0x1402ADC10 MP 0x140377460
		{"getindexforluincstring", 0x0D0}, // SP 0x14029DB50 MP 0x140369C50
		{"issentient", 0x0D1}, // SP 0x14029D7D0 MP 0x140368DA0
		{"isgodmode", 0x0D2}, // SP 0x1402ADD90 MP 0x000000000
		{"getdebugdvar", 0x0D3}, // SP 0x1402ADED0 MP 0x000000000
		{"getdebugdvarint", 0x0D4}, // SP 0x1402ADF60 MP 0x000000000
		{"getdebugdvarfloat", 0x0D5}, // SP 0x1402ADFD0 MP 0x000000000
		{"setsaveddvar", 0x0D6}, // SP 0x1402AE020 MP 0x000000000
		{"getfreeaicount", 0x0D7}, // SP 0x1402AE4E0 MP 0x000000000
		{"getaicount", 0x0D8}, // SP 0x1402AE4F0 MP 0x000000000
		{"getaiarray", 0x0D9}, // SP 0x1402AE7A0 MP 0x000000000
		{"getaispeciesarray", 0x0DA}, // SP 0x1402AF980 MP 0x000000000
		{"getspawnerarray", 0x0DB}, // SP 0x1402AFBD0 MP 0x000000000
		{"getcorpsearray", 0x0DC}, // SP 0x1402AFB20 MP 0x000000000
		{"getspawnerteamarray", 0x0DD}, // SP 0x1402AFCE0 MP 0x000000000
		{"getweaponclipmodel", 0x0DE}, // SP 0x1402AFE20 MP 0x000000000
		{"getbrushmodelcenter", 0x0DF}, // SP 0x14029CAC0 MP 0x000000000
		{"getkeybinding", 0x0E0}, // SP 0x1402A7830 MP 0x000000000
		{"max", 0x0E1}, // SP 0x14028E170 MP 0x14036C720
		{"floor", 0x0E2}, // SP 0x14028E1C0 MP 0x14036C750
		{"ceil", 0x0E3}, // SP 0x14028E260 MP 0x14036C770
		{"exp", 0x0E4}, // SP 0x14028E280 MP 0x14036C790
		{"exp", 0x0E5}, // SP 0x14028E370 MP 0x14036C820
		{"log", 0x0E6}, // SP 0x14028E410 MP 0x14036C840
		{"sqrt", 0x0E7}, // SP 0x14028E470 MP 0x14036CB80
		{"squared", 0x0E8}, // SP 0x14028E4B0 MP 0x14036CBA0
		{"clamp", 0x0E9}, // SP 0x14028E770 MP 0x14036CE40
		{"angleclamp", 0x0EA}, // SP 0x14028E8C0 MP 0x14036CF30
		{"angleclamp180", 0x0EB}, // SP 0x14028E920 MP 0x14036CFB0
		{"vectorfromlinetopoint", 0x0EC}, // SP 0x140290740 MP 0x14036E5A0
		{"pointonsegmentnearesttopoint", 0x0ED}, // SP 0x140290B60 MP 0x14036EB30
		{"distance", 0x0EE}, // SP 0x140291260 MP 0x14036F6E0
		{"distance2d", 0x0EF}, // SP 0x1402913A0 MP 0x14036F790
		{"distancesquared", 0x0F0}, // SP 0x140291600 MP 0x140357E10
		{"length", 0x0F1}, // SP 0x1402918C0 MP 0x140357FB0
		{"length2d", 0x0F2}, // SP 0x140291930 MP 0x140358030
		{"lengthsquared", 0x0F3}, // SP 0x140291AF0 MP 0x140358130
		{"length2dsquared", 0x0F4}, // SP 0x140291B30 MP 0x140358190
		{"closer", 0x0F5}, // SP 0x140291C20 MP 0x1403581E0
		{"vectordot", 0x0F6}, // SP 0x140291EB0 MP 0x140358570
		{"vectorcross", 0x0F7}, // SP 0x140291F80 MP 0x140358640
		{"axistoangles", 0x0F8}, // SP 0x140292690 MP 0x140359050
		{"visionsetthermal", 0x0F9}, // SP 0x1402ABB50 MP 0x1403771E0
		{"visionsetpain", 0x0FA}, // SP 0x1402ABB60 MP 0x140377260
		{"endlobby", 0x0FB}, // SP 0x000000000 MP 0x14037B950
		{"setac130ambience", 0x0FC}, // SP 0x000000000 MP 0x140374E10
		{"getmapcustom", 0x0FD}, // SP 0x000000000 MP 0x140378260
		{"spawnsighttrace", 0x0FE}, // SP 0x000000000 MP 0x1403765A0
		{"incrementcounter", 0x0FF}, // SP 0x1402AE370 MP 0x140374FE0
		{"getcountertotal", 0x100}, // SP 0x1402AE3C0 MP 0x140375100
		{"getlevelticks", 0x101}, // SP 0x1402A1AE0 MP 0x000000000
		{"perlinnoise2d", 0x102}, // SP 0x14028E970 MP 0x000000000
		{"calcrockingangles", 0x103}, // SP 0x14028ED80 MP 0x000000000
		{"reconevent", 0x104}, // SP 0x1405D92F0 MP 0x14042C2C0
		{"reconspatialevent", 0x105}, // SP 0x1405D92F0 MP 0x14042C2D0
		{"setsunflareposition", 0x106}, // SP 0x14028D7F0 MP 0x000000000
		{"createthreatbiasgroup", 0x107}, // SP 0x1402D79E0 MP 0x1400774E0
		{"threatbiasgroupexists", 0x108}, // SP 0x1402D7A10 MP 0x140077510
		{"getthreatbias", 0x109}, // SP 0x1402D7A50 MP 0x140077550
		{"setthreatbias", 0x10A}, // SP 0x1402D7AB0 MP 0x1400775B0
		{"setthreatbiasagainstall", 0x10B}, // SP 0x1402D7B30 MP 0x140077630
		{"setignoremegroup", 0x10C}, // SP 0x1402D7B80 MP 0x140077680
		{"isenemyteam", 0x10D}, // SP 0x1402D7CC0 MP 0x140077820
		{"objective_additionalentity", 0x10E}, // SP 0x1402B7530 MP 0x000000000
		{"objective_state_nomessage", 0x10F}, // SP 0x1402B77F0 MP 0x000000000
		{"objective_string", 0x110}, // SP 0x1402B7800 MP 0x000000000
		{"objective_string_nomessage", 0x111}, // SP 0x1402B7810 MP 0x000000000
		{"objective_additionalposition", 0x112}, // SP 0x1402B79C0 MP 0x000000000
		{"objective_current_nomessage", 0x113}, // SP 0x1402B7A20 MP 0x000000000
		{"vectornormalize", 0x114}, // SP 0x140292150 MP 0x1403587A0
		{"vectortoangles", 0x115}, // SP 0x140292290 MP 0x140358A90
		{"vectortoyaw", 0x116}, // SP 0x1402924D0 MP 0x140358B10
		{"vectorlerp", 0x117}, // SP 0x140292510 MP 0x140358E60
		{"anglestoup", 0x118}, // SP 0x140292C80 MP 0x1403596E0
		{"anglestoright", 0x119}, // SP 0x140292D50 MP 0x140359890
		{"anglestoforward", 0x11A}, // SP 0x140292F80 MP 0x140359930
		{"anglesdelta", 0x11B}, // SP 0x1402930E0 MP 0x140359BC0
		{"combineangles", 0x11C}, // SP 0x1402934C0 MP 0x14035A0F0
		{"transformmove", 0x11D}, // SP 0x140293670 MP 0x14035A280
		{"rotatevector", 0x11E}, // SP 0x1402956B0 MP 0x14035B550
		{"rotatepointaroundvector", 0x11F}, // SP 0x140295850 MP 0x14035B7F0
		{"issubstr", 0x120}, // SP 0x140296960 MP 0x14035CB70
		{"isendstr", 0x121}, // SP 0x1402969A0 MP 0x14035CD40
		{"getsubstr", 0x122}, // SP 0x140296BB0 MP 0x14035CF80
		{"tolower", 0x123}, // SP 0x140296DC0 MP 0x14035D360
		{"strtok", 0x124}, // SP 0x140297230 MP 0x14035DA90
		{"stricmp", 0x125}, // SP 0x140297970 MP 0x14035E690
		{"ambientplay", 0x126}, // SP 0x14029FA50 MP 0x1403679A0
		{"getuavstrengthmax", 0x127}, // SP 0x000000000 MP 0x14037B790
		{"getuavstrengthlevelneutral", 0x128}, // SP 0x000000000 MP 0x14037B7A0
		{"getuavstrengthlevelshowenemyfastsweep", 0x129}, // SP 0x000000000 MP 0x14037B7B0
		{"getuavstrengthlevelshowenemydirectional", 0x12A}, // SP 0x000000000 MP 0x14037B7C0
		{"blockteamradar", 0x12B}, // SP 0x000000000 MP 0x14037B7D0
		{"unblockteamradar", 0x12C}, // SP 0x000000000 MP 0x14037B830
		{"isteamradarblocked", 0x12D}, // SP 0x000000000 MP 0x14037B8B0
		{"getassignedteam", 0x12E}, // SP 0x000000000 MP 0x140378110
		{"setmatchdata", 0x12F}, // SP 0x000000000 MP 0x14037A7B0
		{"getmatchdata", 0x130}, // SP 0x000000000 MP 0x140378420
		{"sendmatchdata", 0x131}, // SP 0x000000000 MP 0x140378490
		{"clearmatchdata", 0x132}, // SP 0x000000000 MP 0x140378F40
		{"setmatchdatadef", 0x133}, // SP 0x000000000 MP 0x14037A820
		{"setmatchclientip", 0x134}, // SP 0x000000000 MP 0x140378FE0
		{"setmatchdataid", 0x135}, // SP 0x000000000 MP 0x140378DD0
		{"setclientmatchdata", 0x136}, // SP 0x000000000 MP 0x140378F70
		{"getclientmatchdata", 0x137}, // SP 0x000000000 MP 0x140378290
		{"setclientmatchdatadef", 0x138}, // SP 0x000000000 MP 0x14037A8D0
		{"sendclientmatchdata", 0x139}, // SP 0x000000000 MP 0x14037ADE0
		{"getbuildversion", 0x13A}, // SP 0x000000000 MP 0x14037AEB0
		{"getbuildnumber", 0x13B}, // SP 0x000000000 MP 0x14037B0E0
		{"getsystemtime", 0x13C}, // SP 0x000000000 MP 0x14037B100
		{"getmatchrulesdata", 0x13D}, // SP 0x000000000 MP 0x14037AA30
		{"isusingmatchrulesdata", 0x13E}, // SP 0x000000000 MP 0x14037AB20
		{"kick", 0x13F}, // SP 0x000000000 MP 0x1403769E0
		{"issplitscreen", 0x140}, // SP 0x1402ACD00 MP 0x140378030
		{"setmapcenter", 0x141}, // SP 0x000000000 MP 0x14037B550
		{"setgameendtime", 0x142}, // SP 0x000000000 MP 0x14037B5A0
		{"visionsetthermal", 0x143}, // SP 0x1402ABA00 MP 0x140376D40
		{"visionsetpain", 0x144}, // SP 0x1402ABA10 MP 0x1403770E0
		{"visionsetnight", 0x145}, // SP 0x1402ABA20 MP 0x140377160
		{"ambientstop", 0x146}, // SP 0x1402A12A0 MP 0x1403699C0
		{"precachemodel", 0x147}, // SP 0x140298040 MP 0x14035F250
		{"precacheshellshock", 0x148}, // SP 0x140298370 MP 0x14035F7A0
		{"precacheitem", 0x149}, // SP 0x14029E7B0 MP 0x140366DA0
		{"precacheshader", 0x14A}, // SP 0x14029ECC0 MP 0x140366E30
		{"precachestring", 0x14B}, // SP 0x14029F2B0 MP 0x1403673D0
		{"precachemenu", 0x14C}, // SP 0x14029F3E0 MP 0x140367540
		{"precacherumble", 0x14D}, // SP 0x14029F5B0 MP 0x1403675F0
		{"precachelocationselector", 0x14E}, // SP 0x1402982C0 MP 0x14035F470
		{"precacheleaderboards", 0x14F}, // SP 0x14029EDB0 MP 0x1403670E0
		{"loadfx", 0x150}, // SP 0x140290A80 MP 0x1403583B0
		{"playfx", 0x151}, // SP 0x140291730 MP 0x140359620
		{"playfxontag", 0x152}, // SP 0x140292A40 MP 0x14035AF90
		{"stopfxontag", 0x153}, // SP 0x140294440 MP 0x14035C4B0
		{"killfxontag", 0x154}, // SP 0x140294DD0 MP 0x14035D560
		{"playloopedfx", 0x155}, // SP 0x140474350 MP 0x14037DA70
		{"spawnfx", 0x156}, // SP 0x1404746B0 MP 0x14037E2D0
		{"triggerfx", 0x157}, // SP 0x140474940 MP 0x14037E8B0
		{"playfxontagforclients", 0x158}, // SP 0x140292ED0 MP 0x14035B2B0
		{"setfxkillondelete", 0x159}, // SP 0x140474640 MP 0x14037E1D0
		{"playimpactheadfatalfx", 0x15A}, // SP 0x140293D50 MP 0x14035BFE0
		{"setwinningteam", 0x15B}, // SP 0x000000000 MP 0x1403778C0
		{"announcement", 0x15C}, // SP 0x000000000 MP 0x140377920
		{"clientannouncement", 0x15D}, // SP 0x000000000 MP 0x1403779A0
		{"setteammode", 0x15E}, // SP 0x000000000 MP 0x140377A40
		{"getteamscore", 0x15F}, // SP 0x000000000 MP 0x140377BE0
		{"setteamscore", 0x160}, // SP 0x000000000 MP 0x140377C50
		{"setclientnamemode", 0x161}, // SP 0x000000000 MP 0x140377D70
		{"updateclientnames", 0x162}, // SP 0x000000000 MP 0x140377DC0
		{"getteamplayersalive", 0x163}, // SP 0x000000000 MP 0x140377EB0
		{"logprint", 0x164}, // SP 0x000000000 MP 0x140375460
		{"worldentnumber", 0x165}, // SP 0x000000000 MP 0x140375480
		{"obituary", 0x166}, // SP 0x000000000 MP 0x1403754A0
		{"positionwouldtelefrag", 0x167}, // SP 0x000000000 MP 0x1403756D0
		{"canspawn", 0x168}, // SP 0x000000000 MP 0x1403759C0
		{"getstarttime", 0x169}, // SP 0x000000000 MP 0x1403760D0
		{"precacheheadicon", 0x16A}, // SP 0x14029F100 MP 0x1403673A0
		{"precacheminimapicon", 0x16B}, // SP 0x000000000 MP 0x140376BE0
		{"precachempanim", 0x16C}, // SP 0x000000000 MP 0x140376C60
		{"map_restart", 0x16D}, // SP 0x000000000 MP 0x140376550
		{"exitlevel", 0x16E}, // SP 0x000000000 MP 0x140376630
		{"addtestclient", 0x16F}, // SP 0x000000000 MP 0x140376680
		{"addagent", 0x170}, // SP 0x000000000 MP 0x140376860
		{"setarchive", 0x171}, // SP 0x000000000 MP 0x140378020
		{"allclientsprint", 0x172}, // SP 0x000000000 MP 0x140376880
		{"clientprint", 0x173}, // SP 0x000000000 MP 0x140376B60
		{"mapexists", 0x174}, // SP 0x000000000 MP 0x140376910
		{"isvalidgametype", 0x175}, // SP 0x000000000 MP 0x140376950
		{"matchend", 0x176}, // SP 0x000000000 MP 0x140378040
		{"setplayerteamrank", 0x177}, // SP 0x000000000 MP 0x140378050
		{"endparty", 0x178}, // SP 0x000000000 MP 0x140378100
		{"setteamradar", 0x179}, // SP 0x000000000 MP 0x14037B5D0
		{"getteamradar", 0x17A}, // SP 0x000000000 MP 0x14037B640
		{"setteamradarstrength", 0x17B}, // SP 0x000000000 MP 0x14037B6A0
		{"getteamradarstrength", 0x17C}, // SP 0x000000000 MP 0x14037B720
		{"getuavstrengthmin", 0x17D}, // SP 0x000000000 MP 0x14037B780
		{"physicsexplosionsphere", 0x17E}, // SP 0x140295500 MP 0x14035E730
		{"physicsexplosioncylinder", 0x17F}, // SP 0x140296890 MP 0x14035FE90
		{"physicsjolt", 0x180}, // SP 0x140295F60 MP 0x14035F2D0
		{"physicsjitter", 0x181}, // SP 0x1402963A0 MP 0x14035F900
		{"setexpfog", 0x182}, // SP 0x140299880 MP 0x140362E20
		{"setexpfogext", 0x183}, // SP 0x140299900 MP 0x140363010
		{"setexpfogdvarsonly", 0x184}, // SP 0x140299F70 MP 0x140363800
		{"setexpfogextdvarsonly", 0x185}, // SP 0x14029A180 MP 0x140363920
		{"setatmosfog", 0x186}, // SP 0x14029A220 MP 0x140363B00
		{"setatmosfogdvarsonly", 0x187}, // SP 0x14029A4B0 MP 0x140363E60
		{"isexplosivedamagemod", 0x188}, // SP 0x1402A2800 MP 0x14036ADB0
		{"radiusdamage", 0x189}, // SP 0x1402A3690 MP 0x14036C110
		{"setplayerignoreradiusdamage", 0x18A}, // SP 0x14028DFA0 MP 0x14036CF90
		{"glassradiusdamage", 0x18B}, // SP 0x1402A3950 MP 0x14036C1E0
		{"earthquake", 0x18C}, // SP 0x14029B460 MP 0x140366A00
		{"getnumparts", 0x18D}, // SP 0x14029A830 MP 0x140363F40
		{"objective_onentity", 0x18E}, // SP 0x1402B76D0 MP 0x140383170
		{"objective_onentitywithrotation", 0x18F}, // SP 0x000000000 MP 0x1403831D0
		{"objective_team", 0x190}, // SP 0x000000000 MP 0x140383310
		{"objective_player", 0x191}, // SP 0x000000000 MP 0x1403833B0
		{"objective_playerteam", 0x192}, // SP 0x000000000 MP 0x140383410
		{"objective_playerenemyteam", 0x193}, // SP 0x000000000 MP 0x1403834B0
		{"objective_playermask_hidefromall", 0x194}, // SP 0x000000000 MP 0x1403835B0
		{"objective_playermask_hidefrom", 0x195}, // SP 0x000000000 MP 0x1403835F0
		{"objective_playermask_showtoall", 0x196}, // SP 0x000000000 MP 0x140382DA0
		{"objective_playermask_showto", 0x197}, // SP 0x000000000 MP 0x140382DE0
		{"iprintln", 0x198}, // SP 0x1402AD860 MP 0x140377360
		{"iprintlnbold", 0x199}, // SP 0x1402AD8C0 MP 0x1403773E0
		{"logstring", 0x19A}, // SP 0x1405D92F0 MP 0x140377C30
		{"getent", 0x19B}, // SP 0x1402B9570 MP 0x1403845D0
		{"getentarray", 0x19C}, // SP 0x1402B95E0 MP 0x1403846A0
		{"getspawnarray", 0x19D}, // SP 0x000000000 MP 0x140384C00
		{"spawnplane", 0x19E}, // SP 0x000000000 MP 0x140377670
		{"spawnstruct", 0x19F}, // SP 0x140374110 MP 0x140442170
		{"spawnhelicopter", 0x1A0}, // SP 0x000000000 MP 0x140377A90
		{"isalive", 0x1A1}, // SP 0x1402AD940 MP 0x140377410
		{"isspawner", 0x1A2}, // SP 0x1402ADB20 MP 0x1403774F0
		{"missile_createattractorent", 0x1A3}, // SP 0x140282D00 MP 0x14034F8E0
		{"missile_createattractororigin", 0x1A4}, // SP 0x140282D10 MP 0x14034F8F0
		{"missile_createrepulsorent", 0x1A5}, // SP 0x140282FD0 MP 0x14034FBA0
		{"missile_createrepulsororigin", 0x1A6}, // SP 0x140282FE0 MP 0x14034FBB0
		{"missile_deleteattractor", 0x1A7}, // SP 0x140282FF0 MP 0x14034FBC0
		{"playsoundatpos", 0x1A8}, // SP 0x000000000 MP 0x140378150
		{"newhudelem", 0x1A9}, // SP 0x14026F000 MP 0x14033CF50
		{"newclienthudelem", 0x1AA}, // SP 0x14026EFB0 MP 0x14033CEF0
		{"newteamhudelem", 0x1AB}, // SP 0x000000000 MP 0x14033CF80
		{"resettimeout", 0x1AC}, // SP 0x140375910 MP 0x1404438F0
		{"isplayer", 0x1AD}, // SP 0x1402ADB90 MP 0x140377390
		{"isplayernumber", 0x1AE}, // SP 0x000000000 MP 0x1403774C0
		{"getpartname", 0x1AF}, // SP 0x14029A9A0 MP 0x140364120
		{"weaponfiretime", 0x1B0}, // SP 0x14029CDC0 MP 0x140363D50
		{"weaponclipsize", 0x1B1}, // SP 0x1402999A0 MP 0x140360B10
		{"weaponisauto", 0x1B2}, // SP 0x14029AA30 MP 0x140361AC0
		{"weaponissemiauto", 0x1B3}, // SP 0x14029ADB0 MP 0x140361CE0
		{"weaponisboltaction", 0x1B4}, // SP 0x14029B360 MP 0x1403622D0
		{"weaponinheritsperks", 0x1B5}, // SP 0x14029B580 MP 0x140362520
		{"weaponburstcount", 0x1B6}, // SP 0x14029B020 MP 0x140361EC0
		{"weapontype", 0x1B7}, // SP 0x14029B7A0 MP 0x140362920
		{"weaponclass", 0x1B8}, // SP 0x14029B8E0 MP 0x140362A60
		{"getnextarraykey", 0x1B9}, // SP 0x1402A35D0 MP 0x14036D000
		{"sortbydistance", 0x1BA}, // SP 0x14028D850 MP 0x14036D690
		{"tablelookup", 0x1BB}, // SP 0x14029CCC0 MP 0x1403664F0
		{"tablelookupbyrow", 0x1BC}, // SP 0x14029D970 MP 0x1403671B0
		{"tablelookupistring", 0x1BD}, // SP 0x14029CF50 MP 0x1403667F0
		{"tablelookupistringbyrow", 0x1BE}, // SP 0x14029DC10 MP 0x140367320
		{"tablelookuprownum", 0x1BF}, // SP 0x14029D280 MP 0x140366C90
		{"tableexists", 0x1C0}, // SP 0x14029D820 MP 0x140366DE0
		{"getmissileowner", 0x1C1}, // SP 0x14029AD10 MP 0x140366490
		{"magicbullet", 0x1C2}, // SP 0x140294EF0 MP 0x14035E270
		{"getweaponflashtagname", 0x1C3}, // SP 0x140295DC0 MP 0x14035F290
		{"averagepoint", 0x1C4}, // SP 0x14029A870 MP 0x140363F90
		{"averagenormal", 0x1C5}, // SP 0x14029AB60 MP 0x140364300
		{"vehicle_getspawnerarray", 0x1C6}, // SP 0x140466C40 MP 0x140564030
		{"playrumbleonposition", 0x1C7}, // SP 0x140299410 MP 0x140360540
		{"playrumblelooponposition", 0x1C8}, // SP 0x140299470 MP 0x1403605E0
		{"stopallrumbles", 0x1C9}, // SP 0x1402996F0 MP 0x140360670
		{"soundexists", 0x1CA}, // SP 0x140290B30 MP 0x14035C2D0
		{"openfile", 0x1CB}, // SP 0x1405D92F0 MP 0x14036B580
		{"closefile", 0x1CC}, // SP 0x1405D92F0 MP 0x14036B590
		{"fprintln", 0x1CD}, // SP 0x1405D92F0 MP 0x14036B710
		{"fprintfields", 0x1CE}, // SP 0x1405D92F0 MP 0x14036B770
		{"freadln", 0x1CF}, // SP 0x1405D92F0 MP 0x14036B780
		{"fgetarg", 0x1D0}, // SP 0x1405D92F0 MP 0x14036B790
		{"setminimap", 0x1D1}, // SP 0x1402A2200 MP 0x14036C3C0
		{"setthermalbodymaterial", 0x1D2}, // SP 0x1402A28F0 MP 0x14036CA90
		{"getarraykeys", 0x1D3}, // SP 0x1402A3410 MP 0x14036CDA0
		{"getfirstarraykey", 0x1D4}, // SP 0x1402A3470 MP 0x14036CF00
		{"getglass", 0x1D5}, // SP 0x14029AD60 MP 0x1403644F0
		{"getglassarray", 0x1D6}, // SP 0x14029B110 MP 0x1403647D0
		{"getglassorigin", 0x1D7}, // SP 0x14029B410 MP 0x1403649E0
		{"isglassdestroyed", 0x1D8}, // SP 0x14029B630 MP 0x140364CF0
		{"destroyglass", 0x1D9}, // SP 0x14029B850 MP 0x140365010
		{"deleteglass", 0x1DA}, // SP 0x14029B9F0 MP 0x1403651E0
		{"getentchannelscount", 0x1DB}, // SP 0x14029C0E0 MP 0x140365F50
		{"getentchannelname", 0x1DC}, // SP 0x14029C160 MP 0x140366130
		{"objective_add", 0x1DD}, // SP 0x1402B7080 MP 0x140382E50
		{"objective_delete", 0x1DE}, // SP 0x1402B74F0 MP 0x140382F80
		{"objective_state", 0x1DF}, // SP 0x1402B77E0 MP 0x140382FF0
		{"objective_icon", 0x1E0}, // SP 0x1402B7820 MP 0x140383090
		{"objective_indentlevel", 0x1E1}, // SP 0x1402B7900 MP 0x000000000
		{"objective_position", 0x1E2}, // SP 0x1402B7980 MP 0x1403830D0
		{"objective_current", 0x1E3}, // SP 0x1402B7A10 MP 0x140383230
		{"weaponinventorytype", 0x1E4}, // SP 0x14029BA70 MP 0x140362C20
		{"weaponstartammo", 0x1E5}, // SP 0x14029BE70 MP 0x140362F00
		{"weaponmaxammo", 0x1E6}, // SP 0x14029BF80 MP 0x140363300
		{"weaponaltweaponname", 0x1E7}, // SP 0x14029C9D0 MP 0x140363950
		{"isweaponcliponly", 0x1E8}, // SP 0x14029D050 MP 0x140364220
		{"isweapondetonationtimed", 0x1E9}, // SP 0x14029D330 MP 0x140364590
		{"isweaponmanuallydetonatedbyemptythrow", 0x1EA}, // SP 0x14029D720 MP 0x140364870
		{"weaponhasthermalscope", 0x1EB}, // SP 0x14029D9F0 MP 0x140364E70
		{"getvehiclenode", 0x1EC}, // SP 0x14045D5C0 MP 0x14055A900
		{"getvehiclenodearray", 0x1ED}, // SP 0x14045D6B0 MP 0x14055A9F0
		{"getallvehiclenodes", 0x1EE}, // SP 0x14045D550 MP 0x14055A890
		{"getnumvehicles", 0x1EF}, // SP 0x1404669B0 MP 0x140563DA0
		{"precachevehicle", 0x1F0}, // SP 0x140466CC0 MP 0x140564110
		{"spawnvehicle", 0x1F1}, // SP 0x140466D20 MP 0x140564170
		{"vehicle_getarray", 0x1F2}, // SP 0x1404669F0 MP 0x140563DE0
		{"pow", 0x1F3}, // SP 0x14028E710 MP 0x14036CBC0
		{"atan2", 0x1F4}, // SP 0x14028D7B0 MP 0x14036BB10
		{"botgetmemoryevents", 0x1F5}, // SP 0x000000000 MP 0x14047A600
		{"botautoconnectenabled", 0x1F6}, // SP 0x000000000 MP 0x14047ABF0
		{"botzonegetcount", 0x1F7}, // SP 0x000000000 MP 0x14047AC20
		{"botzonesetteam", 0x1F8}, // SP 0x000000000 MP 0x14047AF10
		{"botzonenearestcount", 0x1F9}, // SP 0x000000000 MP 0x14047AD50
		{"botmemoryflags", 0x1FA}, // SP 0x000000000 MP 0x14047A940
		{"botflagmemoryevents", 0x1FB}, // SP 0x000000000 MP 0x14047A320
		{"botzonegetindoorpercent", 0x1FC}, // SP 0x000000000 MP 0x14047ACF0
		{"botsentientswap", 0x1FD}, // SP 0x000000000 MP 0x14047A990
		{"isbot", 0x1FE}, // SP 0x000000000 MP 0x14047AF70
		{"isagent", 0x1FF}, // SP 0x000000000 MP 0x14037B2B0
		{"getmaxagents", 0x200}, // SP 0x000000000 MP 0x140450E50
		{"botdebugdrawtrigger", 0x201}, // SP 0x000000000 MP 0x14047A310
		{"botgetclosestnavigablepoint", 0x202}, // SP 0x000000000 MP 0x14047A550
		{"getnodesintrigger", 0x203}, // SP 0x000000000 MP 0x14031FCE0
		{"nodesvisible", 0x204}, // SP 0x1402CC4A0 MP 0x140320EA0
		{"getnodesonpath", 0x205}, // SP 0x1402CC110 MP 0x14031FDD0
		{"getzonecount", 0x206}, // SP 0x000000000 MP 0x1403201B0
		{"getzonenearest", 0x207}, // SP 0x000000000 MP 0x1403201C0
		{"getzonenodes", 0x208}, // SP 0x000000000 MP 0x140320320
		{"getzonepath", 0x209}, // SP 0x000000000 MP 0x140320A10
		{"getzoneorigin", 0x20A}, // SP 0x000000000 MP 0x140320960
		{"getnodezone", 0x20B}, // SP 0x000000000 MP 0x14031FC20
		{"getzonenodesbydist", 0x20C}, // SP 0x000000000 MP 0x140320600
		{"getzonenodeforindex", 0x20D}, // SP 0x000000000 MP 0x1403202E0
		{"getweaponexplosionradius", 0x20E}, // SP 0x1402AFFC0 MP 0x14037AE40
		{"markdangerousnodes", 0x20F},
		{"markdangerousnodesintrigger", 0x210},
		{"nodeexposedtosky", 0x211}, // SP 0x000000000 MP 0x140320C70
		{"findentrances", 0x212}, // SP 0x000000000 MP 0x14031F6F0
		{"badplace_global", 0x213}, // SP 0x000000000 MP 0x14035A190
		{"getpathdist", 0x214}, // SP 0x000000000 MP 0x140375150
		{"getlinkednodes", 0x215}, // SP 0x1402CBE10 MP 0x14031F940
		{"disconnectnodepair", 0x216}, // SP 0x14028E840 MP 0x14036F800
		{"connectnodepair", 0x217}, // SP 0x14028EAC0 MP 0x140357F30
		{"gettimesincelastpaused", 0x218}, // SP 0x1402AE000 MP 0x000000000
		{"precachefxontag", 0x219},
		{"precachetag", 0x21A},
		{"precachesound", 0x21B}, // SP 0x1405D92F0 MP 0x1403780E0
		{"devsetminimapdvarsettings", 0x21C}, // SP 0x000000000 MP 0x140374FD0
		{"loadtransient", 0x21D}, // SP 0x14037AA30 MP 0x000000000
		{"unloadtransient", 0x21E}, // SP 0x14037AC70 MP 0x000000000
		{"unloadalltransients", 0x21F}, // SP 0x14037ABB0 MP 0x000000000
		{"synctransients", 0x220}, // SP 0x14037AB50 MP 0x000000000
		{"aretransientsbusy", 0x221}, // SP 0x14037A730 MP 0x000000000
		{"istransientqueued", 0x222}, // SP 0x14037A7B0 MP 0x000000000
		{"istransientloaded", 0x223}, // SP 0x14037A760 MP 0x000000000
		{"loadstartpointtransient", 0x224}, // SP 0x14037A7F0 MP 0x000000000
		{"distance2dsquared", 0x225}, // SP 0x1402914F0 MP 0x140357BF0
		{"getangledelta3d", 0x226}, // SP 0x14028FE10 MP 0x14036F1C0
		{"activateclientexploder", 0x227}, // SP 0x140290710 MP 0x14035B7B0
		{"trajectorycalculateinitialvelocity", 0x228}, // SP 0x1402909E0 MP 0x14035CED0
		{"trajectorycalculateminimumvelocity", 0x229}, // SP 0x140290DF0 MP 0x14035D200
		{"trajectorycalculateexitangle", 0x22A}, // SP 0x140290FA0 MP 0x14035D4C0
		{"trajectoryestimatedesiredinairtime", 0x22B}, // SP 0x1402911D0 MP 0x14035D830
		{"trajectorycomputedeltaheightattime", 0x22C}, // SP 0x140291530 MP 0x14035DFB0
		{"trajectorycanattemptaccuratejump", 0x22D}, // SP 0x140291910 MP 0x14035E0B0
		{"adddebugcommand", 0x22E}, // SP 0x1405D92F0 MP 0x140367D60
		{"ispointinvolume", 0x22F}, // SP 0x140291190 MP 0x14036F5A0
		{"cinematicgettimeinmsec", 0x230}, // SP 0x1402AB600 MP 0x000000000
		{"cinematicgetframe", 0x231}, // SP 0x1402AB4F0 MP 0x000000000
		{"iscinematicloaded", 0x232}, // SP 0x1402AB4D0 MP 0x000000000
		{"bbprint", 0x233},
		{"getenemysquaddata", 0x234}, // SP 0x1405D92F0 MP 0x1403605D0
		{"lookupsoundlength", 0x235},
		{"getscriptablearray", 0x236}, // SP 0x140294380 MP 0x140360720
		{"clearfog", 0x237}, // SP 0x140297840 MP 0x140360C40
		{"setleveldopplerpreset", 0x238}, // SP 0x1402A87C0 MP 0x14037B1D0
		{"screenshake", 0x239}, // SP 0x1402AA280 MP 0x000000000
		{"isusinghdr", 0x23A}, // SP 0x140292220 MP 0x14035E710
		{"isusingssao", 0x23B}, // SP 0x140292220 MP 0x14035E860
		{"_func_23c", 0x23C}, // SP unk MP 0x1403686B0 capsule
		{"playerphysicstraceinfo", 0x23D}, // SP 0x1402A06D0 MP 0x1403686B0
		{"_func_23e", 0x23E}, // SP 0x1402A0900 MP 0x140368C00 capsule
		{"getminchargetime", 0x23F}, // SP 0x14029DDC0 MP 0x140365220
		{"getchargetimepershot", 0x240}, // SP 0x14029DDC0 MP 0x140365440
		{"getmaxchargeshots", 0x241}, // SP 0x1402ACD00 MP 0x140365460
		{"weaponischargeable", 0x242}, // SP 0x1402ACD00 MP 0x140365470
		{"weaponusesheat", 0x243}, // SP 0x1402ACD00 MP 0x140365630
		{"lootserviceonendgame", 0x244}, // SP 0x000000000 MP 0x1403784C0
		{"luinotifyevent", 0x245}, // SP 0x140292D40 MP 0x14035FAF0
		{"lootserviceonstartgame", 0x246}, // SP 0x000000000 MP 0x1403784E0
		{"tournamentreportplayerspm", 0x247}, // SP 0x000000000 MP 0x140378DA0
		{"tournamentreportwinningteam", 0x248}, // SP 0x000000000 MP 0x140378DB0
		{"tournamentreportendofgame", 0x249}, // SP 0x000000000 MP 0x140378DC0
		{"wakeupphysicssphere", 0x24A}, // SP 0x1402A8F70 MP 0x000000000
		{"wakeupragdollsphere", 0x24B}, // SP 0x1402A8FF0 MP 0x000000000
		{"dopplerpitch", 0x24C}, // SP 0x1402946A0 MP 0x14035ABF0
		{"piecewiselinearlookup", 0x24D}, // SP 0x140290110 MP 0x14036DF90
		{"anglestoaxis", 0x24E}, // SP 0x140292B00 MP 0x140359490
		{"visionsetwater", 0x24F}, // SP 0x1402ABB80 MP 0x000000000
		{"sendscriptusageanalysisdata", 0x250}, // SP 0x1405D92F0 MP 0x140360370
		{"resetscriptusageanalysisdata", 0x251}, // SP 0x1405D92F0 MP 0x1403604B0
		{"instantlylogusageanalysisdata", 0x252}, // SP 0x1405D92F0 MP 0x140360530
		{"invertangles", 0x253}, // SP 0x140293F90 MP 0x14035A6C0
		{"rotatevectorinverted", 0x254}, // SP 0x140293FF0 MP 0x14035A7C0
		{"calculatestartorientation", 0x255}, // SP 0x140294500 MP 0x14035A9F0
		{"droptoground", 0x256}, // SP 0x000000000 MP 0x1403844C0
		{"setdemigodmode", 0x257}, // SP 0x1402ADE20 MP 0x000000000
		{"precachelaser", 0x258}, // SP 0x14029F640 MP 0x1403677A0
		{"_func_259", 0x259}, // SP 0x14029F710 MP 0x1403678C0 font
		{"getcsplinecount", 0x25A}, // SP 0x1402954E0 MP 0x1403611E0
		{"getcsplinepointcount", 0x25B}, // SP 0x1402955F0 MP 0x140361200
		{"getcsplinelength", 0x25C}, // SP 0x140295790 MP 0x1403613C0
		{"getcsplinepointid", 0x25D}, // SP 0x140295AD0 MP 0x1403615F0
		{"getcsplinepointlabel", 0x25E}, // SP 0x140295E00 MP 0x140361A30
		{"getcsplinepointtension", 0x25F}, // SP 0x140296150 MP 0x140361DD0
		{"getcsplinepointposition", 0x260}, // SP 0x1402965B0 MP 0x140362020
		{"getcsplinepointcorridordims", 0x261}, // SP 0x140296740 MP 0x140362470
		{"getcsplinepointtangent", 0x262}, // SP 0x140296B10 MP 0x140362680
		{"getcsplinepointdisttonextpoint", 0x263}, // SP 0x140296CE0 MP 0x140362AE0
		{"calccsplineposition", 0x264}, // SP 0x140296EE0 MP 0x140362D50
		{"calccsplinetangent", 0x265}, // SP 0x1402970D0 MP 0x140363430
		{"calccsplinecorridor", 0x266}, // SP 0x140297580 MP 0x1403636C0
		{"setnojipscore", 0x267}, // SP 0x140297820 MP 0x140363AB0
		{"setnojiptime", 0x268}, // SP 0x140297820 MP 0x140363C90
		{"getpredictedentityposition", 0x269}, // SP 0x000000000 MP 0x14037AEC0
		{"gamedvrprohibitrecording", 0x26A}, // SP 0x1405D92F0 MP 0x140363F80
		{"gamedvrstartrecording", 0x26B}, // SP 0x1405D92F0 MP 0x140364110
		{"gamedvrstoprecording", 0x26C}, // SP 0x1405D92F0 MP 0x1403642F0
		{"gamedvrsetvideometadata", 0x26D}, // SP 0x1405D92F0 MP 0x1403644E0
		{"gamedvrprohibitscreenshots", 0x26E}, // SP 0x1405D92F0 MP 0x140364540
		{"gamedvrsetscreenshotmetadata", 0x26F}, // SP 0x1405D92F0 MP 0x140364700
		{"queuedialog", 0x270}, // SP 0x000000000 MP 0x140378930
		{"speechenablegrammar", 0x271}, // SP 0x1405D92F0 MP 0x000000000
		{"speechenable", 0x272}, // SP 0x1405D92F0 MP 0x000000000
		{"livestreamingenable", 0x273}, // SP 0x1405D92F0 MP 0x140364710
		{"livestreamingsetbitrate", 0x274}, // SP 0x1405D92F0 MP 0x140364860
		{"livestreamingsetmetadata", 0x275}, // SP 0x1405D92F0 MP 0x140364970
		{"livestreamingenablearchiving", 0x276}, // SP 0x1405D92F0 MP 0x1403649D0
		{"triggerportableradarping", 0x277}, // SP 0x000000000 MP 0x14037B980
		{"setglaregrimematerial", 0x278}, // SP 0x000000000 MP 0x14037B980
		{"botgetteamlimit", 0x279}, // SP 0x000000000 MP 0x14047A900
		{"spawnfxforclient", 0x27A}, // SP 0x000000000 MP 0x14037E430
		{"botgetteamdifficulty", 0x27B}, // SP 0x000000000 MP 0x14047A8C0
		{"debugstar", 0x27C}, // SP 0x1405D92F0 MP 0x1403675D0
		{"newdebughudelem", 0x27D}, // SP 0x14029CAC0 MP 0x14033CF40
		{"printlightsetsettings", 0x27E}, // SP 0x1405D92F0 MP 0x140363CE0
		{"lightsetdumpstate", 0x27F}, // SP 0x1405D92F0 MP 0x140363E50
		{"getsquadassaultelo", 0x280}, // SP 0x1405D92F0 MP 0x140360610
		{"loadluifile", 0x281}, // SP 0x14029DC90 MP 0x140369F80
		{"isdedicatedserver", 0x282}, // SP 0x000000000 MP 0x14037BBD0
		{"getplaylistversion", 0x283}, // SP 0x000000000 MP 0x14037BBE0
		{"getplaylistid", 0x284}, // SP 0x000000000 MP 0x14037BC00
		{"getactiveclientcount", 0x285}, // SP 0x000000000 MP 0x1403748B0
		{"issquadsmode", 0x286}, // SP 0x000000000 MP 0x140374B00
		{"getsquadassaultsquadindex", 0x287}, // SP 0x1405D92F0 MP 0x140360660
		{"visionsetpostapply", 0x288}, // SP 0x000000000 MP 0x1403772E0
		{"addbot", 0x289}, // SP 0x000000000 MP 0x140376690
		{"ishairrunning", 0x28A}, // SP 0x1402ACD00 MP 0x140365450
		{"getnearbyarrayelements", 0x28B}, // SP 0x140295B60 MP 0x14035BD90
		{"vectorclamp", 0x28C}, // SP 0x140296230 MP 0x14035C6E0
		{"isalliedsentient", 0x28D}, // SP 0x000000000 MP 0x1400777C0
		{"istestclient", 0x28E}, // SP 0x000000000 MP 0x1403767D0
		{"getrandomnodedestination", 0x28F}, // SP 0x000000000 MP 0x140320120
		{"debuglocalizestring", 0x290}, // SP 0x000000000 MP 0x1403775D0
		{"enablesoundcontextoverride", 0x291}, // SP 0x14029E770 MP 0x140368040
		{"disablesoundcontextoverride", 0x292}, // SP 0x14029E7F0 MP 0x140368150
		{"notifyoncommandremove", 0x293}, // SP 0x1402ADDE0 MP 0x000000000
		{"getsndaliasvalue", 0x294}, // SP 0x1402988E0 MP 0x140365640
		{"setsndaliasvalue", 0x295}, // SP 0x14029C2F0 MP 0x140368F40
		{"packedtablelookup", 0x296}, // SP 0x14029B680 MP 0x140364D50
		{"packedtablesectionlookup", 0x297}, // SP 0x14029BD80 MP 0x140365540
		{"packedtablelookupwithrange", 0x298}, // SP 0x14029C1B0 MP 0x140366230
		{"grappletrace", 0x299}, // SP 0x1402ABA30 MP 0x000000000
		{"stopclientexploder", 0x29A}, // SP 0x1405D92F0 MP 0x14035B9A0
		{"closestpointstwosegs", 0x29B}, // SP 0x140291040 MP 0x14036F330
		{"isremovedentity", 0x29C}, // SP 0x14029D240 MP 0x140368760
		{"tablegetrowcount", 0x29D}, // SP 0x14029DE30 MP 0x1403674E0
		{"tablegetcolumncount", 0x29E}, // SP 0x14029DEC0 MP 0x140367620
		{"batteryusepershot", 0x29F}, // SP 0x000000000 MP 0x140375690
		{"batteryreqtouse", 0x2A0}, // SP 0x000000000 MP 0x1403756A0
		{"isweaponmanuallydetonatedbydoubletap", 0x2A1}, // SP 0x14029D890 MP 0x140364AB0
		{"grapplegetmagnets", 0x2A2}, // SP 0x1402ABC20 MP 0x000000000
		{"getweaponname", 0x2A3}, // SP 0x1402A2F20 MP 0x14036F010
		{"activatepersistentclientexploder", 0x2A4}, // SP 0x140290710 MP 0x14035B890
		{"deployriotshield", 0x2A5}, // SP 0x14029DF00 MP 0x14036D0F0
		{"validatecostume", 0x2A6}, // SP 0x1405D92F0 MP 0x14036DAE0
		{"randomcostume", 0x2A7}, // SP 0x1405D92F0 MP 0x14036DA60
		{"shootblank", 0x2A8}, // SP 0x000000000 MP 0x140376390
		{"boidflockupdate", 0x2A9}, // SP 0x1402AE820 MP 0x000000000
		{"debuggetanimname", 0x2AA}, // SP 0x14029AE80 MP 0x140364550
		{"setspmatchdata", 0x2AB}, // SP 0x1405D92F0 MP 0x14036E590
		{"getspmatchdata", 0x2AC}, // SP 0x1402ACD00 MP 0x14036E310
		{"sendspmatchdata", 0x2AD}, // SP 0x1405D92F0 MP 0x14036E3E0
		{"clearspmatchdata", 0x2AE}, // SP 0x1405D92F0 MP 0x14036E470
		{"setspmatchdatadef", 0x2AF}, // SP 0x1405D92F0 MP 0x14036E730
		{"playcinematicforall", 0x2B0}, // SP 0x000000000 MP 0x140375320
		{"preloadcinematicforall", 0x2B1}, // SP 0x000000000 MP 0x1403753F0
		{"stopcinematicforall", 0x2B2}, // SP 0x000000000 MP 0x140375430
		{"capsuletracepassed", 0x2B3}, // SP 0x000000000 MP 0x140375A40
		{"stopfxontagforclient", 0x2B4}, // SP 0x1405D92F0 MP 0x14035C9D0
		{"killfxontagforclient", 0x2B5}, // SP 0x1405D92F0 MP 0x14035D920
		{"isvector", 0x2B6}, // SP 0x140291AD0 MP 0x14035E150
		{"notifychallengecomplete", 0x2B7}, // SP 0x1402A85B0 MP 0x000000000
		{"lootservicestarttrackingplaytime", 0x2B8}, // SP 0x000000000 MP 0x140378570
		{"lootservicestoptrackingplaytime", 0x2B9}, // SP 0x000000000 MP 0x1403787D0
		{"lootservicevalidateplaytime", 0x2BA}, // SP 0x000000000 MP 0x140378850
		{"recordbreadcrumbdataforplayer", 0x2BB}, // SP 0x000000000 MP 0x140379930
		{"getweaponandattachmentmodels", 0x2BC}, // SP 0x14028EBB0 MP 0x140359E00
		{"changewhizbyautosimparams", 0x2BD}, // SP 0x1402A8640 MP 0x000000000
		{"sysprint", 0x2BE}, // SP 0x14029B310 MP 0x140366D50
		{"objective_mlgspectator", 0x2BF}, // SP 0x000000000 MP 0x140383570
		{"setspcheckpointdata", 0x2C0}, // SP 0x1405D92F0 MP 0x14036E8A0
		{"getspcheckpointdata", 0x2C1}, // SP 0x1405D92F0 MP 0x14036E900
		{"isnumber", 0x2C2}, // SP 0x140291E10 MP 0x14035E670
		{"isonlinegame", 0x2C3}, // SP 0x1402A1A30 MP 0x14036DA40
		{"issystemlink", 0x2C4}, // SP 0x1402A1A30 MP 0x14036DAC0
		{"setsoundmasteringfadetime", 0x2C5}, // SP 0x1402A8810 MP 0x000000000
		{"getstanceandmotionstateforplayer", 0x2C6}, // SP 0x000000000 MP 0x140379A80
		{"nodeisnotusable", 0x2C7}, // SP 0x000000000 MP 0x140320DC0
		{"nodesetnotusable", 0x2C8}, // SP 0x000000000 MP 0x140320E20
		{"spawnlinkedfx", 0x2C9}, // SP 0x000000000 MP 0x14037E5E0
		{"spawnlinkedfxforclient", 0x2CA}, // SP 0x000000000 MP 0x14037E650
		{"getplaylistname", 0x2CB}, // SP 0x000000000 MP 0x14037BC40
		{"getlocaltimestring", 0x2CC}, // SP 0x000000000 MP 0x14037BD00
		{"isonwifi", 0x2CD}, // SP 0x000000000 MP 0x140374820
		{"getbuttonsconfig", 0x2CE}, // SP 0x1402A79A0 MP 0x000000000
		{"getchallengeid", 0x2CF}, // SP 0x000000000 MP 0x14037AB40
		{"nodehasremotemissileset", 0x2D0},
		{"nodegetremotemissilename", 0x2D1},
		{"remotemissileenttracetooriginpassed", 0x2D2},
		{"bombingruntracepassed", 0x2D3},
		{"soundsettraceflags", 0x2D4}, // SP 0x1402A7CB0 MP 0x000000000
		{"handlepickupdeployedriotshield", 0x2D5}, // SP 0x14029E120 MP 0x14036D640
		{"getcodanywherecurrentplatform", 0x2D6}, // SP 0x000000000 MP 0x14037BE30
		{"getcostumefromtable", 0x2D7}, // SP 0x1405D92F0 MP 0x14036DB50
		{"visionsetoverdrive", 0x2D8}, // SP 0x1402ABB70 MP 0x000000000
		{"nodegetsplitgroup", 0x2D9}, // SP 0x000000000 MP 0x140320D50
		{"recordbreadcrumbdataforplayersp", 0x2DA}, // SP 0x1405D92F0 MP 0x000000000
		{"getchallengerewarditem", 0x2DB}, // SP 0x000000000 MP 0x14037ABB0
		{"setentplayerxuidforemblem", 0x2DC}, // SP 0x000000000 MP 0x14037E1C0
		{"resetentplayerxuidforemblems", 0x2DD}, // SP 0x000000000 MP 0x14037E1B0
		{"nodesetremotemissilename", 0x2DE},
		{"isshipbuild", 0x2DF}, // SP 0x140292220 MP 0x14036F7F0
		{"strinsertnumericdelimiters", 0x2E0}, // SP 0x140297C80 MP 0x14035EDE0
		{"isscriptedagent", 0x2E1}, // SP 0x000000000 MP 0x1403788D0
		{"playfxonweapon", 0x2E2}, // SP 0x1405D92F0 MP 0x14035B3B0
		{"stopfxonweapon", 0x2E3}, // SP 0x1405D92F0 MP 0x14035D0B0
		{"killfxonweapon", 0x2E4}, // SP 0x1405D92F0 MP 0x14035E170
		{"getdefaultmaxfaceenemydistance", 0x2E5}, // SP 0x1402A8730 MP 0x000000000
		{"applyaccelerationonentity", 0x2E6}, // SP 0x140295A10 MP 0x14035EFF0
		{"applyimpulseonentity", 0x2E7}, // SP 0x140295CF0 MP 0x14035F120
		{"applyimpulseonentity", 0x2E8}, // SP 0x1402AC850 MP 0x000000000
		{"getinventoryitemtype", 0x2E9}, // SP 0x14029E760 MP 0x140366680
		{"getweaponmodelbounds", 0x2EA}, // SP 0x000000000 MP 0x140376DC0
		{"weaponitemplayidleanim", 0x2EB}, // SP 0x000000000 MP 0x140377540
		{"_func_2ec", 0x2EC}, // SP 0x000000000 MP 0x140377880
		{"getstaticmodelcount", 0x2ED}, // SP 0x1402ACD00 MP 0x14036BA50
		{"getstaticmodelname", 0x2EE}, // SP 0x1402A1E10 MP 0x14036BA60
		{"getstaticmodelbounds", 0x2EF}, // SP 0x1402A1FB0 MP 0x14036BA70
		{"findstaticmodelindex", 0x2F0}, // SP 0x1402A21F0 MP 0x14036BB50
		{"getdynentcount", 0x2F1}, // SP 0x1402ACD00 MP 0x14036BC60
		{"getdynentmodelname", 0x2F2}, // SP 0x1402A1E10 MP 0x14036BC70
		{"getdynentmodelbounds", 0x2F3}, // SP 0x1402A1FB0 MP 0x14036BF90
		{"finddynentwithmodelindex", 0x2F4}, // SP 0x1402A21F0 MP 0x14036C100
		{"getentitymodelname", 0x2F5}, // SP 0x1402A1E10 MP 0x14036C120
		{"getentitymodelbounds", 0x2F6}, // SP 0x1402A1FB0 MP 0x14036C170
		{"findentitywithmodelindex", 0x2F7}, // SP 0x1402A21F0 MP 0x14036C310
		{"_func_2f8", 0x2F8}, // SP 0x14028D700 MP 0x1403582B0
		{"_func_2f9", 0x2F9}, // SP 0x14029CAC0 MP 0x14036D300
		{"_func_2fa", 0x2FA}, // SP 0x14029CAC0 MP 0x14036DDC0
		{"_func_2fb", 0x2FB}, // SP 0x14029DB60 MP 0x140365110
		{"_func_2fc", 0x2FC}, // SP 0x14029CB20 MP 0x140367DD0
		{"_func_2fd", 0x2FD}, // SP 0x14029CEA0 MP 0x140368000
		{"_func_2fe", 0x2FE}, // SP 0x14029CEE0 MP 0x140368290
		{"_func_2ff", 0x2FF}, // SP 0x000000000 MP 0x140367110
		{"_func_300", 0x300}, // SP 0x000000000 MP 0x140377870
		{"_func_301", 0x301}, // SP 0x1405D92F0 MP 0x140363E90
	};

	std::unordered_map<std::string, unsigned> method_map =
	{
		{"motionblurhqenable", 0x8000}, // SP 0x14029B0D0 MP 0x140362FD0
		{"setturretdismountorg", 0x8001}, // SP 0x1402A8AC0 MP 0x000000000
		{"setdamagestage", 0x8002}, // SP 0x000000000 MP 0x14055A000
		{"playsoundtoteam", 0x8003}, // SP 0x000000000 MP 0x140378300
		{"playsoundtoplayer", 0x8004}, // SP 0x000000000 MP 0x1403784F0
		{"playerhide", 0x8005}, // SP 0x000000000 MP 0x14037AB70
		{"playershow", 0x8006}, // SP 0x000000000 MP 0x14037ABD0
		{"showtoplayer", 0x8007}, // SP 0x000000000 MP 0x14037AC50
		{"threatdetectedtoplayer", 0x8008}, // SP 0x000000000 MP 0x14037ACB0
		{"clearthreatdetected", 0x8009}, // SP 0x000000000 MP 0x14037ACC0
		{"enableplayeruse", 0x800A}, // SP 0x000000000 MP 0x140374B10
		{"disableplayeruse", 0x800B}, // SP 0x000000000 MP 0x140374BB0
		{"enableammogeneration", 0x800C}, // SP 0x000000000 MP 0x140374D70
		{"disableammogeneration", 0x800D}, // SP 0x000000000 MP 0x140374C90
		{"makescrambler", 0x800E}, // SP 0x000000000 MP 0x14037B890
		{"makeportableradar", 0x800F}, // SP 0x000000000 MP 0x14037B930
		{"clearscrambler", 0x8010}, // SP 0x000000000 MP 0x14037B910
		{"clearportableradar", 0x8011}, // SP 0x000000000 MP 0x14037B960
		{"addplayermantleblockage", 0x8012}, // SP 0x000000000 MP 0x1403760E0
		{"setteamfortrigger", 0x8013}, // SP 0x000000000 MP 0x14037B130
		{"clientclaimtrigger", 0x8014}, // SP 0x000000000 MP 0x14037B220
		{"clientreleasetrigger", 0x8015}, // SP 0x000000000 MP 0x14037B300
		{"releaseclaimedtrigger", 0x8016}, // SP 0x000000000 MP 0x14037B510
		{"isusingonlinedataoffline", 0x8017}, // SP 0x000000000 MP 0x14037AC10
		{"getrestedtime", 0x8018}, // SP 0x000000000 MP 0x14037ACD0
		{"sendleaderboards", 0x8019}, // SP 0x000000000 MP 0x140378210
		{"isonladder", 0x801A}, // SP 0x14028F380 MP 0x14036F540
		{"getcorpseanim", 0x801B}, // SP 0x000000000 MP 0x14037BD60
		{"playerforcedeathanim", 0x801C}, // SP 0x000000000 MP 0x140374A30
		{"attach", 0x801D}, // SP 0x14029DCF0 MP 0x140369D40
		{"getlightfovinner", 0x801E}, // SP 0x1402AD230 MP 0x000000000
		{"getlightfovouter", 0x801F}, // SP 0x1402AD490 MP 0x000000000
		{"setlightfovrange", 0x8020}, // SP 0x1402AD4C0 MP 0x000000000
		{"getlightexponent", 0x8021}, // SP 0x1402AD640 MP 0x000000000
		{"setlightexponent", 0x8022}, // SP 0x1402AD660 MP 0x000000000
		{"startragdoll", 0x8023}, // SP 0x1402AD920 MP 0x14037BA50
		{"startragdollfromimpact", 0x8024}, // SP 0x1402ADA90 MP 0x000000000
		{"queryshouldearlyragdoll", 0x8025}, // SP 0x1405D92F0 MP 0x14035CB60
		{"shouldearlyragdoll", 0x8026}, // SP 0x1405D92F0 MP 0x140377C40
		{"_meth_8027", 0x8027},
		{"_meth_8028", 0x8028}, // SP 0x1402ADE80 MP 0x000000000
		{"thermaldrawenable", 0x8029}, // SP 0x14029AEC0 MP 0x140362D10
		{"detach", 0x802A}, // SP 0x14029E0E0 MP 0x14036A480
		{"detachall", 0x802B}, // SP 0x14029E180 MP 0x14036A4C0
		{"getattachsize", 0x802C}, // SP 0x1402A0610 MP 0x14036C860
		{"getattachmodelname", 0x802D}, // SP 0x1402A0890 MP 0x14036CE90
		{"getattachtagname", 0x802E}, // SP 0x1402A0A90 MP 0x14036D040
		{"setturretcanaidetach", 0x802F}, // SP 0x1402AE150 MP 0x000000000
		{"setturretfov", 0x8030}, // SP 0x1402AE270 MP 0x000000000
		{"setplayerturretfov", 0x8031}, // SP 0x1402AE410 MP 0x000000000
		{"lerpfov", 0x8032}, // SP 0x1402AE620 MP 0x000000000
		{"lerpfovscale", 0x8033}, // SP 0x1402AE6E0 MP 0x000000000
		{"getvalidcoverpeekouts", 0x8034}, // SP 0x1402CBC10 MP 0x000000000
		{"gethighestnodestance", 0x8035}, // SP 0x1402CBBA0 MP 0x14031F650
		{"doesnodeallowstance", 0x8036}, // SP 0x1402CBA70 MP 0x14031F520
		{"doesnodeforcecombat", 0x8037}, // SP 0x1402CBB20 MP 0x14031F5D0
		{"getgunangles", 0x8038}, // SP 0x1402AB240 MP 0x000000000
		{"magicgrenade", 0x8039}, // SP 0x1402AB320 MP 0x000000000
		{"magicgrenademanual", 0x803A}, // SP 0x1402AB6D0 MP 0x000000000
		{"getentnum", 0x803B}, // SP 0x1402ACA20 MP 0x000000000
		{"launch", 0x803C}, // SP 0x1402ACA40 MP 0x000000000
		{"setsoundblend", 0x803D}, // SP 0x1402ACBA0 MP 0x000000000
		{"makefakeai", 0x803E}, // SP 0x1402AFA80 MP 0x000000000
		{"spawndrone", 0x803F}, // SP 0x1402AFAC0 MP 0x000000000
		{"setcorpseremovetimer", 0x8040}, // SP 0x1402AFB90 MP 0x000000000
		{"setlookattext", 0x8041}, // SP 0x1402AFC50 MP 0x000000000
		{"setspawnerteam", 0x8042}, // SP 0x1402AFCA0 MP 0x000000000
		{"addaieventlistener", 0x8043}, // SP 0x1402A8990 MP 0x000000000
		{"removeaieventlistener", 0x8044}, // SP 0x1402A89F0 MP 0x000000000
		{"getlightcolor", 0x8045}, // SP 0x14028F020 MP 0x14036EDF0
		{"setlightcolor", 0x8046}, // SP 0x14028F1F0 MP 0x14036EEE0
		{"getlightradius", 0x8047}, // SP 0x1402ACE10 MP 0x000000000
		{"setlightradius", 0x8048}, // SP 0x1402ACE50 MP 0x000000000
		{"getattachignorecollision", 0x8049}, // SP 0x1402A1060 MP 0x14036D280
		{"hidepart", 0x804A}, // SP 0x1402A1500 MP 0x14036D940
		{"hidepart_allinstances", 0x804B}, // SP 0x1402A1930 MP 0x14036DC50
		{"hideallparts", 0x804C}, // SP 0x1402A1C90 MP 0x14036E320
		{"showpart", 0x804D}, // SP 0x1402A2000 MP 0x14036E480
		{"showallparts", 0x804E}, // SP 0x1402A2370 MP 0x14036ECC0
		{"linkto", 0x804F}, // SP 0x1402A3670 MP 0x140358AF0
		{"linktoblendtotag", 0x8050}, // SP 0x1402A3AB0 MP 0x140359830
		{"unlink", 0x8051}, // SP 0x1402A3AD0 MP 0x140359850
		{"setnormalhealth", 0x8052}, // SP 0x1402AD0F0 MP 0x140379AD0
		{"dodamage", 0x8053}, // SP 0x1402A2B80 MP 0x14036B270
		{"kill", 0x8054}, // SP 0x1402AD260 MP 0x000000000
		{"show", 0x8055}, // SP 0x1402AD6A0 MP 0x14037A9E0
		{"hide", 0x8056}, // SP 0x1402AD6C0 MP 0x14037AAA0
		{"showonclient", 0x8057}, // SP 0x1402AD7A0 MP 0x000000000
		{"hideonclient", 0x8058}, // SP 0x1402AD7D0 MP 0x000000000
		{"disconnectpaths", 0x8059}, // SP 0x14028E1E0 MP 0x14036F090
		{"connectpaths", 0x805A}, // SP 0x14028E390 MP 0x14036F4C0
		{"disconnectnode", 0x805B}, // SP 0x14028E490 MP 0x14036F5F0
		{"connectnode", 0x805C}, // SP 0x14028E750 MP 0x14036F770
		{"startusingheroonlylighting", 0x805D}, // SP 0x1402A8420 MP 0x000000000
		{"stopusingheroonlylighting", 0x805E}, // SP 0x1402A8440 MP 0x000000000
		{"startusinglessfrequentlighting", 0x805F}, // SP 0x1402A8460 MP 0x000000000
		{"stopusinglessfrequentlighting", 0x8060}, // SP 0x1402A8480 MP 0x000000000
		{"setmovingplatformplayerturnrate", 0x8061}, // SP 0x1402A9250 MP 0x000000000
		{"setthermalfog", 0x8062}, // SP 0x1402A92D0 MP 0x000000000
		{"setnightvisionfog", 0x8063}, // SP 0x1402A9BC0 MP 0x000000000
		{"clearthermalfog", 0x8064}, // SP 0x1402AABA0 MP 0x000000000
		{"clearnightvisionfog", 0x8065}, // SP 0x1402AAC10 MP 0x000000000
		{"digitaldistortsetparams", 0x8066},
		{"setmode", 0x8067}, // SP 0x1402AD9A0 MP 0x140375030
		{"getmode", 0x8068}, // SP 0x1402ADCD0 MP 0x140375210
		{"setturretignoregoals", 0x8069}, // SP 0x1402ADF10 MP 0x000000000
		{"islinked", 0x806A}, // SP 0x14028D6B0 MP 0x1403598E0
		{"enablelinkto", 0x806B}, // SP 0x14028EA50 MP 0x14035A950
		{"playsoundatviewheight", 0x806C}, // SP 0x1402ACB60 MP 0x000000000
		{"prefetchsound", 0x806D},
		{"setpitch", 0x806E}, // SP 0x1402A27D0 MP 0x14035AB10
		{"scalepitch", 0x806F}, // SP 0x1402A23E0 MP 0x14035A9C0
		{"setvolume", 0x8070}, // SP 0x1402A2980 MP 0x14035B0D0
		{"scalevolume", 0x8071}, // SP 0x1402A28C0 MP 0x14035AB50
		{"enableportalgroup", 0x8072}, // SP 0x140291F10 MP 0x000000000
		{"setspeakermapmonotostereo", 0x8073}, // SP 0x1405D92F0 MP 0x000000000
		{"setspeakermapmonoto51", 0x8074}, // SP 0x1405D92F0 MP 0x000000000
		{"setdistributed2dsound", 0x8075}, // SP 0x1405D92F0 MP 0x000000000
		{"playsoundasmaster", 0x8076}, // SP 0x1402ACC40 MP 0x1403798B0
		{"playloopsound", 0x8077}, // SP 0x1402ACC60 MP 0x1403798F0
		{"eqon", 0x8078}, // SP 0x1402ACCD0 MP 0x000000000
		{"eqoff", 0x8079}, // SP 0x1402ACD40 MP 0x000000000
		{"haseq", 0x807A}, // SP 0x1402ACDD0 MP 0x000000000
		{"iswaitingonsound", 0x807B}, // SP 0x1402ACE30 MP 0x000000000
		{"playfoley", 0x807C}, // SP 0x1402ACEE0 MP 0x000000000
		{"getnormalhealth", 0x807D}, // SP 0x1402ACFF0 MP 0x1403799E0
		{"playerlinkto", 0x807E}, // SP 0x14028FA50 MP 0x14035B880
		{"playerlinktodelta", 0x807F}, // SP 0x14028FB40 MP 0x14035B8D0
		{"playerlinkweaponviewtodelta", 0x8080}, // SP 0x14028FB60 MP 0x14035BD70
		{"playerlinktoabsolute", 0x8081}, // SP 0x14028FBE0 MP 0x14035C100
		{"playerlinktoblend", 0x8082}, // SP 0x140292720 MP 0x14035DCB0
		{"playerlinkedoffsetenable", 0x8083}, // SP 0x140290050 MP 0x14035C5A0
		{"setwaypointedgestyle_secondaryarrow", 0x8084}, // SP 0x14026DC10 MP 0x14033BBF0
		{"setwaypointiconoffscreenonly", 0x8085}, // SP 0x14026DC60 MP 0x14033BC00
		{"fadeovertime", 0x8086}, // SP 0x14026E020 MP 0x14033BF50
		{"scaleovertime", 0x8087}, // SP 0x14026E160 MP 0x14033C090
		{"moveovertime", 0x8088}, // SP 0x14026E240 MP 0x14033C170
		{"reset", 0x8089}, // SP 0x14026E380 MP 0x14033C230
		{"destroy", 0x808A}, // SP 0x14026E3C0 MP 0x14033C310
		{"setpulsefx", 0x808B}, // SP 0x14026E500 MP 0x14033C3F0
		{"setplayernamestring", 0x808C}, // SP 0x14026E440 MP 0x14033C360
		{"changefontscaleovertime", 0x808D}, // SP 0x14026E0C0 MP 0x14033BFF0
		{"startignoringspotlight", 0x808E}, // SP 0x1402A83E0 MP 0x000000000
		{"stopignoringspotlight", 0x808F}, // SP 0x1402A8400 MP 0x000000000
		{"dontcastshadows", 0x8090}, // SP 0x1402A8970 MP 0x000000000
		{"castshadows", 0x8091}, // SP 0x1402A89D0 MP 0x000000000
		{"setstablemissile", 0x8092}, // SP 0x1402AD800 MP 0x000000000
		{"playersetgroundreferenceent", 0x8093}, // SP 0x1402A9070 MP 0x1403752A0
		{"dontinterpolate", 0x8094}, // SP 0x1402A0070 MP 0x140358360
		{"_meth_8095", 0x8095}, // SP 0x1402AAC80 MP 0x000000000
		{"_meth_8096", 0x8096}, // SP 0x1402AAD20 MP 0x000000000
		{"_meth_8097", 0x8097}, // SP 0x1402AADC0 MP 0x140377CA0
		{"_meth_8098", 0x8098}, // SP 0x1402AAE70 MP 0x000000000
		{"_meth_8099", 0x8099}, // SP 0x1402AAF90 MP 0x000000000
		{"_meth_809a", 0x809A}, // SP 0x1402AC1D0 MP 0x000000000
		{"useby", 0x809B}, // SP 0x1402AC470 MP 0x140377D00
		{"playsound", 0x809C}, // SP 0x1402AC9B0 MP 0x140377F40
		{"_meth_809d", 0x809D},
		{"_meth_809e", 0x809E},
		{"_meth_809f", 0x809F},
		{"_meth_80a0", 0x80A0},
		{"playerlinkedoffsetdisable", 0x80A1}, // SP 0x140290320 MP 0x14035CAF0
		{"playerlinkedsetviewznear", 0x80A2}, // SP 0x140290680 MP 0x14035CE40
		{"playerlinkedsetusebaseangleforviewclamp", 0x80A3}, // SP 0x140290960 MP 0x14035D290
		{"_meth_80a4", 0x80A4}, // SP 0x140294B30 MP 0x14035F4A0
		{"_meth_80a5", 0x80A5}, // SP 0x140295300 MP 0x14035FD70
		{"_meth_80a6", 0x80A6}, // SP 0x1402958C0 MP 0x000000000
		{"_meth_80a7", 0x80A7}, // SP 0x1402960D0 MP 0x000000000
		{"_meth_80a8", 0x80A8}, // SP 0x140297190 MP 0x000000000
		{"_meth_80a9", 0x80A9}, // SP 0x140297C20 MP 0x000000000
		{"_meth_80aa", 0x80AA}, // SP 0x14029DDD0 MP 0x1403663C0
		{"istouching", 0x80AB}, // SP 0x140290390 MP 0x14035B440
		{"getistouchingentities", 0x80AC}, // SP 0x1402904A0 MP 0x14035B650
		{"_meth_80ad", 0x80AD}, // SP 0x1402987B0 MP 0x14035FAA0
		{"stopsounds", 0x80AE}, // SP 0x140290F40 MP 0x14035C610
		{"playrumbleonentity", 0x80AF}, // SP 0x140299090 MP 0x140360200
		{"playrumblelooponentity", 0x80B0}, // SP 0x140299200 MP 0x1403602A0
		{"stoprumble", 0x80B1}, // SP 0x1402997D0 MP 0x1403608E0
		{"delete", 0x80B2}, // SP 0x14029DAA0 MP 0x140366320
		{"setmodel", 0x80B3}, // SP 0x14029C8F0 MP 0x1403677D0
		{"_meth_80b4", 0x80B4}, // SP 0x14029A0F0 MP 0x140361C30
		{"_meth_80b5", 0x80B5}, // SP 0x14029A250 MP 0x140361F90
		{"_meth_80b6", 0x80B6},
		{"_meth_80b7", 0x80B7},
		{"thermalvisionon", 0x80B8}, // SP 0x14029A450 MP 0x1403623A0
		{"thermalvisionoff", 0x80B9}, // SP 0x14029AAF0 MP 0x140362B70
		{"_meth_80ba", 0x80BA}, // SP 0x14029BF20 MP 0x140364290
		{"_meth_80bb", 0x80BB}, // SP 0x14029C080 MP 0x140364630
		{"_meth_80bc", 0x80BC}, // SP 0x000000000 MP 0x140364910
		{"autospotoverlayon", 0x80BD}, // SP 0x000000000 MP 0x140364A50
		{"_meth_80be", 0x80BE}, // SP 0x000000000 MP 0x140364D40
		{"_meth_80bf", 0x80BF},
		{"setcontents", 0x80C0}, // SP 0x140297CC0 MP 0x14035FDD0
		{"makeusable", 0x80C1}, // SP 0x140297E90 MP 0x1403600D0
		{"makeunusable", 0x80C2}, // SP 0x140297FF0 MP 0x140360320
		{"_meth_80c3", 0x80C3}, // SP 0x1402A1730 MP 0x140359740
		{"_meth_80c4", 0x80C4}, // SP 0x1402A1A80 MP 0x140359DC0
		{"_meth_80c5", 0x80C5}, // SP 0x140266A20 MP 0x000000000
		{"_meth_80c6", 0x80C6}, // SP 0x1402634D0 MP 0x000000000
		{"_meth_80c7", 0x80C7}, // SP 0x1404657D0 MP 0x000000000
		{"_meth_80c8", 0x80C8}, // SP 0x140463A30 MP 0x000000000
		{"_meth_80c9", 0x80C9}, // SP 0x140465630 MP 0x000000000
		{"_meth_80ca", 0x80CA}, // SP 0x1404656C0 MP 0x000000000
		{"_meth_80cb", 0x80CB}, // SP 0x140464F50 MP 0x000000000
		{"settext", 0x80CC}, // SP 0x14026CE20 MP 0x14033AF40
		{"_meth_80cd", 0x80CD}, // SP 0x1405D92F0 MP 0x14033B030
		{"setshader", 0x80CE}, // SP 0x14026CEE0 MP 0x14033B040
		{"_meth_80cf", 0x80CF}, // SP 0x14026D230 MP 0x14033B3A0
		{"_meth_80d0", 0x80D0}, // SP 0x14026D3B0 MP 0x14033B530
		{"_meth_80d1", 0x80D1}, // SP 0x14026D490 MP 0x14033B600
		{"_meth_80d2", 0x80D2}, // SP 0x14026D560 MP 0x14033B6C0
		{"_meth_80d3", 0x80D3}, // SP 0x14026D630 MP 0x14033B760
		{"_meth_80d4", 0x80D4}, // SP 0x14026D700 MP 0x14033B810
		{"_meth_80d5", 0x80D5}, // SP 0x14026D7D0 MP 0x14033B8D0
		{"_meth_80d6", 0x80D6}, // SP 0x14026D8A0 MP 0x14033B970
		{"setclock", 0x80D7}, // SP 0x14026D970 MP 0x14033BA20
		{"setclockup", 0x80D8}, // SP 0x14026D990 MP 0x14033BA40
		{"setvalue", 0x80D9}, // SP 0x14026D9B0 MP 0x14033BA60
		{"setwaypoint", 0x80DA}, // SP 0x14026DA50 MP 0x14033BAE0
		{"_meth_80db", 0x80DB}, // SP 0x14026DBC0 MP 0x14033BBE0
		{"setcursorhint", 0x80DC}, // SP 0x1402983E0 MP 0x1403607E0
		{"sethintstring", 0x80DD}, // SP 0x140299300 MP 0x140361060
		{"_meth_80de", 0x80DE}, // SP 0x000000000 MP 0x140375C60
		{"forceusehinton", 0x80DF}, // SP 0x140299730 MP 0x140361260
		{"_meth_80e0", 0x80E0}, // SP 0x140299930 MP 0x140361530
		{"_meth_80e1", 0x80E1}, // SP 0x140298080 MP 0x140360440
		{"_meth_80e2", 0x80E2}, // SP 0x140298160 MP 0x140360570
		{"entitywillneverchange", 0x80E3}, // SP 0x1405D92F0 MP 0x140360620
		{"_meth_80e4", 0x80E4}, // SP 0x140291140 MP 0x14035D1B0
		{"_meth_80e5", 0x80E5}, // SP 0x1402912D0 MP 0x14035D310
		{"isfiringturret", 0x80E6}, // SP 0x140291490 MP 0x14035D460
		{"startbarrelspin", 0x80E7}, // SP 0x1402915B0 MP 0x14035D650
		{"stopbarrelspin", 0x80E8}, // SP 0x1402916E0 MP 0x14035D8D0
		{"getbarrelspinrate", 0x80E9}, // SP 0x140291A10 MP 0x14035DA40
		{"_meth_80ea", 0x80EA}, // SP 0x000000000 MP 0x14035C870
		{"remotecontrolturretoff", 0x80EB}, // SP 0x000000000 MP 0x14035CDD0
		{"_meth_80ec", 0x80EC}, // SP 0x140293380 MP 0x14035E870
		{"getturretowner", 0x80ED}, // SP 0x140293430 MP 0x14035EE50
		{"_meth_80ee", 0x80EE}, // SP 0x140268590 MP 0x000000000
		{"_meth_80ef", 0x80EF}, // SP 0x1402685F0 MP 0x000000000
		{"_meth_80f0", 0x80F0}, // SP 0x140268640 MP 0x000000000
		{"_meth_80f1", 0x80F1}, // SP 0x140268690 MP 0x000000000
		{"_meth_80f2", 0x80F2}, // SP 0x1402686F0 MP 0x000000000
		{"_meth_80f3", 0x80F3}, // SP 0x140268750 MP 0x000000000
		{"_meth_80f4", 0x80F4}, // SP 0x1402687A0 MP 0x000000000
		{"_meth_80f5", 0x80F5}, // SP 0x140268AF0 MP 0x000000000
		{"_meth_80f6", 0x80F6}, // SP 0x140268CC0 MP 0x000000000
		{"_meth_80f7", 0x80F7}, // SP 0x140268D20 MP 0x000000000
		{"_meth_80f8", 0x80F8}, // SP 0x140268D80 MP 0x000000000
		{"_meth_80f9", 0x80F9}, // SP 0x140268E10 MP 0x000000000
		{"_meth_80fa", 0x80FA}, // SP 0x140268EC0 MP 0x000000000
		{"_meth_80fb", 0x80FB}, // SP 0x140262E80 MP 0x14032F050
		{"_meth_80fc", 0x80FC}, // SP 0x140268890 MP 0x000000000
		{"_meth_80fd", 0x80FD}, // SP 0x1402688E0 MP 0x000000000
		{"_meth_80fe", 0x80FE}, // SP 0x1402689B0 MP 0x000000000
		{"_meth_80ff", 0x80FF}, // SP 0x140268A40 MP 0x000000000
		{"_meth_8100", 0x8100}, // SP 0x1402611F0 MP 0x14032CF70
		{"_meth_8101", 0x8101}, // SP 0x140261420 MP 0x14032D270
		{"_meth_8102", 0x8102}, // SP 0x140268930 MP 0x000000000
		{"_meth_8103", 0x8103}, // SP 0x1402616E0 MP 0x000000000
		{"_meth_8104", 0x8104}, // SP 0x1402667A0 MP 0x000000000
		{"setsentryowner", 0x8105}, // SP 0x1402935E0 MP 0x14035EF40
		{"setsentrycarrier", 0x8106}, // SP 0x140293F00 MP 0x14035F1B0
		{"setturretminimapvisible", 0x8107}, // SP 0x1402942F0 MP 0x14035F6D0
		{"settargetentity", 0x8108}, // SP 0x140294600 MP 0x14035F860
		{"snaptotargetentity", 0x8109}, // SP 0x140294A20 MP 0x14035FB70
		{"cleartargetentity", 0x810A}, // SP 0x140294E90 MP 0x14035FE30
		{"getturrettarget", 0x810B}, // SP 0x140295270 MP 0x140360040
		{"setplayerspread", 0x810C}, // SP 0x1402957F0 MP 0x1403606C0
		{"_meth_810d", 0x810D}, // SP 0x140295A70 MP 0x140360990
		{"_meth_810e", 0x810E}, // SP 0x140295D50 MP 0x1403609A0
		{"_meth_810f", 0x810F}, // SP 0x14028A710 MP 0x000000000
		{"_meth_8110", 0x8110}, // SP 0x14028A730 MP 0x000000000
		{"_meth_8111", 0x8111}, // SP 0x14028A750 MP 0x000000000
		{"_meth_8112", 0x8112}, // SP 0x14028A770 MP 0x000000000
		{"_meth_8113", 0x8113}, // SP 0x14028A790 MP 0x000000000
		{"_meth_8114", 0x8114}, // SP 0x14028A7B0 MP 0x000000000
		{"_meth_8115", 0x8115}, // SP 0x14028A7C0 MP 0x000000000
		{"_meth_8116", 0x8116}, // SP 0x14028A7E0 MP 0x000000000
		{"_meth_8117", 0x8117}, // SP 0x14028A800 MP 0x000000000
		{"_meth_8118", 0x8118}, // SP 0x14028A8E0 MP 0x000000000
		{"_meth_8119", 0x8119}, // SP 0x14028A960 MP 0x000000000
		{"_meth_811a", 0x811A}, // SP 0x140261760 MP 0x14032D140
		{"_meth_811b", 0x811B}, // SP 0x140261870 MP 0x14032D2D0
		{"_meth_811c", 0x811C}, // SP 0x140261950 MP 0x14032D630
		{"_meth_811d", 0x811D}, // SP 0x140261A30 MP 0x14032D780
		{"_meth_811e", 0x811E}, // SP 0x140269090 MP 0x000000000
		{"_meth_811f", 0x811F}, // SP 0x140269510 MP 0x000000000
		{"_meth_8120", 0x8120},
		{"_meth_8121", 0x8121}, // SP 0x1402692D0 MP 0x000000000
		{"_meth_8122", 0x8122}, // SP 0x140269350 MP 0x000000000
		{"_meth_8123", 0x8123}, // SP 0x140269650 MP 0x000000000
		{"_meth_8124", 0x8124},
		{"_meth_8125", 0x8125},
		{"_meth_8126", 0x8126},
		{"_meth_8127", 0x8127},
		{"_meth_8128", 0x8128}, // SP 0x140269650 MP 0x000000000
		{"_meth_8129", 0x8129}, // SP 0x140269370 MP 0x000000000
		{"_meth_812a", 0x812A}, // SP 0x1402680B0 MP 0x000000000
		{"_meth_812b", 0x812B}, // SP 0x1402681E0 MP 0x000000000
		{"_meth_812c", 0x812C}, // SP 0x1402691F0 MP 0x000000000
		{"_meth_812d", 0x812D}, // SP 0x140268290 MP 0x000000000
		{"_meth_812e", 0x812E}, // SP 0x140267290 MP 0x14032B170
		{"_meth_812f", 0x812F}, // SP 0x140267650 MP 0x14032B720
		{"_meth_8130", 0x8130}, // SP 0x140262E20 MP 0x14032F570
		{"_meth_8131", 0x8131}, // SP 0x1402684C0 MP 0x000000000
		{"_meth_8132", 0x8132}, // SP 0x140261580 MP 0x14032CD50
		{"_meth_8133", 0x8133}, // SP 0x140261670 MP 0x14032CF00
		{"_meth_8134", 0x8134}, // SP 0x140268530 MP 0x000000000
		{"setconvergencetime", 0x8135}, // SP 0x140295E90 MP 0x140360A40
		{"setconvergenceheightpercent", 0x8136}, // SP 0x140296650 MP 0x140360E90
		{"setturretteam", 0x8137}, // SP 0x1402967E0 MP 0x140361150
		{"_meth_8138", 0x8138}, // SP 0x140296C90 MP 0x140361320
		{"startfiring", 0x8139}, // SP 0x140296D70 MP 0x140361370
		{"stopfiring", 0x813A}, // SP 0x140296E90 MP 0x1403615A0
		{"_meth_813b", 0x813B}, // SP 0x1402D7DD0 MP 0x140077920
		{"freeentitysentient", 0x813C}, // SP 0x1402D7EE0 MP 0x140077A40
		{"_meth_813d", 0x813D}, // SP 0x1402D7FE0 MP 0x000000000
		{"_meth_813e", 0x813E}, // SP 0x1402D8040 MP 0x000000000
		{"_meth_813f", 0x813F}, // SP 0x1402D80E0 MP 0x000000000
		{"_meth_8140", 0x8140}, // SP 0x14028A390 MP 0x000000000
		{"_meth_8141", 0x8141},
		{"_meth_8142", 0x8142}, // SP 0x14028A3D0 MP 0x000000000
		{"_meth_8143", 0x8143}, // SP 0x14028A370 MP 0x000000000
		{"_meth_8144", 0x8144}, // SP 0x14028A410 MP 0x000000000
		{"_meth_8145", 0x8145}, // SP 0x14028A490 MP 0x000000000
		{"_meth_8146", 0x8146}, // SP 0x14028A4B0 MP 0x000000000
		{"_meth_8147", 0x8147}, // SP 0x14028A4C0 MP 0x000000000
		{"_meth_8148", 0x8148}, // SP 0x14028A4E0 MP 0x000000000
		{"_meth_8149", 0x8149}, // SP 0x14028A500 MP 0x000000000
		{"_meth_814a", 0x814A}, // SP 0x14028A520 MP 0x000000000
		{"_meth_814b", 0x814B}, // SP 0x14028A530 MP 0x000000000
		{"_meth_814c", 0x814C}, // SP 0x14028A550 MP 0x000000000
		{"_meth_814d", 0x814D}, // SP 0x14028A570 MP 0x000000000
		{"_meth_814e", 0x814E}, // SP 0x14028A590 MP 0x000000000
		{"_meth_814f", 0x814F}, // SP 0x14028A5A0 MP 0x000000000
		{"_meth_8150", 0x8150}, // SP 0x14028A5C0 MP 0x000000000
		{"_meth_8151", 0x8151}, // SP 0x14028A5E0 MP 0x000000000
		{"_meth_8152", 0x8152}, // SP 0x14028A650 MP 0x000000000
		{"_meth_8153", 0x8153}, // SP 0x14028A690 MP 0x000000000
		{"_meth_8154", 0x8154}, // SP 0x14028A6E0 MP 0x000000000
		{"_meth_8155", 0x8155}, // SP 0x14028A700 MP 0x000000000
		{"_meth_8156", 0x8156}, // SP 0x1405D92F0 MP 0x140361680
		{"_meth_8157", 0x8157}, // SP 0x140297730 MP 0x140362260
		{"_meth_8158", 0x8158}, // SP 0x140297890 MP 0x140362410
		{"_meth_8159", 0x8159}, // SP 0x140297D20 MP 0x1403629F0
		{"_meth_815a", 0x815A}, // SP 0x140297F90 MP 0x140362CB0
		{"setautorotationdelay", 0x815B}, // SP 0x1402980D0 MP 0x140363040
		{"_meth_815c", 0x815C}, // SP 0x140298800 MP 0x1403638A0
		{"restoredefaultdroppitch", 0x815D}, // SP 0x140298880 MP 0x140363B30
		{"turretfiredisable", 0x815E}, // SP 0x140299040 MP 0x140363CF0
		{"_meth_815f", 0x815F}, // SP 0x1402495F0 MP 0x000000000
		{"_meth_8160", 0x8160}, // SP 0x1402496C0 MP 0x000000000
		{"_meth_8161", 0x8161}, // SP 0x140249790 MP 0x000000000
		{"_meth_8162", 0x8162}, // SP 0x140249810 MP 0x000000000
		{"_meth_8163", 0x8163}, // SP 0x140249950 MP 0x000000000
		{"_meth_8164", 0x8164}, // SP 0x140249A30 MP 0x000000000
		{"_meth_8165", 0x8165}, // SP 0x140249AC0 MP 0x000000000
		{"_meth_8166", 0x8166}, // SP 0x140249B00 MP 0x000000000
		{"_meth_8167", 0x8167}, // SP 0x140249BE0 MP 0x000000000
		{"_meth_8168", 0x8168}, // SP 0x140249C70 MP 0x000000000
		{"_meth_8169", 0x8169}, // SP 0x140262B90 MP 0x14032E0A0
		{"_meth_816a", 0x816A}, // SP 0x140249D10 MP 0x000000000
		{"_meth_816b", 0x816B}, // SP 0x140249EE0 MP 0x000000000
		{"_meth_816c", 0x816C}, // SP 0x14024A0E0 MP 0x000000000
		{"_meth_816d", 0x816D}, // SP 0x14024A200 MP 0x000000000
		{"_meth_816e", 0x816E}, // SP 0x14024A2D0 MP 0x000000000
		{"_meth_816f", 0x816F}, // SP 0x14024A3F0 MP 0x000000000
		{"_meth_8170", 0x8170}, // SP 0x14024A480 MP 0x000000000
		{"_meth_8171", 0x8171}, // SP 0x14024A5C0 MP 0x000000000
		{"_meth_8172", 0x8172}, // SP 0x14024A630 MP 0x000000000
		{"_meth_8173", 0x8173}, // SP 0x14024A710 MP 0x000000000
		{"_meth_8174", 0x8174}, // SP 0x14024A880 MP 0x000000000
		{"_meth_8175", 0x8175}, // SP 0x14024AAD0 MP 0x000000000
		{"_meth_8176", 0x8176}, // SP 0x14024AB30 MP 0x000000000
		{"_meth_8177", 0x8177}, // SP 0x14024AB90 MP 0x000000000
		{"_meth_8178", 0x8178}, // SP 0x14024AC50 MP 0x000000000
		{"getenemysqdist", 0x8179}, // SP 0x1402D77E0 MP 0x1400772C0
		{"_meth_817a", 0x817A}, // SP 0x1402D7820 MP 0x140077300
		{"setthreatbiasgroup", 0x817B}, // SP 0x1402D7BE0 MP 0x1400776E0
		{"getthreatbiasgroup", 0x817C}, // SP 0x1402D7C50 MP 0x140077750
		{"turretfireenable", 0x817D}, // SP 0x140299220 MP 0x140363EE0
		{"setturretmodechangewait", 0x817E}, // SP 0x1402994D0 MP 0x140364690
		{"usetriggerrequirelookat", 0x817F}, // SP 0x140297DE0 MP 0x14035EEE0
		{"getstance", 0x8180}, // SP 0x14029D1C0 MP 0x140364F90
		{"_meth_8181", 0x8181}, // SP 0x14029D430 MP 0x140365230
		{"itemweaponsetammo", 0x8182}, // SP 0x140299FA0 MP 0x140360F20
		{"_meth_8183", 0x8183}, // SP 0x14029A5A0 MP 0x140361420
		{"gettagorigin", 0x8184}, // SP 0x14029E360 MP 0x140368090
		{"gettagangles", 0x8185}, // SP 0x14029E810 MP 0x140368320
		{"_meth_8186", 0x8186}, // SP 0x14028E580 MP 0x14036D490
		{"stunplayer", 0x8187}, // SP 0x14028F160 MP 0x14036DEC0
		{"_meth_8188", 0x8188}, // SP 0x14029B970 MP 0x140366E70
		{"_meth_8189", 0x8189}, // SP 0x14029BB30 MP 0x140367230
		{"setdepthoffield", 0x818A}, // SP 0x14029EF20 MP 0x140368890
		{"setviewmodeldepthoffield", 0x818B}, // SP 0x14029F990 MP 0x140369900
		{"_meth_818c", 0x818C}, // SP 0x1402A15D0 MP 0x14036AD20
		{"_meth_818d", 0x818D}, // SP 0x140247D90 MP 0x000000000
		{"_meth_818e", 0x818E}, // SP 0x140247DE0 MP 0x000000000
		{"_meth_818f", 0x818F}, // SP 0x140247E50 MP 0x000000000
		{"_meth_8190", 0x8190}, // SP 0x140247E90 MP 0x000000000
		{"_meth_8191", 0x8191}, // SP 0x140247F00 MP 0x000000000
		{"_meth_8192", 0x8192}, // SP 0x140247F90 MP 0x000000000
		{"_meth_8193", 0x8193}, // SP 0x140248160 MP 0x000000000
		{"_meth_8194", 0x8194}, // SP 0x1402484A0 MP 0x000000000
		{"_meth_8195", 0x8195}, // SP 0x1402485A0 MP 0x000000000
		{"_meth_8196", 0x8196}, // SP 0x1402485F0 MP 0x000000000
		{"_meth_8197", 0x8197}, // SP 0x140248640 MP 0x000000000
		{"_meth_8198", 0x8198}, // SP 0x1402486A0 MP 0x000000000
		{"_meth_8199", 0x8199}, // SP 0x1402486F0 MP 0x000000000
		{"_meth_819a", 0x819A},
		{"_meth_819b", 0x819B},
		{"_meth_819c", 0x819C},
		{"_meth_819d", 0x819D},
		{"_meth_819e", 0x819E}, // SP 0x1402487C0 MP 0x000000000
		{"_meth_819f", 0x819F}, // SP 0x140248870 MP 0x000000000
		{"_meth_81a0", 0x81A0}, // SP 0x1402488C0 MP 0x000000000
		{"getnegotiationstartnode", 0x81A1}, // SP 0x140248970 MP 0x14044F980
		{"getnegotiationendnode", 0x81A2}, // SP 0x1402489E0 MP 0x14044F830
		{"getnegotiationnextnode", 0x81A3}, // SP 0x140248A50 MP 0x14044F8D0
		{"_meth_81a4", 0x81A4}, // SP 0x140248B30 MP 0x000000000
		{"_meth_81a5", 0x81A5}, // SP 0x140248BD0 MP 0x000000000
		{"_meth_81a6", 0x81A6}, // SP 0x140248CC0 MP 0x000000000
		{"_meth_81a7", 0x81A7}, // SP 0x140248DD0 MP 0x000000000
		{"_meth_81a8", 0x81A8}, // SP 0x140247750 MP 0x000000000
		{"_meth_81a9", 0x81A9}, // SP 0x140248E30 MP 0x000000000
		{"_meth_81aa", 0x81AA}, // SP 0x140248FB0 MP 0x000000000
		{"_meth_81ab", 0x81AB}, // SP 0x1402490D0 MP 0x000000000
		{"_meth_81ac", 0x81AC}, // SP 0x140249170 MP 0x000000000
		{"_meth_81ad", 0x81AD}, // SP 0x140249290 MP 0x000000000
		{"_meth_81ae", 0x81AE}, // SP 0x140249460 MP 0x000000000
		{"_meth_81af", 0x81AF}, // SP 0x1402494B0 MP 0x000000000
		{"_meth_81b0", 0x81B0}, // SP 0x140249500 MP 0x000000000
		{"_meth_81b1", 0x81B1}, // SP 0x1402A15D0 MP 0x14036ADA0
		{"_meth_81b2", 0x81B2}, // SP 0x1402A15D0 MP 0x14036AE60
		{"viewkick", 0x81B3}, // SP 0x1402A1640 MP 0x14036AFA0
		{"_meth_81b4", 0x81B4}, // SP 0x14029B1E0 MP 0x1403668B0
		{"getentitynumber", 0x81B5}, // SP 0x1402998B0 MP 0x140364980
		{"autoboltmissileeffects", 0x81B6}, // SP 0x140299AA0 MP 0x140364B50
		{"enablegrenadetouchdamage", 0x81B7}, // SP 0x140299CA0 MP 0x140364F40
		{"disablegrenadetouchdamage", 0x81B8}, // SP 0x140299F20 MP 0x1403650C0
		{"_meth_81b9", 0x81B9}, // SP 0x14029AF00 MP 0x1403665A0
		{"_meth_81ba", 0x81BA}, // SP 0x14024AF80 MP 0x000000000
		{"_meth_81bb", 0x81BB}, // SP 0x14024B160 MP 0x000000000
		{"_meth_81bc", 0x81BC}, // SP 0x14024B3B0 MP 0x000000000
		{"_meth_81bd", 0x81BD}, // SP 0x14024B4F0 MP 0x000000000
		{"_meth_81be", 0x81BE}, // SP 0x14024B570 MP 0x000000000
		{"_meth_81bf", 0x81BF}, // SP 0x14024B910 MP 0x000000000
		{"_meth_81c0", 0x81C0}, // SP 0x14024BA40 MP 0x000000000
		{"_meth_81c1", 0x81C1}, // SP 0x14024BB20 MP 0x000000000
		{"_meth_81c2", 0x81C2}, // SP 0x14024BB90 MP 0x000000000
		{"_meth_81c3", 0x81C3}, // SP 0x14024BC40 MP 0x000000000
		{"_meth_81c4", 0x81C4}, // SP 0x1402D7F30 MP 0x140077AA0
		{"_meth_81c5", 0x81C5}, // SP 0x1402D7F90 MP 0x140077B00
		{"_meth_81c6", 0x81C6}, // SP 0x14024BD90 MP 0x000000000
		{"_meth_81c7", 0x81C7}, // SP 0x14024C010 MP 0x000000000
		{"_meth_81c8", 0x81C8}, // SP 0x14024C150 MP 0x000000000
		{"_meth_81c9", 0x81C9}, // SP 0x14024C240 MP 0x000000000
		{"_meth_81ca", 0x81CA}, // SP 0x14024C250 MP 0x000000000
		{"_meth_81cb", 0x81CB}, // SP 0x14024C270 MP 0x000000000
		{"_meth_81cc", 0x81CC}, // SP 0x14024C290 MP 0x000000000
		{"_meth_81cd", 0x81CD}, // SP 0x14024C2F0 MP 0x000000000
		{"_meth_81ce", 0x81CE}, // SP 0x14024C360 MP 0x000000000
		{"_meth_81cf", 0x81CF}, // SP 0x14024C430 MP 0x000000000
		{"_meth_81d0", 0x81D0}, // SP 0x14024C520 MP 0x000000000
		{"_meth_81d1", 0x81D1}, // SP 0x14024C560 MP 0x000000000
		{"_meth_81d2", 0x81D2}, // SP 0x14024C5A0 MP 0x000000000
		{"_meth_81d3", 0x81D3}, // SP 0x14024C5E0 MP 0x000000000
		{"_meth_81d4", 0x81D4}, // SP 0x140247BF0 MP 0x000000000
		{"_meth_81d5", 0x81D5}, // SP 0x140247CE0 MP 0x000000000
		{"_meth_81d6", 0x81D6}, // SP 0x140247930 MP 0x000000000
		{"_meth_81d7", 0x81D7}, // SP 0x14029B180 MP 0x140366790
		{"entityradiusdamage", 0x81D8}, // SP 0x1402A3840 MP 0x14036C130
		{"_meth_81d9", 0x81D9}, // SP 0x14028DCF0 MP 0x14036CAB0
		{"bddatachunk::deserialize", 0x81DA}, // SP 0x14028E350 MP 0x14036D0C0
		{"_meth_81db", 0x81DB}, // SP 0x14028E430 MP 0x14036D150
		{"_meth_81dc", 0x81DC}, // SP 0x14029A340 MP 0x140365D80
		{"_meth_81dd", 0x81DD}, // SP 0x14029A4E0 MP 0x140365E90
		{"_meth_81de", 0x81DE}, // SP 0x14029A700 MP 0x140366180
		{"missilesetflightmodedirect", 0x81DF}, // SP 0x14029AA10 MP 0x1403663A0
		{"missilesetflightmodetop", 0x81E0}, // SP 0x14029AB50 MP 0x140366480
		{"getlightintensity", 0x81E1}, // SP 0x14028EA00 MP 0x14036E8B0
		{"_meth_81e2", 0x81E2}, // SP 0x14028EB40 MP 0x14036EAB0
		{"isragdoll", 0x81E3}, // SP 0x14028F2F0 MP 0x14036F2E0
		{"setmovespeedscale", 0x81E4}, // SP 0x1402A2860 MP 0x14036EF80
		{"cameralinkto", 0x81E5}, // SP 0x14028FFC0 MP 0x1403584E0
		{"cameraunlink", 0x81E6}, // SP 0x1402900C0 MP 0x140358750
		{"_meth_81e7", 0x81E7}, // SP 0x140248F30 MP 0x000000000
		{"_meth_81e8", 0x81E8}, // SP 0x1402491F0 MP 0x000000000
		{"_meth_81e9", 0x81E9}, // SP 0x140249230 MP 0x000000000
		{"_meth_81ea", 0x81EA}, // SP 0x140249370 MP 0x000000000
		{"_meth_81eb", 0x81EB}, // SP 0x140249650 MP 0x000000000
		{"_meth_81ec", 0x81EC}, // SP 0x140249710 MP 0x000000000
		{"_meth_81ed", 0x81ED}, // SP 0x1402499B0 MP 0x000000000
		{"_meth_81ee", 0x81EE}, // SP 0x140249A70 MP 0x000000000
		{"_meth_81ef", 0x81EF}, // SP 0x140249B80 MP 0x000000000
		{"_meth_81f0", 0x81F0}, // SP 0x140249C30 MP 0x000000000
		{"_meth_81f1", 0x81F1}, // SP 0x140249CC0 MP 0x000000000
		{"_meth_81f2", 0x81F2}, // SP 0x140249E00 MP 0x000000000
		{"_meth_81f3", 0x81F3}, // SP 0x140249E50 MP 0x000000000
		{"_meth_81f4", 0x81F4}, // SP 0x14024A010 MP 0x000000000
		{"_meth_81f5", 0x81F5}, // SP 0x14024A080 MP 0x000000000
		{"_meth_81f6", 0x81F6}, // SP 0x14024A190 MP 0x000000000
		{"_meth_81f7", 0x81F7}, // SP 0x14024A330 MP 0x000000000
		{"_meth_81f8", 0x81F8}, // SP 0x14024A390 MP 0x000000000
		{"_meth_81f9", 0x81F9}, // SP 0x14024A260 MP 0x000000000
		{"_meth_81fa", 0x81FA}, // SP 0x14024A440 MP 0x000000000
		{"_meth_81fb", 0x81FB}, // SP 0x14024A4F0 MP 0x000000000
		{"_meth_81fc", 0x81FC}, // SP 0x14024A6B0 MP 0x000000000
		{"_meth_81fd", 0x81FD}, // SP 0x14024A780 MP 0x000000000
		{"_meth_81fe", 0x81FE}, // SP 0x14024A950 MP 0x000000000
		{"_meth_81ff", 0x81FF}, // SP 0x14024AC00 MP 0x000000000
		{"_meth_8200", 0x8200}, // SP 0x14024AD60 MP 0x000000000
		{"_meth_8201", 0x8201}, // SP 0x14024AE60 MP 0x000000000
		{"_meth_8202", 0x8202}, // SP 0x14024B1D0 MP 0x000000000
		{"_meth_8203", 0x8203}, // SP 0x14024B2C0 MP 0x000000000
		{"controlslinkto", 0x8204}, // SP 0x1402905D0 MP 0x140358F20
		{"controlsunlink", 0x8205}, // SP 0x1402908B0 MP 0x1403593C0
		{"_meth_8206", 0x8206}, // SP 0x000000000 MP 0x14035AB40
		{"_meth_8207", 0x8207}, // SP 0x1402AA5F0 MP 0x000000000
		{"_meth_8208", 0x8208}, // SP 0x000000000 MP 0x14035ABE0
		{"_meth_8209", 0x8209},
		{"_meth_820a", 0x820A}, // SP 0x000000000 MP 0x14035B100
		{"_meth_820b", 0x820B}, // SP 0x000000000 MP 0x14035B170
		{"_meth_820c", 0x820C}, // SP 0x000000000 MP 0x14035B380
		{"_meth_820d", 0x820D}, // SP 0x000000000 MP 0x14035B210
		{"_meth_820e", 0x820E}, // SP 0x000000000 MP 0x14035B270
		{"drivevehicleandcontrolturret", 0x820F}, // SP 0x140291CD0 MP 0x14035C250
		{"drivevehicleandcontrolturretoff", 0x8210}, // SP 0x140292090 MP 0x14035C420
		{"_meth_8211", 0x8211}, // SP 0x1402923D0 MP 0x14035C930
		{"_meth_8212", 0x8212}, // SP 0x1402931B0 MP 0x14035CBB0
		{"_meth_8213", 0x8213}, // SP 0x1402940E0 MP 0x14035D6A0
		{"_meth_8214", 0x8214}, // SP 0x140296080 MP 0x14035F750
		{"_meth_8215", 0x8215}, // SP 0x1402961E0 MP 0x14035F810
		{"_meth_8216", 0x8216}, // SP 0x140296540 MP 0x14035FB00
		{"_meth_8217", 0x8217}, // SP 0x1402966E0 MP 0x14035FC80
		{"circle", 0x8218}, // SP 0x1402990B0 MP 0x1403620D0
		{"rect", 0x8219}, // SP 0x140299540 MP 0x140362730
		{"_meth_821a", 0x821A}, // SP 0x140299B00 MP 0x140362E50
		{"transfermarkstonewscriptmodel", 0x821B}, // SP 0x14029A1B0 MP 0x140363790
		{"setwatersheeting", 0x821C}, // SP 0x140290290 MP 0x140358930
		{"_meth_821d", 0x821D}, // SP 0x1402A29B0 MP 0x000000000
		{"_meth_821e", 0x821E}, // SP 0x1402A32F0 MP 0x000000000
		{"setweaponhudiconoverride", 0x821F}, // SP 0x140296A30 MP 0x14035FC90
		{"getweaponhudiconoverride", 0x8220}, // SP 0x140297010 MP 0x140360380
		{"_meth_8221", 0x8221}, // SP 0x1402976A0 MP 0x1403609B0
		{"_meth_8222", 0x8222}, // SP 0x140299CF0 MP 0x1403635A0
		{"_meth_8223", 0x8223}, // SP 0x140299B90 MP 0x1403630E0
		{"_meth_8224", 0x8224}, // SP 0x14029A2C0 MP 0x140363C10
		{"_meth_8225", 0x8225}, // SP 0x14029DF50 MP 0x140365F70
		{"_meth_8226", 0x8226}, // SP 0x1402A17F0 MP 0x14036B0B0
		{"vehicleturretcontroloff", 0x8227}, // SP 0x140464260 MP 0x140562970
		{"isturretready", 0x8228}, // SP 0x140464340 MP 0x1405629E0
		{"_meth_8229", 0x8229}, // SP 0x140464570 MP 0x140562C70
		{"dospawn", 0x822A}, // SP 0x1404646D0 MP 0x140562D90
		{"isphysveh", 0x822B}, // SP 0x1404647A0 MP 0x140562E80
		{"crash", 0x822C}, // SP 0x140464840 MP 0x140562F80
		{"launch", 0x822D}, // SP 0x140464980 MP 0x1405630A0
		{"disablecrashing", 0x822E}, // SP 0x140464B20 MP 0x140563240
		{"enablecrashing", 0x822F}, // SP 0x140464C10 MP 0x140563310
		{"setspeed", 0x8230}, // SP 0x140464C90 MP 0x1405633E0
		{"setconveyorbelt", 0x8231}, // SP 0x140464E50 MP 0x140563610
		{"freevehicle", 0x8232}, // SP 0x000000000 MP 0x1405609B0
		{"_meth_8233", 0x8233}, // SP 0x140290E70 MP 0x000000000
		{"_meth_8234", 0x8234}, // SP 0x1402910E0 MP 0x000000000
		{"_meth_8235", 0x8235}, // SP 0x140291670 MP 0x000000000
		{"_meth_8236", 0x8236}, // SP 0x140291A60 MP 0x000000000
		{"_meth_8237", 0x8237}, // SP 0x140292010 MP 0x000000000
		{"_meth_8238", 0x8238}, // SP 0x140292230 MP 0x000000000
		{"_meth_8239", 0x8239}, // SP 0x14029C2A0 MP 0x000000000
		{"_meth_823a", 0x823A}, // SP 0x14029CAD0 MP 0x000000000
		{"_meth_823b", 0x823B}, // SP 0x14029CD70 MP 0x000000000
		{"_meth_823c", 0x823C}, // SP 0x14029CF00 MP 0x000000000
		{"_meth_823d", 0x823D}, // SP 0x140292BA0 MP 0x000000000
		{"_meth_823e", 0x823E}, // SP 0x140292DF0 MP 0x000000000
		{"_meth_823f", 0x823F},
		{"_meth_8240", 0x8240}, // SP 0x1402A3880 MP 0x000000000
		{"_meth_8241", 0x8241}, // SP 0x140248AD0 MP 0x000000000
		{"_meth_8242", 0x8242}, // SP 0x140248EA0 MP 0x000000000
		{"_meth_8243", 0x8243}, // SP 0x1402A1AF0 MP 0x14036B910
		{"_meth_8244", 0x8244}, // SP 0x1402A1E20 MP 0x14036BE30
		{"_meth_8245", 0x8245}, // SP 0x1402A24D0 MP 0x14036C5B0
		{"trackerupdate", 0x8246}, // SP 0x14028DC50 MP 0x14036E800
		{"pingplayer", 0x8247}, // SP 0x000000000 MP 0x140332B50
		{"_meth_8248", 0x8248}, // SP 0x140268FE0 MP 0x140332B40
		{"sayteam", 0x8249}, // SP 0x000000000 MP 0x1403345F0
		{"sayall", 0x824A}, // SP 0x000000000 MP 0x1403346A0
		{"_meth_824b", 0x824B}, // SP 0x000000000 MP 0x140332C90
		{"_meth_824c", 0x824C}, // SP 0x140265080 MP 0x140330FE0
		{"dropscavengerbag", 0x824D}, // SP 0x000000000 MP 0x140332FB0
		{"setjitterparams", 0x824E}, // SP 0x140461E80 MP 0x140560640
		{"sethoverparams", 0x824F}, // SP 0x140461F50 MP 0x140560710
		{"joltbody", 0x8250}, // SP 0x140462070 MP 0x1405607B0
		{"freevehicle", 0x8251}, // SP 0x140462250 MP 0x1405609B0
		{"_meth_8252", 0x8252}, // SP 0x140462300 MP 0x140560A20
		{"getvehicleowner", 0x8253}, // SP 0x140462550 MP 0x140560C70
		{"setvehiclelookattext", 0x8254}, // SP 0x1404625D0 MP 0x140560CF0
		{"setvehicleteam", 0x8255}, // SP 0x140462660 MP 0x140560D80
		{"neargoalnotifydist", 0x8256}, // SP 0x1404626E0 MP 0x140560F10
		{"setgoalpos", 0x8257}, // SP 0x140462750 MP 0x140560F80
		{"setgoalyaw", 0x8258}, // SP 0x140462840 MP 0x1405610A0
		{"cleargoalyaw", 0x8259}, // SP 0x1404628D0 MP 0x140561130
		{"settargetyaw", 0x825A}, // SP 0x140462950 MP 0x1405611B0
		{"cleartargetyaw", 0x825B}, // SP 0x1404629E0 MP 0x140561240
		{"helisetgoal", 0x825C}, // SP 0x140462A60 MP 0x1405612C0
		{"_meth_825d", 0x825D}, // SP 0x140463320 MP 0x140561B60
		{"setturrettargetent", 0x825E}, // SP 0x140463410 MP 0x140561C60
		{"clearturrettargetent", 0x825F}, // SP 0x140463510 MP 0x140561D60
		{"canturrettargetpoint", 0x8260}, // SP 0x1404635A0 MP 0x140561DF0
		{"setlookatent", 0x8261}, // SP 0x1404638A0 MP 0x1405620F0
		{"clearlookatent", 0x8262}, // SP 0x140463950 MP 0x1405621A0
		{"setweapon", 0x8263}, // SP 0x140463AB0 MP 0x140562280
		{"_meth_8264", 0x8264}, // SP 0x140463B20 MP 0x1405622F0
		{"vehicleturretcontrolon", 0x8265}, // SP 0x1404641D0 MP 0x1405628F0
		{"finishplayerdamage", 0x8266}, // SP 0x000000000 MP 0x1403337A0
		{"suicide", 0x8267}, // SP 0x000000000 MP 0x140333E20
		{"_meth_8268", 0x8268}, // SP 0x000000000 MP 0x140334010
		{"_meth_8269", 0x8269}, // SP 0x000000000 MP 0x1403340A0
		{"_meth_826a", 0x826A}, // SP 0x000000000 MP 0x140334120
		{"spawn", 0x826B}, // SP 0x000000000 MP 0x1403341A0
		{"_meth_826c", 0x826C}, // SP 0x000000000 MP 0x140334230
		{"_meth_826d", 0x826D}, // SP 0x000000000 MP 0x140334290
		{"istalking", 0x826E}, // SP 0x000000000 MP 0x140334570
		{"allowspectateteam", 0x826F}, // SP 0x000000000 MP 0x140334750
		{"_meth_8270", 0x8270}, // SP 0x000000000 MP 0x140334980
		{"getguid", 0x8271}, // SP 0x000000000 MP 0x140334B20
		{"_meth_8272", 0x8272}, // SP 0x1402B3CA0 MP 0x14037FA00
		{"_meth_8273", 0x8273}, // SP 0x1402B3EC0 MP 0x14037FC50
		{"clonebrushmodeltoscriptmodel", 0x8274}, // SP 0x1402B4500 MP 0x140380380
		{"_meth_8275", 0x8275}, // SP 0x000000000 MP 0x140380450
		{"scriptmodelclearanim", 0x8276}, // SP 0x000000000 MP 0x140380700
		{"_meth_8277", 0x8277}, // SP 0x000000000 MP 0x140380570
		{"_meth_8278", 0x8278}, // SP 0x140465060 MP 0x1405637F0
		{"attachpath", 0x8279}, // SP 0x1404651E0 MP 0x14055F040
		{"getattachpos", 0x827A}, // SP 0x1404652F0 MP 0x14055F170
		{"startpath", 0x827B}, // SP 0x1404654A0 MP 0x14055F340
		{"setswitchnode", 0x827C}, // SP 0x140465740 MP 0x14055F4D0
		{"setwaitspeed", 0x827D}, // SP 0x140465840 MP 0x14055F560
		{"finishdamage", 0x827E}, // SP 0x000000000 MP 0x14055F5E0
		{"setspeed", 0x827F}, // SP 0x1404658C0 MP 0x14055F840
		{"setspeedimmediate", 0x8280}, // SP 0x140465930 MP 0x14055F8B0
		{"_meth_8281", 0x8281}, // SP 0x140465AC0 MP 0x14055FA60
		{"getspeed", 0x8282}, // SP 0x140465BE0 MP 0x14055FB80
		{"getvelocity", 0x8283}, // SP 0x140465CD0 MP 0x14055FC70
		{"getbodyvelocity", 0x8284}, // SP 0x140465D40 MP 0x14055FCE0
		{"getsteering", 0x8285}, // SP 0x140465DB0 MP 0x14055FD50
		{"getthrottle", 0x8286}, // SP 0x140465E30 MP 0x14055FDE0
		{"turnengineoff", 0x8287}, // SP 0x140465EA0 MP 0x14055FE50
		{"turnengineon", 0x8288}, // SP 0x140465F00 MP 0x14055FEC0
		{"_meth_8289", 0x8289}, // SP 0x140465F60 MP 0x000000000
		{"getgoalspeedmph", 0x828A}, // SP 0x140466020 MP 0x14055FF30
		{"_meth_828b", 0x828B}, // SP 0x140466090 MP 0x14055FFA0
		{"setacceleration", 0x828C}, // SP 0x140466100 MP 0x140560010
		{"resumespeed", 0x828D}, // SP 0x140466170 MP 0x140560080
		{"setyawspeed", 0x828E}, // SP 0x140466200 MP 0x140560110
		{"setyawspeedbyname", 0x828F}, // SP 0x140466360 MP 0x140560270
		{"setmaxpitchroll", 0x8290}, // SP 0x140461CC0 MP 0x140560480
		{"setairresitance", 0x8291}, // SP 0x140461D80 MP 0x140560540
		{"setturningability", 0x8292}, // SP 0x140461E10 MP 0x1405605D0
		{"_meth_8293", 0x8293}, // SP 0x140260F20 MP 0x140334B90
		{"_meth_8294", 0x8294}, // SP 0x000000000 MP 0x140334D50
		{"ismlgspectator", 0x8295}, // SP 0x000000000 MP 0x140334DB0
		{"getclanidhigh", 0x8296}, // SP 0x000000000 MP 0x140334E10
		{"getclanidlow", 0x8297}, // SP 0x000000000 MP 0x140334E70
		{"_meth_8298", 0x8298}, // SP 0x000000000 MP 0x140334ED0
		{"getspectatingplayer", 0x8299}, // SP 0x000000000 MP 0x140334F60
		{"predictstreampos", 0x829A}, // SP 0x000000000 MP 0x140334FE0
		{"_meth_829b", 0x829B}, // SP 0x000000000 MP 0x1403350D0
		{"_meth_829c", 0x829C}, // SP 0x000000000 MP 0x1403350E0
		{"setrank", 0x829D}, // SP 0x000000000 MP 0x1403350F0
		{"_meth_829e", 0x829E}, // SP 0x000000000 MP 0x140335190
		{"setmlgspectator", 0x829F}, // SP 0x140260730 MP 0x14032CA40
		{"visionsyncwithplayer", 0x82A0}, // SP 0x000000000 MP 0x14032ED90
		{"showhudsplash", 0x82A1}, // SP 0x140263850 MP 0x14032FB10
		{"setperk", 0x82A2}, // SP 0x140265490 MP 0x1403297E0
		{"_meth_82a3", 0x82A3}, // SP 0x1402659A0 MP 0x140329D00
		{"_meth_82a4", 0x82A4}, // SP 0x1402661B0 MP 0x14032A460
		{"_meth_82a5", 0x82A5}, // SP 0x140265D40 MP 0x14032A0A0
		{"registerparty", 0x82A6}, // SP 0x000000000 MP 0x1403323C0
		{"_meth_82a7", 0x82A7}, // SP 0x000000000 MP 0x1403324F0
		{"_meth_82a8", 0x82A8}, // SP 0x1405D92F0 MP 0x14032A8F0
		{"_meth_82a9", 0x82A9}, // SP 0x1405D92F0 MP 0x14032A900
		{"moveto", 0x82AA}, // SP 0x1402B2A10 MP 0x14037E950
		{"rotatepitch", 0x82AB}, // SP 0x1402B2F60 MP 0x14037EEB0
		{"rotateyaw", 0x82AC}, // SP 0x1402B2F70 MP 0x14037EEC0
		{"rotateroll", 0x82AD}, // SP 0x1402B2F90 MP 0x14037EEE0
		{"_meth_82ae", 0x82AE}, // SP 0x1402B2C10 MP 0x14037EB00
		{"_meth_82af", 0x82AF}, // SP 0x1402B2D70 MP 0x14037EC90
		{"_meth_82b0", 0x82B0}, // SP 0x1402B2EE0 MP 0x14037EE20
		{"rotateby", 0x82B1}, // SP 0x1402B3030 MP 0x14037EF10
		{"_meth_82b2", 0x82B2}, // SP 0x1402B3460 MP 0x14037F060
		{"_meth_82b3", 0x82B3}, // SP 0x1402B3470 MP 0x14037F070
		{"_meth_82b4", 0x82B4}, // SP 0x1402B3490 MP 0x14037F090
		{"_meth_82b5", 0x82B5}, // SP 0x1402B3410 MP 0x14037F010
		{"_meth_82b6", 0x82B6}, // SP 0x1402B3430 MP 0x14037F030
		{"_meth_82b7", 0x82B7}, // SP 0x1402B3450 MP 0x14037F050
		{"_meth_82b8", 0x82B8}, // SP 0x1402B34B0 MP 0x14037F0B0
		{"_meth_82b9", 0x82B9}, // SP 0x1402B3700 MP 0x14037F3C0
		{"solid", 0x82BA}, // SP 0x1402B45E0 MP 0x1403808A0
		{"notsolid", 0x82BB}, // SP 0x1402B4690 MP 0x140380950
		{"setcandamage", 0x82BC}, // SP 0x1402B3880 MP 0x14037F590
		{"setcanradiusdamage", 0x82BD}, // SP 0x1402B38E0 MP 0x14037F5F0
		{"physicslaunchclient", 0x82BE}, // SP 0x1402B3960 MP 0x14037F670
		{"_meth_82bf", 0x82BF}, // SP 0x000000000 MP 0x1403351A0
		{"_meth_82c0", 0x82C0}, // SP 0x000000000 MP 0x1403351B0
		{"setcarddisplayslot", 0x82C1}, // SP 0x000000000 MP 0x1403351C0
		{"regweaponforfxremoval", 0x82C2}, // SP 0x000000000 MP 0x1403352B0
		{"laststandrevive", 0x82C3}, // SP 0x000000000 MP 0x140331E00
		{"_meth_82c4", 0x82C4}, // SP 0x000000000 MP 0x140331E70
		{"setspectatedefaults", 0x82C5}, // SP 0x000000000 MP 0x140331EE0
		{"getthirdpersoncrosshairoffset", 0x82C6}, // SP 0x000000000 MP 0x140332250
		{"_meth_82c7", 0x82C7}, // SP 0x140262950 MP 0x14032EF00
		{"_meth_82c8", 0x82C8}, // SP 0x140262A60 MP 0x14032EFF0
		{"_meth_82c9", 0x82C9}, // SP 0x140262AB0 MP 0x14032F0D0
		{"_meth_82ca", 0x82CA}, // SP 0x000000000 MP 0x1403322C0
		{"getweaponslistexclusives", 0x82CB}, // SP 0x1402623D0 MP 0x14032E3A0
		{"_meth_82cc", 0x82CC}, // SP 0x1402624C0 MP 0x14032E4B0
		{"_meth_82cd", 0x82CD}, // SP 0x1402625B0 MP 0x14032E650
		{"getweaponslist", 0x82CE}, // SP 0x140262720 MP 0x14032E790
		{"canplayerplacesentry", 0x82CF}, // SP 0x140264D00 MP 0x140331530
		{"canplayerplacetank", 0x82D0}, // SP 0x1402651B0 MP 0x1403292E0
		{"visionsetnakedforplayer", 0x82D1}, // SP 0x140263410 MP 0x14032FC00
		{"visionsetnightforplayer", 0x82D2}, // SP 0x1402636D0 MP 0x14032FC10
		{"visionsetmissilecamforplayer", 0x82D3}, // SP 0x1402636F0 MP 0x14032FC30
		{"visionsetthermalforplayer", 0x82D4}, // SP 0x140263710 MP 0x14032FD20
		{"visionsetpainforplayer", 0x82D5}, // SP 0x140263730 MP 0x14032FD40
		{"setblurforplayer", 0x82D6}, // SP 0x140264890 MP 0x140330B80
		{"_meth_82d7", 0x82D7}, // SP 0x140264C80 MP 0x140331310
		{"_meth_82d8", 0x82D8}, // SP 0x140264C80 MP 0x140331330
		{"_meth_82d9", 0x82D9}, // SP 0x1402ABE90 MP 0x000000000
		{"getbuildnumber", 0x82DA}, // SP 0x1402663A0 MP 0x14032A910
		{"_meth_82db", 0x82DB}, // SP 0x140266AF0 MP 0x14032AE90
		{"_meth_82dc", 0x82DC}, // SP 0x140266CD0 MP 0x14032B120
		{"_meth_82dd", 0x82DD}, // SP 0x140266FE0 MP 0x14032B500
		{"playfx", 0x82DE}, // SP 0x140267330 MP 0x14032B9F0
		{"playerrecoilscaleon", 0x82DF}, // SP 0x140267530 MP 0x14032BD00
		{"_meth_82e0", 0x82E0}, // SP 0x140267600 MP 0x14032BDD0
		{"weaponlockstart", 0x82E1}, // SP 0x1402676E0 MP 0x14032C000
		{"weaponlockfinalize", 0x82E2}, // SP 0x140260240 MP 0x14032C240
		{"disableautoreload", 0x82E3}, // SP 0x140260540 MP 0x14032C6C0
		{"setentertime", 0x82E4}, // SP 0x1402605E0 MP 0x14032C7F0
		{"usinggamepad", 0x82E5}, // SP 0x000000000 MP 0x140332300
		{"_meth_82e6", 0x82E6}, // SP 0x000000000 MP 0x140332380
		{"_meth_82e7", 0x82E7}, // SP 0x000000000 MP 0x140332390
		{"_meth_82e8", 0x82E8}, // SP 0x000000000 MP 0x1403323B0
		{"_meth_82e9", 0x82E9}, // SP 0x000000000 MP 0x1403323A0
		{"_meth_82ea", 0x82EA}, // SP 0x1402628A0 MP 0x14032E5C0
		{"_meth_82eb", 0x82EB}, // SP 0x1402619B0 MP 0x14032D5A0
		{"_meth_82ec", 0x82EC}, // SP 0x140261AA0 MP 0x14032D6D0
		{"issighted", 0x82ED}, // SP 0x140263D40 MP 0x14032FA10
		{"setvelocity", 0x82EE}, // SP 0x1402601C0 MP 0x14032BBE0
		{"_meth_82ef", 0x82EF}, // SP 0x140260AD0 MP 0x14032C3C0
		{"_meth_82f0", 0x82F0}, // SP 0x140260E80 MP 0x14032C5E0
		{"_meth_82f1", 0x82F1}, // SP 0x1402677A0 MP 0x14032BE30
		{"_meth_82f2", 0x82F2}, // SP 0x140260130 MP 0x14032BF20
		{"_meth_82f3", 0x82F3}, // SP 0x140264AD0 MP 0x140330E20
		{"setweaponammostock", 0x82F4}, // SP 0x140264EB0 MP 0x140331350
		{"_meth_82f5", 0x82F5}, // SP 0x140265380 MP 0x1403293A0
		{"getweaponammostock", 0x82F6}, // SP 0x1402655D0 MP 0x140329580
		{"_meth_82f7", 0x82F7}, // SP 0x140265820 MP 0x140329990
		{"stoplocalsound", 0x82F8}, // SP 0x140265F20 MP 0x14032A3B0
		{"setclientdvar", 0x82F9}, // SP 0x140266410 MP 0x14032A990
		{"_meth_82fa", 0x82FA}, // SP 0x1402666C0 MP 0x14032B260
		{"_meth_82fb", 0x82FB}, // SP 0x000000000 MP 0x14032AC30
		{"_meth_82fc", 0x82FC}, // SP 0x000000000 MP 0x14032AD80
		{"allowads", 0x82FD}, // SP 0x140261130 MP 0x14032C940
		{"_meth_82fe", 0x82FE}, // SP 0x140261440 MP 0x14032CAB0
		{"_meth_82ff", 0x82FF}, // SP 0x140261B20 MP 0x14032D890
		{"_meth_8300", 0x8300}, // SP 0x140261C00 MP 0x14032DA40
		{"weaponlocktargettooclose", 0x8301}, // SP 0x140261CE0 MP 0x14032DBB0
		{"setspreadoverride", 0x8302}, // SP 0x140262B00 MP 0x14032ED00
		{"resetspreadoverride", 0x8303}, // SP 0x140262CC0 MP 0x14032EE80
		{"_meth_8304", 0x8304}, // SP 0x140262D90 MP 0x14032EF60
		{"setactionslot", 0x8305}, // SP 0x140261F10 MP 0x14032DD50
		{"setviewkickscale", 0x8306}, // SP 0x1402608B0 MP 0x14032CB90
		{"getviewkickscale", 0x8307}, // SP 0x140260A80 MP 0x14032CCC0
		{"getweaponslistoffhands", 0x8308}, // SP 0x1402621A0 MP 0x14032E120
		{"getweaponslistitems", 0x8309}, // SP 0x1402622E0 MP 0x14032E230
		{"_meth_830a", 0x830A}, // SP 0x140261090 MP 0x14032C720
		{"_meth_830b", 0x830B}, // SP 0x140262F00 MP 0x14032EA90
		{"_meth_830c", 0x830C}, // SP 0x140263380 MP 0x14032F210
		{"takeallweapons", 0x830D}, // SP 0x1402635B0 MP 0x14032F310
		{"getcurrentweapon", 0x830E}, // SP 0x140263750 MP 0x14032F5E0
		{"getcurrentprimaryweapon", 0x830F}, // SP 0x140263A30 MP 0x14032F860
		{"getcurrentoffhand", 0x8310}, // SP 0x140263EF0 MP 0x14032FC50
		{"givestartammo", 0x8311}, // SP 0x1402647A0 MP 0x140330740
		{"switchtoweapon", 0x8312}, // SP 0x140264CA0 MP 0x140330D20
		{"switchtoweaponimmediate", 0x8313}, // SP 0x140264E50 MP 0x140330D90
		{"_meth_8314", 0x8314}, // SP 0x140265950 MP 0x140329520
		{"switchtooffhand", 0x8315}, // SP 0x140265B80 MP 0x1403296D0
		{"_meth_8316", 0x8316}, // SP 0x140264440 MP 0x140330340
		{"getcustomizationbody", 0x8317}, // SP 0x140264600 MP 0x140330520
		{"beginlocationselection", 0x8318}, // SP 0x140260C90 MP 0x14032C470
		{"_meth_8319", 0x8319}, // SP 0x140261020 MP 0x14032C780
		{"_meth_831a", 0x831A}, // SP 0x140263E90 MP 0x1403302E0
		{"_meth_831b", 0x831B}, // SP 0x140264020 MP 0x140330410
		{"_meth_831c", 0x831C}, // SP 0x140264150 MP 0x1403305B0
		{"_meth_831d", 0x831D}, // SP 0x140264270 MP 0x1403306E0
		{"_meth_831e", 0x831E}, // SP 0x140264660 MP 0x140330B20
		{"_meth_831f", 0x831F}, // SP 0x140264750 MP 0x140330CC0
		{"setaimspreadmovementscale", 0x8320}, // SP 0x1402631F0 MP 0x14032F130
		{"closemenu", 0x8321}, // SP 0x140263290 MP 0x14032F3A0
		{"_meth_8322", 0x8322}, // SP 0x140263330 MP 0x14032F4E0
		{"openpopupmenu", 0x8323}, // SP 0x140263430 MP 0x14032F6C0
		{"openpopupmenunomouse", 0x8324}, // SP 0x140263630 MP 0x14032F930
		{"_meth_8325", 0x8325},
		{"freezecontrols", 0x8326}, // SP 0x140263B10 MP 0x14032FD60
		{"disableusability", 0x8327}, // SP 0x140263C10 MP 0x14032FFE0
		{"enableusability", 0x8328}, // SP 0x140263E40 MP 0x1403300F0
		{"_meth_8329", 0x8329}, // SP 0x140266C00 MP 0x14032B640
		{"_meth_832a", 0x832A}, // SP 0x140266D10 MP 0x14032B800
		{"_meth_832b", 0x832B}, // SP 0x140266EA0 MP 0x14032B9C0
		{"_meth_832c", 0x832C}, // SP 0x140267180 MP 0x14032B9E0
		{"deactivatechannelvolumes", 0x832D}, // SP 0x140267410 MP 0x14032BB00
		{"_meth_832e", 0x832E}, // SP 0x140265FA0 MP 0x140329C00
		{"_meth_832f", 0x832F}, // SP 0x140266200 MP 0x140329F00
		{"_meth_8330", 0x8330}, // SP 0x140266560 MP 0x14032A240
		{"_meth_8331", 0x8331}, // SP 0x140266820 MP 0x14032A6F0
		{"isdualwielding", 0x8332}, // SP 0x140266E10 MP 0x14032ABC0
		{"_meth_8333", 0x8333}, // SP 0x140267030 MP 0x14032AD00
		{"isreloading", 0x8334}, // SP 0x140267100 MP 0x14032AEE0
		{"setorigin", 0x8335}, // SP 0x1402677C0 MP 0x14032B8D0
		{"getvelocity", 0x8336}, // SP 0x1402604D0 MP 0x14032BC80
		{"_meth_8337", 0x8337}, // SP 0x140260930 MP 0x14032BE70
		{"forcemantle", 0x8338}, // SP 0x140260990 MP 0x14032C0D0
		{"attackbuttonpressed", 0x8339}, // SP 0x140261500 MP 0x14032CE10
		{"_meth_833a", 0x833A}, // SP 0x1402615F0 MP 0x14032D0B0
		{"meleebuttonpressed", 0x833B}, // SP 0x1402617D0 MP 0x14032D1B0
		{"jumpbuttonpressed", 0x833C}, // SP 0x1402618D0 MP 0x14032D450
		{"getviewheight", 0x833D}, // SP 0x140262070 MP 0x14032DC20
		{"_meth_833e", 0x833E}, // SP 0x1402620C0 MP 0x14032DCE0
		{"isonladder", 0x833F}, // SP 0x140262280 MP 0x14032DF80
		{"setviewmodel", 0x8340}, // SP 0x1402626A0 MP 0x14032DFF0
		{"_meth_8341", 0x8341}, // SP 0x140264090 MP 0x14032FF10
		{"_meth_8342", 0x8342}, // SP 0x1402642C0 MP 0x140330170
		{"_meth_8343", 0x8343}, // SP 0x140260360 MP 0x000000000
		{"_meth_8344", 0x8344}, // SP 0x140260640 MP 0x000000000
		{"_meth_8345", 0x8345}, // SP 0x1402607E0 MP 0x000000000
		{"_meth_8346", 0x8346}, // SP 0x1402609E0 MP 0x000000000
		{"_meth_8347", 0x8347}, // SP 0x14026D000 MP 0x14033B150
		{"forcethirdpersonwhenfollowing", 0x8348}, // SP 0x000000000 MP 0x140374D30
		{"disableforcethirdpersonwhenfollowing", 0x8349}, // SP 0x000000000 MP 0x140374DD0
		{"_meth_834a", 0x834A}, // SP 0x140260C10 MP 0x14032C5D0
		{"_meth_834b", 0x834B}, // SP 0x000000000 MP 0x14037B380
		{"secondaryoffhandbuttonpressed", 0x834C}, // SP 0x140261D40 MP 0x14032D9C0
		{"_meth_834d", 0x834D}, // SP 0x000000000 MP 0x140374B70
		{"_meth_834e", 0x834E}, // SP 0x000000000 MP 0x140374C10
		{"botsetflag", 0x834F}, // SP 0x000000000 MP 0x1404798E0
		{"botsetstance", 0x8350}, // SP 0x000000000 MP 0x14047A0F0
		{"botsetscriptmove", 0x8351}, // SP 0x000000000 MP 0x140479F70
		{"_meth_8352", 0x8352}, // SP 0x000000000 MP 0x140479BC0
		{"_meth_8353", 0x8353}, // SP 0x000000000 MP 0x140479DB0
		{"botclearscriptgoal", 0x8354}, // SP 0x000000000 MP 0x140477C80
		{"getnearestnode", 0x8355}, // SP 0x000000000 MP 0x140479B00
		{"botclearscriptenemy", 0x8356}, // SP 0x000000000 MP 0x140477C00
		{"botsetattacker", 0x8357}, // SP 0x000000000 MP 0x1404796A0
		{"botgetscriptgoal", 0x8358}, // SP 0x000000000 MP 0x1404784A0
		{"botgetscriptgoalradius", 0x8359}, // SP 0x000000000 MP 0x1404785F0
		{"botgetscriptgoalyaw", 0x835A}, // SP 0x000000000 MP 0x1404786F0
		{"botgetscriptgoaltype", 0x835B}, // SP 0x000000000 MP 0x140478670
		{"_meth_835c", 0x835C},
		{"_meth_835d", 0x835D}, // SP 0x000000000 MP 0x140478850
		{"_meth_835e", 0x835E}, // SP 0x000000000 MP 0x140478E10
		{"botfindnoderandom", 0x835F}, // SP 0x000000000 MP 0x140477D00
		{"botmemoryevent", 0x8360}, // SP 0x000000000 MP 0x140478B70
		{"_meth_8361", 0x8361},
		{"_meth_8362", 0x8362}, // SP 0x000000000 MP 0x140478EB0
		{"bothasscriptgoal", 0x8363}, // SP 0x000000000 MP 0x140478890
		{"botgetpersonality", 0x8364}, // SP 0x000000000 MP 0x140478420
		{"_meth_8365", 0x8365}, // SP 0x000000000 MP 0x14047A1C0
		{"_meth_8366", 0x8366},
		{"botsetpersonality", 0x8367}, // SP 0x000000000 MP 0x140479A70
		{"botsetdifficulty", 0x8368}, // SP 0x000000000 MP 0x1404797B0
		{"botgetdifficulty", 0x8369}, // SP 0x000000000 MP 0x140477E90
		{"botgetworldclosestedge", 0x836A}, // SP 0x000000000 MP 0x140478770
		{"_meth_836b", 0x836B}, // SP 0x000000000 MP 0x140478990
		{"botpredictseepoint", 0x836C}, // SP 0x000000000 MP 0x140479420
		{"botcanseeentity", 0x836D}, // SP 0x000000000 MP 0x140477A90
		{"botgetnodesonpath", 0x836E}, // SP 0x000000000 MP 0x140478230
		{"_meth_836f", 0x836F}, // SP 0x000000000 MP 0x140479020
		{"_meth_8370", 0x8370},
		{"_meth_8371", 0x8371}, // SP 0x000000000 MP 0x140477FD0
		{"botsetawareness", 0x8372}, // SP 0x000000000 MP 0x140479720
		{"_meth_8373", 0x8373}, // SP 0x000000000 MP 0x140479610
		{"botgetscriptgoalnode", 0x8374}, // SP 0x000000000 MP 0x140478560
		{"botgetimperfectenemyinfo", 0x8375}, // SP 0x000000000 MP 0x1404780D0
		{"_meth_8376", 0x8376},
		{"botsetpathingstyle", 0x8377}, // SP 0x000000000 MP 0x1404799A0
		{"botsetdifficultysetting", 0x8378}, // SP 0x000000000 MP 0x140479840
		{"botgetdifficultysetting", 0x8379}, // SP 0x000000000 MP 0x140477F20
		{"botgetpathdist", 0x837A}, // SP 0x000000000 MP 0x1404782F0
		{"_meth_837b", 0x837B}, // SP 0x000000000 MP 0x140478910
		{"_meth_837c", 0x837C}, // SP 0x000000000 MP 0x140479510
		{"botclearbutton", 0x837D}, // SP 0x000000000 MP 0x140477B70
		{"_meth_837e", 0x837E}, // SP 0x000000000 MP 0x1404791D0
		{"getnodenumber", 0x837F}, // SP 0x000000000 MP 0x14032E760
		{"setclientowner", 0x8380}, // SP 0x000000000 MP 0x14037AD60
		{"_meth_8381", 0x8381}, // SP 0x000000000 MP 0x14037A3B0
		{"setaisightlinevisible", 0x8382}, // SP 0x1402A15D0 MP 0x140358460
		{"setentityowner", 0x8383}, // SP 0x000000000 MP 0x14037A980
		{"nodeisdisconnected", 0x8384}, // SP 0x000000000 MP 0x14032E920
		{"_meth_8385", 0x8385}, // SP 0x140262C60 MP 0x14032E340
		{"_meth_8386", 0x8386}, // SP 0x000000000 MP 0x14037B1B0
		{"_meth_8387", 0x8387},
		{"_meth_8388", 0x8388}, // SP 0x000000000 MP 0x14044EF90
		{"_meth_8389", 0x8389}, // SP 0x000000000 MP 0x14044E610
		{"setagentattacker", 0x838A}, // SP 0x000000000 MP 0x14044EE40
		{"_meth_838b", 0x838B}, // SP 0x000000000 MP 0x14044E0E0
		{"agentcanseesentient", 0x838C}, // SP 0x000000000 MP 0x14044DF00
		{"setwaypoint", 0x838D}, // SP 0x000000000 MP 0x140450B20
		{"setgoalpos", 0x838E}, // SP 0x000000000 MP 0x140450120
		{"getgoalpos", 0x838F}, // SP 0x000000000 MP 0x14044F670
		{"setgoalnode", 0x8390}, // SP 0x000000000 MP 0x140450090
		{"setgoalentity", 0x8391}, // SP 0x000000000 MP 0x140450000
		{"setgoalradius", 0x8392}, // SP 0x000000000 MP 0x140450260
		{"_meth_8393", 0x8393}, // SP 0x000000000 MP 0x14044FEC0
		{"setorientmode", 0x8394}, // SP 0x000000000 MP 0x140450540
		{"setanimmode", 0x8395}, // SP 0x000000000 MP 0x14044FDA0
		{"setphysicsmode", 0x8396}, // SP 0x000000000 MP 0x140450780
		{"setclipmode", 0x8397}, // SP 0x000000000 MP 0x14044FF60
		{"setmaxturnspeed", 0x8398}, // SP 0x000000000 MP 0x140450370
		{"getmaxturnspeed", 0x8399}, // SP 0x000000000 MP 0x14044F7A0
		{"beginmelee", 0x839A}, // SP 0x000000000 MP 0x14044F2A0
		{"getmlgspectatorteam", 0x839B}, // SP 0x000000000 MP 0x140450840
		{"dotrajectory", 0x839C}, // SP 0x000000000 MP 0x14044F5D0
		{"doanimlerp", 0x839D}, // SP 0x000000000 MP 0x14044F480
		{"setviewheight", 0x839E}, // SP 0x000000000 MP 0x140450AA0
		{"claimnode", 0x839F}, // SP 0x000000000 MP 0x14044F370
		{"relinquishclaimednode", 0x83A0}, // SP 0x000000000 MP 0x14044FC90
		{"_meth_83a1", 0x83A1}, // SP 0x14026DD90 MP 0x14033BCE0
		{"_meth_83a2", 0x83A2}, // SP 0x14026DE40 MP 0x14033BD90
		{"_meth_83a3", 0x83A3}, // SP 0x14026DEB0 MP 0x14033BDF0
		{"_meth_83a4", 0x83A4}, // SP 0x14026DF20 MP 0x14033BE50
		{"_meth_83a5", 0x83A5}, // SP 0x1402A84A0 MP 0x000000000
		{"_meth_83a6", 0x83A6}, // SP 0x140260B80 MP 0x14032CDC0
		{"_meth_83a7", 0x83A7}, // SP 0x140260DE0 MP 0x14032CEA0
		{"_meth_83a8", 0x83A8}, // SP 0x140268480 MP 0x000000000
		{"_meth_83a9", 0x83A9}, // SP 0x1402A8750 MP 0x000000000
		{"_meth_83aa", 0x83AA}, // SP 0x1402A8860 MP 0x000000000
		{"_meth_83ab", 0x83AB}, // SP 0x140268330 MP 0x000000000
		{"_meth_83ac", 0x83AC}, // SP 0x140268410 MP 0x000000000
		{"_meth_83ad", 0x83AD}, // SP 0x1402683A0 MP 0x000000000
		{"_meth_83ae", 0x83AE},
		{"_meth_83af", 0x83AF},
		{"_meth_83b0", 0x83B0}, // SP 0x1405D92F0 MP 0x14032CC20
		{"_meth_83b1", 0x83B1}, // SP 0x1402670A0 MP 0x14032B5D0
		{"_meth_83b2", 0x83B2}, // SP 0x140267250 MP 0x14032B7B0
		{"_meth_83b3", 0x83B3}, // SP 0x1404651B0 MP 0x14055F010
		{"_meth_83b4", 0x83B4}, // SP 0x1404651C0 MP 0x14055F020
		{"_meth_83b5", 0x83B5}, // SP 0x000000000 MP 0x14037BCD0
		{"_meth_83b6", 0x83B6}, // SP 0x14029C100 MP 0x140367570
		{"_meth_83b7", 0x83B7}, // SP 0x140299860 MP 0x140364840
		{"_meth_83b8", 0x83B8}, // SP 0x14028E830 MP 0x140364960
		{"_meth_83b9", 0x83B9}, // SP 0x140261C60 MP 0x14032D930
		{"_meth_83ba", 0x83BA}, // SP 0x140462CB0 MP 0x1405614F0
		{"_meth_83bb", 0x83BB}, // SP 0x140462D60 MP 0x1405615A0
		{"_meth_83bc", 0x83BC}, // SP 0x140462DE0 MP 0x140561620
		{"_meth_83bd", 0x83BD}, // SP 0x14028AB00 MP 0x000000000
		{"_meth_83be", 0x83BE}, // SP 0x140263B80 MP 0x140330060
		{"_meth_83bf", 0x83BF}, // SP 0x140263C60 MP 0x140330200
		{"_meth_83c0", 0x83C0}, // SP 0x140263F70 MP 0x140330470
		{"_meth_83c1", 0x83C1}, // SP 0x1402B3B00 MP 0x14037F840
		{"_meth_83c2", 0x83C2}, // SP 0x1402D7D50 MP 0x1400778A0
		{"_meth_83c3", 0x83C3}, // SP 0x1404623A0 MP 0x140560AC0
		{"_meth_83c4", 0x83C4}, // SP 0x1405D92F0 MP 0x14032D370
		{"_meth_83c5", 0x83C5}, // SP 0x14028AA90 MP 0x000000000
		{"_meth_83c6", 0x83C6}, // SP 0x14024AEF0 MP 0x000000000
		{"_meth_83c7", 0x83C7}, // SP 0x14024B000 MP 0x000000000
		{"_meth_83c8", 0x83C8}, // SP 0x14024B340 MP 0x000000000
		{"_meth_83c9", 0x83C9}, // SP 0x14024B420 MP 0x000000000
		{"_meth_83ca", 0x83CA}, // SP 0x14024B6D0 MP 0x000000000
		{"_meth_83cb", 0x83CB}, // SP 0x14024B660 MP 0x000000000
		{"_meth_83cc", 0x83CC}, // SP 0x14024B740 MP 0x000000000
		{"_meth_83cd", 0x83CD}, // SP 0x14024B7A0 MP 0x000000000
		{"setanimclass", 0x83CE}, // SP 0x000000000 MP 0x1400781E0
		{"enableanimstate", 0x83CF}, // SP 0x000000000 MP 0x140077CB0
		{"_meth_83d0", 0x83D0}, // SP 0x000000000 MP 0x140078250
		{"getanimentry", 0x83D1}, // SP 0x000000000 MP 0x140077DF0
		{"getanimentryname", 0x83D2}, // SP 0x000000000 MP 0x140077FA0
		{"getanimentryalias", 0x83D3}, // SP 0x000000000 MP 0x140077E80
		{"getanimentrycount", 0x83D4}, // SP 0x000000000 MP 0x140077EE0
		{"_meth_83d5", 0x83D5}, // SP 0x1402AE190 MP 0x000000000
		{"issprinting", 0x83D6}, // SP 0x1402613C0 MP 0x14032CB20
		{"_meth_83d7", 0x83D7}, // SP 0x140291320 MP 0x000000000
		{"_meth_83d8", 0x83D8}, // SP 0x1402495B0 MP 0x000000000
		{"_meth_83d9", 0x83D9}, // SP 0x1405D92F0 MP 0x140358020
		{"_meth_83da", 0x83DA}, // SP 0x140264C80 MP 0x000000000
		{"_meth_83db", 0x83DB}, // SP 0x140264C80 MP 0x000000000
		{"_meth_83dc", 0x83DC}, // SP 0x140261B80 MP 0x14032D800
		{"rotateto", 0x83DD}, // SP 0x1402B3200 MP 0x14037EF90
		{"getlookaheaddir", 0x83DE}, // SP 0x000000000 MP 0x14044F710
		{"getpathgoalpos", 0x83DF}, // SP 0x000000000 MP 0x14044FAD0
		{"_meth_83e0", 0x83E0}, // SP 0x1402A15D0 MP 0x14036E1F0
		{"setcorpsefalling", 0x83E1}, // SP 0x000000000 MP 0x140374840
		{"setsurfacetype", 0x83E2}, // SP 0x14029FD00 MP 0x140358090
		{"_meth_83e3", 0x83E3}, // SP 0x1402A0C60 MP 0x140369520
		{"_meth_83e4", 0x83E4}, // SP 0x1402A20D0 MP 0x140369FE0
		{"_meth_83e5", 0x83E5}, // SP 0x1405D92F0 MP 0x14033B020
		{"_meth_83e6", 0x83E6}, // SP 0x1402A7B00 MP 0x000000000
		{"_meth_83e7", 0x83E7}, // SP 0x1402A7C90 MP 0x000000000
		{"visionsetstage", 0x83E8}, // SP 0x140263140 MP 0x14032F7A0
		{"hudoutlineenableforclients", 0x83E9}, // SP 0x14026D2E0 MP 0x14033B450
		{"getlinkedparent", 0x83EA}, // SP 0x14028DB50 MP 0x140359B60
		{"getmovingplatformparent", 0x83EB}, // SP 0x14028E2A0 MP 0x14035A4D0
		{"_meth_83ec", 0x83EC}, // SP 0x000000000 MP 0x14032CEF0
		{"_meth_83ed", 0x83ED}, // SP 0x1402ADF90 MP 0x000000000
		{"_meth_83ee", 0x83EE},
		{"_meth_83ef", 0x83EF}, // SP 0x1402A0B60 MP 0x14036A5C0
		{"_meth_83f0", 0x83F0}, // SP 0x14029F670 MP 0x140369790
		{"makevehiclenotcollidewithplayers", 0x83F1}, // SP 0x1402913E0 MP 0x14035B490
		{"_meth_83f2", 0x83F2}, // SP 0x140269130 MP 0x000000000
		{"_meth_83f3", 0x83F3}, // SP 0x140269190 MP 0x000000000
		{"setscriptablepartstate", 0x83F4}, // SP 0x1402A0760 MP 0x1403589E0
		{"_meth_83f5", 0x83F5}, // SP 0x1405D92F0 MP 0x140359100
		{"_meth_83f6", 0x83F6}, // SP 0x1402A15D0 MP 0x14036E200
		{"_meth_83f7", 0x83F7}, // SP 0x140465020 MP 0x140560DF0
		{"_meth_83f8", 0x83F8}, // SP 0x1405D92F0 MP 0x1403636B0
		{"_meth_83f9", 0x83F9}, // SP 0x1405D92F0 MP 0x140363830
		{"motionblurhqdisable", 0x83FA}, // SP 0x14029BA30 MP 0x140363EA0
		{"_meth_83fb", 0x83FB}, // SP 0x14029BC20 MP 0x1403640D0
		{"_meth_83fc", 0x83FC}, // SP 0x1402AA870 MP 0x000000000
		{"_meth_83fd", 0x83FD}, // SP 0x14024AD20 MP 0x000000000
		{"worldpointtoscreenpos", 0x83FE}, // SP 0x140297A40 MP 0x140360C90
		{"_meth_83ff", 0x83FF}, // SP 0x1402974B0 MP 0x000000000
		{"_meth_8400", 0x8400}, // SP 0x1402498E0 MP 0x000000000
		{"_meth_8401", 0x8401}, // SP 0x000000000 MP 0x140477D90
		{"_meth_8402", 0x8402}, // SP 0x140263950 MP 0x000000000
		{"_meth_8403", 0x8403}, // SP 0x140268F10 MP 0x000000000
		{"_meth_8404", 0x8404}, // SP 0x1402977A0 MP 0x000000000
		{"_meth_8405", 0x8405}, // SP 0x1402978F0 MP 0x000000000
		{"_meth_8406", 0x8406}, // SP 0x14029A680 MP 0x000000000
		{"emissiveblend", 0x8407}, // SP 0x000000000 MP 0x140375810
		{"_meth_8408", 0x8408}, // SP 0x1402687F0 MP 0x000000000
		{"_meth_8409", 0x8409}, // SP 0x140268840 MP 0x000000000
		{"_meth_840a", 0x840A}, // SP 0x1402AF930 MP 0x000000000
		{"_meth_840b", 0x840B}, // SP 0x1402AE490 MP 0x000000000
		{"_meth_840c", 0x840C}, // SP 0x1402AE5D0 MP 0x000000000
		{"_meth_840d", 0x840D}, // SP 0x1402B4000 MP 0x14037FDE0
		{"_meth_840e", 0x840E}, // SP 0x1402B40B0 MP 0x14037FE90
		{"physicsgetlinspeed", 0x840F}, // SP 0x1402B4160 MP 0x14037FF40
		{"_meth_8410", 0x8410}, // SP 0x1402B41F0 MP 0x140380000
		{"physicsgetangvel", 0x8411}, // SP 0x1402B42B0 MP 0x1403800E0
		{"physicsgetangspeed", 0x8412}, // SP 0x1402B4340 MP 0x1403801A0
		{"disablemissileboosting", 0x8413}, // SP 0x000000000 MP 0x14035A430
		{"enablemissileboosting", 0x8414}, // SP 0x000000000 MP 0x14035A480
		{"canspawntestclient", 0x8415}, // SP 0x000000000 MP 0x140376790
		{"spawntestclient", 0x8416}, // SP 0x000000000 MP 0x1403767B0
		{"setgrenadethrowscale", 0x8417}, // SP 0x000000000 MP 0x14036F610
		{"ismantling", 0x8418}, // SP 0x000000000 MP 0x140357DB0
		{"_meth_8419", 0x8419}, // SP 0x000000000 MP 0x1403767E0
		{"_meth_841a", 0x841A}, // SP 0x1405D92F0 MP 0x1403630D0
		{"_meth_841b", 0x841B}, // SP 0x1405D92F0 MP 0x140363500
		{"_meth_841c", 0x841C}, // SP 0x1405D92F0 MP 0x140363590
		{"_meth_841d", 0x841D}, // SP 0x1405D92F0 MP 0x140363620
		{"turretsetbarrelspinenabled", 0x841E}, // SP 0x140299270 MP 0x140364190
		{"_meth_841f", 0x841F}, // SP 0x000000000 MP 0x1403326E0
		{"autospotoverlayoff", 0x8420}, // SP 0x000000000 MP 0x1403328E0
		{"_meth_8421", 0x8421}, // SP 0x14029FEB0 MP 0x140369B60
		{"_meth_8422", 0x8422}, // SP 0x1402A00C0 MP 0x140369E10
		{"doanimrelative", 0x8423}, // SP 0x000000000 MP 0x14044F520
		{"_meth_8424", 0x8424}, // SP 0x1402B30B0 MP 0x000000000
		{"_meth_8425", 0x8425}, // SP 0x1402B3280 MP 0x000000000
		{"_meth_8426", 0x8426}, // SP 0x1402B3320 MP 0x000000000
		{"getcorpseentity", 0x8427}, // SP 0x000000000 MP 0x1403748F0
		{"_meth_8428", 0x8428}, // SP 0x1402ABE20 MP 0x000000000
		{"_meth_8429", 0x8429}, // SP 0x000000000 MP 0x140379520
		{"_meth_842a", 0x842A}, // SP 0x000000000 MP 0x140379B70
		{"queuedialogforplayer", 0x842B}, // SP 0x000000000 MP 0x1403785F0
		{"setmlgcameradefaults", 0x842C}, // SP 0x000000000 MP 0x1403321B0
		{"_meth_842d", 0x842D}, // SP 0x000000000 MP 0x140334A60
		{"_meth_842e", 0x842E}, // SP 0x140262C10 MP 0x14032F2B0
		{"_meth_842f", 0x842F}, // SP 0x140262D40 MP 0x14032F480
		{"_meth_8430", 0x8430}, // SP 0x1405D92F0 MP 0x000000000
		{"getlinkedchildren", 0x8431}, // SP 0x14028E020 MP 0x14035A3A0
		{"_meth_8432", 0x8432}, // SP 0x000000000 MP 0x140479320
		{"playsoundonmovingent", 0x8433}, // SP 0x000000000 MP 0x140377F80
		{"cancelmantle", 0x8434}, // SP 0x140266DD0 MP 0x14032B210
		{"hasfemalecustomizationmodel", 0x8435}, // SP 0x000000000 MP 0x140332F50
		{"_meth_8436", 0x8436},
		{"setscriptabledamageowner", 0x8437}, // SP 0x1402A0440 MP 0x1403586F0
		{"_meth_8438", 0x8438}, // SP 0x1402A0B00 MP 0x140358FF0
		{"_meth_8439", 0x8439}, // SP 0x000000000 MP 0x140379890
		{"_meth_843a", 0x843A}, // SP 0x14024B810 MP 0x000000000
		{"_meth_843b", 0x843B}, // SP 0x14024B8B0 MP 0x000000000
		{"_meth_843c", 0x843C}, // SP 0x140265900 MP 0x1403294C0
		{"_meth_843d", 0x843D}, // SP 0x14024BCE0 MP 0x000000000
		{"_meth_843e", 0x843E}, // SP 0x14029EC40 MP 0x140368630
		{"_meth_843f", 0x843F}, // SP 0x1405D92F0 MP 0x14032D4E0
		{"_meth_8440", 0x8440}, // SP 0x1404639C0 MP 0x140562210
		{"_meth_8441", 0x8441}, // SP 0x140462EA0 MP 0x1405616E0
		{"_meth_8442", 0x8442}, // SP 0x1402A36A0 MP 0x140358B60
		{"_meth_8443", 0x8443}, // SP 0x140266090 MP 0x14032A550
		{"_meth_8444", 0x8444}, // SP 0x1402A8A40 MP 0x000000000
		{"_meth_8445", 0x8445}, // SP 0x1402A8A80 MP 0x000000000
		{"_meth_8446", 0x8446}, // SP 0x1402A2F70 MP 0x14036CC00
		{"_meth_8447", 0x8447}, // SP 0x1405D92F0 MP 0x140363840
		{"_meth_8448", 0x8448}, // SP 0x1405D92F0 MP 0x140363A50
		{"_meth_8449", 0x8449}, // SP 0x1402611B0 MP 0x14032C860
		{"_meth_844a", 0x844A}, // SP 0x140295480 MP 0x1403602C0
		{"_meth_844b", 0x844B}, // SP 0x140295650 MP 0x1403604C0
		{"_meth_844c", 0x844C}, // SP 0x140463020 MP 0x140561860
		{"_meth_844d", 0x844D}, // SP 0x140463040 MP 0x140561880
		{"_meth_844e", 0x844E}, // SP 0x140463060 MP 0x1405618A0
		{"_meth_844f", 0x844F}, // SP 0x140463080 MP 0x1405618C0
		{"_meth_8450", 0x8450}, // SP 0x1402AC9E0 MP 0x000000000
		{"_meth_8451", 0x8451}, // SP 0x1402ACB20 MP 0x000000000
		{"_meth_8452", 0x8452}, // SP 0x140462F60 MP 0x1405617A0
		{"_meth_8453", 0x8453}, // SP 0x140261E30 MP 0x14032DC80
		{"_meth_8454", 0x8454}, // SP 0x1402612F0 MP 0x000000000
		{"_meth_8455", 0x8455}, // SP 0x1402AD1E0 MP 0x000000000
		{"_meth_8456", 0x8456},
		{"_meth_8457", 0x8457},
		{"getvieworigin", 0x8458}, // SP 0x14029DE70 MP 0x140366600
		{"_meth_8459", 0x8459}, // SP 0x140265E70 MP 0x140329A80
		{"_meth_845a", 0x845A}, // SP 0x000000000 MP 0x140359980
		{"stopridingvehicle", 0x845B}, // SP 0x000000000 MP 0x14035A040
		{"_meth_845c", 0x845C}, // SP 0x140264C80 MP 0x000000000
		{"_meth_845d", 0x845D}, // SP 0x14028E7C0 MP 0x14035A8D0
		{"disablemissilestick", 0x845E}, // SP 0x000000000 MP 0x1403762F0
		{"enablemissilestick", 0x845F}, // SP 0x000000000 MP 0x140376340
		{"setmissileminimapvisible", 0x8460}, // SP 0x000000000 MP 0x140347ED0
		{"isoffhandweaponreadytothrow", 0x8461}, // SP 0x1402675A0 MP 0x14032B560
		{"isleaning", 0x8462}, // SP 0x1402614A0 MP 0x14032CC30
		{"makecollidewithitemclip", 0x8463}, // SP 0x140291970 MP 0x14035B8F0
		{"_meth_8464", 0x8464}, // SP 0x140261DC0 MP 0x000000000
		{"visionsetpostapplyforplayer", 0x8465}, // SP 0x000000000 MP 0x14032FE10
		{"_meth_8466", 0x8466}, // SP 0x1402B47A0 MP 0x140380A60
		{"_meth_8467", 0x8467}, // SP 0x1402B4950 MP 0x140380C30
		{"_meth_8468", 0x8468}, // SP 0x1402A2E30 MP 0x000000000
		{"_meth_8469", 0x8469}, // SP 0x1402A3290 MP 0x000000000
		{"_meth_846a", 0x846A}, // SP 0x1402A3360 MP 0x000000000
		{"_meth_846b", 0x846B}, // SP 0x1402A36C0 MP 0x000000000
		{"_meth_846c", 0x846C}, // SP 0x14028DBB0 MP 0x000000000
		{"_meth_846d", 0x846D}, // SP 0x140260B20 MP 0x000000000
		{"_meth_846e", 0x846E}, // SP 0x1402A8530 MP 0x000000000
		{"_meth_846f", 0x846F}, // SP 0x140463100 MP 0x140561940
		{"_meth_8470", 0x8470}, // SP 0x140463140 MP 0x140561980
		{"_meth_8471", 0x8471}, // SP 0x1404631A0 MP 0x1405619E0
		{"_meth_8472", 0x8472}, // SP 0x1404631F0 MP 0x140561A30
		{"_meth_8473", 0x8473}, // SP 0x140463210 MP 0x140561A50
		{"_meth_8474", 0x8474}, // SP 0x140291B70 MP 0x14035E030
		{"_meth_8475", 0x8475}, // SP 0x140291E30 MP 0x14035E0D0
		{"_meth_8476", 0x8476}, // SP 0x140263970 MP 0x14032FE30
		{"_meth_8477", 0x8477}, // SP 0x1402A85E0 MP 0x000000000
		{"_meth_8478", 0x8478}, // SP 0x1402A86E0 MP 0x000000000
		{"_meth_8479", 0x8479}, // SP 0x1405D92F0 MP 0x14032C130
		{"_meth_847a", 0x847A}, // SP 0x1405D92F0 MP 0x14032C420
		{"_meth_847b", 0x847B}, // SP 0x140462450 MP 0x140560B70
		{"_meth_847c", 0x847C}, // SP 0x1404624D0 MP 0x140560BF0
		{"_meth_847d", 0x847D}, // SP 0x140262900 MP 0x14032E940
		{"_meth_847e", 0x847E}, // SP 0x1402629A0 MP 0x14032E9D0
		{"_meth_847f", 0x847F}, // SP 0x14029CB70 MP 0x140367C30
		{"_meth_8480", 0x8480}, // SP 0x140291DA0 MP 0x000000000
		{"_meth_8481", 0x8481}, // SP 0x1405D92F0 MP 0x14032CD40
		{"_meth_8482", 0x8482}, // SP 0x1405D92F0 MP 0x14032CD30
		{"_meth_8483", 0x8483}, // SP 0x000000000 MP 0x14044E5B0
		{"_meth_8484", 0x8484}, // SP 0x140464FB0 MP 0x140563710
		{"_meth_8485", 0x8485}, // SP 0x1404630B0 MP 0x1405618F0
		{"_meth_8486", 0x8486}, // SP 0x000000000 MP 0x14044DF90
		{"_meth_8487", 0x8487}, // SP 0x000000000 MP 0x140380630
		{"_meth_8488", 0x8488}, // SP 0x1402B3150 MP 0x000000000
		{"_meth_8489", 0x8489}, // SP 0x1405D92F0 MP 0x14032CD20
		{"_meth_848a", 0x848A}, // SP 0x140465030 MP 0x140563790
		{"_meth_848b", 0x848B}, // SP 0x140465020 MP 0x140563720
		{"_meth_848c", 0x848C}, // SP 0x1402641A0 MP 0x140330610
		{"_meth_848d", 0x848D}, // SP 0x140264320 MP 0x140330840
		{"_meth_848e", 0x848E}, // SP 0x140264550 MP 0x140330970
		{"_meth_848f", 0x848F}, // SP 0x1402694A0 MP 0x000000000
		{"_meth_8490", 0x8490}, // SP 0x1405D92F0 MP 0x14035E610
		{"_meth_8491", 0x8491}, // SP 0x000000000 MP 0x14037A4A0
		{"_meth_8492", 0x8492}, // SP 0x000000000 MP 0x14037A510
		{"_meth_8493", 0x8493}, // SP 0x1402ACF90 MP 0x000000000
		{"_meth_8494", 0x8494}, // SP 0x1402AD070 MP 0x000000000
		{"_meth_8495", 0x8495}, // SP 0x140268C60 MP 0x000000000
		{"_meth_8496", 0x8496}, // SP 0x000000000 MP 0x140375380
		{"_meth_8497", 0x8497}, // SP 0x000000000 MP 0x140376030
		{"_meth_8498", 0x8498}, // SP 0x140266500 MP 0x14032AB70
		{"_meth_8499", 0x8499}, // SP 0x140266B20 MP 0x14032AF60
		{"_meth_849a", 0x849A}, // SP 0x1405D92F0 MP 0x14032CCB0
		{"_meth_849b", 0x849B}, // SP 0x000000000 MP 0x140375470
		{"_meth_849c", 0x849C}, // SP 0x000000000 MP 0x140375490
		{"_meth_849d", 0x849D}, // SP 0x000000000 MP 0x140375640
		{"_meth_849e", 0x849E}, // SP 0x000000000 MP 0x140375650
		{"_meth_849f", 0x849F}, // SP 0x000000000 MP 0x140375660
		{"_meth_84a0", 0x84A0}, // SP 0x000000000 MP 0x140375680
		{"_meth_84a1", 0x84A1}, // SP 0x000000000 MP 0x140375670
		{"_meth_84a2", 0x84A2}, // SP 0x000000000 MP 0x1403756C0
		{"_meth_84a3", 0x84A3}, // SP 0x000000000 MP 0x1403757F0
		{"_meth_84a4", 0x84A4}, // SP 0x000000000 MP 0x1403756B0
		{"_meth_84a5", 0x84A5}, // SP 0x14029FDA0 MP 0x140369CB0
		{"_meth_84a6", 0x84A6}, // SP 0x1402A02F0 MP 0x14036A120
		{"_meth_84a7", 0x84A7}, // SP 0x1402A04A0 MP 0x14036A190
		{"_meth_84a8", 0x84A8}, // SP 0x1402A0FE0 MP 0x14036A960
		{"_meth_84a9", 0x84A9}, // SP 0x140465C50 MP 0x14055FBF0
		{"_meth_84aa", 0x84AA}, // SP 0x1402979E0 MP 0x1403625A0
		{"_meth_84ab", 0x84AB}, // SP 0x140297E40 MP 0x140362BD0
		{"_meth_84ac", 0x84AC}, // SP 0x140261E70 MP 0x14032DAE0
		{"_meth_84ad", 0x84AD}, // SP 0x1404663D0 MP 0x1405602E0
		{"_meth_84ae", 0x84AE}, // SP 0x1404664D0 MP 0x1405603E0
		{"_meth_84af", 0x84AF}, // SP 0x000000000 MP 0x140333050
		{"_meth_84b0", 0x84B0}, // SP 0x1405D92F0 MP 0x140369850
		{"_meth_84b1", 0x84B1}, // SP 0x1402922D0 MP 0x14035E650
		{"_meth_84b2", 0x84B2}, // SP 0x1402925B0 MP 0x14035E660
		{"_meth_84b3", 0x84B3}, // SP 0x140292CD0 MP 0x14035E700
		{"_meth_84b4", 0x84B4}, // SP 0x140292FC0 MP 0x14035E720
		{"_meth_84b5", 0x84B5}, // SP 0x1405D92F0 MP 0x140364AA0
		{"_meth_84b6", 0x84B6}, // SP 0x1405D92F0 MP 0x14036BC80
		{"_meth_84b7", 0x84B7}, // SP 0x1404655C0 MP 0x14055F460
		{"_meth_84b8", 0x84B8}, // SP 0x1402A09C0 MP 0x14036A6A0
		{"_meth_84b9", 0x84B9}, // SP 0x000000000 MP 0x1403807B0
		{"_meth_84ba", 0x84BA},
		{"_meth_84bb", 0x84BB}, // SP 0x1402643F0 MP 0x140330910
		{"_meth_84bc", 0x84BC}, // SP 0x140264500 MP 0x140330A20
		{"_meth_84bd", 0x84BD}, // SP 0x000000000 MP 0x1403330D0
		{"_meth_84be", 0x84BE}, // SP 0x000000000 MP 0x1403335E0
		{"_meth_84bf", 0x84BF}, // SP 0x140265260 MP 0x1403311D0
		{"_meth_84c0", 0x84C0}, // SP 0x140265710 MP 0x1403291C0
		{"_meth_84c1", 0x84C1}, // SP 0x000000000 MP 0x1403766D0
		{"_meth_84c2", 0x84C2}, // SP 0x14029E430 MP 0x14036AA10
		{"_meth_84c3", 0x84C3}, // SP 0x140463250 MP 0x140561A90
		{"_meth_84c4", 0x84C4}, // SP 0x000000000 MP 0x140450E40
		{"_meth_84c5", 0x84C5}, // SP 0x000000000 MP 0x140450B10
		{"_meth_84c6", 0x84C6}, // SP 0x140260790 MP 0x14032BD80
		{"_meth_84c7", 0x84C7}, // SP 0x140260590 MP 0x000000000
		{"_meth_84c8", 0x84C8}, // SP 0x1402606E0 MP 0x000000000
		{"_meth_84c9", 0x84C9}, // SP 0x140464D30 MP 0x1405634E0
		{"_meth_84ca", 0x84CA}, // SP 0x140464DC0 MP 0x140563580
		{"_meth_84cb", 0x84CB}, // SP 0x000000000 MP 0x140334CC0
		{"_meth_84cc", 0x84CC}, // SP 0x140463270 MP 0x140561AB0
		{"_meth_84cd", 0x84CD}, // SP 0x1402A1470 MP 0x14036A8C0
		{"_meth_84ce", 0x84CE}, // SP 0x1402A15E0 MP 0x14036AD30
		{"_meth_84cf", 0x84CF}, // SP 0x14029ED60 MP 0x1403685E0
		{"_meth_84d0", 0x84D0}, // SP 0x14029EED0 MP 0x1403687A0
		{"_meth_84d1", 0x84D1}, // SP 0x14029F130 MP 0x1403687F0
		{"_meth_84d2", 0x84D2}, // SP 0x14029F300 MP 0x140368AC0
		{"_meth_84d3", 0x84D3}, // SP 0x14029F350 MP 0x140368CE0
		{"_meth_84d4", 0x84D4}, // SP 0x14029F410 MP 0x140368D50
		{"_meth_84d5", 0x84D5}, // SP 0x14029F530 MP 0x140368DF0
		{"_meth_84d6", 0x84D6}, // SP 0x14029F5E0 MP 0x140368ED0
		{"_meth_84d7", 0x84D7},
		{"_meth_84d8", 0x84D8}, // SP 0x1402674E0 MP 0x14032B4A0
		{"_meth_84d9", 0x84D9}, // SP 0x1402B4400 MP 0x140380280
		{"_meth_84da", 0x84DA}, // SP 0x000000000 MP 0x14035B2A0
		{"_meth_84db", 0x84DB}, // SP 0x140248770 MP 0x000000000
		{"_meth_84dc", 0x84DC}, // SP 0x140461FF0 MP 0x000000000
		{"_meth_84dd", 0x84DD},
		{"_meth_84de", 0x84DE},
		{"_meth_84df", 0x84DF}, // SP 0x14028DF20 MP 0x14035B180
		{"_meth_84e0", 0x84E0}, // SP 0x14026E2F0 MP 0x14033C220
		{"_meth_84e1", 0x84E1}, // SP 0x1404632B0 MP 0x140561AF0
		{"_meth_84e2", 0x84E2}, // SP 0x000000000 MP 0x14036F110
		{"_meth_84e3", 0x84E3}, // SP 0x140264A30 MP 0x140331120
		{"_meth_84e4", 0x84E4}, // SP 0x1405D92F0 MP 0x1403604A0
		{"_meth_84e5", 0x84E5}, // SP 0x1402A88B0 MP 0x000000000
		{"_meth_84e6", 0x84E6}, // SP 0x1405D92F0 MP 0x14036C920
		{"_meth_84e7", 0x84E7},
		{"_meth_84e8", 0x84E8}, // SP 0x140264970 MP 0x140330E00
		{"_meth_84e9", 0x84E9}, // SP 0x1402649D0 MP 0x140330E10
		{"_meth_84ea", 0x84EA}, // SP 0x14028ABB0 MP 0x000000000
		{"_meth_84eb", 0x84EB}, // SP 0x14029AF70 MP 0x140364720
		{"_meth_84ec", 0x84EC}, // SP 0x1402A11C0 MP 0x140359610
		{"_meth_84ed", 0x84ED}, // SP 0x140293540 MP 0x140360150
		{"_meth_84ee", 0x84EE}, // SP 0x140298540 MP 0x140361720
		{"_meth_84ef", 0x84EF}, // SP 0x14029B960 MP 0x140363B90
		{"_meth_84f0", 0x84F0}, // SP 0x1404643D0 MP 0x140562AB0
		{"_meth_84f1", 0x84F1}, // SP 0x1404644E0 MP 0x140562BE0
		{"_meth_84f2", 0x84F2}, // SP 0x1405D92F0 MP 0x14035B8C0
		{"_meth_84f3", 0x84F3}, // SP 0x1405D92F0 MP 0x14035B990
		{"_meth_84f4", 0x84F4}, // SP 0x14028E1A0 MP 0x14035B280
		{"_meth_84f5", 0x84F5}, // SP 0x14028E1A0 MP 0x14035B360
		{"_meth_84f6", 0x84F6}, // SP 0x1405D92F0 MP 0x14035B390
		{"_meth_84f7", 0x84F7}, // SP 0x14028E830 MP 0x14035B3A0
		{"_meth_84f8", 0x84F8}, // SP 0x14028E830 MP 0x14035B430
		{"_meth_84f9", 0x84F9}, // SP 0x14028E830 MP 0x14035B540
		{"_meth_84fa", 0x84FA}, // SP 0x1405D92F0 MP 0x14035B7A0
		{"_meth_84fb", 0x84FB}, // SP 0x14029EDE0 MP 0x14036EA10
		{"_meth_84fc", 0x84FC}, // SP 0x140290D70 MP 0x14035A740
		{"_meth_84fd", 0x84FD}, // SP 0x000000000 MP 0x140334AC0
		{"_meth_84fe", 0x84FE}, // SP 0x000000000 MP 0x140376800
		{"_meth_84ff", 0x84FF}, // SP 0x000000000 MP 0x1403768C0
		{"_meth_8500", 0x8500}, // SP 0x14028F5A0 MP 0x14035A550
		{"_meth_8501", 0x8501}, // SP 0x14028E4D0 MP 0x140359550
		{"_meth_8502", 0x8502}, // SP 0x000000000 MP 0x1403767F0
		{"_meth_8503", 0x8503}, // SP 0x14028F6E0 MP 0x14035C670
		{"_meth_8504", 0x8504}, // SP 0x000000000 MP 0x14037A470
		{"_meth_8505", 0x8505}, // SP 0x000000000 MP 0x140374F20
		{"_meth_8506", 0x8506}, // SP 0x000000000 MP 0x14037ADD0
		{"_meth_8507", 0x8507}, // SP 0x000000000 MP 0x14037AAF0
		{"_meth_8508", 0x8508}, // SP 0x000000000 MP 0x140332BC0
		{"_meth_8509", 0x8509}, // SP 0x1402AC990 MP 0x000000000
		{"_meth_850a", 0x850A}, // SP 0x1402646B0 MP 0x140330A80
		{"_meth_850b", 0x850B}, // SP 0x14026DCB0 MP 0x14033BC10
		{"_meth_850c", 0x850C}, // SP 0x000000000 MP 0x140376020
		{"_meth_850d", 0x850D}, // SP 0x14029E8F0 MP 0x1403682B0
		{"_meth_850e", 0x850E}, // SP 0x14029ED00 MP 0x140368570
		{"_meth_850f", 0x850F}, // SP 0x14029A7B0 MP 0x140362600
		{"_meth_8510", 0x8510}, // SP 0x140267840 MP 0x14032BE50
		{"_meth_8511", 0x8511}, // SP 0x140290ED0 MP 0x14035A8E0
		{"_meth_8512", 0x8512}, // SP 0x1404632E0 MP 0x140561B20
		{"_meth_8513", 0x8513}, // SP 0x140463300 MP 0x140561B40
		{"_meth_8514", 0x8514}, // SP 0x14028E830 MP 0x14032B9B0
		{"_meth_8515", 0x8515}, // SP 0x14028E830 MP 0x14032B9D0
		{"_meth_8516", 0x8516}, // SP 0x140299C10 MP 0x140361690
		{"_meth_8517", 0x8517}, // SP 0x000000000 MP 0x1403747A0
		{"_meth_8518", 0x8518}, // SP 0x14026D400 MP 0x14033B580
		{"_meth_8519", 0x8519}, // SP 0x000000000 MP 0x140376990
		{"_meth_851a", 0x851A}, // SP 0x140261370 MP 0x14032C9D0
		{"_meth_851b", 0x851B}, // SP 0x14029F3C0 MP 0x14036EEC0
		{"_meth_851c", 0x851C}, // SP 0x14028E830 MP 0x14036E3D0
		{"_meth_851d", 0x851D}, // SP 0x1402B4480 MP 0x140380300
		{"_meth_851e", 0x851E}, // SP 0x14028A8B0 MP 0x000000000
		{"_meth_851f", 0x851F}, // SP 0x000000000 MP 0x140333EE0
		{"_meth_8520", 0x8520}, // SP 0x000000000 MP 0x14044EEC0
		{"_meth_8521", 0x8521}, // SP 0x140263930 MP 0x000000000
		{"_meth_8522", 0x8522}, // SP 0x000000000 MP 0x140332340
		{"_meth_8523", 0x8523}, // SP 0x1405D92F0 MP 0x140358DC0
		{"_meth_8524", 0x8524}, // SP 0x14028DDD0 MP 0x140359FE0
		{"_meth_8525", 0x8525}, // SP 0x000000000 MP 0x140374C50
		{"_meth_8526", 0x8526}, // SP 0x000000000 MP 0x140374CF0
		{"_meth_8527", 0x8527}, // SP 0x14026DD20 MP 0x14033BC70
		{"_meth_8528", 0x8528}, // SP 0x000000000 MP 0x140332E80
		{"_meth_8529", 0x8529}, // SP 0x14028F7C0 MP 0x14035C9C0
		{"_meth_852a", 0x852A}, // SP 0x140297EF0 MP 0x14035F080
		{"_meth_852b", 0x852B}, // SP 0x000000000 MP 0x140332AD0
		{"_meth_852c", 0x852C}, // SP 0x000000000 MP 0x140331F50
		{"_meth_852d", 0x852D}, // SP 0x14028E830 MP 0x14035B7E0
		{"_meth_852e", 0x852E}, // SP 0x000000000 MP 0x140376A60
		{"_meth_852f", 0x852F}, // SP 0x000000000 MP 0x1404508B0
		{"_meth_8530", 0x8530}, // SP 0x000000000 MP 0x14044F1E0
		{"_meth_8531", 0x8531}, // SP 0x000000000 MP 0x1404503F0
		{"_meth_8532", 0x8532}, // SP 0x000000000 MP 0x140333250
		{"_meth_8533", 0x8533}, // SP 0x000000000 MP 0x14044EC20
		{"_meth_8534", 0x8534}, // SP 0x000000000 MP 0x140450670
		{"_meth_8535", 0x8535}, // SP 0x000000000 MP 0x14044FD10
		{"_meth_8536", 0x8536}, // SP 0x000000000 MP 0x1404504B0
		{"_meth_8537", 0x8537}, // SP 0x000000000 MP 0x1404502E0
		{"_meth_8538", 0x8538}, // SP 0x000000000 MP 0x1404506F0
		{"_meth_8539", 0x8539}, // SP 0x000000000 MP 0x140333F40
		{"_meth_853a", 0x853A}, // SP 0x000000000 MP 0x14037F320
		{"_meth_853b", 0x853B}, // SP 0x000000000 MP 0x14044E080
		{"_meth_853c", 0x853C}, // SP 0x14028F0D0 MP 0x14035BF40
		{"_meth_853d", 0x853D}, // SP 0x14028E830 MP 0x14032CCA0
		{"_meth_853e", 0x853E}, // SP 0x000000000 MP 0x140376AC0
		{"_meth_853f", 0x853F}, // SP 0x000000000 MP 0x14044DFF0
		{"_meth_8540", 0x8540}, // SP 0x140299D70 MP 0x140363160
		{"_meth_8541", 0x8541}, // SP 0x14028F260 MP 0x14035C380
		{"_meth_8542", 0x8542}, // SP 0x000000000 MP 0x140376C00
		{"_meth_8543", 0x8543}, // SP 0x000000000 MP 0x140375800
		{"_meth_8544", 0x8544}, // SP 0x000000000 MP 0x14044FA10
		{"_meth_8545", 0x8545}, // SP 0x000000000 MP 0x140333640
		{"_meth_8546", 0x8546},
		{"_meth_8547", 0x8547}, // SP 0x000000000 MP 0x140450C20
		{"_meth_8548", 0x8548}, // SP 0x140262130 MP 0x14032DF00
		{"_meth_8549", 0x8549}, // SP 0x000000000 MP 0x140333FA0
		{"_meth_854a", 0x854A}, // SP 0x000000000 MP 0x140376C90
		{"_meth_854b", 0x854B}, // SP 0x000000000 MP 0x14044FBA0
		{"_meth_854c", 0x854C}, // SP 0x000000000 MP 0x1403331F0
		{"_meth_854d", 0x854D}, // SP 0x140290CE0 MP 0x14035C300
		{"_meth_854e", 0x854E}, // SP 0x000000000 MP 0x140450A10
		{"_meth_854f", 0x854F}, // SP 0x000000000 MP 0x14044FC10
		{"_meth_8550", 0x8550}, // SP 0x1402B4720 MP 0x1403809E0
		{"_meth_8551", 0x8551}, // SP 0x1402A15D0 MP 0x14035FB60
		{"_meth_8552", 0x8552}, // SP 0x000000000 MP 0x140450C30
		{"_meth_8553", 0x8553}, // SP 0x000000000 MP 0x140375E30
		{"_meth_8554", 0x8554}, // SP 0x000000000 MP 0x14044F3F0
		{"_meth_8555", 0x8555}, // SP 0x000000000 MP 0x140450D30
		{"_meth_8556", 0x8556}, // SP 0x000000000 MP 0x140374980
		{"_meth_8557", 0x8557}, // SP 0x000000000 MP 0x1403749D0
		{"_meth_8558", 0x8558}, // SP 0x000000000 MP 0x140078140
		{"_meth_8559", 0x8559}, // SP 0x1405D92F0 MP 0x14032D760
		{"_meth_855a", 0x855A}, // SP 0x1405D92F0 MP 0x14032D770
		{"_meth_855b", 0x855B}, // SP 0x1405D92F0 MP 0x14032D7F0
		{"_meth_855c", 0x855C}, // SP 0x000000000 MP 0x140376CB0
		{"_meth_855d", 0x855D}, // SP 0x1402A0220 MP 0x1403585F0
		{"_meth_855e", 0x855E}, // SP 0x140293780 MP 0x14035E8C0
		{"_meth_855f", 0x855F}, // SP 0x000000000 MP 0x140374EC0
		{"_meth_8560", 0x8560}, // SP 0x000000000 MP 0x140374F70
		{"_meth_8561", 0x8561}, // SP 0x140292D90 MP 0x14035FB10
		{"_meth_8562", 0x8562}, // SP 0x140293080 MP 0x14035FD80
		{"_meth_8563", 0x8563}, // SP 0x1402B2FB0 MP 0x14037EF00
		{"_meth_8564", 0x8564}, // SP 0x1402CBB60 MP 0x14031F610
		{"_meth_8565", 0x8565}, // SP 0x1402478E0 MP 0x000000000
		{"_meth_8566", 0x8566}, // SP 0x1402A8F10 MP 0x000000000
		{"_meth_8567", 0x8567}, // SP 0x140248F70 MP 0x000000000
		{"_meth_8568", 0x8568}, // SP 0x1402A3820 MP 0x14036E3F0
		{"_meth_8569", 0x8569}, // SP 0x1402982F0 MP 0x140363510
		{"_meth_856a", 0x856A}, // SP 0x1402984C0 MP 0x140363630
		{"_meth_856b", 0x856B}, // SP 0x140296FA0 MP 0x1403619C0
		{"_meth_856c", 0x856C}, // SP 0x140297420 MP 0x140361BA0
		{"_meth_856d", 0x856D}, // SP 0x140297640 MP 0x140361E60
		{"_meth_856e", 0x856E}, // SP 0x14028ABF0 MP 0x000000000
		{"_meth_856f", 0x856F}, // SP 0x14028AC10 MP 0x000000000
		{"_meth_8570", 0x8570}, // SP 0x14028AC60 MP 0x000000000
		{"_meth_8571", 0x8571}, // SP 0x140249010 MP 0x000000000
		{"_meth_8572", 0x8572}, // SP 0x14024AE00 MP 0x000000000
		{"_meth_8573", 0x8573}, // SP 0x140260B70 MP 0x14032C5C0
		{"_meth_8574", 0x8574}, // SP 0x140265AD0 MP 0x140329B40
		{"_meth_8575", 0x8575}, // SP 0x140265C90 MP 0x140329E40
		{"_meth_8576", 0x8576}, // SP 0x000000000 MP 0x140478050
		{"_meth_8577", 0x8577}, // SP 0x000000000 MP 0x140333650
		{"_meth_8578", 0x8578}, // SP 0x140297D90 MP 0x14035FFF0
		{"_meth_8579", 0x8579}, // SP 0x1402933D0 MP 0x14035FF90
		{"_meth_857a", 0x857A}, // SP 0x000000000 MP 0x1403336F0
		{"_meth_857b", 0x857B}, // SP 0x000000000 MP 0x14032B050
		{"_meth_857c", 0x857C}, // SP 0x1405D92F0 MP 0x14036D0E0
		{"_meth_857d", 0x857D}, // SP 0x000000000 MP 0x140334820
		{"_meth_857e", 0x857E}, // SP 0x000000000 MP 0x1403348E0
		{"_meth_857f", 0x857F}, // SP 0x000000000 MP 0x1403329F0
	};

	std::unordered_map<std::string, unsigned> token_map =
	{
		{"pl#", 0x001},
		{"-", 0x002},
		{"radius`", 0x003},
		{"note:", 0x004},
		{"_", 0x005},
		{"_custom", 0x006},
		{"a", 0x007},
		{"ability", 0x008},
		{"accumulate", 0x009},
		{"accuracy", 0x00A},
		{"actionslot1", 0x00B},
		{"actionslot2", 0x00C},
		{"actionslot3", 0x00D},
		{"actionslot4", 0x00E},
		{"actionslot5", 0x00F},
		{"actionslot6", 0x010},
		{"actionslot7", 0x011},
		{"actionslot8", 0x012},
		{"activator", 0x013},
		{"active", 0x014},
		{"activecostume", 0x015},
		{"activeemblemslot", 0x016},
		{"activesquadmember", 0x017},
		{"activevisionset", 0x018},
		{"activevisionsetduration", 0x019},
		{"agent", 0x01A},
		{"agenthealth", 0x01B},
		{"agentname", 0x01C},
		{"agentteam", 0x01D},
		{"ai_event", 0x01E},
		{"ai_sight_line_cycle_group", 0x01F},
		{"ai_sight_line_group", 0x020},
		{"aim_highest_bone", 0x021},
		{"aim_vis_bone", 0x022},
		{"aiSpread", 0x023},
		{"aisquadmembers", 0x024},
		{"alert", 0x025},
		{"alertlevel", 0x026},
		{"alertlevelint", 0x027},
		{"alien", 0x028},
		{"alienplayerloadout", 0x029},
		{"alienplayerstats", 0x02A},
		{"aliensession", 0x02B},
		{"alignx", 0x02C},
		{"aligny", 0x02D},
		{"all", 0x02E},
		{"allies", 0x02F},
		{"allowcrouch", 0x030},
		{"allowdeath", 0x031},
		{"allowjump", 0x032},
		{"allowladders", 0x033},
		{"allowpain", 0x034},
		{"allowprone", 0x035},
		{"allstreaksrestricted", 0x036},
		{"alpha", 0x037},
		{"altmode", 0x038},
		{"always", 0x039},
		{"ambient", 0x03A},
		{"ambienttrack", 0x03B},
		{"ambienttrack_ac130", 0x03C},
		{"ambush", 0x03D},
		{"ambush_nodes_only", 0x03E},
		{"angle_deltas", 0x03F},
		{"anglelerprate", 0x040},
		{"angles", 0x041},
		{"anim_angle_delta", 0x042},
		{"anim_deltas", 0x043},
		{"anim_pose", 0x044},
		{"anim_will_finish", 0x045},
		{"animation", 0x046},
		{"animscript", 0x047},
		{"archived", 0x048},
		{"archivetime", 0x049},
		{"armor", 0x04A},
		{"asleep", 0x04B},
		{"aspectratio", 0x04C},
		{"assaultstreaks", 0x04D},
		{"assignedbucket", 0x04E},
		{"assists", 0x04F},
		{"attachment", 0x050},
		{"attachmentclassrestricted", 0x051},
		{"attachmentrestricted", 0x052},
		{"attachments", 0x053},
		{"attachtag", 0x054},
		{"attacker", 0x055},
		{"attackeraccuracy", 0x056},
		{"attackercount", 0x057},
		{"attackerisjuggernaut", 0x058},
		{"attackerpos", 0x059},
		{"author", 0x05A},
		{"auto_ai", 0x05B},
		{"auto_change", 0x05C},
		{"auto_nonai", 0x05D},
		{"avoidanceboundshalfsize", 0x05E},
		{"awards", 0x05F},
		{"axis", 0x060},
		{"b", 0x061},
		{"back", 0x062},
		{"back_left", 0x063},
		{"back_low", 0x064},
		{"back_mid", 0x065},
		{"back_right", 0x066},
		{"back_up", 0x067},
		{"background", 0x068},
		{"bad_guys", 0x069},
		{"bad_path", 0x06A},
		{"badplaceawareness", 0x06B},
		{"ball_off", 0x06C},
		{"ball_on", 0x06D},
		{"ball_pass", 0x06E},
		{"bandwidthdown", 0x06F},
		{"bandwidthtestcount", 0x070},
		{"bandwidthup", 0x071},
		{"baselineoverflow_max", 0x072},
		{"baselineoverflow_worst", 0x073},
		{"battery_discharge_begin", 0x074},
		{"battery_discharge_end", 0x075},
		{"begin", 0x076},
		{"begin_custom_anim", 0x077},
		{"begin_firing", 0x078},
		{"begin_firing_left", 0x079},
		{"bestweapon", 0x07A},
		{"bestweaponindex", 0x07B},
		{"bipods", 0x07C},
		{"birthtime", 0x07D},
		{"bl_rotor1", 0x07E},
		{"bl_rotor2", 0x07F},
		{"bl_rotor3", 0x080},
		{"blackops2prestige", 0x081},
		{"blackops2rank", 0x082},
		{"blade_hide", 0x083},
		{"blade_show", 0x084},
		{"blockfriendlies", 0x085},
		{"blurradius", 0x086},
		{"body", 0x087},
		{"body_animate_jnt", 0x088},
		{"bottomarc", 0x089},
		{"br_rotor1", 0x08A},
		{"br_rotor2", 0x08B},
		{"br_rotor3", 0x08C},
		{"breadcrumbheader", 0x08D},
		{"buff", 0x08E},
		{"bullet_hitshield", 0x08F},
		{"bullethit", 0x090},
		{"bulletwhizby", 0x091},
		{"c", 0x092},
		{"callingcardindex", 0x093},
		{"camo", 0x094},
		{"cancel_location", 0x095},
		{"canclimbladders", 0x096},
		{"canjumppath", 0x097},
		{"cardicon", 0x098},
		{"cardnameplate", 0x099},
		{"cardtitle", 0x09A},
		{"cgmchecksum", 0x09B},
		{"ch_prestige", 0x09C},
		{"ch_prestige_max", 0x09D},
		{"chainfallback", 0x09E},
		{"chainnode", 0x09F},
		{"challengeprogress", 0x0A0},
		{"challengestate", 0x0A1},
		{"chest", 0x0A2},
		{"churnscores", 0x0A3},
		{"chyron_message1", 0x0A4},
		{"chyron_message2", 0x0A5},
		{"chyron_message3", 0x0A6},
		{"civilian", 0x0A7},
		{"clanidhigh", 0x0A8},
		{"clanidlow", 0x0A9},
		{"classname", 0x0AA},
		{"clipdistance", 0x0AB},
		{"code_classname", 0x0AC},
		{"code_damageradius", 0x0AD},
		{"code_move", 0x0AE},
		{"code_move_slide", 0x0AF},
		{"codecallback_agentadded", 0x0B0},
		{"codecallback_agentdamaged", 0x0B1},
		{"codecallback_agentkilled", 0x0B2},
		{"codecallback_bullethitentity", 0x0B3},
		{"codecallback_codeendgame", 0x0B4},
		{"codecallback_entitydamage", 0x0B5},
		{"codecallback_entityoutofworld", 0x0B6},
		{"codecallback_handleinstantmessage", 0x0B7},
		{"codecallback_hostmigration", 0x0B8},
		{"codecallback_leaderdialog", 0x0B9},
		{"codecallback_partymembers", 0x0BA},
		{"codecallback_playerconnect", 0x0BB},
		{"codecallback_playerdamage", 0x0BC},
		{"codecallback_playerdisconnect", 0x0BD},
		{"codecallback_playergrenadesuicide", 0x0BE},
		{"codecallback_playerkilled", 0x0BF},
		{"codecallback_playerlaststand", 0x0C0},
		{"codecallback_playermigrated", 0x0C1},
		{"codecallback_startgametype", 0x0C2},
		{"codecallback_vehicledamage", 0x0C3},
		{"color", 0x0C4},
		{"color_blind_toggled", 0x0C5},
		{"combat", 0x0C6},
		{"combatmode", 0x0C7},
		{"combatrecord", 0x0C8},
		{"commonoption", 0x0C9},
		{"confirm_location", 0x0CA},
		{"connection_id", 0x0CB},
		{"connectionidchunkhigh", 0x0CC},
		{"connectionidchunklow", 0x0CD},
		{"consolegame", 0x0CE},
		{"consoleidchunkhigh", 0x0CF},
		{"consoleidchunklow", 0x0D0},
		{"constrained", 0x0D1},
		{"contact", 0x0D2},
		{"contextleanenabled", 0x0D3},
		{"convergencetime", 0x0D4},
		{"coopactivesquadmember", 0x0D5},
		{"coopsquadmembers", 0x0D6},
		{"costumes", 0x0D7},
		{"count", 0x0D8},
		{"cover", 0x0D9},
		{"cover_approach", 0x0DA},
		{"coversearchinterval", 0x0DB},
		{"createstruct", 0x0DC},
		{"createtime", 0x0DD},
		{"criticalbulletdamagedist", 0x0DE},
		{"crouch", 0x0DF},
		{"currency", 0x0E0},
		{"current", 0x0E1},
		{"currentanimtime", 0x0E2},
		{"currentgen", 0x0E3},
		{"currentwinstreak", 0x0E4},
		{"cursorhint", 0x0E5},
		{"custom_attach_00", 0x0E6},
		{"custom_attach_01", 0x0E7},
		{"custom_attach_02", 0x0E8},
		{"custom_attach_03", 0x0E9},
		{"custom_attach_04", 0x0EA},
		{"custom_attach_05", 0x0EB},
		{"custom_attach_06", 0x0EC},
		{"custom_attach_07", 0x0ED},
		{"custom_attach_08", 0x0EE},
		{"custom_attach_09", 0x0EF},
		{"custom_attach_10", 0x0F0},
		{"custom_attach_11", 0x0F1},
		{"custom_attach_12", 0x0F2},
		{"custom_attach_13", 0x0F3},
		{"custom_attach_14", 0x0F4},
		{"custom_attach_15", 0x0F5},
		{"customclasses", 0x0F6},
		{"customization_loaded", 0x0F7},
		{"d", 0x0F8},
		{"dailychallengeid", 0x0F9},
		{"damage", 0x0FA},
		{"damage_notdone", 0x0FB},
		{"damagedir", 0x0FC},
		{"damagelocation", 0x0FD},
		{"damagemod", 0x0FE},
		{"damagemultiplier", 0x0FF},
		{"damageshield", 0x100},
		{"damagetaken", 0x101},
		{"damageweapon", 0x102},
		{"damageyaw", 0x103},
		{"dangerreactduration", 0x104},
		{"datalength", 0x105},
		{"dcid", 0x106},
		{"dead", 0x107},
		{"death", 0x108},
		{"deathangles", 0x109},
		{"deathinvulnerabletime", 0x10A},
		{"deathplant", 0x10B},
		{"deathpos", 0x10C},
		{"deaths", 0x10D},
		{"deathshield", 0x10E},
		{"defaultclasses", 0x10F},
		{"defense", 0x110},
		{"defense_level", 0x111},
		{"delayeddeath", 0x112},
		{"deploy_riotshield", 0x113},
		{"desc", 0x114},
		{"descmodified", 0x115},
		{"desiredangle", 0x116},
		{"destructible_type", 0x117},
		{"detectable", 0x118},
		{"detected", 0x119},
		{"detonate", 0x11A},
		{"device_id_high", 0x11B},
		{"device_id_low", 0x11C},
		{"deviceconnectionhistory", 0x11D},
		{"deviceusefrequency", 0x11E},
		{"diequietly", 0x11F},
		{"diffusefraction", 0x120},
		{"direct", 0x121},
		{"disable", 0x122},
		{"disableplayeradsloscheck", 0x123},
		{"dlight", 0x124},
		{"dmg", 0x125},
		{"dodamagetoall", 0x126},
		{"dodangerreact", 0x127},
		{"doffar", 0x128},
		{"dofnear", 0x129},
		{"dofphysicalfocusdistance", 0x12A},
		{"dofphysicalfstop", 0x12B},
		{"dog", 0x12C},
		{"doghandler", 0x12D},
		{"doingambush", 0x12E},
		{"done", 0x12F},
		{"dontavoidplayer", 0x130},
		{"dotofdeath", 0x131},
		{"down", 0x132},
		{"downaimlimit", 0x133},
		{"drawoncompass", 0x134},
		{"dropweapon", 0x135},
		{"duration", 0x136},
		{"eftarc", 0x137},
		{"empty", 0x138},
		{"empty_offhand", 0x139},
		{"enable", 0x13A},
		{"enablehudlighting", 0x13B},
		{"enableshadows", 0x13C},
		{"end_firing", 0x13D},
		{"end_firing_left", 0x13E},
		{"end_script", 0x13F},
		{"enddeaths", 0x140},
		{"endkills", 0x141},
		{"enemy", 0x142},
		{"enemy_sighted", 0x143},
		{"enemy_sighted_lost", 0x144},
		{"enemy_visible", 0x145},
		{"enemyname", 0x146},
		{"enemyplatform", 0x147},
		{"enemyradarmode", 0x148},
		{"enemyxuidhigh", 0x149},
		{"enemyxuidlow", 0x14A},
		{"energy_fire", 0x14B},
		{"engagemaxdist", 0x14C},
		{"engagemaxfalloffdist", 0x14D},
		{"engagemindist", 0x14E},
		{"engageminfalloffdist", 0x14F},
		{"enhanceable", 0x150},
		{"entity", 0x151},
		{"entitydeleted", 0x152},
		{"entityoverflow_max", 0x153},
		{"entityoverflow_worst", 0x154},
		{"equipment", 0x155},
		{"equipmentsetups", 0x156},
		{"escaped", 0x157},
		{"exclusive", 0x158},
		{"exo_ability_activate", 0x159},
		{"exo_adrenaline_fire", 0x15A},
		{"exo_boost", 0x15B},
		{"exo_dodge", 0x15C},
		{"exo_power", 0x15D},
		{"exo_slide", 0x15E},
		{"exo_slide_hit_player", 0x15F},
		{"exoattachment1", 0x160},
		{"exoattachment2", 0x161},
		{"experience", 0x162},
		{"explode", 0x163},
		{"exposedduration", 0x164},
		{"extracustomclassesentitlement", 0x165},
		{"extracustomclassesprestige", 0x166},
		{"extrascore0", 0x167},
		{"extrascore1", 0x168},
		{"face_angle", 0x169},
		{"face_angle_3d", 0x16A},
		{"face_angle_abs", 0x16B},
		{"face_angle_rel", 0x16C},
		{"face_current", 0x16D},
		{"face_default", 0x16E},
		{"face_direction", 0x16F},
		{"face_enemy", 0x170},
		{"face_enemy_or_motion", 0x171},
		{"face_goal", 0x172},
		{"face_motion", 0x173},
		{"face_point", 0x174},
		{"facemotion", 0x175},
		{"failed", 0x176},
		{"falling", 0x177},
		{"fast_radar", 0x178},
		{"favoriteenemy", 0x179},
		{"finalaccuracy", 0x17A},
		{"first_person", 0x17B},
		{"firstplayedsptime", 0x17C},
		{"fixednode", 0x17D},
		{"fixednodesaferadius", 0x17E},
		{"fl_rotor1", 0x17F},
		{"fl_rotor2", 0x180},
		{"fl_rotor3", 0x181},
		{"flash", 0x182},
		{"flashbang", 0x183},
		{"foley", 0x184},
		{"follow", 0x185},
		{"followmax", 0x186},
		{"followmin", 0x187},
		{"font", 0x188},
		{"fontscale", 0x189},
		{"foot_ik_active", 0x18A},
		{"foot_ik_blend_in", 0x18B},
		{"foot_ik_blend_out", 0x18C},
		{"foot_ik_inactive", 0x18D},
		{"footstepdetectdist", 0x18E},
		{"footstepdetectdistsprint", 0x18F},
		{"footstepdetectdistwalk", 0x190},
		{"force_off", 0x191},
		{"force_on", 0x192},
		{"force_fully_on", 0x193},
		{"forcepartyskillignore", 0x194},
		{"forceragdollimmediate", 0x195},
		{"forcespectatorclient", 0x196},
		{"foregrip_off", 0x197},
		{"foreground", 0x198},
		{"forward", 0x199},
		{"fov", 0x19A},
		{"fovcosine", 0x19B},
		{"fovcosinebusy", 0x19C},
		{"fovcosinez", 0x19D},
		{"fr_rotor1", 0x19E},
		{"fr_rotor2", 0x19F},
		{"fr_rotor3", 0x1A0},
		{"fraction", 0x1A1},
		{"frag", 0x1A2},
		{"free", 0x1A3},
		{"freecamera", 0x1A4},
		{"freelook", 0x1A5},
		{"frequency", 0x1A6},
		{"friendlyfire", 0x1A7},
		{"front_left", 0x1A8},
		{"front_right", 0x1A9},
		{"frontshieldanglecos", 0x1AA},
		{"fs_concrete", 0x1AB},
		{"fs_dirt", 0x1AC},
		{"fs_metal", 0x1AD},
		{"fs_wood", 0x1AE},
		{"game_extrainfo", 0x1AF},
		{"gamecount", 0x1B0},
		{"gamertag", 0x1B1},
		{"gamesplayed", 0x1B2},
		{"gametype", 0x1B3},
		{"gender", 0x1B4},
		{"ghostsprestige", 0x1B5},
		{"ghostsrank", 0x1B6},
		{"glass", 0x1B7},
		{"glass_damaged", 0x1B8},
		{"glass_destroyed", 0x1B9},
		{"globalcostume", 0x1BA},
		{"gloves", 0x1BB},
		{"glowalpha", 0x1BC},
		{"glowcolor", 0x1BD},
		{"goal", 0x1BE},
		{"goal_changed", 0x1BF},
		{"goal_reached", 0x1C0},
		{"goal_yaw", 0x1C1},
		{"goalheight", 0x1C2},
		{"goalpos", 0x1C3},
		{"goalradius", 0x1C4},
		{"goaltime", 0x1C5},
		{"goalweight", 0x1C6},
		{"goingtoruntopos", 0x1C7},
		{"gravity", 0x1C8},
		{"gravity_noclip", 0x1C9},
		{"grenade", 0x1CA},
		{"grenade_fire", 0x1CB},
		{"grenade_off", 0x1CC},
		{"grenade_on", 0x1CD},
		{"grenade_pullback", 0x1CE},
		{"grenade_return_hand_tag", 0x1CF},
		{"grenadeammo", 0x1D0},
		{"grenadeawareness", 0x1D1},
		{"grenadedanger", 0x1D2},
		{"grenadeweapon", 0x1D3},
		{"ground_slam", 0x1D4},
		{"ground_slam_hit_player", 0x1D5},
		{"groundentchanged", 0x1D6},
		{"groundtype", 0x1D7},
		{"gunblockedbywall", 0x1D8},
		{"gunshot", 0x1D9},
		{"gunshot_teammate", 0x1DA},
		{"hardcoremodeon", 0x1DB},
		{"hasdoublexpitem", 0x1DC},
		{"hasradar", 0x1DD},
		{"hasvalidcostumeselfieimage", 0x1DE},
		{"head", 0x1DF},
		{"headicon", 0x1E0},
		{"headiconteam", 0x1E1},
		{"headshots", 0x1E2},
		{"health", 0x1E3},
		{"healthregen", 0x1E4},
		{"height", 0x1E5},
		{"helmet", 0x1E6},
		{"hidein3rdperson", 0x1E7},
		{"hidewhendead", 0x1E8},
		{"hidewhenindemo", 0x1E9},
		{"hidewheninmenu", 0x1EA},
		{"high_priority", 0x1EB},
		{"highlyawareradius", 0x1EC},
		{"hindlegstraceoffset", 0x1ED},
		{"hintstring", 0x1EE},
		{"hit_by_missile", 0x1EF},
		{"horzalign", 0x1F0},
		{"host_sucks_end_game", 0x1F1},
		{"hostfailures", 0x1F2},
		{"hostquits", 0x1F3},
		{"hostsuccesses", 0x1F4},
		{"human", 0x1F5},
		{"iconnew", 0x1F6},
		{"iconunlocked", 0x1F7},
		{"ignoreall", 0x1F8},
		{"ignoreclosefoliage", 0x1F9},
		{"ignoreexplosionevents", 0x1FA},
		{"ignoreforfixednodesafecheck", 0x1FB},
		{"ignoreme", 0x1FC},
		{"ignorerandombulletdamage", 0x1FD},
		{"ignoresuppression", 0x1FE},
		{"ignoretriggers", 0x1FF},
		{"ikweight", 0x200},
		{"index", 0x201},
		{"infinite_energy", 0x202},
		{"info_notnull", 0x203},
		{"info_player_start", 0x204},
		{"init", 0x205},
		{"initstructs", 0x206},
		{"insolid", 0x207},
		{"intermission", 0x208},
		{"interval", 0x209},
		{"inuse", 0x20A},
		{"invalid_parent", 0x20B},
		{"invisible", 0x20C},
		{"isradarblocked", 0x20D},
		{"item", 0x20E},
		{"j_exo_rcket_arm02", 0x20F},
		{"j_exoclav04_l", 0x210},
		{"j_exoclav04_r", 0x211},
		{"j_exohip04_l", 0x212},
		{"j_exohip04_r", 0x213},
		{"j_eyeball_le", 0x214},
		{"j_eyeball_ri", 0x215},
		{"j_gun", 0x216},
		{"j_head", 0x217},
		{"j_hip_l", 0x218},
		{"j_hip_r", 0x219},
		{"j_knee_le", 0x21A},
		{"j_knee_ri", 0x21B},
		{"j_left_elbow", 0x21C},
		{"j_left_hand", 0x21D},
		{"j_left_shoulder", 0x21E},
		{"j_mainroot", 0x21F},
		{"j_neck", 0x220},
		{"j_right_elbow", 0x221},
		{"j_right_hand", 0x222},
		{"j_right_hand_placement", 0x223},
		{"j_right_shoulder", 0x224},
		{"j_rocket", 0x225},
		{"j_spine4", 0x226},
		{"j_spinelower", 0x227},
		{"j_spineupper", 0x228},
		{"jumpcost", 0x229},
		{"jumping", 0x22A},
		{"justclass", 0x22B},
		{"kdratio", 0x22C},
		{"keepclaimednode", 0x22D},
		{"keepclaimednodeifvalid", 0x22E},
		{"keepnodeduringscriptedanim", 0x22F},
		{"key1", 0x230},
		{"key2", 0x231},
		{"kill_timestamp", 0x232},
		{"killanimscript", 0x233},
		{"killcamentity", 0x234},
		{"killcamentitylookat", 0x235},
		{"kills", 0x236},
		{"killstreak", 0x237},
		{"killstreakcount", 0x238},
		{"killstreakrestricted", 0x239},
		{"killstreakunlocked", 0x23A},
		{"knife_off", 0x23B},
		{"knife_on", 0x23C},
		{"known_event", 0x23D},
		{"label", 0x23E},
		{"ladder_down", 0x23F},
		{"ladder_up", 0x240},
		{"land", 0x241},
		{"last_stand_count", 0x242},
		{"lastattacker", 0x243},
		{"lastenemysightpos", 0x244},
		{"lastplayedtime", 0x245},
		{"laststand", 0x246},
		{"leanamount", 0x247},
		{"ledge", 0x248},
		{"left", 0x249},
		{"leftaimlimit", 0x24A},
		{"leftarc", 0x24B},
		{"lethal", 0x24C},
		{"lifecount", 0x24D},
		{"light", 0x24E},
		{"lives", 0x24F},
		{"loadouts", 0x250},
		{"lockorientation", 0x251},
		{"lod", 0x252},
		{"look", 0x253},
		{"lookahead", 0x254},
		{"lookaheaddir", 0x255},
		{"lookaheaddist", 0x256},
		{"lookaheadhitsstairs", 0x257},
		{"lookforward", 0x258},
		{"lookright", 0x259},
		{"looktarget", 0x25A},
		{"lookup", 0x25B},
		{"loot", 0x25C},
		{"lootnew", 0x25D},
		{"loses", 0x25E},
		{"low_priority", 0x25F},
		{"lowresbackground", 0x260},
		{"luinotifyserver", 0x261},
		{"mag_eject", 0x262},
		{"mag_eject_left", 0x263},
		{"main", 0x264},
		{"manual", 0x265},
		{"manual_ai", 0x266},
		{"manual_change", 0x267},
		{"map", 0x268},
		{"matchid", 0x269},
		{"matchmakingsettingsversion", 0x26A},
		{"matchmakingtesttype", 0x26B},
		{"max_time", 0x26C},
		{"maxfaceenemydist", 0x26D},
		{"maxhealth", 0x26E},
		{"maxrange", 0x26F},
		{"maxsightdistsqrd", 0x270},
		{"maxturnspeed", 0x271},
		{"maxvisibledist", 0x272},
		{"melee_fired", 0x273},
		{"melee_hit_react", 0x274},
		{"meleeattackdist", 0x275},
		{"menuresponse", 0x276},
		{"micro_dlc_bits_last_gen", 0x277},
		{"micro_dlc_bits_next_gen", 0x278},
		{"middle_left", 0x279},
		{"middle_right", 0x27A},
		{"migrateablequits", 0x27B},
		{"min_energy", 0x27C},
		{"min_time", 0x27D},
		{"minpaindamage", 0x27E},
		{"minusedistsq", 0x27F},
		{"missile_fire", 0x280},
		{"missile_passed_target", 0x281},
		{"missile_stuck", 0x282},
		{"mlgversion", 0x283},
		{"mod", 0x284},
		{"mod_crush", 0x285},
		{"mod_energy", 0x286},
		{"mod_explosive", 0x287},
		{"mod_explosive_bullet", 0x288},
		{"mod_falling", 0x289},
		{"mod_grenade", 0x28A},
		{"mod_grenade_splash", 0x28B},
		{"mod_head_shot", 0x28C},
		{"mod_impact", 0x28D},
		{"mod_melee", 0x28E},
		{"mod_melee_alien", 0x28F},
		{"mod_melee_alt", 0x290},
		{"mod_melee_dog", 0x291},
		{"mod_pistol_bullet", 0x292},
		{"mod_projectile", 0x293},
		{"mod_projectile_splash", 0x294},
		{"mod_rifle_bullet", 0x295},
		{"mod_suicide", 0x296},
		{"mod_trigger_hurt", 0x297},
		{"mod_unknown", 0x298},
		{"model", 0x299},
		{"modeprefix", 0x29A},
		{"modifiers", 0x29B},
		{"motiontrackerenabled", 0x29C},
		{"mounted_dlc_bits", 0x29D},
		{"movedone", 0x29E},
		{"movemode", 0x29F},
		{"munition", 0x2A0},
		{"munition_level", 0x2A1},
		{"mw3prestige", 0x2A2},
		{"mw3rank", 0x2A3},
		{"name", 0x2A4},
		{"namemodified", 0x2A5},
		{"near_goal", 0x2A6},
		{"nearz", 0x2A7},
		{"neutral", 0x2A8},
		{"never", 0x2A9},
		{"newenemyreactiondistsq", 0x2AA},
		{"newentitlement", 0x2AB},
		{"nextgen", 0x2AC},
		{"nextreadblackops2", 0x2AD},
		{"nextreadghosts0", 0x2AE},
		{"nextreadghosts1", 0x2AF},
		{"nextreadmw3", 0x2B0},
		{"night_vision_off", 0x2B1},
		{"night_vision_on", 0x2B2},
		{"no_bot_random_path", 0x2B3},
		{"no_cover", 0x2B4},
		{"no_gravity", 0x2B5},
		{"noattackeraccuracymod", 0x2B6},
		{"noclip", 0x2B7},
		{"node", 0x2B8},
		{"node_not_safe", 0x2B9},
		{"node_out_of_range", 0x2BA},
		{"node_relinquished", 0x2BB},
		{"node_taken", 0x2BC},
		{"nodeoffsetpos", 0x2BD},
		{"nododgemove", 0x2BE},
		{"nogravity", 0x2BF},
		{"nogrenadereturnthrow", 0x2C0},
		{"noncombat", 0x2C1},
		{"none", 0x2C2},
		{"nonmigrateablequits", 0x2C3},
		{"nophysics", 0x2C4},
		{"normal", 0x2C5},
		{"normal_radar", 0x2C6},
		{"northyaw", 0x2C7},
		{"notifyname", 0x2C8},
		{"notinsolid", 0x2C9},
		{"num0", 0x2CA},
		{"num1", 0x2CB},
		{"num2", 0x2CC},
		{"num3", 0x2CD},
		{"num4", 0x2CE},
		{"objective", 0x2CF},
		{"obstacle", 0x2D0},
		{"offense", 0x2D1},
		{"offense_level", 0x2D2},
		{"offhand", 0x2D3},
		{"offhand_end", 0x2D4},
		{"offhandweapon", 0x2D5},
		{"oldtime", 0x2D6},
		{"ondeactivate", 0x2D7},
		{"onenterstate", 0x2D8},
		{"only_sky", 0x2D9},
		{"onlygoodnearestnodes", 0x2DA},
		{"onwifi", 0x2DB},
		{"operationsdeadline", 0x2DC},
		{"oriented", 0x2DD},
		{"orientto_complete", 0x2DE},
		{"origin", 0x2DF},
		{"other", 0x2E0},
		{"over", 0x2E1},
		{"owner", 0x2E2},
		{"pacifist", 0x2E3},
		{"pacifistwait", 0x2E4},
		{"pain", 0x2E5},
		{"pantssize", 0x2E6},
		{"parentindex", 0x2E7},
		{"parentname", 0x2E8},
		{"partyid", 0x2E9},
		{"pasttitledata", 0x2EA},
		{"patch", 0x2EB},
		{"patchbacking", 0x2EC},
		{"path_blocked", 0x2ED},
		{"path_changed", 0x2EE},
		{"path_dir_change", 0x2EF},
		{"path_enemy", 0x2F0},
		{"path_need_dodge", 0x2F1},
		{"path_set", 0x2F2},
		{"pathenemyfightdist", 0x2F3},
		{"pathenemylookahead", 0x2F4},
		{"pathgoalpos", 0x2F5},
		{"pathlookaheaddist", 0x2F6},
		{"pathrandompercent", 0x2F7},
		{"pc", 0x2F8},
		{"pccg", 0x2F9},
		{"pelvis", 0x2FA},
		{"perk1", 0x2FB},
		{"perk2", 0x2FC},
		{"perk3", 0x2FD},
		{"perk4", 0x2FE},
		{"perk5", 0x2FF},
		{"perk6", 0x300},
		{"perkclassrestricted", 0x301},
		{"perkrestricted", 0x302},
		{"perks", 0x303},
		{"perkslots", 0x304},
		{"pers", 0x305},
		{"persistentperksunlocked", 0x306},
		{"persistentweaponsunlocked", 0x307},
		{"phone_off", 0x308},
		{"phone_on", 0x309},
		{"physics_finished", 0x30A},
		{"physics_impact", 0x30B},
		{"pickup", 0x30C},
		{"pickup_riotshield", 0x30D},
		{"pistol", 0x30E},
		{"pitchamount", 0x30F},
		{"pitchconvergencetime", 0x310},
		{"plane_waypoint", 0x311},
		{"playedblackops2", 0x312},
		{"playedghosts", 0x313},
		{"playedmw3", 0x314},
		{"player", 0x315},
		{"player_controller", 0x316},
		{"player_pushed", 0x317},
		{"playercardbackground", 0x318},
		{"playercardpatch", 0x319},
		{"playercardpatchbacking", 0x31A},
		{"playerconnectionhistory", 0x31B},
		{"playerid", 0x31C},
		{"playerip", 0x31D},
		{"playername", 0x31E},
		{"playerpositions", 0x31F},
		{"players", 0x320},
		{"playerSpread", 0x321},
		{"playerxuidhigh", 0x322},
		{"playerxuidlow", 0x323},
		{"playing", 0x324},
		{"points", 0x325},
		{"position", 0x326},
		{"positioninworld", 0x327},
		{"postsharpturnlookaheaddist", 0x328},
		{"precache", 0x329},
		{"predicted_projectile_impact", 0x32A},
		{"prestige", 0x32B},
		{"prestigedoublexp", 0x32C},
		{"prestigedoublexpmaxtimeplayed", 0x32D},
		{"prestigeshoptokens", 0x32E},
		{"prestigeshoptokensentitlement", 0x32F},
		{"prevanimdelta", 0x330},
		{"prevnode", 0x331},
		{"prevscript", 0x332},
		{"primary", 0x333},
		{"primaryattachment1", 0x334},
		{"primaryattachment2", 0x335},
		{"primaryattachment3", 0x336},
		{"primaryattachments", 0x337},
		{"primaryattachkit", 0x338},
		{"primarycamo", 0x339},
		{"primaryfurniturekit", 0x33A},
		{"primaryoffhand", 0x33B},
		{"primaryreticle", 0x33C},
		{"primaryweapon", 0x33D},
		{"privatematchactivesquadmember", 0x33E},
		{"privatematchcustomclasses", 0x33F},
		{"privatematchsquadmembers", 0x340},
		{"projectile_impact", 0x341},
		{"projectile_impact_player", 0x342},
		{"prone", 0x343},
		{"proneok", 0x344},
		{"providecoveringfire", 0x345},
		{"ps3", 0x346},
		{"ps4", 0x347},
		{"psoffsettime", 0x348},
		{"pushable", 0x349},
		{"radaralwayson", 0x34A},
		{"radarmode", 0x34B},
		{"radarshowenemydirection", 0x34C},
		{"radarstrength", 0x34D},
		{"radius", 0x34E},
		{"ragdoll_early_result", 0x34F},
		{"raise_riotshield", 0x350},
		{"rank", 0x351},
		{"rate", 0x352},
		{"reached_end_node", 0x353},
		{"reached_wait_node", 0x354},
		{"reached_wait_speed", 0x355},
		{"reactiontargetpos", 0x356},
		{"realtimedelta", 0x357},
		{"receiver", 0x358},
		{"recipename", 0x359},
		{"reciprocality", 0x35A},
		{"reflection_clear_color", 0x35B},
		{"reinforcement", 0x35C},
		{"relativedir", 0x35D},
		{"relativeoffset", 0x35E},
		{"reload", 0x35F},
		{"reload_start", 0x360},
		{"remotemissilespawn", 0x361},
		{"rendertotexture", 0x362},
		{"reportindex", 0x363},
		{"reports", 0x364},
		{"reputation", 0x365},
		{"requestarrivalnotify", 0x366},
		{"requirement_beat100waves", 0x367},
		{"requirement_beat200waves", 0x368},
		{"requirement_beat50waves", 0x369},
		{"requirement_beatenzombies", 0x36A},
		{"requirement_maxarmorproficiency", 0x36B},
		{"requirement_maxweaponproficiency", 0x36C},
		{"requirement_playedallmaps", 0x36D},
		{"requirement_unlockedprison", 0x36E},
		{"requirement_unlockedtier2", 0x36F},
		{"requirement_unlockedtier3", 0x370},
		{"reserved", 0x371},
		{"respawndelay", 0x372},
		{"result", 0x373},
		{"reticle", 0x374},
		{"return_pitch", 0x375},
		{"reverse", 0x376},
		{"revives", 0x377},
		{"right", 0x378},
		{"rightaimlimit", 0x379},
		{"rightarc", 0x37A},
		{"riotshield_damaged", 0x37B},
		{"riotshield_hit", 0x37C},
		{"rocket", 0x37D},
		{"rocket_off", 0x37E},
		{"rocket_on", 0x37F},
		{"rotatedone", 0x380},
		{"rotation", 0x381},
		{"run", 0x382},
		{"runcost", 0x383},
		{"runto_arrived", 0x384},
		{"safetochangescript", 0x385},
		{"scavenger", 0x386},
		{"scope_cap", 0x387},
		{"scope_center", 0x388},
		{"scope_top", 0x389},
		{"score", 0x38A},
		{"script", 0x38B},
		{"script_brushmodel", 0x38C},
		{"script_clut", 0x38D},
		{"script_context", 0x38E},
		{"script_delay", 0x38F},
		{"script_goal_changed", 0x390},
		{"script_label", 0x391},
		{"script_lightset", 0x392},
		{"script_linkname", 0x393},
		{"script_model", 0x394},
		{"script_noteworthy", 0x395},
		{"script_origin", 0x396},
		{"script_parent", 0x397},
		{"script_parentname", 0x398},
		{"script_pushable", 0x399},
		{"script_vehicle", 0x39A},
		{"script_vehicle_collision", 0x39B},
		{"script_vehicle_collmap", 0x39C},
		{"script_vehicle_corpse", 0x39D},
		{"script_visionset", 0x39E},
		{"script_water", 0x39F},
		{"script_reverb", 0x3A0},
		{"script_zone", 0x3A1},
		{"scriptable", 0x3A2},
		{"scriptableactor", 0x3A3},
		{"scripted_viewmodel_anim", 0x3A4},
		{"scriptedarrivalent", 0x3A5},
		{"search_end", 0x3A6},
		{"secondary", 0x3A7},
		{"secondaryattachment1", 0x3A8},
		{"secondaryattachment2", 0x3A9},
		{"secondaryattachments", 0x3AA},
		{"secondaryattachkit", 0x3AB},
		{"secondarycamo", 0x3AC},
		{"secondaryfurniturekit", 0x3AD},
		{"secondaryoffhand", 0x3AE},
		{"secondaryreticle", 0x3AF},
		{"secondaryweapon", 0x3B0},
		{"sentry", 0x3B1},
		{"sentry_manual", 0x3B2},
		{"sentry_offline", 0x3B3},
		{"servertimecount", 0x3B4},
		{"servertimeexceedcount", 0x3B5},
		{"servertimemax", 0x3B6},
		{"servertimetotal", 0x3B7},
		{"servertimetotalexceed", 0x3B8},
		{"sessionstate", 0x3B9},
		{"sessionteam", 0x3BA},
		{"sharpturn", 0x3BB},
		{"sharpturnlookaheaddist", 0x3BC},
		{"sharpturnnotifydist", 0x3BD},
		{"sharpturntooclosetodestdist", 0x3BE},
		{"shirt", 0x3BF},
		{"showinkillcam", 0x3C0},
		{"showkillcam", 0x3C1},
		{"sightlatency", 0x3C2},
		{"silenced_shot", 0x3C3},
		{"skill_points", 0x3C4},
		{"skillbucket", 0x3C5},
		{"skillrating", 0x3C6},
		{"skillratingtype", 0x3C7},
		{"slidevelocity", 0x3C8},
		{"slowmo_active", 0x3C9},
		{"slowmo_passive", 0x3CA},
		{"smoke", 0x3CB},
		{"snd_channelvolprio_holdbreath", 0x3CC},
		{"snd_channelvolprio_pain", 0x3CD},
		{"snd_channelvolprio_shellshock", 0x3CE},
		{"snd_enveffectsprio_level", 0x3CF},
		{"snd_enveffectsprio_shellshock", 0x3D0},
		{"sort", 0x3D1},
		{"sound_blend", 0x3D2},
		{"soundeventdone", 0x3D3},
		{"space", 0x3D4},
		{"spawned", 0x3D5},
		{"spawner", 0x3D6},
		{"spawnflags", 0x3D7},
		{"spawnpos", 0x3D8},
		{"spawntime", 0x3D9},
		{"specialgrenade", 0x3DA},
		{"spectatekillcam", 0x3DB},
		{"spectating_cycle", 0x3DC},
		{"spectator", 0x3DD},
		{"speechcommand", 0x3DE},
		{"speed", 0x3DF},
		{"splatter", 0x3E0},
		{"splineplanereachednode", 0x3E1},
		{"sprint_begin", 0x3E2},
		{"sprint_end", 0x3E3},
		{"sprint_slide_begin", 0x3E4},
		{"sprint_slide_end", 0x3E5},
		{"squad_base", 0x3E6},
		{"squad_mode", 0x3E7},
		{"squad_name", 0x3E8},
		{"squadhq", 0x3E9},
		{"squadmembers", 0x3EA},
		{"squadmemxp", 0x3EB},
		{"squadname", 0x3EC},
		{"stairs", 0x3ED},
		{"stairsstate", 0x3EE},
		{"stand", 0x3EF},
		{"start_blend", 0x3F0},
		{"start_move", 0x3F1},
		{"start_ragdoll", 0x3F2},
		{"startdeaths", 0x3F3},
		{"startdeploy_riotshield", 0x3F4},
		{"startingoffsetforlife", 0x3F5},
		{"startkills", 0x3F6},
		{"state_changed", 0x3F7},
		{"statelocked", 0x3F8},
		{"stencil_disable", 0x3F9},
		{"stencil_onesided", 0x3FA},
		{"stencil_twosided", 0x3FB},
		{"stencilfunc_always", 0x3FC},
		{"stencilfunc_equal", 0x3FD},
		{"stencilfunc_greater", 0x3FE},
		{"stencilfunc_greaterequal", 0x3FF},
		{"stencilfunc_less", 0x400},
		{"stencilfunc_lessequal", 0x401},
		{"stencilfunc_never", 0x402},
		{"stencilfunc_notequal", 0x403},
		{"stencilop_decr", 0x404},
		{"stencilop_decrsat", 0x405},
		{"stencilop_incr", 0x406},
		{"stencilop_incrsat", 0x407},
		{"stencilop_invert", 0x408},
		{"stencilop_keep", 0x409},
		{"stencilop_replace", 0x40A},
		{"stencilop_zero", 0x40B},
		{"stop", 0x40C},
		{"stop_soon", 0x40D},
		{"stopanimdistsq", 0x40E},
		{"stopsoonnotifydist", 0x40F},
		{"streak", 0x410},
		{"streaktype", 0x411},
		{"suckedashost", 0x412},
		{"suncolor", 0x413},
		{"sundirection", 0x414},
		{"sunlight", 0x415},
		{"support", 0x416},
		{"support_level", 0x417},
		{"suppression", 0x418},
		{"suppression_end", 0x419},
		{"suppressionduration", 0x41A},
		{"suppressionmeter", 0x41B},
		{"suppressionstarttime", 0x41C},
		{"suppressionTime", 0x41D},
		{"suppressionwait", 0x41E},
		{"surfacetype", 0x41F},
		{"surprisedbymedistsq", 0x420},
		{"swimmer", 0x421},
		{"switched_var_grenade", 0x422},
		{"syncedmeleetarget", 0x423},
		{"tactical", 0x424},
		{"tag", 0x425},
		{"tag_ai_aim_target", 0x426},
		{"tag_aim", 0x427},
		{"tag_aim_animated", 0x428},
		{"tag_aim_pivot", 0x429},
		{"tag_barrel", 0x42A},
		{"tag_blade_off", 0x42B},
		{"tag_body", 0x42C},
		{"tag_brass", 0x42D},
		{"tag_brass_2", 0x42E},
		{"tag_butt", 0x42F},
		{"tag_camera", 0x430},
		{"tag_clip", 0x431},
		{"tag_clip_dual", 0x432},
		{"tag_clip_dual2", 0x433},
		{"tag_detach", 0x434},
		{"tag_engine_left", 0x435},
		{"tag_engine_right", 0x436},
		{"tag_eotech_reticle", 0x437},
		{"tag_eye", 0x438},
		{"tag_flash", 0x439},
		{"tag_flash_11", 0x43A},
		{"tag_flash_2", 0x43B},
		{"tag_flash_22", 0x43C},
		{"tag_flash_3", 0x43D},
		{"tag_flash_launcher", 0x43E},
		{"tag_flash_silenced", 0x43F},
		{"tag_fx", 0x440},
		{"tag_gasmask", 0x441},
		{"tag_gasmask2", 0x442},
		{"tag_ik_ankle_fl", 0x443},
		{"tag_ik_ankle_fr", 0x444},
		{"tag_ik_ankle_kl", 0x445},
		{"tag_ik_ankle_kr", 0x446},
		{"tag_ik_ankle_ml", 0x447},
		{"tag_ik_ankle_mr", 0x448},
		{"tag_ik_footflat_fl", 0x449},
		{"tag_ik_footflat_fr", 0x44A},
		{"tag_ik_footflat_kl", 0x44B},
		{"tag_ik_footflat_kr", 0x44C},
		{"tag_ik_footflat_ml", 0x44D},
		{"tag_ik_footflat_mr", 0x44E},
		{"tag_ik_hip_fl", 0x44F},
		{"tag_ik_hip_fr", 0x450},
		{"tag_ik_hip_kl", 0x451},
		{"tag_ik_hip_kr", 0x452},
		{"tag_ik_hip_ml", 0x453},
		{"tag_ik_hip_mr", 0x454},
		{"tag_ik_knee_fl", 0x455},
		{"tag_ik_knee_fr", 0x456},
		{"tag_ik_knee_kl", 0x457},
		{"tag_ik_knee_kr", 0x458},
		{"tag_ik_knee_ml", 0x459},
		{"tag_ik_knee_mr", 0x45A},
		{"tag_ik_loc_le", 0x45B},
		{"tag_ik_loc_le_foregrip", 0x45C},
		{"tag_ik_loc_le_launcher", 0x45D},
		{"tag_ik_loc_le_shotgun", 0x45E},
		{"tag_ik_target", 0x45F},
		{"tag_inhand", 0x460},
		{"tag_jetblast_fx", 0x461},
		{"tag_jetpack", 0x462},
		{"tag_knife_attach", 0x463},
		{"tag_knife_fx", 0x464},
		{"tag_laser", 0x465},
		{"tag_launcher", 0x466},
		{"tag_magnifier_eotech_reticle", 0x467},
		{"tag_mobile_cover_upright", 0x468},
		{"tag_motion_tracker_bl", 0x469},
		{"tag_motion_tracker_br", 0x46A},
		{"tag_motion_tracker_fx", 0x46B},
		{"tag_motion_tracker_tl", 0x46C},
		{"tag_origin", 0x46D},
		{"tag_player", 0x46E},
		{"tag_popout", 0x46F},
		{"tag_rail_master_off", 0x470},
		{"tag_rail_master_on", 0x471},
		{"tag_rail_side_off", 0x472},
		{"tag_rail_side_on", 0x473},
		{"tag_reticle_acog", 0x474},
		{"tag_reticle_default", 0x475},
		{"tag_reticle_default2", 0x476},
		{"tag_reticle_hamr", 0x477},
		{"tag_reticle_on", 0x478},
		{"tag_reticle_red_dot", 0x479},
		{"tag_reticle_reflex", 0x47A},
		{"tag_reticle_tavor_scope", 0x47B},
		{"tag_reticle_thermal_scope", 0x47C},
		{"tag_rider", 0x47D},
		{"tag_riot_shield", 0x47E},
		{"tag_rocket", 0x47F},
		{"tag_scope_ads_off", 0x480},
		{"tag_scope_ads_on", 0x481},
		{"tag_shield_back", 0x482},
		{"tag_shotgun", 0x483},
		{"tag_show_alt", 0x484},
		{"tag_sight_off", 0x485},
		{"tag_sight_on", 0x486},
		{"tag_stow_back_mid_attach", 0x487},
		{"tag_stowed_back", 0x488},
		{"tag_stowed_hip_rear", 0x489},
		{"tag_sync", 0x48A},
		{"tag_tip", 0x48B},
		{"tag_turret", 0x48C},
		{"tag_turret_base", 0x48D},
		{"tag_turret_pitch", 0x48E},
		{"tag_turret_yaw", 0x48F},
		{"tag_weapon", 0x490},
		{"tag_weapon_chest", 0x491},
		{"tag_weapon_left", 0x492},
		{"tag_weapon_right", 0x493},
		{"tag_wheel_back_left", 0x494},
		{"tag_wheel_back_right", 0x495},
		{"tag_wheel_front_left", 0x496},
		{"tag_wheel_front_right", 0x497},
		{"tag_wheel_middle_left", 0x498},
		{"tag_wheel_middle_right", 0x499},
		{"tag_wheel_spin_left01", 0x49A},
		{"tag_wheel_spin_left02", 0x49B},
		{"tag_wheel_spin_left03", 0x49C},
		{"tag_wheel_spin_right01", 0x49D},
		{"tag_wheel_spin_right02", 0x49E},
		{"tag_wheel_spin_right03", 0x49F},
		{"takedamage", 0x4A0},
		{"target", 0x4A1},
		{"target_script_trigger", 0x4A2},
		{"targetname", 0x4A3},
		{"team", 0x4A4},
		{"team3", 0x4A5},
		{"teambalanced", 0x4A6},
		{"teammode_axisallies", 0x4A7},
		{"teammode_ffa", 0x4A8},
		{"teammovewaittime", 0x4A9},
		{"their_score", 0x4AA},
		{"thermal", 0x4AB},
		{"thermalbodymaterial", 0x4AC},
		{"third_person", 0x4AD},
		{"threatbias", 0x4AE},
		{"threatbiasgroup", 0x4AF},
		{"threatsightdelayenabled", 0x4B0},
		{"threatsightdelayfalloff", 0x4B1},
		{"threshold", 0x4B2},
		{"throwingknife", 0x4B3},
		{"time", 0x4B4},
		{"timeplayedtotal", 0x4B5},
		{"titlenew", 0x4B6},
		{"titleunlocked", 0x4B7},
		{"top", 0x4B8},
		{"toparc", 0x4B9},
		{"totalxp", 0x4BA},
		{"touch", 0x4BB},
		{"touching_platform", 0x4BC},
		{"traverse_complete", 0x4BD},
		{"traverse_soon", 0x4BE},
		{"traversecost", 0x4BF},
		{"traversesoonnotifydist", 0x4C0},
		{"trend", 0x4C1},
		{"trigger", 0x4C2},
		{"trigger_damage", 0x4C3},
		{"trigger_use", 0x4C4},
		{"trigger_use_touch", 0x4C5},
		{"truck_cam", 0x4C6},
		{"turnrate", 0x4C7},
		{"turret_deactivate", 0x4C8},
		{"turret_fire", 0x4C9},
		{"turret_no_vis", 0x4CA},
		{"turret_not_on_target", 0x4CB},
		{"turret_on_target", 0x4CC},
		{"turret_on_vistarget", 0x4CD},
		{"turret_pitch_clamped", 0x4CE},
		{"turret_rotate_stopped", 0x4CF},
		{"turret_yaw_clamped", 0x4D0},
		{"turretinvulnerability", 0x4D1},
		{"turretownerchange", 0x4D2},
		{"turretstatechange", 0x4D3},
		{"type", 0x4D4},
		{"ucdidhigh", 0x4D5},
		{"ucdidlow", 0x4D6},
		{"unlockedcamo", 0x4D7},
		{"unlockedreticles", 0x4D8},
		{"unlockpoints", 0x4D9},
		{"unresolved_collision", 0x4DA},
		{"up", 0x4DB},
		{"upaimlimit", 0x4DC},
		{"upgradechallengeprogress", 0x4DD},
		{"upgradechallengestage", 0x4DE},
		{"upgradepoints", 0x4DF},
		{"upgradepurchased", 0x4E0},
		{"useable", 0x4E1},
		{"usechokepoints", 0x4E2},
		{"usecombatscriptatcover", 0x4E3},
		{"usedemblemslots", 0x4E4},
		{"useorcaavoidance", 0x4E5},
		{"usepathsmoothingvalues", 0x4E6},
		{"veh_boatbounce", 0x4E7},
		{"veh_boost_activated", 0x4E8},
		{"veh_boost_deactivated", 0x4E9},
		{"veh_brake", 0x4EA},
		{"veh_collision", 0x4EB},
		{"veh_contact", 0x4EC},
		{"veh_jolt", 0x4ED},
		{"veh_landed", 0x4EE},
		{"veh_leftground", 0x4EF},
		{"veh_pathdir", 0x4F0},
		{"veh_pathspeed", 0x4F1},
		{"veh_pathtype", 0x4F2},
		{"veh_predictedcollision", 0x4F3},
		{"veh_speed", 0x4F4},
		{"veh_throttle", 0x4F5},
		{"veh_topspeed", 0x4F6},
		{"veh_transmission", 0x4F7},
		{"vehicle_dismount", 0x4F8},
		{"vehicle_mount", 0x4F9},
		{"vehicletype", 0x4FA},
		{"velocity", 0x4FB},
		{"vertalign", 0x4FC},
		{"visionsetmissilecam", 0x4FD},
		{"visionsetmissilecamduration", 0x4FE},
		{"visionsetnaked", 0x4FF},
		{"visionsetnakedduration", 0x500},
		{"visionsetnight", 0x501},
		{"visionsetnightduration", 0x502},
		{"visionsetpain", 0x503},
		{"visionsetpainduration", 0x504},
		{"visionsetthermal", 0x505},
		{"visionsetthermalduration", 0x506},
		{"vote", 0x507},
		{"wait", 0x508},
		{"walk", 0x509},
		{"walkdist", 0x50A},
		{"walkdistfacingmotion", 0x50B},
		{"wastacticalinsertion", 0x50C},
		{"waypoint_reached", 0x50D},
		{"weapon", 0x50E},
		{"weapon_change", 0x50F},
		{"weapon_dropped", 0x510},
		{"weapon_fired", 0x511},
		{"weapon_model_change", 0x512},
		{"weapon_switch_invalid", 0x513},
		{"weapon_switch_started", 0x514},
		{"weapon_taken", 0x515},
		{"weaponchange", 0x516},
		{"weaponclassrestricted", 0x517},
		{"weaponinfo", 0x518},
		{"weaponrank", 0x519},
		{"weaponrestricted", 0x51A},
		{"weaponsetups", 0x51B},
		{"weaponstats", 0x51C},
		{"weeklychallengeid", 0x51D},
		{"weight", 0x51E},
		{"width", 0x51F},
		{"wildcard1", 0x520},
		{"wildcard2", 0x521},
		{"wildcard3", 0x522},
		{"wildcardslots", 0x523},
		{"win_streak", 0x524},
		{"winlossratio", 0x525},
		{"wins", 0x526},
		{"won_match", 0x527},
		{"world", 0x528},
		{"worldmodel", 0x529},
		{"worldspawn", 0x52A},
		{"x", 0x52B},
		{"xb3", 0x52C},
		{"xenon", 0x52D},
		{"xp", 0x52E},
		{"xp_multiplier", 0x52F},
		{"xpmaxmultipliertimeplayed", 0x530},
		{"xpmultiplier", 0x531},
		{"xuid", 0x532},
		{"y", 0x533},
		{"yawconvergencetime", 0x534},
		{"your_score", 0x535},
		{"z", 0x536},
		{"zonly_physics", 0x537},
		{"codescripts/delete", 0x53D},
		{"codescripts/struct", 0x53E},
		{"codescripts/message", 0x53F},
		{"maps/mp/gametypes/_callbacksetup", 0x540},
	};
}
