#include <std_include.hpp>

// This file has been generated.
// Do not touch!

// https://github.com/xensik/gsc-tool/blob/dev/src/h1/xsk/resolver.cpp :)

namespace scripting
{
	std::unordered_map<std::string, unsigned> function_map =
	{
		{"precacheturret", 0x001}, // SP 0x2D08B0 MP 0x43A900
		{"getweaponarray", 0x002}, // SP 0x2C3860 MP 0x444720
		{"createprintchannel", 0x003}, // SP 0x2CDC80 MP 0x437B40
		{"updategamerprofileall", 0x004}, // SP 0x2DF5D0 MP 0x000000
		{"clearlocalizedstrings", 0x005}, // SP 0x2D8990 MP 0x000000
		{"setphysicsgravitydir", 0x006}, // SP 0x2DCBF0 MP 0x000000
		{"gettimescale", 0x007}, // SP 0x2DC8D0 MP 0x000000
		{"settimescale", 0x008}, // SP 0x2DC8F0 MP 0x000000
		{"setslowmotionview", 0x009}, // SP 0x2DC970 MP 0x000000
		{"teleportscene", 0x00A}, // SP 0x2DC910 MP 0x000000
		{"forcesharedammo", 0x00B}, // SP 0x2DC800 MP 0x000000
		{"refreshhudcompass", 0x00C}, // SP 0x2D8C10 MP 0x000000
		{"refreshhudammocounter", 0x00D}, // SP 0x2D8C40 MP 0x000000
		{"notifyoncommand", 0x00E}, // SP 0x2D8C70 MP 0x000000
		{"setprintchannel", 0x00F}, // SP 0x2CDCE0 MP 0x437C20
		{"print", 0x010}, // SP 0x2CDD70 MP 0x437C40
		{"println", 0x011}, // SP 0x2CDFB0 MP 0x438000
		{"print3d", 0x012}, // SP 0x2CEE00 MP 0x438CE0
		{"line", 0x013}, // SP 0x2CEE30 MP 0x438CF0
		{"box", 0x014}, // SP 0x2CEE80 MP 0x438D00
		{"orientedbox", 0x015}, // SP 0x2CF040 MP 0x4390B0
		{"sphere", 0x016}, // SP 0x2CF060 MP 0x439160
		{"cylinder", 0x017}, // SP 0x2CF0D0 MP 0x4392E0
		{"spawnturret", 0x018}, // SP 0x2D0360 MP 0x43A4B0
		{"canspawnturret", 0x019}, // SP 0x2D0890 MP 0x43A7E0
		{"assert", 0x01A}, // SP 0x2CF150 MP 0x439350
		{"pausecinematicingame", 0x01B}, // SP 0x2DF520 MP 0x000000
		{"drawcompassfriendlies", 0x01C}, // SP 0x2DF550 MP 0x000000
		{"bulletspread", 0x01D}, // SP 0x2DF790 MP 0x000000
		{"bullettracer", 0x01E}, // SP 0x2D6B20 MP 0x000000
		{"badplace_delete", 0x01F}, // SP 0x2C6BD0 MP 0x4495F0
		{"badplace_cylinder", 0x020}, // SP 0x2C73D0 MP 0x449DE0
		{"badplace_arc", 0x021}, // SP 0x2C79E0 MP 0x44A490
		{"badplace_brush", 0x022}, // SP 0x2C8610 MP 0x44AC30
		{"clearallcorpses", 0x023}, // SP 0x2D7EB0 MP 0x000000
		{"setturretnode", 0x024}, // SP 0x2FC0E0 MP 0x000000
		{"unsetturretnode", 0x025}, // SP 0x2FC180 MP 0x000000
		{"setnodepriority", 0x026}, // SP 0x2FC000 MP 0x000000
		{"_func_027", 0x027}, // SP 0x2FBEC0 MP 0x000000
		{"setdebugorigin", 0x02A}, // SP 0x2D8280 MP 0x000000
		{"setdebugangles", 0x02B}, // SP 0x2D8290 MP 0x000000
		{"updategamerprofile", 0x02C}, // SP 0x2D8450 MP 0x000000
		{"assertex", 0x02D}, // SP 0x2CF1F0 MP 0x439430
		{"assertmsg", 0x02E}, // SP 0x2CF340 MP 0x439540
		{"isdefined", 0x02F}, // SP 0x2CF920 MP 0x439B50
		{"isvalidmissile", 0x030}, // SP 0x2D00C0 MP 0x43A2A0
		{"isstring", 0x031}, // SP 0x2D0110 MP 0x43A390
		{"setomnvar", 0x032}, // SP 0x2D19D0 MP 0x43BE30
		{"getomnvar", 0x033}, // SP 0x2D1C90 MP 0x43C170
		{"setdvar", 0x034}, // SP 0x2BA9D0 MP 0x43C980
		{"setdynamicdvar", 0x035}, // SP 0x000000 MP 0x43CBE0
		{"setdvarifuninitialized", 0x036}, // SP 0x2BB0E0 MP 0x43D3A0
		{"setdevdvar", 0x037}, // SP 0x2BAE10 MP 0x43CF60
		{"setdevdvarifuninitialized", 0x038}, // SP 0x2BB480 MP 0x43D9D0
		{"getdvar", 0x039}, // SP 0x2BB7B0 MP 0x43DAC0
		{"getdvarint", 0x03A}, // SP 0x2BBB90 MP 0x43E340
		{"getdvarfloat", 0x03B}, // SP 0x2BC0C0 MP 0x43E580
		{"getdvarvector", 0x03C}, // SP 0x2BCAE0 MP 0x43E940
		{"gettime", 0x03D}, // SP 0x2BD990 MP 0x43F350
		{"gettimeutc", 0x03E}, // SP 0x2BD9A0 MP 0x43F390
		{"getradiometricunit", 0x03F}, // SP 0x2BD9C0 MP 0x43F590
		{"getentbynum", 0x040}, // SP 0x2BDA80 MP 0x43F5D0
		{"getscreenwidth", 0x041}, // SP 0x2BDAC0 MP 0x000000
		{"getscreenheight", 0x042}, // SP 0x2BDAD0 MP 0x000000
		{"getweaponmodel", 0x043}, // SP 0x2BDBF0 MP 0x43F6C0
		{"getculldist", 0x044}, // SP 0x2D6DE0 MP 0x000000
		{"sethalfresparticles", 0x045}, // SP 0x2D6F30 MP 0x000000
		{"getmapsunlight", 0x046}, // SP 0x2D6F70 MP 0x000000
		{"setsunlight", 0x047}, // SP 0x2D6FD0 MP 0x0B8780
		{"resetsunlight", 0x048}, // SP 0x2D70A0 MP 0x0B8810
		{"getmapsundirection", 0x049}, // SP 0x2D71D0 MP 0x000000
		{"getmapsunangles", 0x04A}, // SP 0x2D7200 MP 0x000000
		{"setsundirection", 0x04B}, // SP 0x2D7330 MP 0x000000
		{"lerpsundirection", 0x04C}, // SP 0x2D74A0 MP 0x000000
		{"lerpsunangles", 0x04D}, // SP 0x2D77E0 MP 0x000000
		{"resetsundirection", 0x04E}, // SP 0x2D79D0 MP 0x000000
		{"enableforcedsunshadows", 0x04F}, // SP 0x2D7A00 MP 0x000000
		{"enableforcednosunshadows", 0x050}, // SP 0x2D7A20 MP 0x000000
		{"disableforcedsunshadows", 0x051}, // SP 0x2D7A40 MP 0x000000
		{"enableouterspacemodellighting", 0x052}, // SP 0x2D7A60 MP 0x000000
		{"disableouterspacemodellighting", 0x053}, // SP 0x2D7B60 MP 0x000000
		{"remapstage", 0x054}, // SP 0x2D8670 MP 0x000000
		{"changelevel", 0x055}, // SP 0x2DED50 MP 0x000000
		{"missionsuccess", 0x056}, // SP 0x2DEDF0 MP 0x000000
		{"missionfailed", 0x057}, // SP 0x2DEED0 MP 0x000000
		{"cinematic", 0x058}, // SP 0x2DEEF0 MP 0x000000
		{"cinematicingame", 0x059}, // SP 0x2DEFD0 MP 0x000000
		{"cinematicingamesync", 0x05A}, // SP 0x2DF070 MP 0x000000
		{"cinematicingameloop", 0x05B}, // SP 0x2DF150 MP 0x000000
		{"cinematicingameloopresident", 0x05C}, // SP 0x2DF2F0 MP 0x000000
		{"iscinematicplaying", 0x05D}, // SP 0x2DF370 MP 0x000000
		{"stopcinematicingame", 0x05E}, // SP 0x2DF500 MP 0x000000
		{"getweapondisplayname", 0x05F}, // SP 0x2BE240 MP 0x43FE40
		{"getweaponbasename", 0x060}, // SP 0x2BE4D0 MP 0x440150
		{"getweaponattachments", 0x061}, // SP 0x2BE820 MP 0x4407D0
		{"getweaponattachmentdisplaynames", 0x062}, // SP 0x2BEBA0 MP 0x4410F0
		{"getweaponcamoname", 0x063}, // SP 0x2BFBC0 MP 0x4421F0
		{"getweaponreticlename", 0x064}, // SP 0x2C0030 MP 0x442680
		{"getanimlength", 0x065}, // SP 0x2C2010 MP 0x4446C0
		{"animhasnotetrack", 0x066}, // SP 0x2C2410 MP 0x4447F0
		{"getnotetracktimes", 0x067}, // SP 0x2C2500 MP 0x444A10
		{"spawn", 0x068}, // SP 0x2CFF60 MP 0x439BE0
		{"spawnloopsound", 0x069}, // SP 0x2CFF60 MP 0x439BE0
		{"spawnloopingsound", 0x06A}, // SP 0x4C6570 MP 0x443210
		{"bullettrace", 0x06B}, // SP 0x2BAC20 MP 0x439D30
		{"target_setmaxsize", 0x06C}, // SP 0x2EAEE0 MP 0x000000
		{"target_setcolor", 0x06D}, // SP 0x2EA9F0 MP 0x000000
		{"target_setdelay", 0x06E}, // SP 0x2EAB80 MP 0x000000
		{"getstartorigin", 0x06F}, // SP 0x289CA0 MP 0x0E0750
		{"getstartangles", 0x070}, // SP 0x289B90 MP 0x0E0A10
		{"getcycleoriginoffset", 0x071}, // SP 0x289A40 MP 0x000000
		{"magicgrenade", 0x072}, // SP 0x2DF3E0 MP 0x000000
		{"magicgrenademanual", 0x073}, // SP 0x2CF9E0 MP 0x43D1B0
		{"setblur", 0x074}, // SP 0x2DB190 MP 0x000000
		{"musicplay", 0x075}, // SP 0x2DB2B0 MP 0x000000
		{"musicstop", 0x076}, // SP 0x2DB4E0 MP 0x000000
		{"soundfade", 0x077}, // SP 0x2DB6E0 MP 0x000000
		{"addsoundsubmix", 0x078}, // SP 0x2BEB80 MP 0x43FB10
		{"clearsoundsubmix", 0x079}, // SP 0x2BED00 MP 0x43FB60
		{"clearallsubmixes", 0x07A}, // SP 0x2BED70 MP 0x43FDC0
		{"blendsoundsubmix", 0x07B}, // SP 0x2BEDD0 MP 0x43FF20
		{"makesoundsubmixsticky", 0x07C}, // SP 0x2BEE10 MP 0x43FF40
		{"makesoundsubmixunsticky", 0x07D}, // SP 0x2BEE90 MP 0x43FFC0
		{"soundsettimescalefactor", 0x07E}, // SP 0x2DB940 MP 0x000000
		{"soundresettimescale", 0x07F}, // SP 0x2DC960 MP 0x000000
		{"levelsoundfade", 0x080}, // SP 0x2DB7A0 MP 0x000000
		{"precachenightvisioncodeassets", 0x081}, // SP 0x2DECA0 MP 0x000000
		{"precachedigitaldistortcodeassets", 0x083}, // SP 0x2DECC0 MP 0x000000
		{"precacheminimapsentrycodeassets", 0x084}, // SP 0x2DED30 MP 0x000000
		{"savegame", 0x085}, // SP 0x2DCA60 MP 0x000000
		{"issavesuccessful", 0x086}, // SP 0x2DD7A0 MP 0x000000
		{"issaverecentlyloaded", 0x087}, // SP 0x2DD7C0 MP 0x000000
		{"savegamenocommit", 0x088}, // SP 0x2DCED0 MP 0x000000
		{"commitsave", 0x089}, // SP 0x2DD7E0 MP 0x000000
		{"commitwouldbevalid", 0x08A}, // SP 0x2DD880 MP 0x000000
		{"getfxvisibility", 0x08B}, // SP 0x2DF570 MP 0x000000
		{"setculldist", 0x08C}, // SP 0x2D6D90 MP 0x000000
		{"bullettracepassed", 0x08D}, // SP 0x2BB600 MP 0x43A830
		{"sighttracepassed", 0x08E}, // SP 0x2BB860 MP 0x43AD40
		{"physicstrace", 0x08F}, // SP 0x2BBF70 MP 0x43B500
		{"playerphysicstrace", 0x090}, // SP 0x2BC200 MP 0x43B7B0
		{"getgroundposition", 0x091}, // SP 0x2BE560 MP 0x43D4B0
		{"getmovedelta", 0x092}, // SP 0x2C2780 MP 0x442560
		{"getangledelta", 0x093}, // SP 0x2C2F20 MP 0x442B50
		{"getnorthyaw", 0x094}, // SP 0x2C3E10 MP 0x443980
		{"getcommandfromkey", 0x095}, // SP 0x2DB5B0 MP 0x000000
		{"getsticksconfig", 0x096}, // SP 0x2DB760 MP 0x000000
		{"weaponfightdist", 0x097}, // SP 0x2DD930 MP 0x000000
		{"weaponmaxdist", 0x098}, // SP 0x2DD980 MP 0x000000
		{"isturretactive", 0x099}, // SP 0x2DD8F0 MP 0x000000
		{"getstarttime", 0x09A}, // SP 0x2DAA40 MP 0x000000
		{"target_alloc", 0x09B}, // SP 0x2EA180 MP 0x000000
		{"target_flush", 0x09C}, // SP 0x2EA5B0 MP 0x000000
		{"target_set", 0x09D}, // SP 0x2EA890 MP 0x000000
		{"target_remove", 0x09E}, // SP 0x2EA850 MP 0x000000
		{"target_setshader", 0x09F}, // SP 0x2EB560 MP 0x000000
		{"target_setoffscreenshader", 0x0A0}, // SP 0x2EB190 MP 0x000000
		{"target_isinrect", 0x0A1}, // SP 0x2EA730 MP 0x000000
		{"target_isincircle", 0x0A2}, // SP 0x2EA6C0 MP 0x000000
		{"target_startreticlelockon", 0x0A3}, // SP 0x2EB6B0 MP 0x000000
		{"target_clearreticlelockon", 0x0A4}, // SP 0x2EA1A0 MP 0x000000
		{"target_getarray", 0x0A5}, // SP 0x2EA640 MP 0x000000
		{"target_istarget", 0x0A6}, // SP 0x2EA7D0 MP 0x000000
		{"target_setattackmode", 0x0A7}, // SP 0x2EA8B0 MP 0x000000
		{"target_setjavelinonly", 0x0A8}, // SP 0x2EAD90 MP 0x000000
		{"target_hidefromplayer", 0x0A9}, // SP 0x2EA6B0 MP 0x000000
		{"target_showtoplayer", 0x0AA}, // SP 0x2EB6A0 MP 0x000000
		{"target_setscaledrendermode", 0x0AB}, // SP 0x2EB410 MP 0x000000
		{"target_drawcornersonly", 0x0AC}, // SP 0x2EA1D0 MP 0x000000
		{"target_drawsquare", 0x0AD}, // SP 0x2EA440 MP 0x000000
		{"target_drawsingle", 0x0AE}, // SP 0x2EA320 MP 0x000000
		{"target_setminsize", 0x0AF}, // SP 0x2EB020 MP 0x000000
		{"setnorthyaw", 0x0B0}, // SP 0x2C3EB0 MP 0x443D70
		{"setslowmotion", 0x0B1}, // SP 0x2BC560 MP 0x43D2E0
		{"randomint", 0x0B2}, // SP 0x2BE910 MP 0x43DB30
		{"randomfloat", 0x0B3}, // SP 0x2BEA90 MP 0x43E2C0
		{"randomintrange", 0x0B4}, // SP 0x2BEB30 MP 0x43E2F0
		{"randomfloatrange", 0x0B5}, // SP 0x2BED20 MP 0x43E470
		{"sin", 0x0B6}, // SP 0x2BEE50 MP 0x43E5F0
		{"cos", 0x0B7}, // SP 0x2BEF00 MP 0x43E6B0
		{"tan", 0x0B8}, // SP 0x2BF080 MP 0x43E6E0
		{"asin", 0x0B9}, // SP 0x2BF1A0 MP 0x43E7B0
		{"acos", 0x0BA}, // SP 0x2BF750 MP 0x43E8F0
		{"atan", 0x0BB}, // SP 0x2BF9A0 MP 0x43EBA0
		{"int", 0x0BC}, // SP 0x2BFDD0 MP 0x43ECF0
		{"float", 0x0BD}, // SP 0x2BFFC0 MP 0x43F160
		{"abs", 0x0BE}, // SP 0x2C04C0 MP 0x43F320
		{"min", 0x0BF}, // SP 0x2C0500 MP 0x43F360
		{"objective_additionalcurrent", 0x0C0}, // SP 0x2E6840 MP 0x000000
		{"objective_ring", 0x0C1}, // SP 0x2E6980 MP 0x000000
		{"objective_setpointertextoverride", 0x0C2}, // SP 0x2E6A40 MP 0x000000
		{"getnode", 0x0C3}, // SP 0x2FB900 MP 0x3FC1F0
		{"getnodearray", 0x0C4}, // SP 0x2FBA10 MP 0x3FC300
		{"getallnodes", 0x0C5}, // SP 0x2FB6E0 MP 0x3FBFD0
		{"getnodesinradius", 0x0C6}, // SP 0x2FBB20 MP 0x3FC4B0
		{"getnodesinradiussorted", 0x0C7}, // SP 0x2FBB30 MP 0x3FC4C0
		{"getclosestnodeinsight", 0x0C8}, // SP 0x2FB730 MP 0x3FC020
		{"getreflectionlocs", 0x0C9}, // SP 0x2D89B0 MP 0x000000
		{"getreflectionreferencelocs", 0x0CA}, // SP 0x2D8C00 MP 0x000000
		{"getvehicletracksegment", 0x0CB}, // SP 0x4EB1D0 MP 0x000000
		{"getvehicletracksegmentarray", 0x0CC}, // SP 0x4EB2B0 MP 0x000000
		{"getallvehicletracksegments", 0x0CD}, // SP 0x4EB180 MP 0x000000
		{"isarray", 0x0CE}, // SP 0x2CA9D0 MP 0x436610
		{"isai", 0x0CF}, // SP 0x2D93A0 MP 0x0B8480
		{"getindexforluincstring", 0x0D0}, // SP 0x2D11B0 MP 0x43AEA0
		{"issentient", 0x0D1}, // SP 0x2D0130 MP 0x43A460
		{"isgodmode", 0x0D2}, // SP 0x2D94B0 MP 0x000000
		{"getdebugdvar", 0x0D3}, // SP 0x2DA640 MP 0x000000
		{"getdebugdvarint", 0x0D4}, // SP 0x2DA6D0 MP 0x000000
		{"getdebugdvarfloat", 0x0D5}, // SP 0x2DA700 MP 0x000000
		{"setsaveddvar", 0x0D6}, // SP 0x2DA730 MP 0x000000
		{"getfreeaicount", 0x0D7}, // SP 0x2DAA50 MP 0x000000
		{"getaicount", 0x0D8}, // SP 0x2DAA60 MP 0x000000
		{"getaiarray", 0x0D9}, // SP 0x2DACC0 MP 0x000000
		{"getaispeciesarray", 0x0DA}, // SP 0x2DAD90 MP 0x000000
		{"getspawnerarray", 0x0DB}, // SP 0x2DAFE0 MP 0x000000
		{"getcorpsearray", 0x0DC}, // SP 0x2DAF30 MP 0x000000
		{"getspawnerteamarray", 0x0DD}, // SP 0x2DB0F0 MP 0x000000
		{"getweaponclipmodel", 0x0DE}, // SP 0x2DB230 MP 0x000000
		{"getbrushmodelcenter", 0x0DF}, // SP 0x2DB440 MP 0x000000
		{"getkeybinding", 0x0E0}, // SP 0x2DB450 MP 0x000000
		{"max", 0x0E1}, // SP 0x2C05E0 MP 0x43F560
		{"floor", 0x0E2}, // SP 0x2C0650 MP 0x43F5B0
		{"ceil", 0x0E3}, // SP 0x2C07B0 MP 0x43F620
		{"exp", 0x0E4}, // SP 0x2C0A00 MP 0x43F640
		{"exp", 0x0E5}, // SP 0x2C0A20 MP 0x43F670
		{"log", 0x0E6}, // SP 0x2C0A40 MP 0x43F6A0
		{"sqrt", 0x0E7}, // SP 0x2C0A80 MP 0x43F7A0
		{"squared", 0x0E8}, // SP 0x2C0B10 MP 0x43F8D0
		{"clamp", 0x0E9}, // SP 0x2C0D50 MP 0x43F940
		{"angleclamp", 0x0EA}, // SP 0x2C0E40 MP 0x43FA60
		{"angleclamp180", 0x0EB}, // SP 0x2C11C0 MP 0x43FAC0
		{"vectorfromlinetopoint", 0x0EC}, // SP 0x2C3460 MP 0x4415F0
		{"pointonsegmentnearesttopoint", 0x0ED}, // SP 0x2C38D0 MP 0x442060
		{"distance", 0x0EE}, // SP 0x2C4190 MP 0x442A60
		{"distance2d", 0x0EF}, // SP 0x2C43B0 MP 0x442AF0
		{"distancesquared", 0x0F0}, // SP 0x2C46B0 MP 0x442E20
		{"length", 0x0F1}, // SP 0x2C4960 MP 0x442EC0
		{"length2d", 0x0F2}, // SP 0x2C4A70 MP 0x442FE0
		{"lengthsquared", 0x0F3}, // SP 0x2C4BC0 MP 0x443090
		{"length2dsquared", 0x0F4}, // SP 0x2C4D10 MP 0x443280
		{"closer", 0x0F5}, // SP 0x2C4E10 MP 0x443650
		{"vectordot", 0x0F6}, // SP 0x2C4FF0 MP 0x4437D0
		{"vectorcross", 0x0F7}, // SP 0x2C5260 MP 0x4438D0
		{"axistoangles", 0x0F8}, // SP 0x2C5F50 MP 0x444550
		{"visionsetthermal", 0x0F9}, // SP 0x2D6D50 MP 0x0B7F50
		{"visionsetpain", 0x0FA}, // SP 0x2D6D60 MP 0x0B8030
		{"endlobby", 0x0FB}, // SP 0x000000 MP 0x0D0860
		{"setac130ambience", 0x0FC}, // SP 0x000000 MP 0x0DFE30
		{"getmapcustom", 0x0FD}, // SP 0x000000 MP 0x0BB330
		{"spawnsighttrace", 0x0FE}, // SP 0x000000 MP 0x0E2320
		{"incrementcounter", 0x0FF}, // SP 0x2D8EF0 MP 0x0E0C10
		{"getcountertotal", 0x100}, // SP 0x2D9050 MP 0x0E1740
		{"getlevelticks", 0x101}, // SP 0x2BDA70 MP 0x000000
		{"perlinnoise2d", 0x102}, // SP 0x2C1250 MP 0x000000
		{"calcrockingangles", 0x103}, // SP 0x2C1360 MP 0x000000
		{"reconevent", 0x104}, // SP 0x5D2CD0 MP 0x4F8C00
		{"reconspatialevent", 0x105}, // SP 0x5D2CE0 MP 0x4F8C10
		{"setsunflareposition", 0x106}, // SP 0x2C4F70 MP 0x000000
		{"createthreatbiasgroup", 0x107}, // SP 0x307B30 MP 0x08F880
		{"threatbiasgroupexists", 0x108}, // SP 0x307B60 MP 0x08F8B0
		{"getthreatbias", 0x109}, // SP 0x307BA0 MP 0x08F8F0
		{"setthreatbias", 0x10A}, // SP 0x307C00 MP 0x08F950
		{"setthreatbiasagainstall", 0x10B}, // SP 0x307C80 MP 0x08F9D0
		{"setignoremegroup", 0x10C}, // SP 0x307CD0 MP 0x08FA20
		{"isenemyteam", 0x10D}, // SP 0x307E10 MP 0x08FBC0
		{"objective_additionalentity", 0x10E}, // SP 0x2E6290 MP 0x000000
		{"objective_state_nomessage", 0x10F}, // SP 0x2E6590 MP 0x000000
		{"objective_string", 0x110}, // SP 0x2E65A0 MP 0x000000
		{"objective_string_nomessage", 0x111}, // SP 0x2E65B0 MP 0x000000
		{"objective_additionalposition", 0x112}, // SP 0x2E67B0 MP 0x000000
		{"objective_current_nomessage", 0x113}, // SP 0x2E6830 MP 0x000000
		{"vectornormalize", 0x114}, // SP 0x2C5470 MP 0x4439D0
		{"vectortoangles", 0x115}, // SP 0x2C55F0 MP 0x443E10
		{"vectortoyaw", 0x116}, // SP 0x2C5940 MP 0x444100
		{"vectorlerp", 0x117}, // SP 0x2C5AA0 MP 0x4441A0
		{"anglestoup", 0x118}, // SP 0x2C6240 MP 0x444790
		{"anglestoright", 0x119}, // SP 0x2C6400 MP 0x4449C0
		{"anglestoforward", 0x11A}, // SP 0x2C6460 MP 0x444A60
		{"anglesdelta", 0x11B}, // SP 0x2C6590 MP 0x444C70
		{"combineangles", 0x11C}, // SP 0x2C6A00 MP 0x444F00
		{"transformmove", 0x11D}, // SP 0x2C6C10 MP 0x445080
		{"rotatevector", 0x11E}, // SP 0x2C9010 MP 0x446C90
		{"rotatepointaroundvector", 0x11F}, // SP 0x2C91E0 MP 0x446E90
		{"issubstr", 0x120}, // SP 0x2CA530 MP 0x448130
		{"isendstr", 0x121}, // SP 0x2CA5F0 MP 0x4481D0
		{"getsubstr", 0x122}, // SP 0x2CA6F0 MP 0x4482E0
		{"tolower", 0x123}, // SP 0x2CACC0 MP 0x448E30
		{"strtok", 0x124}, // SP 0x2CB0F0 MP 0x4492B0
		{"stricmp", 0x125}, // SP 0x2CB800 MP 0x449FC0
		{"ambientplay", 0x126}, // SP 0x2BBC10 MP 0x43B1A0
		{"getuavstrengthmax", 0x127}, // SP 0x000000 MP 0x0CF770
		{"getuavstrengthlevelneutral", 0x128}, // SP 0x000000 MP 0x0CF850
		{"getuavstrengthlevelshowenemyfastsweep", 0x129}, // SP 0x000000 MP 0x0CF920
		{"getuavstrengthlevelshowenemydirectional", 0x12A}, // SP 0x000000 MP 0x0CFA10
		{"blockteamradar", 0x12B}, // SP 0x000000 MP 0x0CFB10
		{"unblockteamradar", 0x12C}, // SP 0x000000 MP 0x0D00D0
		{"isteamradarblocked", 0x12D}, // SP 0x000000 MP 0x0D0430
		{"getassignedteam", 0x12E}, // SP 0x000000 MP 0x0BAC90
		{"setmatchdata", 0x12F}, // SP 0x000000 MP 0x0C7ED0
		{"getmatchdata", 0x130}, // SP 0x000000 MP 0x0BB7F0
		{"sendmatchdata", 0x131}, // SP 0x000000 MP 0x0BBC30
		{"clearmatchdata", 0x132}, // SP 0x000000 MP 0x0BFF00
		{"setmatchdatadef", 0x133}, // SP 0x000000 MP 0x0C81E0
		{"setmatchclientip", 0x134}, // SP 0x000000 MP 0x0C0310
		{"setmatchdataid", 0x135}, // SP 0x000000 MP 0x0BF830
		{"setclientmatchdata", 0x136}, // SP 0x000000 MP 0x0C0000
		{"getclientmatchdata", 0x137}, // SP 0x000000 MP 0x0BB4A0
		{"setclientmatchdatadef", 0x138}, // SP 0x000000 MP 0x0C8730
		{"sendclientmatchdata", 0x139}, // SP 0x000000 MP 0x0C9F50
		{"getbuildversion", 0x13A}, // SP 0x000000 MP 0x0CAC50
		{"getbuildnumber", 0x13B}, // SP 0x000000 MP 0x0CADA0
		{"getsystemtime", 0x13C}, // SP 0x000000 MP 0x0CB000
		{"getmatchrulesdata", 0x13D}, // SP 0x000000 MP 0x0C8EB0
		{"isusingmatchrulesdata", 0x13E}, // SP 0x000000 MP 0x0C9170
		{"kick", 0x13F}, // SP 0x000000 MP 0x0E4200
		{"issplitscreen", 0x140}, // SP 0x2D8440 MP 0x0B9EB0
		{"setmapcenter", 0x141}, // SP 0x000000 MP 0x0CD940
		{"setgameendtime", 0x142}, // SP 0x000000 MP 0x0CDB60
		{"visionsetthermal", 0x143}, // SP 0x2D6BE0 MP 0x0B7B80
		{"visionsetpain", 0x144}, // SP 0x2D6BF0 MP 0x0B7C50
		{"visionsetnight", 0x145}, // SP 0x2D6C00 MP 0x0B7D50
		{"ambientstop", 0x146}, // SP 0x2BDF30 MP 0x43D630
		{"precachemodel", 0x147}, // SP 0x2CC0B0 MP 0x44B080
		{"precacheshellshock", 0x148}, // SP 0x2CC460 MP 0x44B4A0
		{"precacheitem", 0x149}, // SP 0x2BAF20 MP 0x4399B0
		{"precacheshader", 0x14A}, // SP 0x2BB0A0 MP 0x439BA0
		{"precachestring", 0x14B}, // SP 0x2BB590 MP 0x43A800
		{"precachemenu", 0x14C}, // SP 0x2BB740 MP 0x43AAD0
		{"precacherumble", 0x14D}, // SP 0x2BB830 MP 0x43AD10
		{"precachelocationselector", 0x14E}, // SP 0x2CC2B0 MP 0x44B170
		{"precacheleaderboards", 0x14F}, // SP 0x2BB3D0 MP 0x43A270
		{"loadfx", 0x150}, // SP 0x2C45B0 MP 0x4444A0
		{"playfx", 0x151}, // SP 0x2C57B0 MP 0x4451F0
		{"playfxontag", 0x152}, // SP 0x2C6D30 MP 0x446B50
		{"stopfxontag", 0x153}, // SP 0x2C8EF0 MP 0x448660
		{"killfxontag", 0x154}, // SP 0x2C9500 MP 0x449C90
		{"playloopedfx", 0x155}, // SP 0x4F7EB0 MP 0x1CE5A0
		{"spawnfx", 0x156}, // SP 0x4F8230 MP 0x1CEE00
		{"triggerfx", 0x157}, // SP 0x4F84E0 MP 0x1CF3E0
		{"playfxontagforclients", 0x158}, // SP 0x2C7780 MP 0x4472A0
		{"setfxkillondelete", 0x159}, // SP 0x4F81C0 MP 0x1CED00
		{"playimpactheadfatalfx", 0x15A}, // SP 0x2C8830 MP 0x447F80
		{"setwinningteam", 0x15B}, // SP 0x000000 MP 0x0B8B70
		{"announcement", 0x15C}, // SP 0x000000 MP 0x0B8D70
		{"clientannouncement", 0x15D}, // SP 0x000000 MP 0x0B8E00
		{"setteammode", 0x15E}, // SP 0x000000 MP 0x0B9070
		{"getteamscore", 0x15F}, // SP 0x000000 MP 0x0B92D0
		{"setteamscore", 0x160}, // SP 0x000000 MP 0x0B9490
		{"setclientnamemode", 0x161}, // SP 0x000000 MP 0x0B9600
		{"updateclientnames", 0x162}, // SP 0x000000 MP 0x0B9870
		{"getteamplayersalive", 0x163}, // SP 0x000000 MP 0x0B9A60
		{"logprint", 0x164}, // SP 0x000000 MP 0x0DB730
		{"worldentnumber", 0x165}, // SP 0x000000 MP 0x0DB830
		{"obituary", 0x166}, // SP 0x000000 MP 0x0DB9F0
		{"positionwouldtelefrag", 0x167}, // SP 0x000000 MP 0x0DCF60
		{"canspawn", 0x168}, // SP 0x000000 MP 0x0DE2A0
		{"getstarttime", 0x169}, // SP 0x000000 MP 0x0E10B0
		{"precacheheadicon", 0x16A}, // SP 0x2BB450 MP 0x43A5A0
		{"precacheminimapicon", 0x16B}, // SP 0x000000 MP 0x0E4CD0
		{"precachempanim", 0x16C}, // SP 0x000000 MP 0x0E4D80
		{"map_restart", 0x16D}, // SP 0x000000 MP 0x0E20A0
		{"exitlevel", 0x16E}, // SP 0x000000 MP 0x0E2670
		{"addtestclient", 0x16F}, // SP 0x000000 MP 0x0E28C0
		{"addagent", 0x170}, // SP 0x000000 MP 0x0E3590
		{"setarchive", 0x171}, // SP 0x000000 MP 0x0B9E80
		{"allclientsprint", 0x172}, // SP 0x000000 MP 0x0E3840
		{"clientprint", 0x173}, // SP 0x000000 MP 0x0E4950
		{"mapexists", 0x174}, // SP 0x000000 MP 0x0E3BE0
		{"isvalidgametype", 0x175}, // SP 0x000000 MP 0x0E3E10
		{"matchend", 0x176}, // SP 0x000000 MP 0x0B9F80
		{"setplayerteamrank", 0x177}, // SP 0x000000 MP 0x0BA120
		{"endparty", 0x178}, // SP 0x000000 MP 0x0BA790
		{"setteamradar", 0x179}, // SP 0x000000 MP 0x0CE3D0
		{"getteamradar", 0x17A}, // SP 0x000000 MP 0x0CE5C0
		{"setteamradarstrength", 0x17B}, // SP 0x000000 MP 0x0CEBE0
		{"getteamradarstrength", 0x17C}, // SP 0x000000 MP 0x0CF390
		{"getuavstrengthmin", 0x17D}, // SP 0x000000 MP 0x0CF650
		{"physicsexplosionsphere", 0x17E}, // SP 0x2C9C20 MP 0x44B1B0
		{"physicsexplosioncylinder", 0x17F}, // SP 0x2CAFB0 MP 0x44C5B0
		{"physicsjolt", 0x180}, // SP 0x2CA890 MP 0x44BC90
		{"physicsjitter", 0x181}, // SP 0x2CAA80 MP 0x44C060
		{"setexpfog", 0x182}, // SP 0x2CD880 MP 0x436D30
		{"setexpfogext", 0x183}, // SP 0x2CD9C0 MP 0x436DE0
		{"setexpfogdvarsonly", 0x184}, // SP 0x2CDEF0 MP 0x437400
		{"setexpfogextdvarsonly", 0x185}, // SP 0x2CDFC0 MP 0x437540
		{"setatmosfog", 0x186}, // SP 0x2CE170 MP 0x4375C0
		{"setatmosfogdvarsonly", 0x187}, // SP 0x2CE6D0 MP 0x4378A0
		{"isexplosivedamagemod", 0x188}, // SP 0x2BEFE0 MP 0x43EC90
		{"radiusdamage", 0x189}, // SP 0x2C04F0 MP 0x43FA50
		{"setplayerignoreradiusdamage", 0x18A}, // SP 0x2C1340 MP 0x440970
		{"glassradiusdamage", 0x18B}, // SP 0x2C0680 MP 0x43FB80
		{"earthquake", 0x18C}, // SP 0x2CE580 MP 0x438010
		{"getnumparts", 0x18D}, // SP 0x2CE8F0 MP 0x437A20
		{"objective_onentity", 0x18E}, // SP 0x2E6450 MP 0x457970
		{"objective_onentitywithrotation", 0x18F}, // SP 0x000000 MP 0x4579D0
		{"objective_team", 0x190}, // SP 0x000000 MP 0x457B10
		{"objective_player", 0x191}, // SP 0x000000 MP 0x457BB0
		{"objective_playerteam", 0x192}, // SP 0x000000 MP 0x457C10
		{"objective_playerenemyteam", 0x193}, // SP 0x000000 MP 0x457CB0
		{"objective_playermask_hidefromall", 0x194}, // SP 0x000000 MP 0x457DB0
		{"objective_playermask_hidefrom", 0x195}, // SP 0x000000 MP 0x457DF0
		{"objective_playermask_showtoall", 0x196}, // SP 0x000000 MP 0x457E60
		{"objective_playermask_showto", 0x197}, // SP 0x000000 MP 0x457EA0
		{"iprintln", 0x198}, // SP 0x2D8FD0 MP 0x0B8450
		{"iprintlnbold", 0x199}, // SP 0x2D90A0 MP 0x0B8590
		{"logstring", 0x19A}, // SP 0x2DB790 MP 0x0B9480
		{"getent", 0x19B}, // SP 0x2E84B0 MP 0x458EB0
		{"getentarray", 0x19C}, // SP 0x2E8520 MP 0x458F80
		{"getspawnarray", 0x19D}, // SP 0x000000 MP 0x459550
		{"spawnplane", 0x19E}, // SP 0x000000 MP 0x0B89A0
		{"spawnstruct", 0x19F}, // SP 0x3C7BA0 MP 0x50ECD0
		{"spawnhelicopter", 0x1A0}, // SP 0x000000 MP 0x0B9130
		{"isalive", 0x1A1}, // SP 0x2D9120 MP 0x0B85C0
		{"isspawner", 0x1A2}, // SP 0x2D9270 MP 0x0B8730
		{"missile_createattractorent", 0x1A3}, // SP 0x2B03A0 MP 0x42CE40
		{"missile_createattractororigin", 0x1A4}, // SP 0x2B03B0 MP 0x42CE50
		{"missile_createrepulsorent", 0x1A5}, // SP 0x2B0670 MP 0x42D100
		{"missile_createrepulsororigin", 0x1A6}, // SP 0x2B0680 MP 0x42D110
		{"missile_deleteattractor", 0x1A7}, // SP 0x2B0690 MP 0x42D120
		{"playsoundatpos", 0x1A8}, // SP 0x000000 MP 0x0BAFB0
		{"newhudelem", 0x1A9}, // SP 0x29BEE0 MP 0x41A250
		{"newclienthudelem", 0x1AA}, // SP 0x29BE80 MP 0x41A1F0
		{"newteamhudelem", 0x1AB}, // SP 0x000000 MP 0x41A280
		{"resettimeout", 0x1AC}, // SP 0x3C93F0 MP 0x510450
		{"isplayer", 0x1AD}, // SP 0x2D9300 MP 0x0B8240
		{"isplayernumber", 0x1AE}, // SP 0x000000 MP 0x0B8610
		{"getpartname", 0x1AF}, // SP 0x2CEA40 MP 0x437B50
		{"weaponfiretime", 0x1B0}, // SP 0x2CFEC0 MP 0x436E20
		{"weaponclipsize", 0x1B1}, // SP 0x2CD0B0 MP 0x44C9C0
		{"weaponisauto", 0x1B2}, // SP 0x2CDF20 MP 0x435450
		{"weaponissemiauto", 0x1B3}, // SP 0x2CE1A0 MP 0x435740
		{"weaponisboltaction", 0x1B4}, // SP 0x2CE790 MP 0x435EA0
		{"weaponinheritsperks", 0x1B5}, // SP 0x2CE930 MP 0x436070
		{"weaponburstcount", 0x1B6}, // SP 0x2CE4F0 MP 0x435A20
		{"weapontype", 0x1B7}, // SP 0x2CEBA0 MP 0x4361B0
		{"weaponclass", 0x1B8}, // SP 0x2CED70 MP 0x436390
		{"getnextarraykey", 0x1B9}, // SP 0x2C1210 MP 0x4415B0
		{"sortbydistance", 0x1BA}, // SP 0x2C1920 MP 0x4422A0
		{"tablelookup", 0x1BB}, // SP 0x2D10F0 MP 0x439A10
		{"tablelookupbyrow", 0x1BC}, // SP 0x2BB350 MP 0x43BBD0
		{"tablelookupistring", 0x1BD}, // SP 0x2D1910 MP 0x43A730
		{"tablelookupistringbyrow", 0x1BE}, // SP 0x2BB510 MP 0x43C0F0
		{"tablelookuprownum", 0x1BF}, // SP 0x2D1D40 MP 0x43AC60
		{"tableexists", 0x1C0}, // SP 0x2BABD0 MP 0x43B8C0
		{"getmissileowner", 0x1C1}, // SP 0x2CDC90 MP 0x4377E0
		{"magicbullet", 0x1C2}, // SP 0x2CA1A0 MP 0x44B740
		{"getweaponflashtagname", 0x1C3}, // SP 0x2CAF70 MP 0x44C7D0
		{"averagepoint", 0x1C4}, // SP 0x2BAF60 MP 0x43B910
		{"averagenormal", 0x1C5}, // SP 0x2BB170 MP 0x43BC50
		{"vehicle_getspawnerarray", 0x1C6}, // SP 0x4E9050 MP 0x60E8F0
		{"playrumbleonposition", 0x1C7}, // SP 0x2CCC00 MP 0x44C280
		{"playrumblelooponposition", 0x1C8}, // SP 0x2CCC60 MP 0x44C3F0
		{"stopallrumbles", 0x1C9}, // SP 0x2CCCE0 MP 0x44C450
		{"soundexists", 0x1CA}, // SP 0x2C3400 MP 0x445BD0
		{"openfile", 0x1CB}, // SP 0x2BEB10 MP 0x43F930
		{"closefile", 0x1CC}, // SP 0x2BEB20 MP 0x43FA40
		{"fprintln", 0x1CD}, // SP 0x2BEB90 MP 0x43FB70
		{"fprintfields", 0x1CE}, // SP 0x2BED10 MP 0x43FE30
		{"freadln", 0x1CF}, // SP 0x2BEDC0 MP 0x43FF30
		{"fgetarg", 0x1D0}, // SP 0x2BEDE0 MP 0x43FFB0
		{"setminimap", 0x1D1}, // SP 0x2BFC60 MP 0x440B10
		{"setthermalbodymaterial", 0x1D2}, // SP 0x2C0170 MP 0x441260
		{"getarraykeys", 0x1D3}, // SP 0x2C0D20 MP 0x441430
		{"getfirstarraykey", 0x1D4}, // SP 0x2C0E10 MP 0x441580
		{"getglass", 0x1D5}, // SP 0x2BB400 MP 0x43BEB0
		{"getglassarray", 0x1D6}, // SP 0x2BB6D0 MP 0x43C2C0
		{"getglassorigin", 0x1D7}, // SP 0x2BB9D0 MP 0x43C600
		{"isglassdestroyed", 0x1D8}, // SP 0x2BBB40 MP 0x43CAB0
		{"destroyglass", 0x1D9}, // SP 0x2BBEC0 MP 0x43CB20
		{"deleteglass", 0x1DA}, // SP 0x2BC080 MP 0x43CD10
		{"getentchannelscount", 0x1DB}, // SP 0x2BCBE0 MP 0x43D7D0
		{"getentchannelname", 0x1DC}, // SP 0x2BCC00 MP 0x43DA60
		{"objective_add", 0x1DD}, // SP 0x2E5DB0 MP 0x457650
		{"objective_delete", 0x1DE}, // SP 0x2E6250 MP 0x457780
		{"objective_state", 0x1DF}, // SP 0x2E6580 MP 0x4577F0
		{"objective_icon", 0x1E0}, // SP 0x2E65C0 MP 0x457890
		{"objective_indentlevel", 0x1E1}, // SP 0x2E66C0 MP 0x000000
		{"objective_position", 0x1E2}, // SP 0x2E6760 MP 0x4578D0
		{"objective_current", 0x1E3}, // SP 0x2E6820 MP 0x457A30
		{"weaponinventorytype", 0x1E4}, // SP 0x2CEFB0 MP 0x4364A0
		{"weaponstartammo", 0x1E5}, // SP 0x2CF170 MP 0x436660
		{"weaponmaxammo", 0x1E6}, // SP 0x2CF350 MP 0x436820
		{"weaponaltweaponname", 0x1E7}, // SP 0x2CF850 MP 0x436BD0
		{"isweaponcliponly", 0x1E8}, // SP 0x2D0180 MP 0x4371C0
		{"isweapondetonationtimed", 0x1E9}, // SP 0x2D0460 MP 0x437440
		{"isweaponmanuallydetonatedbyemptythrow", 0x1EA}, // SP 0x2D1040 MP 0x437660
		{"weaponhasthermalscope", 0x1EB}, // SP 0x2D1640 MP 0x437C50
		{"getvehiclenode", 0x1EC}, // SP 0x4DF7A0 MP 0x605190
		{"getvehiclenodearray", 0x1ED}, // SP 0x4DF890 MP 0x605280
		{"getallvehiclenodes", 0x1EE}, // SP 0x4DF730 MP 0x605120
		{"getnumvehicles", 0x1EF}, // SP 0x4E8D80 MP 0x60E620
		{"precachevehicle", 0x1F0}, // SP 0x4E90D0 MP 0x60E9D0
		{"spawnvehicle", 0x1F1}, // SP 0x4E9170 MP 0x60EA70
		{"vehicle_getarray", 0x1F2}, // SP 0x4E8DC0 MP 0x60E660
		{"pow", 0x1F3}, // SP 0x2C0CE0 MP 0x43F8F0
		{"atan2", 0x1F4}, // SP 0x2BFB20 MP 0x43EBD0
		{"botgetmemoryevents", 0x1F5}, // SP 0x000000 MP 0x5478E0
		{"botautoconnectenabled", 0x1F6}, // SP 0x000000 MP 0x547ED0
		{"botzonegetcount", 0x1F7}, // SP 0x000000 MP 0x547F00
		{"botzonesetteam", 0x1F8}, // SP 0x000000 MP 0x5481F0
		{"botzonenearestcount", 0x1F9}, // SP 0x000000 MP 0x548030
		{"botmemoryflags", 0x1FA}, // SP 0x000000 MP 0x547C20
		{"botflagmemoryevents", 0x1FB}, // SP 0x000000 MP 0x547600
		{"botzonegetindoorpercent", 0x1FC}, // SP 0x000000 MP 0x547FD0
		{"botsentientswap", 0x1FD}, // SP 0x000000 MP 0x547C70
		{"isbot", 0x1FE}, // SP 0x000000 MP 0x548250
		{"isagent", 0x1FF}, // SP 0x000000 MP 0x0CBBA0
		{"getmaxagents", 0x200}, // SP 0x000000 MP 0x51DAA0
		{"botdebugdrawtrigger", 0x201}, // SP 0x000000 MP 0x5475F0
		{"botgetclosestnavigablepoint", 0x202}, // SP 0x000000 MP 0x547830
		{"getnodesintrigger", 0x203}, // SP 0x000000 MP 0x3FC4D0
		{"nodesvisible", 0x204}, // SP 0x2FBF30 MP 0x3FD6C0
		{"getnodesonpath", 0x205}, // SP 0x2FBB40 MP 0x3FC5C0
		{"getzonecount", 0x206}, // SP 0x000000 MP 0x3FC9D0
		{"getzonenearest", 0x207}, // SP 0x000000 MP 0x3FC9E0
		{"getzonenodes", 0x208}, // SP 0x000000 MP 0x3FCB40
		{"getzonepath", 0x209}, // SP 0x000000 MP 0x3FD230
		{"getzoneorigin", 0x20A}, // SP 0x000000 MP 0x3FD180
		{"getnodezone", 0x20B}, // SP 0x000000 MP 0x3FC410
		{"getzonenodesbydist", 0x20C}, // SP 0x000000 MP 0x3FCE20
		{"getzonenodeforindex", 0x20D}, // SP 0x000000 MP 0x3FCB00
		{"getweaponexplosionradius", 0x20E}, // SP 0x2DB3D0 MP 0x0C9C50
		{"nodeexposedtosky", 0x211}, // SP 0x000000 MP 0x3FD490
		{"findentrances", 0x212}, // SP 0x000000 MP 0x3FBEE0
		{"badplace_global", 0x213}, // SP 0x000000 MP 0x44B2E0
		{"getpathdist", 0x214}, // SP 0x000000 MP 0x0E1DC0
		{"getlinkednodes", 0x215}, // SP 0x2FB840 MP 0x3FC130
		{"disconnectnodepair", 0x216}, // SP 0x2C62B0 MP 0x4485E0
		{"connectnodepair", 0x217}, // SP 0x2C6500 MP 0x4487E0
		{"gettimesincelastpaused", 0x218}, // SP 0x2D8D80 MP 0x000000
		{"precachesound", 0x21B}, // SP 0x2D7EA0 MP 0x0BA320
		{"devsetminimapdvarsettings", 0x21C}, // SP 0x000000 MP 0x0E0B90
		{"loadtransient", 0x21D}, // SP 0x3CE540 MP 0x000000
		{"unloadtransient", 0x21E}, // SP 0x3CE780 MP 0x000000
		{"unloadalltransients", 0x21F}, // SP 0x3CE6C0 MP 0x000000
		{"synctransients", 0x220}, // SP 0x3CE660 MP 0x000000
		{"aretransientsbusy", 0x221}, // SP 0x3CE240 MP 0x000000
		{"istransientqueued", 0x222}, // SP 0x3CE2C0 MP 0x000000
		{"istransientloaded", 0x223}, // SP 0x3CE270 MP 0x000000
		{"loadstartpointtransient", 0x224}, // SP 0x3CE300 MP 0x000000
		{"distance2dsquared", 0x225}, // SP 0x2C44E0 MP 0x442C50
		{"getangledelta3d", 0x226}, // SP 0x2C35F0 MP 0x4430F0
		{"activateclientexploder", 0x227}, // SP 0x2C9110 MP 0x44CBB0
		{"trajectorycalculateinitialvelocity", 0x228}, // SP 0x2C9450 MP 0x4355C0
		{"trajectorycalculateminimumvelocity", 0x229}, // SP 0x2C9A50 MP 0x435990
		{"trajectorycalculateexitangle", 0x22A}, // SP 0x2C9B00 MP 0x435B70
		{"trajectoryestimatedesiredinairtime", 0x22B}, // SP 0x2C9F50 MP 0x435FD0
		{"trajectorycomputedeltaheightattime", 0x22C}, // SP 0x2CA120 MP 0x436280
		{"trajectorycanattemptaccuratejump", 0x22D}, // SP 0x2CA6D0 MP 0x436410
		{"adddebugcommand", 0x22E}, // SP 0x2CF420 MP 0x4395B0
		{"ispointinvolume", 0x22F}, // SP 0x2C40E0 MP 0x4428C0
		{"cinematicgettimeinmsec", 0x230}, // SP 0x2DF4E0 MP 0x000000
		{"cinematicgetframe", 0x231}, // SP 0x2DF3C0 MP 0x000000
		{"iscinematicloaded", 0x232}, // SP 0x2DF3A0 MP 0x000000
		{"getenemysquaddata", 0x234}, // SP 0x2CC2E0 MP 0x438300
		{"getscriptablearray", 0x236}, // SP 0x2CC4E0 MP 0x438620
		{"clearfog", 0x237}, // SP 0x2CC000 MP 0x44D9C0
		{"setleveldopplerpreset", 0x238}, // SP 0x2DC150 MP 0x0CB750
		{"screenshake", 0x239}, // SP 0x2DE0B0 MP 0x000000
		{"isusinghdr", 0x23A}, // SP 0x2CACB0 MP 0x436950
		{"isusingssao", 0x23B}, // SP 0x2CAEC0 MP 0x4369D0
		{"playerphysicstraceinfo", 0x23D}, // SP 0x2BC680 MP 0x43BB20
		{"_func_23e", 0x23E}, // SP 0x2BC820 MP 0x43BF00
		{"getminchargetime", 0x23F}, // SP 0x2D1900 MP 0x4383C0
		{"getchargetimepershot", 0x240}, // SP 0x2D19C0 MP 0x4385F0
		{"getmaxchargeshots", 0x241}, // SP 0x2D1A10 MP 0x438610
		{"weaponischargeable", 0x242}, // SP 0x2D1A90 MP 0x4386E0
		{"weaponusesheat", 0x243}, // SP 0x2D1C40 MP 0x4386F0
		{"lootserviceonendgame", 0x244}, // SP 0x000000 MP 0x0BC080
		{"luinotifyevent", 0x245}, // SP 0x2CB7F0 MP 0x437430
		{"lootserviceonstartgame", 0x246}, // SP 0x000000 MP 0x0BC4D0
		{"tournamentreportplayerspm", 0x247}, // SP 0x000000 MP 0x0BF710
		{"tournamentreportwinningteam", 0x248}, // SP 0x000000 MP 0x0BF7D0
		{"tournamentreportendofgame", 0x249}, // SP 0x000000 MP 0x0BF800
		{"wakeupphysicssphere", 0x24A}, // SP 0x2DCD00 MP 0x000000
		{"wakeupragdollsphere", 0x24B}, // SP 0x2DCDA0 MP 0x000000
		{"dopplerpitch", 0x24C}, // SP 0x2C7F60 MP 0x445E70
		{"piecewiselinearlookup", 0x24D}, // SP 0x2C2B90 MP 0x440DE0
		{"anglestoaxis", 0x24E}, // SP 0x2C60A0 MP 0x444600
		{"visionsetwater", 0x24F}, // SP 0x2D6D80 MP 0x000000
		{"sendscriptusageanalysisdata", 0x250}, // SP 0x2CC0A0 MP 0x437ED0
		{"resetscriptusageanalysisdata", 0x251}, // SP 0x2CC1C0 MP 0x437FF0
		{"instantlylogusageanalysisdata", 0x252}, // SP 0x2CC290 MP 0x4382A0
		{"invertangles", 0x253}, // SP 0x2C7690 MP 0x4454E0
		{"rotatevectorinverted", 0x254}, // SP 0x2C7830 MP 0x445880
		{"calculatestartorientation", 0x255}, // SP 0x2C7D20 MP 0x445C00
		{"droptoground", 0x256}, // SP 0x000000 MP 0x458DA0
		{"setdemigodmode", 0x257}, // SP 0x2D9520 MP 0x000000
		{"precachelaser", 0x258}, // SP 0x2BBA40 MP 0x43AE70
		{"_func_259", 0x259}, // SP 0x2BBA70 MP 0x43AF90
		{"getcsplinecount", 0x25A}, // SP 0x2CCC40 MP 0x439140
		{"getcsplinepointcount", 0x25B}, // SP 0x2CCDF0 MP 0x4392F0
		{"getcsplinelength", 0x25C}, // SP 0x2CCF40 MP 0x439550
		{"getcsplinepointid", 0x25D}, // SP 0x2CD170 MP 0x4398E0
		{"getcsplinepointlabel", 0x25E}, // SP 0x2CD290 MP 0x439AC0
		{"getcsplinepointtension", 0x25F}, // SP 0x2CD610 MP 0x43A110
		{"getcsplinepointposition", 0x260}, // SP 0x2CD8B0 MP 0x43A3B0
		{"getcsplinepointcorridordims", 0x261}, // SP 0x2CDB10 MP 0x43A5D0
		{"getcsplinepointtangent", 0x262}, // SP 0x2CDD80 MP 0x43AB00
		{"getcsplinepointdisttonextpoint", 0x263}, // SP 0x2CE0A0 MP 0x43AF00
		{"calccsplineposition", 0x264}, // SP 0x2CE420 MP 0x43B430
		{"calccsplinetangent", 0x265}, // SP 0x2CE810 MP 0x43B6E0
		{"calccsplinecorridor", 0x266}, // SP 0x2CEAC0 MP 0x43BA50
		{"setnojipscore", 0x267}, // SP 0x2CEE10 MP 0x43BFE0
		{"setnojiptime", 0x268}, // SP 0x2CEF40 MP 0x43C270
		{"getpredictedentityposition", 0x269}, // SP 0x000000 MP 0x0CA120
		{"gamedvrprohibitrecording", 0x26A}, // SP 0x2CF0E0 MP 0x43C5F0
		{"gamedvrstartrecording", 0x26B}, // SP 0x2CF1E0 MP 0x43C930
		{"gamedvrstoprecording", 0x26C}, // SP 0x2CF210 MP 0x43CAA0
		{"gamedvrsetvideometadata", 0x26D}, // SP 0x2CF410 MP 0x43CB00
		{"gamedvrprohibitscreenshots", 0x26E}, // SP 0x2CF480 MP 0x43CB10
		{"gamedvrsetscreenshotmetadata", 0x26F}, // SP 0x2CF600 MP 0x43CBD0
		{"queuedialog", 0x270}, // SP 0x000000 MP 0x0BEAB0
		{"speechenablegrammar", 0x271}, // SP 0x2D9500 MP 0x000000
		{"speechenable", 0x272}, // SP 0x2D9510 MP 0x000000
		{"livestreamingenable", 0x273}, // SP 0x2CF610 MP 0x43CCF0
		{"livestreamingsetbitrate", 0x274}, // SP 0x2CF7C0 MP 0x43CD00
		{"livestreamingsetmetadata", 0x275}, // SP 0x2CF840 MP 0x43CD50
		{"livestreamingenablearchiving", 0x276}, // SP 0x2CF970 MP 0x43D080
		{"triggerportableradarping", 0x277}, // SP 0x000000 MP 0x0D0360
		{"botgetteamlimit", 0x279}, // SP 0x000000 MP 0x547BE0
		{"spawnfxforclient", 0x27A}, // SP 0x000000 MP 0x1CEF60
		{"botgetteamdifficulty", 0x27B}, // SP 0x000000 MP 0x547BA0
		{"debugstar", 0x27C}, // SP 0x2CEF60 MP 0x438E00
		{"newdebughudelem", 0x27D}, // SP 0x29BED0 MP 0x41A240
		{"printlightsetsettings", 0x27E}, // SP 0x2CED20 MP 0x436E10
		{"lightsetdumpstate", 0x27F}, // SP 0x2CEDF0 MP 0x436F20
		{"getsquadassaultelo", 0x280}, // SP 0x2CC2F0 MP 0x4385E0
		{"loadluifile", 0x281}, // SP 0x2D11C0 MP 0x43B090
		{"isdedicatedserver", 0x282}, // SP 0x000000 MP 0x0D0BD0
		{"getplaylistversion", 0x283}, // SP 0x000000 MP 0x0D0C70
		{"getplaylistid", 0x284}, // SP 0x000000 MP 0x0D0D40
		{"getactiveclientcount", 0x285}, // SP 0x000000 MP 0x0D3D20
		{"issquadsmode", 0x286}, // SP 0x000000 MP 0x0D4440
		{"getsquadassaultsquadindex", 0x287}, // SP 0x2CC4D0 MP 0x438600
		{"visionsetpostapply", 0x288}, // SP 0x000000 MP 0x0B8180
		{"addbot", 0x289}, // SP 0x000000 MP 0x0E2980
		{"ishairrunning", 0x28A}, // SP 0x2D0450 MP 0x43DAB0
		{"getnearbyarrayelements", 0x28B}, // SP 0x2C95C0 MP 0x447350
		{"vectorclamp", 0x28C}, // SP 0x2C9DC0 MP 0x447BB0
		{"isalliedsentient", 0x28D}, // SP 0x000000 MP 0x08FB60
		{"istestclient", 0x28E}, // SP 0x000000 MP 0x0E2FB0
		{"getrandomnodedestination", 0x28F}, // SP 0x000000 MP 0x3FC940
		{"debuglocalizestring", 0x290}, // SP 0x000000 MP 0x0B8840
		{"enablesoundcontextoverride", 0x291}, // SP 0x2BEF30 MP 0x440040
		{"disablesoundcontextoverride", 0x292}, // SP 0x2BF060 MP 0x4400A0
		{"notifyoncommandremove", 0x293}, // SP 0x2D8CB0 MP 0x000000
		{"getsndaliasvalue", 0x294}, // SP 0x2D08E0 MP 0x43DB80
		{"setsndaliasvalue", 0x295}, // SP 0x2BD070 MP 0x441830
		{"packedtablelookup", 0x296}, // SP 0x2CF490 MP 0x438990
		{"packedtablesectionlookup", 0x297}, // SP 0x2CFB50 MP 0x438D10
		{"packedtablelookupwithrange", 0x298}, // SP 0x2D0270 MP 0x439450
		{"grappletrace", 0x299}, // SP 0x2D6C10 MP 0x000000
		{"stopclientexploder", 0x29A}, // SP 0x2C9430 MP 0x44CD70
		{"closestpointstwosegs", 0x29B}, // SP 0x2C3F50 MP 0x442750
		{"isremovedentity", 0x29C}, // SP 0x2CFB10 MP 0x439CF0
		{"tablegetrowcount", 0x29D}, // SP 0x2BB770 MP 0x43C330
		{"tablegetcolumncount", 0x29E}, // SP 0x2BBB00 MP 0x43C940
		{"batteryusepershot", 0x29F}, // SP 0x000000 MP 0x0E3890
		{"batteryreqtouse", 0x2A0}, // SP 0x000000 MP 0x0E3900
		{"isweaponmanuallydetonatedbydoubletap", 0x2A1}, // SP 0x2D1220 MP 0x437920
		{"grapplegetmagnets", 0x2A2}, // SP 0x2D6E40 MP 0x000000
		{"getweaponname", 0x2A3}, // SP 0x2BE770 MP 0x440600
		{"activatepersistentclientexploder", 0x2A4}, // SP 0x2C91A0 MP 0x44CCB0
		{"deployriotshield", 0x2A5}, // SP 0x2BEAC0 MP 0x446770
		{"validatecostume", 0x2A6}, // SP 0x2BEEF0 MP 0x446E30
		{"randomcostume", 0x2A7}, // SP 0x2BEE80 MP 0x446AF0
		{"shootblank", 0x2A8}, // SP 0x000000 MP 0x0B8290
		{"boidflockupdate", 0x2A9}, // SP 0x2D9580 MP 0x000000
		{"debuggetanimname", 0x2AA}, // SP 0x2CED30 MP 0x437EE0
		{"setspmatchdata", 0x2AB}, // SP 0x2BF6A0 MP 0x447D40
		{"getspmatchdata", 0x2AC}, // SP 0x2BF0E0 MP 0x4477C0
		{"sendspmatchdata", 0x2AD}, // SP 0x2BF1F0 MP 0x447B80
		{"clearspmatchdata", 0x2AE}, // SP 0x2BF620 MP 0x447BA0
		{"setspmatchdatadef", 0x2AF}, // SP 0x2BF7A0 MP 0x447D60
		{"playcinematicforall", 0x2B0}, // SP 0x000000 MP 0x0DAB80
		{"preloadcinematicforall", 0x2B1}, // SP 0x000000 MP 0x0DB390
		{"stopcinematicforall", 0x2B2}, // SP 0x000000 MP 0x0DB530
		{"capsuletracepassed", 0x2B3}, // SP 0x000000 MP 0x0DEE10
		{"stopfxontagforclient", 0x2B4}, // SP 0x2C91D0 MP 0x448FE0
		{"killfxontagforclient", 0x2B5}, // SP 0x2C9AE0 MP 0x44A310
		{"isvector", 0x2B6}, // SP 0x2CA870 MP 0x436530
		{"notifychallengecomplete", 0x2B7}, // SP 0x2DB5E0 MP 0x000000
		{"lootservicestarttrackingplaytime", 0x2B8}, // SP 0x000000 MP 0x0BD640
		{"lootservicestoptrackingplaytime", 0x2B9}, // SP 0x000000 MP 0x0BE3A0
		{"lootservicevalidateplaytime", 0x2BA}, // SP 0x000000 MP 0x0BEFC0
		{"recordbreadcrumbdataforplayer", 0x2BB}, // SP 0x000000 MP 0x0C28B0
		{"getweaponandattachmentmodels", 0x2BC}, // SP 0x2C0FD0 MP 0x443470
		{"changewhizbyautosimparams", 0x2BD}, // SP 0x2DB640 MP 0x000000
		{"sysprint", 0x2BE}, // SP 0x2CE050 MP 0x4382B0
		{"objective_mlgspectator", 0x2BF}, // SP 0x000000 MP 0x457D70
		{"setspcheckpointdata", 0x2C0}, // SP 0x2BF7C0 MP 0x447E00
		{"getspcheckpointdata", 0x2C1}, // SP 0x2BF890 MP 0x447E10
		{"isnumber", 0x2C2}, // SP 0x2CAA60 MP 0x436740
		{"isonlinegame", 0x2C3}, // SP 0x2BD760 MP 0x43EE60
		{"issystemlink", 0x2C4}, // SP 0x2BD790 MP 0x43F300
		{"setsoundmasteringfadetime", 0x2C5}, // SP 0x2DC1C0 MP 0x000000
		{"getstanceandmotionstateforplayer", 0x2C6}, // SP 0x000000 MP 0x0C3310
		{"nodeisnotusable", 0x2C7}, // SP 0x000000 MP 0x3FD5E0
		{"nodesetnotusable", 0x2C8}, // SP 0x000000 MP 0x3FD640
		{"spawnlinkedfx", 0x2C9}, // SP 0x000000 MP 0x1CF110
		{"spawnlinkedfxforclient", 0x2CA}, // SP 0x000000 MP 0x1CF180
		{"getplaylistname", 0x2CB}, // SP 0x000000 MP 0x0D10A0
		{"getlocaltimestring", 0x2CC}, // SP 0x000000 MP 0x0D1E60
		{"isonwifi", 0x2CD}, // SP 0x000000 MP 0x0D37B0
		{"getbuttonsconfig", 0x2CE}, // SP 0x2DB610 MP 0x000000
		{"getchallengeid", 0x2CF}, // SP 0x000000 MP 0x0C91F0
		{"soundsettraceflags", 0x2D4}, // SP 0x2DBA00 MP 0x000000
		{"handlepickupdeployedriotshield", 0x2D5}, // SP 0x2BECB0 MP 0x446A10
		{"getcodanywherecurrentplatform", 0x2D6}, // SP 0x000000 MP 0x0D2570
		{"getcostumefromtable", 0x2D7}, // SP 0x2BF040 MP 0x446F70
		{"visionsetoverdrive", 0x2D8}, // SP 0x2D6D70 MP 0x000000
		{"nodegetsplitgroup", 0x2D9}, // SP 0x000000 MP 0x3FD570
		{"recordbreadcrumbdataforplayersp", 0x2DA}, // SP 0x2DC210 MP 0x000000
		{"getchallengerewarditem", 0x2DB}, // SP 0x000000 MP 0x0C9310
		{"setentplayerxuidforemblem", 0x2DC}, // SP 0x000000 MP 0x1CECF0
		{"resetentplayerxuidforemblems", 0x2DD}, // SP 0x000000 MP 0x1CECE0
		{"isshipbuild", 0x2DF}, // SP 0x2C0190 MP 0x448FD0
		{"strinsertnumericdelimiters", 0x2E0}, // SP 0x2CBB20 MP 0x44A740
		{"isscriptedagent", 0x2E1}, // SP 0x000000 MP 0x0E1C40
		{"playfxonweapon", 0x2E2}, // SP 0x2C7F50 MP 0x4476F0
		{"stopfxonweapon", 0x2E3}, // SP 0x2C9440 MP 0x4496C0
		{"killfxonweapon", 0x2E4}, // SP 0x2C9AF0 MP 0x44AA30
		{"getdefaultmaxfaceenemydistance", 0x2E5}, // SP 0x2DB830 MP 0x000000
		{"applyaccelerationonentity", 0x2E6}, // SP 0x2CA0A0 MP 0x44B6A0
		{"applyimpulseonentity", 0x2E7}, // SP 0x2CA570 MP 0x44BB70
		{"applyimpulseonentity", 0x2E8}, // SP 0x2D7C10 MP 0x000000
		{"getinventoryitemtype", 0x2E9}, // SP 0x2BABC0 MP 0x439170
		{"getweaponmodelbounds", 0x2EA}, // SP 0x000000 MP 0x0BA7A0
		{"weaponitemplayidleanim", 0x2EB}, // SP 0x000000 MP 0x0BBD60
		{"_func_2ec", 0x2EC}, // SP 0x000000 MP 0x0BE870
		{"getstaticmodelcount", 0x2ED}, // SP 0x2BEE40 MP 0x440030
		{"getstaticmodelname", 0x2EE}, // SP 0x2BEEC0 MP 0x440090
		{"getstaticmodelbounds", 0x2EF}, // SP 0x2BEF70 MP 0x4400E0
		{"findstaticmodelindex", 0x2F0}, // SP 0x2BF0F0 MP 0x440360
		{"getdynentcount", 0x2F1}, // SP 0x2BF190 MP 0x440450
		{"getdynentmodelname", 0x2F2}, // SP 0x2BF200 MP 0x440530
		{"getdynentmodelbounds", 0x2F3}, // SP 0x2BF630 MP 0x440590
		{"finddynentwithmodelindex", 0x2F4}, // SP 0x2BF7B0 MP 0x440760
		{"getentitymodelname", 0x2F5}, // SP 0x2BF820 MP 0x4407C0
		{"getentitymodelbounds", 0x2F6}, // SP 0x2BF8A0 MP 0x440900
		{"findentitywithmodelindex", 0x2F7}, // SP 0x2BFB10 MP 0x4409E0
		{"_func_2f8", 0x2F8}, // SP 0x2BF570 MP 0x441780
		{"_func_2f9", 0x2F9}, // SP 0x2C44D0 MP 0x4456F0
		{"_func_2fa", 0x2FA}, // SP 0x2C4540 MP 0x446360
		{"_func_2fb", 0x2FB}, // SP 0x2D1760 MP 0x437F20
		{"_func_2fc", 0x2FC}, // SP 0x2CF430 MP 0x4395C0
		{"_func_2fd", 0x2FD}, // SP 0x2CF780 MP 0x439970
		{"_func_2fe", 0x2FE}, // SP 0x2CF820 MP 0x4399F0
		{"_func_2ff", 0x2FF}, // SP 0x000000 MP 0x43A2F0
		{"_func_300", 0x300}, // SP 0x000000 MP 0x0BC820
		{"_func_301", 0x301}, // SP 0x2CF050 MP 0x43C5E0
		{"_meth_302", 0x302}, // SP 0x000000 MP 0x0D3EC0
		{"_meth_303", 0x303}, // SP 0x2D14A0 MP 0x43A1A0
		{"_meth_304", 0x304}, // SP 0x2BAB10 MP 0x43B0F0
		{"_meth_305", 0x305}, // SP 0x000000 MP 0x0BCB60
		{"_meth_306", 0x306}, // SP 0x000000 MP 0x0BD6D0
		{"_meth_307", 0x307}, // SP 0x000000 MP 0x0BE100
		{"_meth_308", 0x308}, // SP 0x2C4550 MP 0x4467D0
		{"_meth_309", 0x309}, // SP 0x000000 MP 0x0BC940
		{"_meth_30a", 0x30A}, // SP 0x2BC310 MP 0x43D090
	};

	std::unordered_map<std::string, unsigned> method_map =
	{
		{"motionblurhqenable", 0x8000}, // SP 0x2CE360 MP 0x435670
		{"setturretdismountorg", 0x8001}, // SP 0x2DC4E0 MP 0x000000
		{"setdamagestage", 0x8002}, // SP 0x000000 MP 0x6048A0
		{"playsoundtoteam", 0x8003}, // SP 0x000000 MP 0x0BB6A0
		{"playsoundtoplayer", 0x8004}, // SP 0x000000 MP 0x0BC9D0
		{"playerhide", 0x8005}, // SP 0x000000 MP 0x0C9190
		{"playershow", 0x8006}, // SP 0x000000 MP 0x0C9290
		{"showtoplayer", 0x8007}, // SP 0x000000 MP 0x0C9370
		{"threatdetectedtoplayer", 0x8008}, // SP 0x000000 MP 0x0C94A0
		{"clearthreatdetected", 0x8009}, // SP 0x000000 MP 0x0C9570
		{"enableplayeruse", 0x800A}, // SP 0x000000 MP 0x0D47B0
		{"disableplayeruse", 0x800B}, // SP 0x000000 MP 0x0D4B10
		{"enableammogeneration", 0x800C}, // SP 0x000000 MP 0x0D5A10
		{"disableammogeneration", 0x800D}, // SP 0x000000 MP 0x0D4DB0
		{"makescrambler", 0x800E}, // SP 0x000000 MP 0x0CFA20
		{"makeportableradar", 0x800F}, // SP 0x000000 MP 0x0CFF90
		{"clearscrambler", 0x8010}, // SP 0x000000 MP 0x0CFC40
		{"clearportableradar", 0x8011}, // SP 0x000000 MP 0x0D0280
		{"addplayermantleblockage", 0x8012}, // SP 0x000000 MP 0x0E11E0
		{"setteamfortrigger", 0x8013}, // SP 0x000000 MP 0x0CB4C0
		{"clientclaimtrigger", 0x8014}, // SP 0x000000 MP 0x0CBE10
		{"clientreleasetrigger", 0x8015}, // SP 0x000000 MP 0x0CC740
		{"releaseclaimedtrigger", 0x8016}, // SP 0x000000 MP 0x0CD360
		{"isusingonlinedataoffline", 0x8017}, // SP 0x000000 MP 0x0C9400
		{"getrestedtime", 0x8018}, // SP 0x000000 MP 0x0C99C0
		{"sendleaderboards", 0x8019}, // SP 0x000000 MP 0x0BB2C0
		{"isonladder", 0x801A}, // SP 0x2C33B0 MP 0x444260
		{"getcorpseanim", 0x801B}, // SP 0x000000 MP 0x0D9D00
		{"playerforcedeathanim", 0x801C}, // SP 0x000000 MP 0x0DC870
		{"attach", 0x801D}, // SP 0x2D1570 MP 0x43B610
		{"getlightfovinner", 0x801E}, // SP 0x2D81B0 MP 0x000000
		{"getlightfovouter", 0x801F}, // SP 0x2D8250 MP 0x000000
		{"setlightfovrange", 0x8020}, // SP 0x2D82D0 MP 0x000000
		{"getlightexponent", 0x8021}, // SP 0x2D8560 MP 0x000000
		{"setlightexponent", 0x8022}, // SP 0x2D8630 MP 0x000000
		{"startragdoll", 0x8023}, // SP 0x2D88C0 MP 0x0D7980
		{"startragdollfromimpact", 0x8024}, // SP 0x2D88E0 MP 0x000000
		{"queryshouldearlyragdoll", 0x8025}, // SP 0x2C8360 MP 0x449F50
		{"logstring", 0x8026}, // SP 0x2DB820 MP 0x0B94E0
		{"_meth_8028", 0x8028}, // SP 0x2D8CF0 MP 0x000000
		{"thermaldrawenable", 0x8029}, // SP 0x2CE130 MP 0x435410
		{"detach", 0x802A}, // SP 0x2D1C50 MP 0x43BE70
		{"detachall", 0x802B}, // SP 0x2D1DF0 MP 0x43C030
		{"getattachsize", 0x802C}, // SP 0x2BC140 MP 0x43E4C0
		{"getattachmodelname", 0x802D}, // SP 0x2BC4F0 MP 0x43E740
		{"getattachtagname", 0x802E}, // SP 0x2BC7B0 MP 0x43E800
		{"setturretcanaidetach", 0x802F}, // SP 0x2DA880 MP 0x000000
		{"setturretfov", 0x8030}, // SP 0x2DA8C0 MP 0x000000
		{"setplayerturretfov", 0x8031}, // SP 0x2DA9C0 MP 0x000000
		{"lerpfov", 0x8032}, // SP 0x2DAB40 MP 0x000000
		{"lerpfovscale", 0x8033}, // SP 0x2DAC00 MP 0x000000
		{"getvalidcoverpeekouts", 0x8034}, // SP 0x2FB620 MP 0x000000
		{"gethighestnodestance", 0x8035}, // SP 0x2FB5B0 MP 0x3FBE40
		{"doesnodeallowstance", 0x8036}, // SP 0x2FB480 MP 0x3FBD10
		{"doesnodeforcecombat", 0x8037}, // SP 0x2FB530 MP 0x3FBDC0
		{"getgunangles", 0x8038}, // SP 0x2DF0D0 MP 0x000000
		{"magicgrenade", 0x8039}, // SP 0x2DF1D0 MP 0x000000
		{"magicgrenademanual", 0x803A}, // SP 0x2DF6A0 MP 0x000000
		{"getentnum", 0x803B}, // SP 0x2D7F00 MP 0x000000
		{"launch", 0x803C}, // SP 0x2D7F20 MP 0x000000
		{"setsoundblend", 0x803D}, // SP 0x2D80F0 MP 0x000000
		{"makefakeai", 0x803E}, // SP 0x2DAE90 MP 0x000000
		{"spawndrone", 0x803F}, // SP 0x2DAED0 MP 0x000000
		{"setcorpseremovetimer", 0x8040}, // SP 0x2DAFA0 MP 0x000000
		{"setlookattext", 0x8041}, // SP 0x2DB060 MP 0x000000
		{"setspawnerteam", 0x8042}, // SP 0x2DB0B0 MP 0x000000
		{"addaieventlistener", 0x8043}, // SP 0x2DC350 MP 0x000000
		{"removeaieventlistener", 0x8044}, // SP 0x2DC410 MP 0x000000
		{"getlightcolor", 0x8045}, // SP 0x2C2E60 MP 0x443B50
		{"setlightcolor", 0x8046}, // SP 0x2C30B0 MP 0x443E70
		{"getlightradius", 0x8047}, // SP 0x2D7B40 MP 0x000000
		{"setlightradius", 0x8048}, // SP 0x2D7B80 MP 0x000000
		{"getattachignorecollision", 0x8049}, // SP 0x2BCC50 MP 0x43EC10
		{"hidepart", 0x804A}, // SP 0x2BD670 MP 0x43ED60
		{"hidepart_allinstances", 0x804B}, // SP 0x2BD870 MP 0x43F430
		{"hideallparts", 0x804C}, // SP 0x2BDAE0 MP 0x43F990
		{"showpart", 0x804D}, // SP 0x2BDE40 MP 0x43FCB0
		{"showallparts", 0x804E}, // SP 0x2BE310 MP 0x4402C0
		{"linkto", 0x804F}, // SP 0x2BEDF0 MP 0x442660
		{"linktoblendtotag", 0x8050}, // SP 0x2BF160 MP 0x442EA0
		{"unlink", 0x8051}, // SP 0x2BF210 MP 0x442F30
		{"setnormalhealth", 0x8052}, // SP 0x2D87D0 MP 0x0C3620
		{"dodamage", 0x8053}, // SP 0x2BF2B0 MP 0x43EE80
		{"kill", 0x8054}, // SP 0x2D89C0 MP 0x000000
		{"show", 0x8055}, // SP 0x2D8DA0 MP 0x0C8890
		{"hide", 0x8056}, // SP 0x2D8EA0 MP 0x0C8B00
		{"showonclient", 0x8057}, // SP 0x2D8EC0 MP 0x000000
		{"hideonclient", 0x8058}, // SP 0x2D8F40 MP 0x000000
		{"disconnectpaths", 0x8059}, // SP 0x2C59B0 MP 0x447E30
		{"connectpaths", 0x805A}, // SP 0x2C5EC0 MP 0x4480B0
		{"disconnectnode", 0x805B}, // SP 0x2C6080 MP 0x448260
		{"connectnode", 0x805C}, // SP 0x2C6170 MP 0x4482C0
		{"startusingheroonlylighting", 0x805D}, // SP 0x2DC240 MP 0x000000
		{"stopusingheroonlylighting", 0x805E}, // SP 0x2DC260 MP 0x000000
		{"startusinglessfrequentlighting", 0x805F}, // SP 0x2DC280 MP 0x000000
		{"stopusinglessfrequentlighting", 0x8060}, // SP 0x2DC2A0 MP 0x000000
		{"setmovingplatformplayerturnrate", 0x8061}, // SP 0x2DD040 MP 0x000000
		{"setthermalfog", 0x8062}, // SP 0x2DD0C0 MP 0x000000
		{"setnightvisionfog", 0x8063}, // SP 0x2DD9D0 MP 0x000000
		{"clearthermalfog", 0x8064}, // SP 0x2DEA00 MP 0x000000
		{"clearnightvisionfog", 0x8065}, // SP 0x2DEA70 MP 0x000000
		{"setmode", 0x8067}, // SP 0x2D9180 MP 0x0D81F0
		{"getmode", 0x8068}, // SP 0x2D93F0 MP 0x0D9DD0
		{"setturretignoregoals", 0x8069}, // SP 0x2DA680 MP 0x000000
		{"islinked", 0x806A}, // SP 0x2BF6B0 MP 0x443040
		{"enablelinkto", 0x806B}, // SP 0x2C0DA0 MP 0x444430
		{"playsoundatviewheight", 0x806C}, // SP 0x2D80B0 MP 0x000000
		{"setpitch", 0x806E}, // SP 0x2C37A0 MP 0x44C250
		{"scalepitch", 0x806F}, // SP 0x2C3430 MP 0x44C030
		{"setvolume", 0x8070}, // SP 0x2C3A60 MP 0x44C420
		{"scalevolume", 0x8071}, // SP 0x2C37D0 MP 0x44C370
		{"enableportalgroup", 0x8072}, // SP 0x2CADC0 MP 0x000000
		{"setspeakermapmonotostereo", 0x8073}, // SP 0x2D7D50 MP 0x000000
		{"setspeakermapmonoto51", 0x8074}, // SP 0x2D7DC0 MP 0x000000
		{"setdistributed2dsound", 0x8075}, // SP 0x2D7DD0 MP 0x000000
		{"playsoundasmaster", 0x8076}, // SP 0x2D8190 MP 0x0C22C0
		{"playloopsound", 0x8077}, // SP 0x2D81E0 MP 0x0C24C0
		{"eqon", 0x8078}, // SP 0x2D82A0 MP 0x000000
		{"eqoff", 0x8079}, // SP 0x2D84D0 MP 0x000000
		{"haseq", 0x807A}, // SP 0x2D8500 MP 0x000000
		{"iswaitingonsound", 0x807B}, // SP 0x2D8540 MP 0x000000
		{"playfoley", 0x807C}, // SP 0x2D8580 MP 0x000000
		{"getnormalhealth", 0x807D}, // SP 0x2D8750 MP 0x0C2B40
		{"playerlinkto", 0x807E}, // SP 0x2C1F80 MP 0x4454D0
		{"playerlinktodelta", 0x807F}, // SP 0x2C1F90 MP 0x445560
		{"playerlinkweaponviewtodelta", 0x8080}, // SP 0x2C2070 MP 0x445990
		{"playerlinktoabsolute", 0x8081}, // SP 0x2C2090 MP 0x4459B0
		{"playerlinktoblend", 0x8082}, // SP 0x2C5B80 MP 0x4477E0
		{"playerlinkedoffsetenable", 0x8083}, // SP 0x2C2700 MP 0x446210
		{"setwaypointedgestyle_secondaryarrow", 0x8084}, // SP 0x29AA40 MP 0x419840
		{"setwaypointiconoffscreenonly", 0x8085}, // SP 0x29AA90 MP 0x419850
		{"fadeovertime", 0x8086}, // SP 0x29AE50 MP 0x419BA0
		{"scaleovertime", 0x8087}, // SP 0x29AF90 MP 0x419CE0
		{"moveovertime", 0x8088}, // SP 0x29B070 MP 0x419DC0
		{"reset", 0x8089}, // SP 0x29B1B0 MP 0x419E80
		{"destroy", 0x808A}, // SP 0x29B1F0 MP 0x419F60
		{"setpulsefx", 0x808B}, // SP 0x29B330 MP 0x41A040
		{"setplayernamestring", 0x808C}, // SP 0x29B270 MP 0x419FB0
		{"changefontscaleovertime", 0x808D}, // SP 0x29AEF0 MP 0x419C40
		{"startignoringspotlight", 0x808E}, // SP 0x2DC1A0 MP 0x000000
		{"stopignoringspotlight", 0x808F}, // SP 0x2DC220 MP 0x000000
		{"dontcastshadows", 0x8090}, // SP 0x2DC810 MP 0x000000
		{"castshadows", 0x8091}, // SP 0x2DC830 MP 0x000000
		{"setstablemissile", 0x8092}, // SP 0x2D8F70 MP 0x000000
		{"playersetgroundreferenceent", 0x8093}, // SP 0x2DCE40 MP 0x0E2430
		{"dontinterpolate", 0x8094}, // SP 0x2C0C90 MP 0x449A90
		{"dospawn", 0x8095}, // SP 0x2DEAE0 MP 0x000000
		{"stalingradspawn", 0x8096}, // SP 0x2DEB80 MP 0x000000
		{"getorigin", 0x8097}, // SP 0x2DEC20 MP 0x0B9540
		{"getcentroid", 0x8098}, // SP 0x2DECE0 MP 0x000000
		{"getshootatpos", 0x8099}, // SP 0x2DEE20 MP 0x000000
		{"getdebugeye", 0x809A}, // SP 0x2D7440 MP 0x000000
		{"useby", 0x809B}, // SP 0x2D7720 MP 0x0B9650
		{"playsound", 0x809C}, // SP 0x2D7E00 MP 0x0B9C40
		{"playerlinkedoffsetdisable", 0x80A1}, // SP 0x2C2920 MP 0x446700
		{"playerlinkedsetviewznear", 0x80A2}, // SP 0x2C3020 MP 0x446A60
		{"playerlinkedsetusebaseangleforviewclamp", 0x80A3}, // SP 0x2C3330 MP 0x447070
		{"lerpviewangleclamp", 0x80A4}, // SP 0x2C8370 MP 0x449860
		{"setviewangleresistance", 0x80A5}, // SP 0x2C8B40 MP 0x44A270
		{"springcamenabled", 0x80A6}, // SP 0x2C9270 MP 0x000000
		{"springcamdisabled", 0x80A7}, // SP 0x2C9BA0 MP 0x000000
		{"linktoplayerview", 0x80A8}, // SP 0x2CAED0 MP 0x000000
		{"unlinkfromplayerview", 0x80A9}, // SP 0x2CB8D0 MP 0x000000
		{"geteye", 0x80AA}, // SP 0x2D17D0 MP 0x438C20
		{"istouching", 0x80AB}, // SP 0x2C28D0 MP 0x445030
		{"getistouchingentities", 0x80AC}, // SP 0x2C2D10 MP 0x4452B0
		{"stoploopsound", 0x80AD}, // SP 0x2CC600 MP 0x44B650
		{"stopsounds", 0x80AE}, // SP 0x2C3A90 MP 0x446280
		{"playrumbleonentity", 0x80AF}, // SP 0x2CC820 MP 0x44BE30
		{"playrumblelooponentity", 0x80B0}, // SP 0x2CCA30 MP 0x44BED0
		{"stoprumble", 0x80B1}, // SP 0x2CCEB0 MP 0x44C6B0
		{"delete", 0x80B2}, // SP 0x2D16B0 MP 0x438AB0
		{"setmodel", 0x80B3}, // SP 0x2CF280 MP 0x439370
		{"laseron", 0x80B4}, // SP 0x2CD320 MP 0x44CBF0
		{"laseroff", 0x80B5}, // SP 0x2CD6A0 MP 0x44D240
		{"thermalvisionon", 0x80B8}, // SP 0x2CD960 MP 0x44D4F0
		{"thermalvisionoff", 0x80B9}, // SP 0x2CDE30 MP 0x44DA70
		{"thermalvisionfofoverlayon", 0x80BA}, // SP 0x2CF070 MP 0x437230
		{"thermalvisionfofoverlayoff", 0x80BB}, // SP 0x2CF220 MP 0x4374E0
		{"autospotoverlayon", 0x80BC}, // SP 0x000000 MP 0x437700
		{"autospotoverlayoff", 0x80BD}, // SP 0x000000 MP 0x4378D0
		{"seteyesonuplinkenabled", 0x80BE}, // SP 0x000000 MP 0x437C30
		{"setcontents", 0x80C0}, // SP 0x2CBA60 MP 0x44A430
		{"makeusable", 0x80C1}, // SP 0x2CBCA0 MP 0x44A860
		{"makeunusable", 0x80C2}, // SP 0x2CBF30 MP 0x44ABD0
		{"_meth_80c3", 0x80C3}, // SP 0x2C2350 MP 0x44ADA0
		{"_meth_80c4", 0x80C4}, // SP 0x2C26C0 MP 0x44B610
		{"setwhizbyprobabilities", 0x80C5}, // SP 0x28CCC0 MP 0x000000
		{"visionsetnakedforplayer_lerp", 0x80C6}, // SP 0x2911D0 MP 0x000000
		{"setwaitnode", 0x80C7}, // SP 0x4E8850 MP 0x000000
		{"returnplayercontrol", 0x80C8}, // SP 0x4E69D0 MP 0x000000
		{"vehphys_starttrack", 0x80C9}, // SP 0x4E86B0 MP 0x000000
		{"vehphys_clearautodisable", 0x80CA}, // SP 0x4E8740 MP 0x000000
		{"vehicleusealtblendedaudio", 0x80CB}, // SP 0x4E7F80 MP 0x000000
		{"settext", 0x80CC}, // SP 0x299BD0 MP 0x418B90
		{"clearalltextafterhudelem", 0x80CD}, // SP 0x299CC0 MP 0x418C80
		{"setshader", 0x80CE}, // SP 0x299CD0 MP 0x418C90
		{"settargetent", 0x80CF}, // SP 0x29A040 MP 0x418FF0
		{"cleartargetent", 0x80D0}, // SP 0x29A1E0 MP 0x419180
		{"settimer", 0x80D1}, // SP 0x29A2C0 MP 0x419250
		{"settimerup", 0x80D2}, // SP 0x29A390 MP 0x419310
		{"settimerstatic", 0x80D3}, // SP 0x29A460 MP 0x4193B0
		{"settenthstimer", 0x80D4}, // SP 0x29A530 MP 0x419460
		{"settenthstimerup", 0x80D5}, // SP 0x29A600 MP 0x419520
		{"settenthstimerstatic", 0x80D6}, // SP 0x29A6D0 MP 0x4195C0
		{"setclock", 0x80D7}, // SP 0x29A7A0 MP 0x419670
		{"setclockup", 0x80D8}, // SP 0x29A7C0 MP 0x419690
		{"setvalue", 0x80D9}, // SP 0x29A7E0 MP 0x4196B0
		{"setwaypoint", 0x80DA}, // SP 0x29A880 MP 0x419730
		{"setwaypointedgestyle_rotatingicon", 0x80DB}, // SP 0x29A9F0 MP 0x419830
		{"setcursorhint", 0x80DC}, // SP 0x2CC300 MP 0x44B510
		{"sethintstring", 0x80DD}, // SP 0x2CCAC0 MP 0x44BEF0
		{"settertiaryhintstring", 0x80DE}, // SP 0x000000 MP 0x0E4B40
		{"forceusehinton", 0x80DF}, // SP 0x2CCD30 MP 0x44C2B0
		{"forceusehintoff", 0x80E0}, // SP 0x2CCFF0 MP 0x44C540
		{"makesoft", 0x80E1}, // SP 0x2CC050 MP 0x44AD40
		{"makehard", 0x80E2}, // SP 0x2CC0F0 MP 0x44B0C0
		{"willneverchange", 0x80E3}, // SP 0x2CC2A0 MP 0x44B130
		{"startfiring", 0x80E4}, // SP 0x2C3DC0 MP 0x446D90
		{"stopfiring", 0x80E5}, // SP 0x2C3E60 MP 0x446F20
		{"isfiringturret", 0x80E6}, // SP 0x2C4130 MP 0x4471C0
		{"startbarrelspin", 0x80E7}, // SP 0x2C4220 MP 0x447540
		{"stopbarrelspin", 0x80E8}, // SP 0x2C4480 MP 0x447600
		{"getbarrelspinrate", 0x80E9}, // SP 0x2C4660 MP 0x447770
		{"remotecontrolturret", 0x80EA}, // SP 0x000000 MP 0x446460
		{"remotecontrolturretoff", 0x80EB}, // SP 0x000000 MP 0x4468F0
		{"shootturret", 0x80EC}, // SP 0x2C64B0 MP 0x448590
		{"getturretowner", 0x80ED}, // SP 0x2C6720 MP 0x448750
		{"enabledeathshield", 0x80EE}, // SP 0x295860 MP 0x000000
		{"nightvisiongogglesforceon", 0x80EF}, // SP 0x2958C0 MP 0x000000
		{"nightvisiongogglesforceoff", 0x80F0}, // SP 0x295910 MP 0x000000
		{"enableinvulnerability", 0x80F1}, // SP 0x295960 MP 0x000000
		{"disableinvulnerability", 0x80F2}, // SP 0x2959C0 MP 0x000000
		{"enablebreaching", 0x80F3}, // SP 0x295A20 MP 0x000000
		{"disablebreaching", 0x80F4}, // SP 0x295A70 MP 0x000000
		{"forceviewmodelanimation", 0x80F5}, // SP 0x295DC0 MP 0x000000
		{"disableturretdismount", 0x80F6}, // SP 0x295F90 MP 0x000000
		{"enableturretdismount", 0x80F7}, // SP 0x295FF0 MP 0x000000
		{"uploadscore", 0x80F8}, // SP 0x296050 MP 0x000000
		{"uploadtime", 0x80F9}, // SP 0x2960E0 MP 0x000000
		{"uploadleaderboards", 0x80FA}, // SP 0x296190 MP 0x000000
		{"giveachievement", 0x80FB}, // SP 0x290DC0 MP 0x40C6A0
		{"hidehud", 0x80FC}, // SP 0x295B60 MP 0x000000
		{"showhud", 0x80FD}, // SP 0x295BB0 MP 0x000000
		{"mountvehicle", 0x80FE}, // SP 0x295C80 MP 0x000000
		{"dismountvehicle", 0x80FF}, // SP 0x295D10 MP 0x000000
		{"enableslowaim", 0x8100}, // SP 0x28ED50 MP 0x40A500
		{"disableslowaim", 0x8101}, // SP 0x28EFA0 MP 0x40A820
		{"usehintsinvehicle", 0x8102}, // SP 0x295C00 MP 0x000000
		{"vehicleattackbuttonpressed", 0x8103}, // SP 0x28F310 MP 0x000000
		{"setwhizbyoffset", 0x8104}, // SP 0x28CA40 MP 0x000000
		{"setsentryowner", 0x8105}, // SP 0x2C68A0 MP 0x448D80
		{"setsentrycarrier", 0x8106}, // SP 0x2C6B40 MP 0x449100
		{"setturretminimapvisible", 0x8107}, // SP 0x2C7600 MP 0x449570
		{"settargetentity", 0x8108}, // SP 0x2C7940 MP 0x4497C0
		{"snaptotargetentity", 0x8109}, // SP 0x2C7E40 MP 0x449B70
		{"cleartargetentity", 0x810A}, // SP 0x2C8300 MP 0x449F60
		{"getturrettarget", 0x810B}, // SP 0x2C87A0 MP 0x44A280
		{"setplayerspread", 0x810C}, // SP 0x2C8FB0 MP 0x44B020
		{"setaispread", 0x810D}, // SP 0x2C9140 MP 0x44B120
		{"setsuppressiontime", 0x810E}, // SP 0x2C93C0 MP 0x44B1A0
		{"setflaggedanimknobrestart", 0x810F}, // SP 0x2B80E0 MP 0x000000
		{"setflaggedanimknoblimitedrestart", 0x8110}, // SP 0x2B8100 MP 0x000000
		{"setflaggedanimknoball", 0x8111}, // SP 0x2B8120 MP 0x000000
		{"setflaggedanimknoballrestart", 0x8112}, // SP 0x2B8140 MP 0x000000
		{"setflaggedanim", 0x8113}, // SP 0x2B8160 MP 0x000000
		{"setflaggedanimlimited", 0x8114}, // SP 0x2B8180 MP 0x000000
		{"setflaggedanimrestart", 0x8115}, // SP 0x2B8190 MP 0x000000
		{"setflaggedanimlimitedrestart", 0x8116}, // SP 0x2B81B0 MP 0x000000
		{"useanimtree", 0x8117}, // SP 0x2B81D0 MP 0x000000
		{"stopuseanimtree", 0x8118}, // SP 0x2B82B0 MP 0x000000
		{"setanimtime", 0x8119}, // SP 0x2B8330 MP 0x000000
		{"allowstand", 0x811A}, // SP 0x28F390 MP 0x40A6F0
		{"allowcrouch", 0x811B}, // SP 0x28F4A0 MP 0x40A8A0
		{"allowprone", 0x811C}, // SP 0x28F580 MP 0x40AC20
		{"allowlean", 0x811D}, // SP 0x28F660 MP 0x40AD70
		{"allowswim", 0x811E}, // SP 0x294D10 MP 0x000000
		{"setocclusion", 0x811F}, // SP 0x295190 MP 0x000000
		{"deactivateocclusion", 0x8121}, // SP 0x294F50 MP 0x000000
		{"deactivateallocclusion", 0x8122}, // SP 0x294FD0 MP 0x000000
		{"isocclusionenabled", 0x8123}, // SP 0x2952D0 MP 0x000000
		{"iseqenabled", 0x8128}, // SP 0x295320 MP 0x000000
		{"seteq", 0x8129}, // SP 0x294FF0 MP 0x000000
		{"seteqbands", 0x812A}, // SP 0x295370 MP 0x000000
		{"deactivateeq", 0x812B}, // SP 0x2954A0 MP 0x000000
		{"seteqlerp", 0x812C}, // SP 0x294E70 MP 0x000000
		{"islookingat", 0x812D}, // SP 0x295550 MP 0x000000
		{"isthrowinggrenade", 0x812E}, // SP 0x28D580 MP 0x408590
		{"isfiring", 0x812F}, // SP 0x28D970 MP 0x408BD0
		{"ismeleeing", 0x8130}, // SP 0x290B30 MP 0x40CC70
		{"setautopickup", 0x8131}, // SP 0x295790 MP 0x000000
		{"allowmelee", 0x8132}, // SP 0x28F1B0 MP 0x40A2E0
		{"allowfire", 0x8133}, // SP 0x28F2A0 MP 0x40A490
		{"enablehealthshield", 0x8134}, // SP 0x295800 MP 0x000000
		{"setconvergencetime", 0x8135}, // SP 0x2C9770 MP 0x44B3D0
		{"setconvergenceheightpercent", 0x8136}, // SP 0x2C9D30 MP 0x44BAE0
		{"setturretteam", 0x8137}, // SP 0x2C9FF0 MP 0x44BC00
		{"maketurretsolid", 0x8138}, // SP 0x2CA680 MP 0x44BFE0
		{"maketurretoperable", 0x8139}, // SP 0x2CA820 MP 0x44C200
		{"maketurretinoperable", 0x813A}, // SP 0x2CAA00 MP 0x44C3A0
		{"makeentitysentient", 0x813B}, // SP 0x307F30 MP 0x08FCC0
		{"freeentitysentient", 0x813C}, // SP 0x307650 MP 0x08FDE0
		{"isindoor", 0x813D}, // SP 0x307780 MP 0x000000
		{"_meth_813e", 0x813E}, // SP 0x3077E0 MP 0x000000
		{"_meth_813f", 0x813F}, // SP 0x3078C0 MP 0x000000
		{"_meth_8140", 0x8140}, // SP 0x2B7D60 MP 0x000000
		{"_meth_8142", 0x8142}, // SP 0x2B7DA0 MP 0x000000
		{"_meth_8143", 0x8143}, // SP 0x2B7D40 MP 0x000000
		{"_meth_8144", 0x8144}, // SP 0x2B7DE0 MP 0x000000
		{"_meth_8145", 0x8145}, // SP 0x2B7E60 MP 0x000000
		{"_meth_8146", 0x8146}, // SP 0x2B7E80 MP 0x000000
		{"_meth_8147", 0x8147}, // SP 0x2B7E90 MP 0x000000
		{"_meth_8148", 0x8148}, // SP 0x2B7EB0 MP 0x000000
		{"_meth_8149", 0x8149}, // SP 0x2B7ED0 MP 0x000000
		{"_meth_814a", 0x814A}, // SP 0x2B7EF0 MP 0x000000
		{"_meth_814b", 0x814B}, // SP 0x2B7F00 MP 0x000000
		{"_meth_814c", 0x814C}, // SP 0x2B7F20 MP 0x000000
		{"_meth_814d", 0x814D}, // SP 0x2B7F40 MP 0x000000
		{"_meth_814e", 0x814E}, // SP 0x2B7F60 MP 0x000000
		{"_meth_814f", 0x814F}, // SP 0x2B7F70 MP 0x000000
		{"_meth_8150", 0x8150}, // SP 0x2B7F90 MP 0x000000
		{"_meth_8151", 0x8151}, // SP 0x2B7FB0 MP 0x000000
		{"_meth_8152", 0x8152}, // SP 0x2B8020 MP 0x000000
		{"_meth_8153", 0x8153}, // SP 0x2B8060 MP 0x000000
		{"_meth_8154", 0x8154}, // SP 0x2B80B0 MP 0x000000
		{"_meth_8155", 0x8155}, // SP 0x2B80D0 MP 0x000000
		{"_meth_8156", 0x8156}, // SP 0x2CAA50 MP 0x44C4A0
		{"_meth_8157", 0x8157}, // SP 0x2CB3E0 MP 0x44CD00
		{"_meth_8158", 0x8158}, // SP 0x2CB4F0 MP 0x44D2D0
		{"_meth_8159", 0x8159}, // SP 0x2CB780 MP 0x44D610
		{"_meth_815a", 0x815A}, // SP 0x2CBB90 MP 0x44DA10
		{"setautorotationdelay", 0x815B}, // SP 0x2CBD60 MP 0x435380
		{"_meth_815c", 0x815C}, // SP 0x2CC3E0 MP 0x435AF0
		{"restoredefaultdroppitch", 0x815D}, // SP 0x2CC5A0 MP 0x435F70
		{"turretfiredisable", 0x815E}, // SP 0x2CC660 MP 0x4360F0
		{"_meth_815f", 0x815F}, // SP 0x275890 MP 0x000000
		{"_meth_8160", 0x8160}, // SP 0x275960 MP 0x000000
		{"_meth_8161", 0x8161}, // SP 0x275A40 MP 0x000000
		{"_meth_8162", 0x8162}, // SP 0x275AE0 MP 0x000000
		{"_meth_8163", 0x8163}, // SP 0x275C30 MP 0x000000
		{"_meth_8164", 0x8164}, // SP 0x275D10 MP 0x000000
		{"_meth_8165", 0x8165}, // SP 0x275DA0 MP 0x000000
		{"_meth_8166", 0x8166}, // SP 0x275DE0 MP 0x000000
		{"_meth_8167", 0x8167}, // SP 0x275EC0 MP 0x000000
		{"_meth_8168", 0x8168}, // SP 0x275F50 MP 0x000000
		{"_meth_8169", 0x8169}, // SP 0x2907C0 MP 0x40BB60
		{"_meth_816a", 0x816A}, // SP 0x275FF0 MP 0x000000
		{"_meth_816b", 0x816B}, // SP 0x2761C0 MP 0x000000
		{"_meth_816c", 0x816C}, // SP 0x2763C0 MP 0x000000
		{"_meth_816d", 0x816D}, // SP 0x2764E0 MP 0x000000
		{"_meth_816e", 0x816E}, // SP 0x2765B0 MP 0x000000
		{"_meth_816f", 0x816F}, // SP 0x2766D0 MP 0x000000
		{"_meth_8170", 0x8170}, // SP 0x276760 MP 0x000000
		{"_meth_8171", 0x8171}, // SP 0x2768A0 MP 0x000000
		{"_meth_8172", 0x8172}, // SP 0x276910 MP 0x000000
		{"_meth_8173", 0x8173}, // SP 0x276A00 MP 0x000000
		{"_meth_8174", 0x8174}, // SP 0x276B70 MP 0x000000
		{"_meth_8175", 0x8175}, // SP 0x276DC0 MP 0x000000
		{"_meth_8176", 0x8176}, // SP 0x276E20 MP 0x000000
		{"_meth_8177", 0x8177}, // SP 0x276E80 MP 0x000000
		{"_meth_8178", 0x8178}, // SP 0x276F40 MP 0x000000
		{"getenemysqdist", 0x8179}, // SP 0x307920 MP 0x08F660
		{"_meth_817a", 0x817A}, // SP 0x307960 MP 0x08F6A0
		{"setthreatbiasgroup", 0x817B}, // SP 0x307D30 MP 0x08FA80
		{"getthreatbiasgroup", 0x817C}, // SP 0x307DA0 MP 0x08FAF0
		{"turretfireenable", 0x817D}, // SP 0x2CC740 MP 0x436150
		{"setturretmodechangewait", 0x817E}, // SP 0x2CCA50 MP 0x436430
		{"usetriggerrequirelookat", 0x817F}, // SP 0x2CBD00 MP 0x44A940
		{"getstance", 0x8180}, // SP 0x2D01F0 MP 0x437D20
		{"setstance", 0x8181}, // SP 0x2D0510 MP 0x4383D0
		{"itemweaponsetammo", 0x8182}, // SP 0x2CD4C0 MP 0x44D340
		{"getammocount", 0x8183}, // SP 0x2CDBC0 MP 0x44D8B0
		{"gettagorigin", 0x8184}, // SP 0x2BC330 MP 0x43CD60
		{"gettagangles", 0x8185}, // SP 0x2BC400 MP 0x43D0C0
		{"shellshock", 0x8186}, // SP 0x2C1C50 MP 0x441280
		{"stunplayer", 0x8187}, // SP 0x2C2470 MP 0x441FD0
		{"stopshellshock", 0x8188}, // SP 0x2CE9C0 MP 0x438910
		{"_meth_8189", 0x8189}, // SP 0x2CEC20 MP 0x438B30
		{"setdepthoffield", 0x818A}, // SP 0x2BC900 MP 0x43D7F0
		{"setviewmodeldepthoffield", 0x818B}, // SP 0x2BD7B0 MP 0x43E3B0
		{"_meth_818c", 0x818C}, // SP 0x2BE760 MP 0x43F610
		{"_meth_818d", 0x818D}, // SP 0x273F90 MP 0x000000
		{"_meth_818e", 0x818E}, // SP 0x273FE0 MP 0x000000
		{"_meth_818f", 0x818F}, // SP 0x274050 MP 0x000000
		{"_meth_8190", 0x8190}, // SP 0x274090 MP 0x000000
		{"_meth_8191", 0x8191}, // SP 0x274100 MP 0x000000
		{"_meth_8192", 0x8192}, // SP 0x274190 MP 0x000000
		{"_meth_8193", 0x8193}, // SP 0x274360 MP 0x000000
		{"_meth_8194", 0x8194}, // SP 0x2746C0 MP 0x000000
		{"_meth_8195", 0x8195}, // SP 0x2747C0 MP 0x000000
		{"_meth_8196", 0x8196}, // SP 0x274810 MP 0x000000
		{"_meth_8197", 0x8197}, // SP 0x274860 MP 0x000000
		{"_meth_8198", 0x8198}, // SP 0x2748C0 MP 0x000000
		{"_meth_8199", 0x8199}, // SP 0x274910 MP 0x000000
		{"_meth_819e", 0x819E}, // SP 0x2749E0 MP 0x000000
		{"_meth_819f", 0x819F}, // SP 0x274A90 MP 0x000000
		{"_meth_81a0", 0x81A0}, // SP 0x274AE0 MP 0x000000
		{"getnegotiationstartnode", 0x81A1}, // SP 0x274B90 MP 0x51C5D0
		{"getnegotiationendnode", 0x81A2}, // SP 0x274C00 MP 0x51C480
		{"getnegotiationnextnode", 0x81A3}, // SP 0x274C70 MP 0x51C520
		{"_meth_81a4", 0x81A4}, // SP 0x274D50 MP 0x000000
		{"_meth_81a5", 0x81A5}, // SP 0x274DF0 MP 0x000000
		{"_meth_81a6", 0x81A6}, // SP 0x274EE0 MP 0x000000
		{"_meth_81a7", 0x81A7}, // SP 0x275000 MP 0x000000
		{"_meth_81a8", 0x81A8}, // SP 0x2738E0 MP 0x000000
		{"_meth_81a9", 0x81A9}, // SP 0x275060 MP 0x000000
		{"_meth_81aa", 0x81AA}, // SP 0x275200 MP 0x000000
		{"_meth_81ab", 0x81AB}, // SP 0x275330 MP 0x000000
		{"_meth_81ac", 0x81AC}, // SP 0x2753D0 MP 0x000000
		{"_meth_81ad", 0x81AD}, // SP 0x275510 MP 0x000000
		{"_meth_81ae", 0x81AE}, // SP 0x275700 MP 0x000000
		{"_meth_81af", 0x81AF}, // SP 0x275750 MP 0x000000
		{"_meth_81b0", 0x81B0}, // SP 0x2757A0 MP 0x000000
		{"_meth_81b1", 0x81B1}, // SP 0x2BE800 MP 0x43F660
		{"_meth_81b2", 0x81B2}, // SP 0x2BE810 MP 0x43F690
		{"viewkick", 0x81B3}, // SP 0x2BE960 MP 0x43F7C0
		{"localtoworldcoords", 0x81B4}, // SP 0x2CE230 MP 0x437DA0
		{"getentitynumber", 0x81B5}, // SP 0x2CCC90 MP 0x4367D0
		{"getentityvelocity", 0x81B6}, // SP 0x2CCE50 MP 0x436960
		{"enablegrenadetouchdamage", 0x81B7}, // SP 0x2CCFA0 MP 0x436AD0
		{"disablegrenadetouchdamage", 0x81B8}, // SP 0x2CD060 MP 0x436B80
		{"_meth_81b9", 0x81B9}, // SP 0x2CDE90 MP 0x4379C0
		{"_meth_81ba", 0x81BA}, // SP 0x277290 MP 0x000000
		{"_meth_81bb", 0x81BB}, // SP 0x277470 MP 0x000000
		{"_meth_81bc", 0x81BC}, // SP 0x2776C0 MP 0x000000
		{"_meth_81bd", 0x81BD}, // SP 0x277810 MP 0x000000
		{"_meth_81be", 0x81BE}, // SP 0x2778A0 MP 0x000000
		{"_meth_81bf", 0x81BF}, // SP 0x277C50 MP 0x000000
		{"_meth_81c0", 0x81C0}, // SP 0x277DA0 MP 0x000000
		{"_meth_81c1", 0x81C1}, // SP 0x277E90 MP 0x000000
		{"_meth_81c2", 0x81C2}, // SP 0x277F00 MP 0x000000
		{"_meth_81c3", 0x81C3}, // SP 0x277FB0 MP 0x000000
		{"_meth_81c4", 0x81C4}, // SP 0x3076A0 MP 0x08FE40
		{"_meth_81c5", 0x81C5}, // SP 0x307700 MP 0x08FEA0
		{"_meth_81c6", 0x81C6}, // SP 0x278100 MP 0x000000
		{"_meth_81c7", 0x81C7}, // SP 0x278380 MP 0x000000
		{"_meth_81c8", 0x81C8}, // SP 0x2784F0 MP 0x000000
		{"_meth_81c9", 0x81C9}, // SP 0x2733B0 MP 0x000000
		{"_meth_81ca", 0x81CA}, // SP 0x2733C0 MP 0x000000
		{"_meth_81cb", 0x81CB}, // SP 0x2733E0 MP 0x000000
		{"_meth_81cc", 0x81CC}, // SP 0x273400 MP 0x000000
		{"_meth_81cd", 0x81CD}, // SP 0x273460 MP 0x000000
		{"_meth_81ce", 0x81CE}, // SP 0x2734D0 MP 0x000000
		{"_meth_81cf", 0x81CF}, // SP 0x2735A0 MP 0x000000
		{"_meth_81d0", 0x81D0}, // SP 0x273690 MP 0x000000
		{"_meth_81d1", 0x81D1}, // SP 0x2736D0 MP 0x000000
		{"_meth_81d2", 0x81D2}, // SP 0x273710 MP 0x000000
		{"_meth_81d3", 0x81D3}, // SP 0x273750 MP 0x000000
		{"_meth_81d4", 0x81D4}, // SP 0x273DC0 MP 0x000000
		{"_meth_81d5", 0x81D5}, // SP 0x273EC0 MP 0x000000
		{"_meth_81d6", 0x81D6}, // SP 0x273AE0 MP 0x000000
		{"_meth_81d7", 0x81D7}, // SP 0x2CDFF0 MP 0x437BC0
		{"entityradiusdamage", 0x81D8}, // SP 0x2C0610 MP 0x43FB20
		{"detonate", 0x81D9}, // SP 0x2C0EF0 MP 0x440460
		{"damageconetrace", 0x81DA}, // SP 0x2C17C0 MP 0x440DC0
		{"_meth_81db", 0x81DB}, // SP 0x2C1BB0 MP 0x4410B0
		{"_meth_81dc", 0x81DC}, // SP 0x2CD3B0 MP 0x436FF0
		{"_meth_81dd", 0x81DD}, // SP 0x2CD710 MP 0x437290
		{"_meth_81de", 0x81DE}, // SP 0x2CD7D0 MP 0x437350
		{"missilesetflightmodedirect", 0x81DF}, // SP 0x2CDAF0 MP 0x4375F0
		{"missilesetflightmodetop", 0x81E0}, // SP 0x2CDC70 MP 0x4377D0
		{"getlightintensity", 0x81E1}, // SP 0x2C2880 MP 0x443780
		{"setlightintensity", 0x81E2}, // SP 0x2C2990 MP 0x443850
		{"isragdoll", 0x81E3}, // SP 0x2C3140 MP 0x444150
		{"setmovespeedscale", 0x81E4}, // SP 0x2BE470 MP 0x440680
		{"cameralinkto", 0x81E5}, // SP 0x2C4270 MP 0x444FA0
		{"cameraunlink", 0x81E6}, // SP 0x2C4560 MP 0x4451A0
		{"_meth_81e7", 0x81E7}, // SP 0x275180 MP 0x000000
		{"_meth_81e8", 0x81E8}, // SP 0x275450 MP 0x000000
		{"_meth_81e9", 0x81E9}, // SP 0x275490 MP 0x000000
		{"_meth_81ea", 0x81EA}, // SP 0x2755F0 MP 0x000000
		{"_meth_81eb", 0x81EB}, // SP 0x2758F0 MP 0x000000
		{"_meth_81ec", 0x81EC}, // SP 0x2759B0 MP 0x000000
		{"_meth_81ed", 0x81ED}, // SP 0x275C90 MP 0x000000
		{"_meth_81ee", 0x81EE}, // SP 0x275D50 MP 0x000000
		{"_meth_81ef", 0x81EF}, // SP 0x275E60 MP 0x000000
		{"_meth_81f0", 0x81F0}, // SP 0x275F10 MP 0x000000
		{"_meth_81f1", 0x81F1}, // SP 0x275FA0 MP 0x000000
		{"_meth_81f2", 0x81F2}, // SP 0x2760E0 MP 0x000000
		{"_meth_81f3", 0x81F3}, // SP 0x276130 MP 0x000000
		{"_meth_81f4", 0x81F4}, // SP 0x2762F0 MP 0x000000
		{"_meth_81f5", 0x81F5}, // SP 0x276360 MP 0x000000
		{"_meth_81f6", 0x81F6}, // SP 0x276470 MP 0x000000
		{"_meth_81f7", 0x81F7}, // SP 0x276610 MP 0x000000
		{"_meth_81f8", 0x81F8}, // SP 0x276670 MP 0x000000
		{"_meth_81f9", 0x81F9}, // SP 0x276540 MP 0x000000
		{"_meth_81fa", 0x81FA}, // SP 0x276720 MP 0x000000
		{"_meth_81fb", 0x81FB}, // SP 0x2767D0 MP 0x000000
		{"_meth_81fc", 0x81FC}, // SP 0x2769A0 MP 0x000000
		{"_meth_81fd", 0x81FD}, // SP 0x276A70 MP 0x000000
		{"_meth_81fe", 0x81FE}, // SP 0x276C40 MP 0x000000
		{"_meth_81ff", 0x81FF}, // SP 0x276EF0 MP 0x000000
		{"_meth_8200", 0x8200}, // SP 0x277050 MP 0x000000
		{"_meth_8201", 0x8201}, // SP 0x277150 MP 0x000000
		{"_meth_8202", 0x8202}, // SP 0x2774E0 MP 0x000000
		{"_meth_8203", 0x8203}, // SP 0x2775D0 MP 0x000000
		{"controlslinkto", 0x8204}, // SP 0x2C4D60 MP 0x445B00
		{"controlsunlink", 0x8205}, // SP 0x2C5110 MP 0x445DA0
		{"_meth_8206", 0x8206}, // SP 0x000000 MP 0x4477D0
		{"_meth_8207", 0x8207}, // SP 0x2DE440 MP 0x000000
		{"_meth_8208", 0x8208}, // SP 0x000000 MP 0x447AF0
		{"_meth_820a", 0x820A}, // SP 0x000000 MP 0x447B90
		{"_meth_820b", 0x820B}, // SP 0x000000 MP 0x447D50
		{"_meth_820c", 0x820C}, // SP 0x000000 MP 0x447F70
		{"_meth_820d", 0x820D}, // SP 0x000000 MP 0x447DF0
		{"_meth_820e", 0x820E}, // SP 0x000000 MP 0x447E20
		{"drivevehicleandcontrolturret", 0x820F}, // SP 0x2C6930 MP 0x4491A0
		{"drivevehicleandcontrolturretoff", 0x8210}, // SP 0x2C7540 MP 0x449630
		{"_meth_8211", 0x8211}, // SP 0x2C7C20 MP 0x449AE0
		{"_meth_8212", 0x8212}, // SP 0x2C8D20 MP 0x44A0E0
		{"_meth_8213", 0x8213}, // SP 0x2C9840 MP 0x44AE90
		{"_meth_8214", 0x8214}, // SP 0x2CB450 MP 0x44C950
		{"_meth_8215", 0x8215}, // SP 0x2CB4A0 MP 0x44CB00
		{"_meth_8216", 0x8216}, // SP 0x2CB5D0 MP 0x44CCA0
		{"_meth_8217", 0x8217}, // SP 0x2CB720 MP 0x44CCF0
		{"circle", 0x8218}, // SP 0x2D0720 MP 0x439750 - iw6 name is WorldPointInReticle_Circle
		{"rect", 0x8219}, // SP 0x2D12D0 MP 0x439F20
		{"getpointinbounds", 0x821A}, // SP 0x2D1850 MP 0x43A680
		{"transfermarkstonewscriptmodel", 0x821B}, // SP 0x2D1FC0 MP 0x43B020
		{"setwatersheeting", 0x821C}, // SP 0x2C47C0 MP 0x445400
		{"_meth_821d", 0x821D}, // SP 0x2C02D0 MP 0x000000
		{"_meth_821e", 0x821E}, // SP 0x2C0A60 MP 0x000000
		{"setweaponhudiconoverride", 0x821F}, // SP 0x2CB930 MP 0x44D150
		{"getweaponhudiconoverride", 0x8220}, // SP 0x2CC1D0 MP 0x44D750
		{"_meth_8221", 0x8221}, // SP 0x2CC6B0 MP 0x4356B0
		{"_meth_8222", 0x8222}, // SP 0x2CDCF0 MP 0x437140
		{"_meth_8223", 0x8223}, // SP 0x2CD9F0 MP 0x436F30
		{"_meth_8224", 0x8224}, // SP 0x2CE3A0 MP 0x437750
		{"isitemunlocked", 0x8225}, // SP 0x2D1E80 MP 0x438700
		{"getrankedplayerdata", 0x8226}, // SP 0x2C2210 MP 0x4432D0
		{"vehicleturretcontroloff", 0x8227}, // SP 0x4E7230 MP 0x60E170
		{"isturretready", 0x8228}, // SP 0x4E7310 MP 0x609890
		{"_meth_8229", 0x8229}, // SP 0x4E7560 MP 0x609B20
		{"vehicle_dospawn", 0x822A}, // SP 0x4E76C0 MP 0x609C40
		{"vehicle_isphysveh", 0x822B}, // SP 0x4E7790 MP 0x609D30
		{"vehphys_crash", 0x822C}, // SP 0x4E7830 MP 0x609E30
		{"vehphys_launch", 0x822D}, // SP 0x4E7970 MP 0x609F50
		{"vehphys_disablecrashing", 0x822E}, // SP 0x4E7B40 MP 0x60A0F0
		{"vehphys_enablecrashing", 0x822F}, // SP 0x4E7C30 MP 0x60A1C0
		{"vehphys_setspeed", 0x8230}, // SP 0x4E7CB0 MP 0x60A290
		{"vehphys_setconveyorbelt", 0x8231}, // SP 0x4E7E80 MP 0x60A4C0
		{"freevehicle", 0x8232}, // SP 0x000000 MP 0x60C1B0
		{"_meth_8233", 0x8233}, // SP 0x2C3800 MP 0x000000
		{"_meth_8234", 0x8234}, // SP 0x2C3D60 MP 0x000000
		{"_meth_8235", 0x8235}, // SP 0x2C4410 MP 0x000000
		{"_meth_8236", 0x8236}, // SP 0x2C4730 MP 0x000000
		{"_meth_8237", 0x8237}, // SP 0x2C4EF0 MP 0x000000
		{"_meth_8238", 0x8238}, // SP 0x2C5310 MP 0x000000
		{"_meth_8239", 0x8239}, // SP 0x2CF5B0 MP 0x000000
		{"_meth_823a", 0x823A}, // SP 0x2CF7D0 MP 0x000000
		{"_meth_823b", 0x823B}, // SP 0x2CF990 MP 0x000000
		{"_meth_823c", 0x823C}, // SP 0x2D0070 MP 0x000000
		{"_meth_823d", 0x823D}, // SP 0x2C86C0 MP 0x000000
		{"_meth_823e", 0x823E}, // SP 0x2C8A50 MP 0x000000
		{"_meth_8240", 0x8240}, // SP 0x2C4AD0 MP 0x000000
		{"_meth_8241", 0x8241}, // SP 0x274CF0 MP 0x000000
		{"_meth_8242", 0x8242}, // SP 0x2750F0 MP 0x000000
		{"setcommonplayerdata", 0x8243}, // SP 0x2C2550 MP 0x443C20 - Could be also setrankedplayerdata
		{"_meth_8244", 0x8244}, // SP 0x2C2A00 MP 0x4442D0
		{"_meth_8245", 0x8245}, // SP 0x2C3190 MP 0x444850
		{"trackerupdate", 0x8246}, // SP 0x2C51C0 MP 0x447650
		{"pingplayer", 0x8247}, // SP 0x000000 MP 0x412070
		{"buttonpressed", 0x8248}, // SP 0x294C60 MP 0x411F90
		{"sayteam", 0x8249}, // SP 0x000000 MP 0x410520
		{"sayall", 0x824A}, // SP 0x000000 MP 0x4105D0
		{"setspawnweapon", 0x824B}, // SP 0x000000 MP 0x4120E0
		{"dropitem", 0x824C}, // SP 0x292EF0 MP 0x406060
		{"dropscavengerbag", 0x824D}, // SP 0x000000 MP 0x4124F0
		{"setjitterparams", 0x824E}, // SP 0x4E4DA0 MP 0x60BE40
		{"sethoverparams", 0x824F}, // SP 0x4E4E70 MP 0x60BF10
		{"joltbody", 0x8250}, // SP 0x4E4F90 MP 0x60BFB0
		{"freevehicle", 0x8251}, // SP 0x4E5190 MP 0x60C1B0
		{"_meth_8252", 0x8252}, // SP 0x4E5240 MP 0x60C220
		{"getvehicleowner", 0x8253}, // SP 0x4E5490 MP 0x60C470
		{"setvehiclelookattext", 0x8254}, // SP 0x4E5510 MP 0x60C4F0
		{"setvehicleteam", 0x8255}, // SP 0x4E55A0 MP 0x60C580
		{"neargoalnotifydist", 0x8256}, // SP 0x4E5630 MP 0x60C710
		{"setgoalpos", 0x8257}, // SP 0x4E56A0 MP 0x60C780
		{"setgoalyaw", 0x8258}, // SP 0x4E57B0 MP 0x60C8A0
		{"cleargoalyaw", 0x8259}, // SP 0x4E5840 MP 0x60C930
		{"settargetyaw", 0x825A}, // SP 0x4E58C0 MP 0x60C9B0
		{"cleartargetyaw", 0x825B}, // SP 0x4E5950 MP 0x60CA40
		{"helisetgoal", 0x825C}, // SP 0x4E59D0 MP 0x60CAC0
		{"_meth_825d", 0x825D}, // SP 0x4E62A0 MP 0x60D360
		{"setturrettargetent", 0x825E}, // SP 0x4E63A0 MP 0x60D460
		{"clearturrettargetent", 0x825F}, // SP 0x4E64A0 MP 0x60D560
		{"canturrettargetpoint", 0x8260}, // SP 0x4E6530 MP 0x60D5F0
		{"setlookatent", 0x8261}, // SP 0x4E6840 MP 0x60D8F0
		{"clearlookatent", 0x8262}, // SP 0x4E68F0 MP 0x60D9A0
		{"setvehweapon", 0x8263}, // SP 0x4E6A50 MP 0x60DA80
		{"fireweapon", 0x8264}, // SP 0x4E6AC0 MP 0x60DAF0
		{"vehicleturretcontrolon", 0x8265}, // SP 0x4E71A0 MP 0x60E0F0
		{"finishplayerdamage", 0x8266}, // SP 0x000000 MP 0x40F530
		{"suicide", 0x8267}, // SP 0x000000 MP 0x40FD30
		{"_meth_8268", 0x8268}, // SP 0x000000 MP 0x40FF20
		{"clientiprintln", 0x8269}, // SP 0x000000 MP 0x40FFD0
		{"clientiprintlnbold", 0x826A}, // SP 0x000000 MP 0x410050
		{"spawnspectator", 0x826B}, // SP 0x000000 MP 0x4100D0
		{"_meth_826c", 0x826C}, // SP 0x000000 MP 0x410160
		{"cloneplayer", 0x826D}, // SP 0x000000 MP 0x4101C0
		{"istalking", 0x826E}, // SP 0x000000 MP 0x4104A0
		{"allowspectateteam", 0x826F}, // SP 0x000000 MP 0x410680
		{"_meth_8270", 0x8270}, // SP 0x000000 MP 0x4108B0
		{"getguid", 0x8271}, // SP 0x000000 MP 0x410A50
		{"_meth_8272", 0x8272}, // SP 0x2E2760 MP 0x454340
		{"_meth_8273", 0x8273}, // SP 0x2E2990 MP 0x454590
		{"clonebrushmodeltoscriptmodel", 0x8274}, // SP 0x2E30A0 MP 0x454CC0
		{"scriptmodelplayanim", 0x8275}, // SP 0x000000 MP 0x454D90
		{"scriptmodelclearanim", 0x8276}, // SP 0x000000 MP 0x455190
		{"_meth_8277", 0x8277}, // SP 0x000000 MP 0x454FB0
		{"_meth_8278", 0x8278}, // SP 0x4E8090 MP 0x60A6A0
		{"attachpath", 0x8279}, // SP 0x4E8230 MP 0x60A840
		{"getattachpos", 0x827A}, // SP 0x4E8350 MP 0x60A970
		{"startpath", 0x827B}, // SP 0x4E8520 MP 0x60AB40
		{"setswitchnode", 0x827C}, // SP 0x4E87C0 MP 0x60ACD0
		{"setwaitspeed", 0x827D}, // SP 0x4E88C0 MP 0x60AD60
		{"finishdamage", 0x827E}, // SP 0x000000 MP 0x60ADE0
		{"vehicle_setspeed", 0x827F}, // SP 0x4E3EF0 MP 0x60B040
		{"vehicle_setspeedimmediate", 0x8280}, // SP 0x4E3F60 MP 0x60B0B0
		{"vehicle_rotateyaw", 0x8281}, // SP 0x4E4110 MP 0x60B260
		{"vehicle_getspeed", 0x8282}, // SP 0x4E4230 MP 0x60B380
		{"vehicle_getvelocity", 0x8283}, // SP 0x4E4320 MP 0x60B470
		{"vehicle_getbodyvelocity", 0x8284}, // SP 0x4E4390 MP 0x60B4E0
		{"vehicle_getsteering", 0x8285}, // SP 0x4E4400 MP 0x60B550
		{"vehicle_getthrottle", 0x8286}, // SP 0x4E4480 MP 0x60B5E0
		{"vehicle_turnengineoff", 0x8287}, // SP 0x4E44F0 MP 0x60B650
		{"vehicle_turnengineon", 0x8288}, // SP 0x4E4550 MP 0x60B6C0
		{"vehicle_orientto", 0x8289}, // SP 0x4E45B0 MP 0x000000
		{"getgoalspeedmph", 0x828A}, // SP 0x4E4690 MP 0x60B730
		{"setacceleration", 0x828B}, // SP 0x4E4700 MP 0x60B7A0
		{"setdeceleration", 0x828C}, // SP 0x4E4770 MP 0x60B810
		{"resumespeed", 0x828D}, // SP 0x4E47E0 MP 0x60B880
		{"setyawspeed", 0x828E}, // SP 0x4E4870 MP 0x60B910
		{"setyawspeedbyname", 0x828F}, // SP 0x4E49D0 MP 0x60BA70
		{"setmaxpitchroll", 0x8290}, // SP 0x4E4BE0 MP 0x60BC80
		{"setairresitance", 0x8291}, // SP 0x4E4CA0 MP 0x60BD40
		{"setturningability", 0x8292}, // SP 0x4E4D30 MP 0x60BDD0
		{"getxuid", 0x8293}, // SP 0x28EA30 MP 0x410AC0
		{"getucdidhigh", 0x8294}, // SP 0x000000 MP 0x410C80
		{"ismlgspectator", 0x8295}, // SP 0x000000 MP 0x410CE0
		{"getclanidhigh", 0x8296}, // SP 0x000000 MP 0x410D40
		{"getclanidlow", 0x8297}, // SP 0x000000 MP 0x410DA0
		{"ishost", 0x8298}, // SP 0x000000 MP 0x410E00
		{"getspectatingplayer", 0x8299}, // SP 0x000000 MP 0x410E90
		{"predictstreampos", 0x829A}, // SP 0x000000 MP 0x410F10
		{"updatescores", 0x829B}, // SP 0x000000 MP 0x411030
		{"updatedmscores", 0x829C}, // SP 0x000000 MP 0x411040
		{"setrank", 0x829D}, // SP 0x000000 MP 0x411050
		{"_meth_829e", 0x829E}, // SP 0x000000 MP 0x4110F0
		{"setmlgspectator", 0x829F}, // SP 0x28E1B0 MP 0x409FD0
		{"visionsyncwithplayer", 0x82A0}, // SP 0x000000 MP 0x40C160
		{"showhudsplash", 0x82A1}, // SP 0x291570 MP 0x40D190
		{"setperk", 0x82A2}, // SP 0x293310 MP 0x406B20
		{"hasperk", 0x82A3}, // SP 0x293820 MP 0x407100
		{"clearperks", 0x82A4}, // SP 0x294030 MP 0x407860
		{"unsetperk", 0x82A5}, // SP 0x293BC0 MP 0x4074A0
		{"registerparty", 0x82A6}, // SP 0x000000 MP 0x411740
		{"getfireteammembers", 0x82A7}, // SP 0x000000 MP 0x411870
		{"noclip", 0x82A8}, // SP 0x294080 MP 0x407CF0
		{"ufo", 0x82A9}, // SP 0x294230 MP 0x407D00
		{"moveto", 0x82AA}, // SP 0x2E1470 MP 0x453290
		{"movex", 0x82AB}, // SP 0x2E19A0 MP 0x4537F0
		{"movey", 0x82AC}, // SP 0x2E19B0 MP 0x453800
		{"movez", 0x82AD}, // SP 0x2E19D0 MP 0x453820
		{"movegravity", 0x82AE}, // SP 0x2E1610 MP 0x453440
		{"moveslide", 0x82AF}, // SP 0x2E1790 MP 0x4535D0
		{"stopmoveslide", 0x82B0}, // SP 0x2E1920 MP 0x453760
		{"rotateto", 0x82B1}, // SP 0x2E1A70 MP 0x453850
		{"rotatepitch", 0x82B2}, // SP 0x2E1EB0 MP 0x4539A0
		{"rotateyaw", 0x82B3}, // SP 0x2E1EC0 MP 0x4539B0
		{"rotateroll", 0x82B4}, // SP 0x2E1EE0 MP 0x4539D0
		{"addpitch", 0x82B5}, // SP 0x2E1E60 MP 0x453950
		{"addyaw", 0x82B6}, // SP 0x2E1E80 MP 0x453970
		{"addroll", 0x82B7}, // SP 0x2E1EA0 MP 0x453990
		{"vibrate", 0x82B8}, // SP 0x2E1F00 MP 0x4539F0
		{"rotatevelocity", 0x82B9}, // SP 0x2E2160 MP 0x453D00
		{"solid", 0x82BA}, // SP 0x2E3180 MP 0x455340
		{"notsolid", 0x82BB}, // SP 0x2E3230 MP 0x452FA0
		{"setcandamage", 0x82BC}, // SP 0x2E2300 MP 0x453ED0
		{"setcanradiusdamage", 0x82BD}, // SP 0x2E2360 MP 0x453F30
		{"physicslaunchclient", 0x82BE}, // SP 0x2E23E0 MP 0x453FB0
		{"setcardicon", 0x82BF}, // SP 0x000000 MP 0x411100
		{"setcardnameplate", 0x82C0}, // SP 0x000000 MP 0x411110
		{"setcarddisplayslot", 0x82C1}, // SP 0x000000 MP 0x411120
		{"regweaponforfxremoval", 0x82C2}, // SP 0x000000 MP 0x411230
		{"laststandrevive", 0x82C3}, // SP 0x000000 MP 0x411290
		{"startlaststand", 0x82C4}, // SP 0x000000 MP 0x411300
		{"setspectatedefaults", 0x82C5}, // SP 0x000000 MP 0x411370
		{"getthirdpersoncrosshairoffset", 0x82C6}, // SP 0x000000 MP 0x4115D0
		{"disableweaponpickup", 0x82C7}, // SP 0x290660 MP 0x40C550
		{"enableweaponpickup", 0x82C8}, // SP 0x290770 MP 0x40C640
		{"issplitscreenplayer", 0x82C9}, // SP 0x290840 MP 0x40C750
		{"issplitscreenplayerprimary", 0x82CA}, // SP 0x000000 MP 0x411640
		{"getweaponslistoffhands", 0x82CB}, // SP 0x290060 MP 0x40B8B0
		{"getweaponslistitems", 0x82CC}, // SP 0x290170 MP 0x40B9C0
		{"getweaponslistexclusives", 0x82CD}, // SP 0x290280 MP 0x40BBE0
		{"getweaponslist", 0x82CE}, // SP 0x290410 MP 0x40BCF0
		{"canplayerplacesentry", 0x82CF}, // SP 0x292B20 MP 0x4063C0
		{"canplayerplacetank", 0x82D0}, // SP 0x293020 MP 0x406670
		{"visionsetnakedforplayer", 0x82D1}, // SP 0x291110 MP 0x40D290
		{"visionsetnightforplayer", 0x82D2}, // SP 0x2913D0 MP 0x40D2A0
		{"visionsetmissilecamforplayer", 0x82D3}, // SP 0x2913F0 MP 0x40D2C0
		{"visionsetthermalforplayer", 0x82D4}, // SP 0x291530 MP 0x40D3E0
		{"visionsetpainforplayer", 0x82D5}, // SP 0x291550 MP 0x40D400
		{"setblurforplayer", 0x82D6}, // SP 0x292660 MP 0x40E430
		{"getplayerweaponmodel", 0x82D7}, // SP 0x292A50 MP 0x4061A0
		{"getplayerknifemodel", 0x82D8}, // SP 0x292B00 MP 0x4061C0
		{"updateplayermodelwithweapons", 0x82D9}, // SP 0x2D70D0 MP 0x000000
		{"notifyonplayercommand", 0x82DA}, // SP 0x294240 MP 0x407D10
		{"canmantle", 0x82DB}, // SP 0x28CD90 MP 0x408310
		{"forcemantle", 0x82DC}, // SP 0x28CFB0 MP 0x408630
		{"ismantling", 0x82DD}, // SP 0x28D2C0 MP 0x408990
		{"playfx", 0x82DE}, // SP 0x28D630 MP 0x408EC0
		{"player_recoilscaleon", 0x82DF}, // SP 0x28D850 MP 0x409200
		{"player_recoilscaleoff", 0x82E0}, // SP 0x28D920 MP 0x4092D0
		{"weaponlockstart", 0x82E1}, // SP 0x28DA00 MP 0x409530
		{"weaponlockfinalize", 0x82E2}, // SP 0x28DCC0 MP 0x4097A0
		{"weaponlockfree", 0x82E3}, // SP 0x28DFC0 MP 0x409C50 - was disableautoreload
		{"setentertime", 0x82E4}, // SP 0x28E060 MP 0x409E60
		{"usinggamepad", 0x82E5}, // SP 0x000000 MP 0x411680
		{"_meth_82e6", 0x82E6}, // SP 0x000000 MP 0x411700
		{"_meth_82e7", 0x82E7}, // SP 0x000000 MP 0x411710
		{"_meth_82e8", 0x82E8}, // SP 0x000000 MP 0x411730
		{"_meth_82e9", 0x82E9}, // SP 0x000000 MP 0x411720
		{"_meth_82ea", 0x82EA}, // SP 0x2905B0 MP 0x40BAD0
		{"fragbuttonpressed", 0x82EB}, // SP 0x28F5E0 MP 0x40AB90
		{"secondaryoffhandbuttonpressed", 0x82EC}, // SP 0x28F6D0 MP 0x40ACC0
		{"issighted", 0x82ED}, // SP 0x2919D0 MP 0x40D2E0
		{"setvelocity", 0x82EE}, // SP 0x28DC30 MP 0x4090E0
		{"_meth_82ef", 0x82EF}, // SP 0x28E570 MP 0x409920
		{"getnormalizedmovement", 0x82F0}, // SP 0x28E980 MP 0x409B70
		{"playlocalsound", 0x82F1}, // SP 0x28DAC0 MP 0x409330
		{"stoplocalsound", 0x82F2}, // SP 0x28DBA0 MP 0x409420
		{"setweaponammoclip", 0x82F3}, // SP 0x2928A0 MP 0x405D60
		{"setweaponammostock", 0x82F4}, // SP 0x292D20 MP 0x4061E0
		{"getweaponammoclip", 0x82F5}, // SP 0x293200 MP 0x406730
		{"getweaponammostock", 0x82F6}, // SP 0x293450 MP 0x406970
		{"anyammoforweaponmodes", 0x82F7}, // SP 0x2936A0 MP 0x406D30
		{"setclientomnvar", 0x82F8}, // SP 0x293DA0 MP 0x4077B0
		{"setclientdvar", 0x82F9}, // SP 0x2942B0 MP 0x407D90
		{"setclientdvars", 0x82FA}, // SP 0x28C960 MP 0x4086D0
		{"setclientspawnsighttraces", 0x82FB}, // SP 0x000000 MP 0x408050
		{"clientspawnsighttracepassed", 0x82FC}, // SP 0x000000 MP 0x408200
		{"allowads", 0x82FD}, // SP 0x28EC70 MP 0x409ED0
		{"allowjump", 0x82FE}, // SP 0x28EFC0 MP 0x40A0B0
		{"allowladder", 0x82FF}, // SP 0x28F750 MP 0x40AE80
		{"allowmantle", 0x8300}, // SP 0x28F830 MP 0x40B030
		{"allowsprint", 0x8301}, // SP 0x28F910 MP 0x40B1A0
		{"setspreadoverride", 0x8302}, // SP 0x290890 MP 0x40C0D0
		{"resetspreadoverride", 0x8303}, // SP 0x2909D0 MP 0x40C4D0
		{"setaimspreadmovementscale", 0x8304}, // SP 0x290AA0 MP 0x40C5B0
		{"setactionslot", 0x8305}, // SP 0x28FB60 MP 0x40B340
		{"setviewkickscale", 0x8306}, // SP 0x28E330 MP 0x40A120
		{"getviewkickscale", 0x8307}, // SP 0x28E520 MP 0x40A250
		{"getweaponslistall", 0x8308}, // SP 0x28FDF0 MP 0x40B690
		{"getweaponslistprimaries", 0x8309}, // SP 0x28FF50 MP 0x40B7A0
		{"getnormalizedcameramovement", 0x830A}, // SP 0x28EBC0 MP 0x409CB0
		{"giveweapon", 0x830B}, // SP 0x290B90 MP 0x40C270
		{"takeweapon", 0x830C}, // SP 0x291080 MP 0x40C920
		{"takeallweapons", 0x830D}, // SP 0x2912B0 MP 0x40CB30
		{"getcurrentweapon", 0x830E}, // SP 0x291410 MP 0x40CED0
		{"getcurrentprimaryweapon", 0x830F}, // SP 0x291750 MP 0x40D0C0
		{"getcurrentoffhand", 0x8310}, // SP 0x291C00 MP 0x40D600
		{"hasweapon", 0x8311}, // SP 0x292480 MP 0x40E0D0
		{"switchtoweapon", 0x8312}, // SP 0x292A70 MP 0x40E5F0
		{"switchtoweaponimmediate", 0x8313}, // SP 0x292C90 MP 0x405F20
		{"gethybridscopestate", 0x8314}, // SP 0x2937D0 MP 0x406CD0
		{"switchtooffhand", 0x8315}, // SP 0x293A00 MP 0x406E20
		{"setoffhandsecondaryclass", 0x8316}, // SP 0x2920D0 MP 0x40DC70
		{"getoffhandSecondaryclass", 0x8317}, // SP 0x2923B0 MP 0x40DEF0
		{"beginlocationselection", 0x8318}, // SP 0x28E770 MP 0x409A10
		{"endlocationselection", 0x8319}, // SP 0x28EB50 MP 0x409D10
		{"disableweapons", 0x831A}, // SP 0x291CA0 MP 0x40DA50
		{"enableweapons", 0x831B}, // SP 0x291E70 MP 0x40DB40
		{"disableoffhandweapons", 0x831C}, // SP 0x291EE0 MP 0x40DD40
		{"enableoffhandweapons", 0x831D}, // SP 0x292080 MP 0x40DE90
		{"disableweaponswitch", 0x831E}, // SP 0x292430 MP 0x40E3D0
		{"enableweaponswitch", 0x831F}, // SP 0x292610 MP 0x40E570
		{"openpopupmenu", 0x8320}, // SP 0x290EF0 MP 0x40C7B0
		{"openpopupmenunomouse", 0x8321}, // SP 0x290F90 MP 0x40C9C0
		{"closepopupmenu", 0x8322}, // SP 0x291030 MP 0x40CBC0
		{"openmenu", 0x8323}, // SP 0x291130 MP 0x40CCE0
		{"closemenu", 0x8324}, // SP 0x291330 MP 0x40CFB0
		{"freezecontrols", 0x8326}, // SP 0x291880 MP 0x40D420
		{"disableusability", 0x8327}, // SP 0x291980 MP 0x40D6D0
		{"enableusability", 0x8328}, // SP 0x291BB0 MP 0x40D8D0
		{"setwhizbyspreads", 0x8329}, // SP 0x28CEE0 MP 0x408AD0
		{"setwhizbyradii", 0x832A}, // SP 0x28CFF0 MP 0x408CB0
		{"setreverb", 0x832B}, // SP 0x28D180 MP 0x408E90
		{"deactivatereverb", 0x832C}, // SP 0x28D460 MP 0x408EB0
		{"setvolmod", 0x832D}, // SP 0x28D730 MP 0x408FD0
		{"givestartammo", 0x832E}, // SP 0x293E20 MP 0x407000
		{"givemaxammo", 0x832F}, // SP 0x294090 MP 0x407300
		{"getfractionstartammo", 0x8330}, // SP 0x28C800 MP 0x407640
		{"getfractionmaxammo", 0x8331}, // SP 0x28CAC0 MP 0x407AF0
		{"isdualwielding", 0x8332}, // SP 0x28D0F0 MP 0x407FE0
		{"isreloading", 0x8333}, // SP 0x28D310 MP 0x408180
		{"isswitchingweapon", 0x8334}, // SP 0x28D3E0 MP 0x408360
		{"setorigin", 0x8335}, // SP 0x28DAE0 MP 0x408DA0
		{"getvelocity", 0x8336}, // SP 0x28DF50 MP 0x409180
		{"setplayerangles", 0x8337}, // SP 0x28E3B0 MP 0x409370
		{"getplayerangles", 0x8338}, // SP 0x28E430 MP 0x409600
		{"usebuttonpressed", 0x8339}, // SP 0x28F130 MP 0x40A3A0
		{"attackbuttonpressed", 0x833A}, // SP 0x28F220 MP 0x40A660
		{"adsbuttonpressed", 0x833B}, // SP 0x28F400 MP 0x40A760
		{"meleebuttonpressed", 0x833C}, // SP 0x28F500 MP 0x40AA40
		{"playerads", 0x833D}, // SP 0x28FCC0 MP 0x40B210
		{"isonground", 0x833E}, // SP 0x28FD10 MP 0x40B2D0
		{"isusingturret", 0x833F}, // SP 0x28FEF0 MP 0x40B570
		{"setviewmodel", 0x8340}, // SP 0x290390 MP 0x40B5E0
		{"setoffhandprimaryclass", 0x8341}, // SP 0x291DB0 MP 0x40D800
		{"getoffhandprimaryclass", 0x8342}, // SP 0x292000 MP 0x40DAB0
		{"enablefocus", 0x8343}, // SP 0x28DDE0 MP 0x000000
		{"disablefocus", 0x8344}, // SP 0x28E0C0 MP 0x000000
		{"enableaudiozoom", 0x8345}, // SP 0x28E260 MP 0x000000
		{"disableaudiozoom", 0x8346}, // SP 0x28E480 MP 0x000000
		{"_meth_8347", 0x8347}, // SP 0x299DF0 MP 0x418DA0
		{"forcethirdpersonwhenfollowing", 0x8348}, // SP 0x000000 MP 0x0DF510
		{"disableforcethirdpersonwhenfollowing", 0x8349}, // SP 0x000000 MP 0x0DFA20
		{"_meth_834a", 0x834A}, // SP 0x28E6F0 MP 0x409B60
		{"setscriptmoverkillcam", 0x834B}, // SP 0x000000 MP 0x0CC3F0
		{"secondaryoffhandbuttonpressed", 0x834C}, // SP 0x28F970 MP 0x40AFB0
		{"_meth_834d", 0x834D}, // SP 0x000000 MP 0x0DCF20
		{"_meth_834e", 0x834E}, // SP 0x000000 MP 0x0DD7F0
		{"botsetflag", 0x834F}, // SP 0x000000 MP 0x546BC0
		{"botsetstance", 0x8350}, // SP 0x000000 MP 0x5473D0
		{"botsetscriptmove", 0x8351}, // SP 0x000000 MP 0x547250
		{"_meth_8352", 0x8352}, // SP 0x000000 MP 0x546EA0
		{"botsetscriptgoal", 0x8353}, // SP 0x000000 MP 0x547090
		{"botclearscriptgoal", 0x8354}, // SP 0x000000 MP 0x544F60
		{"getnearestnode", 0x8355}, // SP 0x000000 MP 0x546DE0
		{"botclearscriptenemy", 0x8356}, // SP 0x000000 MP 0x544EE0
		{"botsetattacker", 0x8357}, // SP 0x000000 MP 0x546980
		{"botgetscriptgoal", 0x8358}, // SP 0x000000 MP 0x545780
		{"botgetscriptgoalradius", 0x8359}, // SP 0x000000 MP 0x5458D0
		{"botgetscriptgoalyaw", 0x835A}, // SP 0x000000 MP 0x5459D0
		{"botgetscriptgoaltype", 0x835B}, // SP 0x000000 MP 0x545950
		{"_meth_835d", 0x835D}, // SP 0x000000 MP 0x545B30
		{"_meth_835e", 0x835E}, // SP 0x000000 MP 0x5460F0
		{"botfindnoderandom", 0x835F}, // SP 0x000000 MP 0x544FE0
		{"botmemoryevent", 0x8360}, // SP 0x000000 MP 0x545E50
		{"botnodepick", 0x8362}, // SP 0x000000 MP 0x546190
		{"bothasscriptgoal", 0x8363}, // SP 0x000000 MP 0x545B70
		{"botgetpersonality", 0x8364}, // SP 0x000000 MP 0x545700
		{"_meth_8365", 0x8365}, // SP 0x000000 MP 0x5474A0
		{"botsetpersonality", 0x8367}, // SP 0x000000 MP 0x546D50
		{"botsetdifficulty", 0x8368}, // SP 0x000000 MP 0x546A90
		{"botgetdifficulty", 0x8369}, // SP 0x000000 MP 0x545170
		{"botgetworldclosestedge", 0x836A}, // SP 0x000000 MP 0x545A50
		{"_meth_836b", 0x836B}, // SP 0x000000 MP 0x545C70
		{"botpredictseepoint", 0x836C}, // SP 0x000000 MP 0x546700
		{"botcanseeentity", 0x836D}, // SP 0x000000 MP 0x544D70
		{"botgetnodesonpath", 0x836E}, // SP 0x000000 MP 0x545510
		{"_meth_836f", 0x836F}, // SP 0x000000 MP 0x546300
		{"_meth_8371", 0x8371}, // SP 0x000000 MP 0x5452B0
		{"botsetawareness", 0x8372}, // SP 0x000000 MP 0x546A00
		{"_meth_8373", 0x8373}, // SP 0x000000 MP 0x5468F0
		{"botgetscriptgoalnode", 0x8374}, // SP 0x000000 MP 0x545840
		{"botgetimperfectenemyinfo", 0x8375}, // SP 0x000000 MP 0x5453B0
		{"botsetpathingstyle", 0x8377}, // SP 0x000000 MP 0x546C80
		{"botsetdifficultysetting", 0x8378}, // SP 0x000000 MP 0x546B20
		{"botgetdifficultysetting", 0x8379}, // SP 0x000000 MP 0x545200
		{"botgetpathdist", 0x837A}, // SP 0x000000 MP 0x5455D0
		{"_meth_837b", 0x837B}, // SP 0x000000 MP 0x545BF0
		{"_meth_837c", 0x837C}, // SP 0x000000 MP 0x5467F0
		{"botclearbutton", 0x837D}, // SP 0x000000 MP 0x544E50
		{"_meth_837e", 0x837E}, // SP 0x000000 MP 0x5464B0
		{"getnodenumber", 0x837F}, // SP 0x000000 MP 0x40BF90
		{"setclientowner", 0x8380}, // SP 0x000000 MP 0x0C9690
		{"setotherent", 0x8381}, // SP 0x000000 MP 0x0C5BB0
		{"setaisightlinevisible", 0x8382}, // SP 0x2C6B30 MP 0x449220
		{"setentityowner", 0x8383}, // SP 0x000000 MP 0x0C8510
		{"nodeisdisconnected", 0x8384}, // SP 0x000000 MP 0x40C0B0
		{"_meth_8385", 0x8385}, // SP 0x290920 MP 0x40BE80
		{"_meth_8386", 0x8386}, // SP 0x000000 MP 0x0CB540
		{"_meth_8388", 0x8388}, // SP 0x000000 MP 0x51BBE0
		{"_meth_8389", 0x8389}, // SP 0x000000 MP 0x51B240
		{"setagentattacker", 0x838A}, // SP 0x000000 MP 0x51BA90
		{"_meth_838b", 0x838B}, // SP 0x000000 MP 0x51AD10
		{"agentcanseesentient", 0x838C}, // SP 0x000000 MP 0x51AB30
		{"setwaypoint", 0x838D}, // SP 0x000000 MP 0x51D770
		{"setgoalpos", 0x838E}, // SP 0x000000 MP 0x51CD70
		{"getgoalpos", 0x838F}, // SP 0x000000 MP 0x51C2C0
		{"setgoalnode", 0x8390}, // SP 0x000000 MP 0x51CCE0
		{"setgoalentity", 0x8391}, // SP 0x000000 MP 0x51CC50
		{"setgoalradius", 0x8392}, // SP 0x000000 MP 0x51CEB0
		{"scragentsetanimscale", 0x8393}, // SP 0x000000 MP 0x51CB10
		{"setorientmode", 0x8394}, // SP 0x000000 MP 0x51D190
		{"setanimmode", 0x8395}, // SP 0x000000 MP 0x51C9F0
		{"setphysicsmode", 0x8396}, // SP 0x000000 MP 0x51D3D0
		{"setclipmode", 0x8397}, // SP 0x000000 MP 0x51CBB0
		{"setmaxturnspeed", 0x8398}, // SP 0x000000 MP 0x51CFC0
		{"getmaxturnspeed", 0x8399}, // SP 0x000000 MP 0x51C3F0
		{"beginmelee", 0x839A}, // SP 0x000000 MP 0x51BEF0
		{"getmlgspectatorteam", 0x839B}, // SP 0x000000 MP 0x51D490
		{"dotrajectory", 0x839C}, // SP 0x000000 MP 0x51C220
		{"doanimlerp", 0x839D}, // SP 0x000000 MP 0x51C0D0
		{"setviewheight", 0x839E}, // SP 0x000000 MP 0x51D6F0
		{"claimnode", 0x839F}, // SP 0x000000 MP 0x51BFC0
		{"relinquishclaimednode", 0x83A0}, // SP 0x000000 MP 0x51C8E0
		{"_meth_83a1", 0x83A1}, // SP 0x29ABC0 MP 0x419930
		{"_meth_83a2", 0x83A2}, // SP 0x29AC70 MP 0x4199E0
		{"_meth_83a3", 0x83A3}, // SP 0x29ACE0 MP 0x419A40
		{"_meth_83a4", 0x83A4}, // SP 0x29AD50 MP 0x419AA0
		{"_meth_83a5", 0x83A5}, // SP 0x2DC2C0 MP 0x000000
		{"_meth_83a6", 0x83A6}, // SP 0x28E640 MP 0x40A350
		{"_meth_83a7", 0x83A7}, // SP 0x28E8C0 MP 0x40A430
		{"_meth_83a8", 0x83A8}, // SP 0x295740 MP 0x000000
		{"_meth_83a9", 0x83A9}, // SP 0x2DC610 MP 0x000000
		{"_meth_83aa", 0x83AA}, // SP 0x2DC6A0 MP 0x000000
		{"_meth_83ab", 0x83AB}, // SP 0x2955F0 MP 0x000000
		{"_meth_83ac", 0x83AC}, // SP 0x2956D0 MP 0x000000
		{"_meth_83ad", 0x83AD}, // SP 0x295660 MP 0x000000
		{"_meth_83b0", 0x83B0}, // SP 0x28F0C0 MP 0x40A220
		{"_meth_83b1", 0x83B1}, // SP 0x28D380 MP 0x408A60
		{"_meth_83b2", 0x83B2}, // SP 0x28D530 MP 0x408C60
		{"_meth_83b3", 0x83B3}, // SP 0x4E8200 MP 0x60A810
		{"_meth_83b4", 0x83B4}, // SP 0x4E8210 MP 0x60A820
		{"_meth_83b5", 0x83B5}, // SP 0x000000 MP 0x0D93F0
		{"_meth_83b6", 0x83B6}, // SP 0x2CF0F0 MP 0x439280
		{"_meth_83b7", 0x83B7}, // SP 0x2CCBE0 MP 0x436640
		{"_meth_83b8", 0x83B8}, // SP 0x2CCC30 MP 0x436730
		{"_meth_83b9", 0x83B9}, // SP 0x28F890 MP 0x40AF20
		{"_meth_83ba", 0x83BA}, // SP 0x4E5C30 MP 0x60CCF0
		{"_meth_83bb", 0x83BB}, // SP 0x4E5CE0 MP 0x60CDA0
		{"_meth_83bc", 0x83BC}, // SP 0x4E5D60 MP 0x60CE20
		{"_meth_83bd", 0x83BD}, // SP 0x2B84D0 MP 0x000000
		{"_meth_83be", 0x83BE}, // SP 0x2918F0 MP 0x40D750
		{"_meth_83bf", 0x83BF}, // SP 0x291AD0 MP 0x40D950
		{"_meth_83c0", 0x83C0}, // SP 0x291D00 MP 0x40DBA0
		{"_meth_83c1", 0x83C1}, // SP 0x2E25A0 MP 0x454180
		{"_meth_83c2", 0x83C2}, // SP 0x307EB0 MP 0x08FC40
		{"_meth_83c3", 0x83C3}, // SP 0x4E52E0 MP 0x60C2C0
		{"_meth_83c4", 0x83C4}, // SP 0x28F020 MP 0x40A940
		{"_meth_83c5", 0x83C5}, // SP 0x2B8460 MP 0x000000
		{"_meth_83c6", 0x83C6}, // SP 0x277200 MP 0x000000
		{"_meth_83c7", 0x83C7}, // SP 0x277310 MP 0x000000
		{"_meth_83c8", 0x83C8}, // SP 0x277650 MP 0x000000
		{"_meth_83c9", 0x83C9}, // SP 0x277740 MP 0x000000
		{"_meth_83ca", 0x83CA}, // SP 0x277A10 MP 0x000000
		{"_meth_83cb", 0x83CB}, // SP 0x2779A0 MP 0x000000
		{"_meth_83cc", 0x83CC}, // SP 0x277A80 MP 0x000000
		{"_meth_83cd", 0x83CD}, // SP 0x277AE0 MP 0x000000
		{"setanimclass", 0x83CE}, // SP 0x000000 MP 0x090580
		{"enableanimstate", 0x83CF}, // SP 0x000000 MP 0x090050
		{"setanimstate", 0x83D0}, // SP 0x000000 MP 0x0905F0
		{"getanimentry", 0x83D1}, // SP 0x000000 MP 0x090190
		{"getanimentryname", 0x83D2}, // SP 0x000000 MP 0x090340
		{"getanimentryalias", 0x83D3}, // SP 0x000000 MP 0x090220
		{"getanimentrycount", 0x83D4}, // SP 0x000000 MP 0x090280
		{"_meth_83d5", 0x83D5}, // SP 0x2D8DC0 MP 0x000000
		{"issprinting", 0x83D6}, // SP 0x28EF40 MP 0x40A040
		{"_meth_83d7", 0x83D7}, // SP 0x2C4000 MP 0x000000
		{"_meth_83d8", 0x83D8}, // SP 0x275850 MP 0x000000
		{"setclothtype", 0x83D9}, // SP 0x2C0670 MP 0x4492A0
		{"_meth_83da", 0x83DA}, // SP 0x2DC560 MP 0x000000
		{"_meth_83db", 0x83DB}, // SP 0x2DC5D0 MP 0x000000
		{"_meth_83dc", 0x83DC}, // SP 0x28F7B0 MP 0x40ADE0
		{"rotateto", 0x83DD}, // SP 0x2E1C40 MP 0x4538D0
		{"getlookaheaddir", 0x83DE}, // SP 0x000000 MP 0x51C360
		{"getpathgoalpos", 0x83DF}, // SP 0x000000 MP 0x51C720
		{"setrocketcorpse", 0x83E0}, // SP 0x2C26B0 MP 0x442290
		{"setcorpsefalling", 0x83E1}, // SP 0x000000 MP 0x0DB3D0
		{"setsurfacetype", 0x83E2}, // SP 0x2C0960 MP 0x4494D0
		{"aiphysicstrace", 0x83E3}, // SP 0x2BCCD0 MP 0x43C370
		{"_meth_83e4", 0x83E4}, // SP 0x2BE100 MP 0x43CE20
		{"_meth_83e5", 0x83E5}, // SP 0x299CB0 MP 0x418C70
		{"_meth_83e6", 0x83E6}, // SP 0x2DB850 MP 0x000000
		{"_meth_83e7", 0x83E7}, // SP 0x2DB9E0 MP 0x000000
		{"visionsetstage", 0x83E8}, // SP 0x290E40 MP 0x40CDF0
		{"hudoutlineenableforclients", 0x83E9}, // SP 0x29A0F0 MP 0x4190A0
		{"getlinkedparent", 0x83EA}, // SP 0x2BF830 MP 0x443220
		{"getmovingplatformparent", 0x83EB}, // SP 0x2C0530 MP 0x443F10
		{"setnameplatematerial", 0x83EC}, // SP 0x000000 MP 0x40A480
		{"_meth_83ed", 0x83ED}, // SP 0x2D8D40 MP 0x000000
		{"_meth_83ef", 0x83EF}, // SP 0x2C16C0 MP 0x442910
		{"_meth_83f0", 0x83F0}, // SP 0x2C0230 MP 0x440F60
		{"makevehiclenotcollidewithplayers", 0x83F1}, // SP 0x2C6190 MP 0x4484E0
		{"_meth_83f2", 0x83F2}, // SP 0x294DB0 MP 0x000000
		{"_meth_83f3", 0x83F3}, // SP 0x294E10 MP 0x000000
		{"setscriptablepartstate", 0x83F4}, // SP 0x2C1610 MP 0x44A030
		{"_meth_83f5", 0x83F5}, // SP 0x2C1DE0 MP 0x44AA20
		{"_meth_83f6", 0x83F6}, // SP 0x2C2770 MP 0x442550
		{"_meth_83f7", 0x83F7}, // SP 0x4E5620 MP 0x60C5F0
		{"_meth_83f8", 0x83F8}, // SP 0x2CE8E0 MP 0x4369E0
		{"_meth_83f9", 0x83F9}, // SP 0x2CE9B0 MP 0x436A80
		{"motionblurhqdisable", 0x83FA}, // SP 0x2CEE40 MP 0x436FB0
		{"_meth_83fb", 0x83FB}, // SP 0x2CEF70 MP 0x437100
		{"_meth_83fc", 0x83FC}, // SP 0x2DE6D0 MP 0x000000
		{"_meth_83fd", 0x83FD}, // SP 0x277010 MP 0x000000
		{"worldpointtoscreenpos", 0x83FE}, // SP 0x2CC840 MP 0x435CA0
		{"_meth_83ff", 0x83FF}, // SP 0x2CB310 MP 0x000000
		{"_meth_8400", 0x8400}, // SP 0x275BC0 MP 0x000000
		{"_meth_8401", 0x8401}, // SP 0x000000 MP 0x545070
		{"_meth_8402", 0x8402}, // SP 0x291670 MP 0x000000
		{"_meth_8403", 0x8403}, // SP 0x2961E0 MP 0x000000
		{"_meth_8404", 0x8404}, // SP 0x2CB550 MP 0x000000
		{"_meth_8405", 0x8405}, // SP 0x2CB6A0 MP 0x000000
		{"_meth_8406", 0x8406}, // SP 0x2CE710 MP 0x000000
		{"emissiveblend", 0x8407}, // SP 0x000000 MP 0x0E4370
		{"_meth_8408", 0x8408}, // SP 0x295AC0 MP 0x000000
		{"_meth_8409", 0x8409}, // SP 0x295B10 MP 0x000000
		{"_meth_840a", 0x840A}, // SP 0x2DAD40 MP 0x000000
		{"_meth_840b", 0x840B}, // SP 0x2D92C0 MP 0x000000
		{"_meth_840c", 0x840C}, // SP 0x2D9350 MP 0x000000
		{"_meth_840d", 0x840D}, // SP 0x2E2B00 MP 0x454720
		{"_meth_840e", 0x840E}, // SP 0x2E2BB0 MP 0x4547D0
		{"physicsgetlinspeed", 0x840F}, // SP 0x2E2C60 MP 0x454880
		{"_meth_8410", 0x8410}, // SP 0x2E2D20 MP 0x454940
		{"physicsgetangvel", 0x8411}, // SP 0x2E2E00 MP 0x454A20
		{"physicsgetangspeed", 0x8412}, // SP 0x2E2EC0 MP 0x454AE0
		{"disablemissileboosting", 0x8413}, // SP 0x000000 MP 0x446DE0
		{"enablemissileboosting", 0x8414}, // SP 0x000000 MP 0x4470F0
		{"canspawntestclient", 0x8415}, // SP 0x000000 MP 0x0E2B80
		{"spawntestclient", 0x8416}, // SP 0x000000 MP 0x0E2D80
		{"setgrenadethrowscale", 0x8417}, // SP 0x000000 MP 0x4409F0
		{"ismantling", 0x8418}, // SP 0x000000 MP 0x441200
		{"_meth_8419", 0x8419}, // SP 0x000000 MP 0x0E3010
		{"_meth_841a", 0x841A}, // SP 0x2CE570 MP 0x435830
		{"_meth_841b", 0x841B}, // SP 0x2CE6C0 MP 0x435C10
		{"_meth_841c", 0x841C}, // SP 0x2CE700 MP 0x436550
		{"_meth_841d", 0x841D}, // SP 0x2CE800 MP 0x436760
		{"turretsetbarrelspinenabled", 0x841E}, // SP 0x2CC790 MP 0x436300
		{"_meth_841f", 0x841F}, // SP 0x000000 MP 0x411A60
		{"autospotoverlayoff", 0x8420}, // SP 0x000000 MP 0x411C60
		{"_meth_8421", 0x8421}, // SP 0x2C07D0 MP 0x441460
		{"_meth_8422", 0x8422}, // SP 0x2C0B30 MP 0x441E10
		{"doanimrelative", 0x8423}, // SP 0x000000 MP 0x51C170
		{"_meth_8424", 0x8424}, // SP 0x2E1AF0 MP 0x000000
		{"_meth_8425", 0x8425}, // SP 0x2E1CC0 MP 0x000000
		{"_meth_8426", 0x8426}, // SP 0x2E1D60 MP 0x000000
		{"getcorpseentity", 0x8427}, // SP 0x000000 MP 0x0DBCB0
		{"_meth_8428", 0x8428}, // SP 0x2D7060 MP 0x000000
		{"_meth_8429", 0x8429}, // SP 0x000000 MP 0x0C1AE0
		{"_meth_842a", 0x842A}, // SP 0x000000 MP 0x0C4CF0
		{"queuedialogforplayer", 0x842B}, // SP 0x000000 MP 0x0BCE80
		{"setmlgcameradefaults", 0x842C}, // SP 0x000000 MP 0x411530
		{"_meth_842d", 0x842D}, // SP 0x000000 MP 0x410990
		{"_meth_842e", 0x842E}, // SP 0x290980 MP 0x40C8C0
		{"_meth_842f", 0x842F}, // SP 0x290A50 MP 0x40CAD0
		{"_meth_8430", 0x8430}, // SP 0x295780 MP 0x000000
		{"getlinkedchildren", 0x8431}, // SP 0x2C01A0 MP 0x443AC0
		{"_meth_8432", 0x8432}, // SP 0x000000 MP 0x546600
		{"playsoundonmovingent", 0x8433}, // SP 0x000000 MP 0x0B9DE0
		{"cancelmantle", 0x8434}, // SP 0x28D0B0 MP 0x408680
		{"hasfemalecustomizationmodel", 0x8435}, // SP 0x000000 MP 0x4122F0
		{"setscriptabledamageowner", 0x8437}, // SP 0x2C12E0 MP 0x449D80
		{"_meth_8438", 0x8438}, // SP 0x2C1BF0 MP 0x44A8E0
		{"_meth_8439", 0x8439}, // SP 0x000000 MP 0x0C2140
		{"_meth_843a", 0x843A}, // SP 0x277B50 MP 0x000000
		{"_meth_843b", 0x843B}, // SP 0x277BF0 MP 0x000000
		{"_meth_843c", 0x843C}, // SP 0x293780 MP 0x406AC0
		{"_meth_843d", 0x843D}, // SP 0x278050 MP 0x000000
		{"_meth_843e", 0x843E}, // SP 0x2BC730 MP 0x43D430
		{"_meth_843f", 0x843F}, // SP 0x28F030 MP 0x40AAD0
		{"_meth_8440", 0x8440}, // SP 0x4E6960 MP 0x60DA10
		{"_meth_8441", 0x8441}, // SP 0x4E5E20 MP 0x60CEE0
		{"linkto", 0x8442}, // SP 0x2BEED0 MP 0x442800
		{"_meth_8443", 0x8443}, // SP 0x293F10 MP 0x407950
		{"_meth_8444", 0x8444}, // SP 0x2DC850 MP 0x000000
		{"_meth_8445", 0x8445}, // SP 0x2DC890 MP 0x000000
		{"getcacplayerdata", 0x8446}, // SP 0x2C3AF0 MP 0x444D60
		{"_meth_8447", 0x8447}, // SP 0x2CEAB0 MP 0x436B20
		{"_meth_8448", 0x8448}, // SP 0x2CEB90 MP 0x436CD0
		{"_meth_8449", 0x8449}, // SP 0x28ECF0 MP 0x409D80
		{"_meth_844a", 0x844A}, // SP 0x2C89F0 MP 0x44A800
		{"_meth_844b", 0x844B}, // SP 0x2C8CC0 MP 0x44A9B0
		{"_meth_844c", 0x844C}, // SP 0x4E5FA0 MP 0x60D060
		{"_meth_844d", 0x844D}, // SP 0x4E5FC0 MP 0x60D080
		{"_meth_844e", 0x844E}, // SP 0x4E5FE0 MP 0x60D0A0
		{"_meth_844f", 0x844F}, // SP 0x4E6000 MP 0x60D0C0
		{"_meth_8450", 0x8450}, // SP 0x2D7EC0 MP 0x000000
		{"_meth_8451", 0x8451}, // SP 0x2D8020 MP 0x000000
		{"_meth_8452", 0x8452}, // SP 0x4E5EE0 MP 0x60CFA0
		{"_meth_8453", 0x8453}, // SP 0x28FA60 MP 0x40B270
		{"_meth_8454", 0x8454}, // SP 0x28EE50 MP 0x000000
		{"_meth_8455", 0x8455}, // SP 0x2D8060 MP 0x000000
		{"getvieworigin", 0x8458}, // SP 0x2D1A20 MP 0x4390C0
		{"_meth_8459", 0x8459}, // SP 0x293CF0 MP 0x406FF0
		{"_meth_845a", 0x845A}, // SP 0x000000 MP 0x446520
		{"stopridingvehicle", 0x845B}, // SP 0x000000 MP 0x446960
		{"_meth_845c", 0x845C}, // SP 0x2DC5F0 MP 0x000000
		{"_meth_845d", 0x845D}, // SP 0x2C0AA0 MP 0x4442C0
		{"disablemissilestick", 0x845E}, // SP 0x000000 MP 0x0B7D00
		{"enablemissilestick", 0x845F}, // SP 0x000000 MP 0x0B8130
		{"setmissileminimapvisible", 0x8460}, // SP 0x000000 MP 0x4253C0
		{"isoffhandweaponreadytothrow", 0x8461}, // SP 0x28D8C0 MP 0x4089F0
		{"isleaning", 0x8462}, // SP 0x28F050 MP 0x40A1B0
		{"makecollidewithitemclip", 0x8463}, // SP 0x2C6680 MP 0x448F30
		{"_meth_8464", 0x8464}, // SP 0x28F9F0 MP 0x000000
		{"visionsetpostapplyforplayer", 0x8465}, // SP 0x000000 MP 0x40D4D0
		{"_meth_8466", 0x8466}, // SP 0x2E3340 MP 0x453040
		{"_meth_8467", 0x8467}, // SP 0x2E3510 MP 0x453210
		{"_meth_8468", 0x8468}, // SP 0x2C3CC0 MP 0x000000
		{"_meth_8469", 0x8469}, // SP 0x2C4080 MP 0x000000
		{"_meth_846a", 0x846A}, // SP 0x2C4300 MP 0x000000
		{"_meth_846b", 0x846B}, // SP 0x2C4850 MP 0x000000
		{"_meth_846c", 0x846C}, // SP 0x2C5070 MP 0x000000
		{"_meth_846d", 0x846D}, // SP 0x28E5C0 MP 0x000000
		{"_meth_846e", 0x846E}, // SP 0x2DC390 MP 0x000000
		{"_meth_846f", 0x846F}, // SP 0x4E6080 MP 0x60D140
		{"_meth_8470", 0x8470}, // SP 0x4E60C0 MP 0x60D180
		{"_meth_8471", 0x8471}, // SP 0x4E6120 MP 0x60D1E0
		{"_meth_8472", 0x8472}, // SP 0x4E6170 MP 0x60D230
		{"_meth_8473", 0x8473}, // SP 0x4E6190 MP 0x60D250
		{"_meth_8474", 0x8474}, // SP 0x2C49D0 MP 0x447B00
		{"_meth_8475", 0x8475}, // SP 0x2C4C90 MP 0x447D70
		{"_meth_8476", 0x8476}, // SP 0x291690 MP 0x40D4F0
		{"_meth_8477", 0x8477}, // SP 0x2DC460 MP 0x000000
		{"_meth_8478", 0x8478}, // SP 0x2DC580 MP 0x000000
		{"_meth_8479", 0x8479}, // SP 0x28E610 MP 0x409660
		{"_meth_847a", 0x847A}, // SP 0x28E630 MP 0x409980
		{"_meth_847b", 0x847B}, // SP 0x4E5390 MP 0x60C370
		{"_meth_847c", 0x847C}, // SP 0x4E5410 MP 0x60C3F0
		{"_meth_847d", 0x847D}, // SP 0x290610 MP 0x40BEE0
		{"_meth_847e", 0x847E}, // SP 0x2906B0 MP 0x40BFC0
		{"_meth_847f", 0x847F}, // SP 0x2CF620 MP 0x439610
		{"_meth_8480", 0x8480}, // SP 0x2C4C20 MP 0x000000
		{"_meth_8481", 0x8481}, // SP 0x28F120 MP 0x40A2D0
		{"_meth_8482", 0x8482}, // SP 0x28F110 MP 0x40A2C0
		{"_meth_8483", 0x8483}, // SP 0x000000 MP 0x51B1E0
		{"_meth_8484", 0x8484}, // SP 0x4E7FE0 MP 0x60A5C0
		{"_meth_8485", 0x8485}, // SP 0x4E6030 MP 0x60D0F0
		{"_meth_8486", 0x8486}, // SP 0x000000 MP 0x51ABC0
		{"_meth_8487", 0x8487}, // SP 0x000000 MP 0x455090
		{"_meth_8488", 0x8488}, // SP 0x2E1B90 MP 0x000000
		{"_meth_8489", 0x8489}, // SP 0x28F100 MP 0x40A2B0
		{"_meth_848a", 0x848A}, // SP 0x4E8060 MP 0x60A640
		{"_meth_848b", 0x848B}, // SP 0x4E8050 MP 0x60A5D0
		{"_meth_848c", 0x848C}, // SP 0x291F30 MP 0x40DDA0
		{"_meth_848d", 0x848D}, // SP 0x292190 MP 0x40DF80
		{"_meth_848e", 0x848E}, // SP 0x292300 MP 0x40E1D0
		{"_meth_848f", 0x848F}, // SP 0x295120 MP 0x000000
		{"_meth_8490", 0x8490}, // SP 0x2C4EE0 MP 0x447F60
		{"_meth_8491", 0x8491}, // SP 0x000000 MP 0x0C6980
		{"_meth_8492", 0x8492}, // SP 0x000000 MP 0x0C7320
		{"_meth_8493", 0x8493}, // SP 0x2D7D60 MP 0x000000
		{"_meth_8494", 0x8494}, // SP 0x2D7E20 MP 0x000000
		{"_meth_8495", 0x8495}, // SP 0x295F30 MP 0x000000
		{"_meth_8496", 0x8496}, // SP 0x000000 MP 0x0E28D0
		{"_meth_8497", 0x8497}, // SP 0x000000 MP 0x0B7A80
		{"_meth_8498", 0x8498}, // SP 0x28C7A0 MP 0x407F90
		{"_meth_8499", 0x8499}, // SP 0x28CDE0 MP 0x4083E0
		{"_meth_849a", 0x849A}, // SP 0x28F0F0 MP 0x40A240
		{"_meth_849b", 0x849B}, // SP 0x000000 MP 0x0E2BA0
		{"_meth_849c", 0x849C}, // SP 0x000000 MP 0x0E2D70
		{"_meth_849d", 0x849D}, // SP 0x000000 MP 0x0E2E80
		{"_meth_849e", 0x849E}, // SP 0x000000 MP 0x0E3000
		{"_meth_849f", 0x849F}, // SP 0x000000 MP 0x0E3210
		{"_meth_84a0", 0x84A0}, // SP 0x000000 MP 0x0E3820
		{"_meth_84a1", 0x84A1}, // SP 0x000000 MP 0x0E3580
		{"_meth_84a2", 0x84A2}, // SP 0x000000 MP 0x0E3D40
		{"_meth_84a3", 0x84A3}, // SP 0x000000 MP 0x0E3FF0
		{"_meth_84a4", 0x84A4}, // SP 0x000000 MP 0x0E3C60
		{"_meth_84a5", 0x84A5}, // SP 0x2BD9E0 MP 0x43E620
		{"_meth_84a6", 0x84A6}, // SP 0x2BDB80 MP 0x43E880
		{"_meth_84a7", 0x84A7}, // SP 0x2BDCD0 MP 0x43EA30
		{"_meth_84a8", 0x84A8}, // SP 0x2BE6E0 MP 0x43F3B0
		{"_meth_84a9", 0x84A9}, // SP 0x4E42A0 MP 0x60B3F0
		{"_meth_84aa", 0x84AA}, // SP 0x2CB640 MP 0x44D490
		{"_meth_84ab", 0x84AB}, // SP 0x2CBA10 MP 0x44D700
		{"_meth_84ac", 0x84AC}, // SP 0x28FAA0 MP 0x40B0D0
		{"_meth_84ad", 0x84AD}, // SP 0x4E4A40 MP 0x60BAE0
		{"_meth_84ae", 0x84AE}, // SP 0x4E4B40 MP 0x60BBE0
		{"_meth_84af", 0x84AF}, // SP 0x000000 MP 0x412350
		{"_meth_84b0", 0x84B0}, // SP 0x2D11A0 MP 0x43ABB0
		{"_meth_84b1", 0x84B1}, // SP 0x2C5370 MP 0x4480A0
		{"_meth_84b2", 0x84B2}, // SP 0x2C56D0 MP 0x448170
		{"_meth_84b3", 0x84B3}, // SP 0x2C6010 MP 0x448280
		{"_meth_84b4", 0x84B4}, // SP 0x2C6330 MP 0x4482B0
		{"_meth_84b5", 0x84B5}, // SP 0x2CF980 MP 0x43D0B0
		{"_meth_84b6", 0x84B6}, // SP 0x2BE900 MP 0x444AB0
		{"_meth_84b7", 0x84B7}, // SP 0x4E8640 MP 0x60AC60
		{"_meth_84b8", 0x84B8}, // SP 0x2BE3A0 MP 0x43F1D0
		{"_meth_84b9", 0x84B9}, // SP 0x000000 MP 0x455250
		{"_meth_84bb", 0x84BB}, // SP 0x292260 MP 0x40E070
		{"_meth_84bc", 0x84BC}, // SP 0x2922B0 MP 0x40E2B0
		{"_meth_84bd", 0x84BD}, // SP 0x000000 MP 0x4123D0
		{"_meth_84be", 0x84BE}, // SP 0x000000 MP 0x412590
		{"_meth_84bf", 0x84BF}, // SP 0x2930E0 MP 0x406530
		{"_meth_84c0", 0x84C0}, // SP 0x293590 MP 0x406850
		{"_meth_84c1", 0x84C1}, // SP 0x000000 MP 0x0B8C00
		{"_meth_84c2", 0x84C2}, // SP 0x2BA6B0 MP 0x43C670
		{"_meth_84c3", 0x84C3}, // SP 0x4E61D0 MP 0x60D290
		{"_meth_84c4", 0x84C4}, // SP 0x000000 MP 0x51DA90
		{"_meth_84c5", 0x84C5}, // SP 0x000000 MP 0x51D760
		{"_meth_84c6", 0x84C6}, // SP 0x28E210 MP 0x409280
		{"_meth_84c7", 0x84C7}, // SP 0x28E010 MP 0x000000
		{"_meth_84c8", 0x84C8}, // SP 0x28E160 MP 0x000000
		{"_meth_84c9", 0x84C9}, // SP 0x4E7D50 MP 0x60A390
		{"_meth_84ca", 0x84CA}, // SP 0x4E7DF0 MP 0x60A430
		{"_meth_84cb", 0x84CB}, // SP 0x000000 MP 0x410BF0
		{"_meth_84cc", 0x84CC}, // SP 0x4E61F0 MP 0x60D2B0
		{"_meth_84cd", 0x84CD}, // SP 0x2C1DF0 MP 0x442D70
		{"_meth_84ce", 0x84CE}, // SP 0x2C1FB0 MP 0x442F70
		{"_meth_84cf", 0x84CF}, // SP 0x2BF700 MP 0x440540
		{"_meth_84d0", 0x84D0}, // SP 0x2BF7D0 MP 0x440710
		{"_meth_84d1", 0x84D1}, // SP 0x2BF910 MP 0x440770
		{"_meth_84d2", 0x84D2}, // SP 0x2BFAC0 MP 0x4408B0
		{"_meth_84d3", 0x84D3}, // SP 0x2BFE40 MP 0x440990
		{"_meth_84d4", 0x84D4}, // SP 0x2BFEB0 MP 0x440AC0
		{"_meth_84d5", 0x84D5}, // SP 0x2BFF20 MP 0x440CD0
		{"_meth_84d6", 0x84D6}, // SP 0x2C0110 MP 0x440D20
		{"_meth_84d8", 0x84D8}, // SP 0x28D800 MP 0x408930
		{"_meth_84d9", 0x84D9}, // SP 0x2E2FA0 MP 0x454BC0
		{"_meth_84da", 0x84DA}, // SP 0x000000 MP 0x447F50
		{"_meth_84db", 0x84DB}, // SP 0x274990 MP 0x000000
		{"_meth_84dc", 0x84DC}, // SP 0x4E4F10 MP 0x000000
		{"_meth_84df", 0x84DF}, // SP 0x2C5560 MP 0x44C4B0
		{"_meth_84e0", 0x84E0}, // SP 0x29B120 MP 0x419E70
		{"_meth_84e1", 0x84E1}, // SP 0x4E6230 MP 0x60D2F0
		{"_meth_84e2", 0x84E2}, // SP 0x000000 MP 0x448410
		{"_meth_84e3", 0x84E3}, // SP 0x292800 MP 0x405FB0
		{"_meth_84e4", 0x84E4}, // SP 0x2C8B30 MP 0x44A9A0
		{"_meth_84e5", 0x84E5}, // SP 0x2DC750 MP 0x000000
		{"_meth_84e6", 0x84E6}, // SP 0x2BEA70 MP 0x445580
		{"_meth_84e8", 0x84E8}, // SP 0x292740 MP 0x40E5D0
		{"_meth_84e9", 0x84E9}, // SP 0x2927A0 MP 0x40E5E0
		{"_meth_84ea", 0x84EA}, // SP 0x2B8580 MP 0x000000
		{"_meth_84eb", 0x84EB}, // SP 0x2CEE90 MP 0x438310
		{"_meth_84ec", 0x84EC}, // SP 0x2C1E80 MP 0x44AC20
		{"_meth_84ed", 0x84ED}, // SP 0x2CBDF0 MP 0x437A60
		{"_meth_84ee", 0x84EE}, // SP 0x2CFC40 MP 0x438E10
		{"_meth_84ef", 0x84EF}, // SP 0x2CED10 MP 0x436D60
		{"_meth_84f0", 0x84F0}, // SP 0x4E73A0 MP 0x609960
		{"_meth_84f1", 0x84F1}, // SP 0x4E74D0 MP 0x609A90
		{"_meth_84f2", 0x84F2}, // SP 0x2C6580 MP 0x44D480
		{"_meth_84f3", 0x84F3}, // SP 0x2C67B0 MP 0x44D560
		{"_meth_84f4", 0x84F4}, // SP 0x2C5990 MP 0x44C810
		{"_meth_84f5", 0x84F5}, // SP 0x2C5B60 MP 0x44C9A0
		{"_meth_84f6", 0x84F6}, // SP 0x2C5F40 MP 0x44CAF0
		{"_meth_84f7", 0x84F7}, // SP 0x2C6000 MP 0x44CBE0
		{"_meth_84f8", 0x84F8}, // SP 0x2C6160 MP 0x44CCE0
		{"_meth_84f9", 0x84F9}, // SP 0x2C62A0 MP 0x44D140
		{"_meth_84fa", 0x84FA}, // SP 0x2C63F0 MP 0x44D230
		{"_meth_84fb", 0x84FB}, // SP 0x2BF9D0 MP 0x447EB0
		{"_meth_84fc", 0x84FC}, // SP 0x2C5650 MP 0x447220
		{"_meth_84fd", 0x84FD}, // SP 0x000000 MP 0x4109F0
		{"_meth_84fe", 0x84FE}, // SP 0x000000 MP 0x0B90C0
		{"_meth_84ff", 0x84FF}, // SP 0x000000 MP 0x0B9280
		{"_meth_8500", 0x8500}, // SP 0x2C17E0 MP 0x443F90
		{"_meth_8501", 0x8501}, // SP 0x2C08B0 MP 0x442CB0
		{"_meth_8502", 0x8502}, // SP 0x000000 MP 0x0E3260
		{"_meth_8503", 0x8503}, // SP 0x2C74D0 MP 0x44DAD0
		{"_meth_8504", 0x8504}, // SP 0x000000 MP 0x0C5FE0
		{"_meth_8505", 0x8505}, // SP 0x000000 MP 0x0E0980
		{"_meth_8506", 0x8506}, // SP 0x000000 MP 0x0C9DD0
		{"_meth_8507", 0x8507}, // SP 0x000000 MP 0x0C90E0
		{"_meth_8508", 0x8508}, // SP 0x000000 MP 0x411EC0
		{"_meth_8509", 0x8509}, // SP 0x2D7DE0 MP 0x000000
		{"_meth_850a", 0x850A}, // SP 0x292570 MP 0x40E310
		{"_meth_850b", 0x850B}, // SP 0x29AAE0 MP 0x419860
		{"_meth_850c", 0x850C}, // SP 0x000000 MP 0x0B7A30
		{"_meth_850d", 0x850D}, // SP 0x2BF100 MP 0x4401E0
		{"_meth_850e", 0x850E}, // SP 0x2BF250 MP 0x440370
		{"_meth_850f", 0x850F}, // SP 0x2CDA70 MP 0x44D680
		{"_meth_8510", 0x8510}, // SP 0x28DB80 MP 0x409350
		{"_meth_8511", 0x8511}, // SP 0x2C5A30 MP 0x447590
		{"_meth_8512", 0x8512}, // SP 0x4E6260 MP 0x60D320
		{"_meth_8513", 0x8513}, // SP 0x4E6280 MP 0x60D340
		{"_meth_8514", 0x8514}, // SP 0x28D570 MP 0x408E80
		{"_meth_8515", 0x8515}, // SP 0x28D620 MP 0x408EA0
		{"_meth_8516", 0x8516}, // SP 0x2CD200 MP 0x44C830
		{"_meth_8517", 0x8517}, // SP 0x000000 MP 0x0D2A90
		{"_meth_8518", 0x8518}, // SP 0x29A230 MP 0x4191D0
		{"_meth_8519", 0x8519}, // SP 0x000000 MP 0x0B94F0
		{"_meth_851a", 0x851A}, // SP 0x28EED0 MP 0x409F60
		{"_meth_851b", 0x851B}, // SP 0x2BFFA0 MP 0x448290
		{"_meth_851c", 0x851C}, // SP 0x2BF180 MP 0x447AE0
		{"_meth_851d", 0x851D}, // SP 0x2E3020 MP 0x454C40
		{"_meth_851e", 0x851E}, // SP 0x2B8280 MP 0x000000
		{"_meth_851f", 0x851F}, // SP 0x000000 MP 0x40FDF0
		{"_meth_8520", 0x8520}, // SP 0x000000 MP 0x51BB10
		{"_meth_8521", 0x8521}, // SP 0x291650 MP 0x000000
		{"_meth_8522", 0x8522}, // SP 0x000000 MP 0x4116C0
		{"_meth_8523", 0x8523}, // SP 0x2BF050 MP 0x442820
		{"_meth_8524", 0x8524}, // SP 0x2BFB60 MP 0x443720
		{"_meth_8525", 0x8525}, // SP 0x000000 MP 0x0DE060
		{"_meth_8526", 0x8526}, // SP 0x000000 MP 0x0DE6B0
		{"_meth_8527", 0x8527}, // SP 0x29AB50 MP 0x4198C0
		{"_meth_8528", 0x8528}, // SP 0x000000 MP 0x411FA0
		{"_meth_8529", 0x8529}, // SP 0x2C7710 MP 0x435530
		{"_meth_852a", 0x852A}, // SP 0x2CBE90 MP 0x44AB30
		{"_meth_852b", 0x852B}, // SP 0x000000 MP 0x411E50
		{"_meth_852c", 0x852C}, // SP 0x000000 MP 0x4113E0
		{"_meth_852d", 0x852D}, // SP 0x2C6450 MP 0x44D330
		{"_meth_852e", 0x852E}, // SP 0x000000 MP 0x0B95A0
		{"_meth_852f", 0x852F}, // SP 0x000000 MP 0x51D500
		{"_meth_8530", 0x8530}, // SP 0x000000 MP 0x51BE30
		{"_meth_8531", 0x8531}, // SP 0x000000 MP 0x51D040
		{"_meth_8532", 0x8532}, // SP 0x000000 MP 0x40EF70
		{"_meth_8533", 0x8533}, // SP 0x000000 MP 0x51B870
		{"_meth_8534", 0x8534}, // SP 0x000000 MP 0x51D2C0
		{"_meth_8535", 0x8535}, // SP 0x000000 MP 0x51C960
		{"_meth_8536", 0x8536}, // SP 0x000000 MP 0x51D100
		{"_meth_8537", 0x8537}, // SP 0x000000 MP 0x51CF30
		{"_meth_8538", 0x8538}, // SP 0x000000 MP 0x51D340
		{"_meth_8539", 0x8539}, // SP 0x000000 MP 0x40FE50
		{"_meth_853a", 0x853A}, // SP 0x000000 MP 0x453C60
		{"_meth_853b", 0x853B}, // SP 0x000000 MP 0x51ACB0
		{"_meth_853c", 0x853C}, // SP 0x2C6810 MP 0x44D570
		{"_meth_853d", 0x853D}, // SP 0x28F0E0 MP 0x40A230
		{"_meth_853e", 0x853E}, // SP 0x000000 MP 0x0B97D0
		{"_meth_853f", 0x853F}, // SP 0x000000 MP 0x51AC20
		{"_meth_8540", 0x8540}, // SP 0x2D1AA0 MP 0x43A930
		{"_meth_8541", 0x8541}, // SP 0x2C6AA0 MP 0x44D810
		{"_meth_8542", 0x8542}, // SP 0x000000 MP 0x0B9AF0
		{"_meth_8543", 0x8543}, // SP 0x000000 MP 0x0E4100
		{"_meth_8544", 0x8544}, // SP 0x000000 MP 0x51C660
		{"_meth_8545", 0x8545}, // SP 0x000000 MP 0x40EEC0
		{"_meth_8547", 0x8547}, // SP 0x000000 MP 0x51D870
		{"_meth_8548", 0x8548}, // SP 0x28FD80 MP 0x40B4F0
		{"_meth_8549", 0x8549}, // SP 0x000000 MP 0x40FEB0
		{"_meth_854a", 0x854A}, // SP 0x000000 MP 0x0B9E90
		{"_meth_854b", 0x854B}, // SP 0x000000 MP 0x51C7F0
		{"_meth_854c", 0x854C}, // SP 0x000000 MP 0x40EE60
		{"_meth_854d", 0x854D}, // SP 0x2C3710 MP 0x445D20
		{"_meth_854e", 0x854E}, // SP 0x000000 MP 0x51D660
		{"_meth_854f", 0x854F}, // SP 0x000000 MP 0x51C860
		{"_meth_8550", 0x8550}, // SP 0x2E32C0 MP 0x453030
		{"_meth_8551", 0x8551}, // SP 0x2CC650 MP 0x44B730
		{"_meth_8552", 0x8552}, // SP 0x000000 MP 0x51D880
		{"_meth_8553", 0x8553}, // SP 0x000000 MP 0x0B7050
		{"_meth_8554", 0x8554}, // SP 0x000000 MP 0x51C040
		{"_meth_8555", 0x8555}, // SP 0x000000 MP 0x51D980
		{"_meth_8556", 0x8556}, // SP 0x000000 MP 0x0D3F40
		{"_meth_8557", 0x8557}, // SP 0x000000 MP 0x0D4240
		{"_meth_8558", 0x8558}, // SP 0x000000 MP 0x0904E0
		{"_meth_8559", 0x8559}, // SP 0x28F040 MP 0x40AD50
		{"_meth_855a", 0x855A}, // SP 0x28F0B0 MP 0x40AD60
		{"_meth_855b", 0x855B}, // SP 0x28F0D0 MP 0x40AE70
		{"_meth_855c", 0x855C}, // SP 0x000000 MP 0x0BA090
		{"_meth_855d", 0x855D}, // SP 0x2C0EA0 MP 0x449C80
		{"_meth_855e", 0x855E}, // SP 0x2C6DF0 MP 0x448860
		{"_meth_855f", 0x855F}, // SP 0x000000 MP 0x0D60B0
		{"_meth_8560", 0x8560}, // SP 0x000000 MP 0x0D6CC0
		{"_meth_8561", 0x8561}, // SP 0x2CB870 MP 0x437570
		{"_meth_8562", 0x8562}, // SP 0x2CBAC0 MP 0x437610
		{"_meth_8563", 0x8563}, // SP 0x2E19F0 MP 0x453840
		{"_meth_8564", 0x8564}, // SP 0x2FB570 MP 0x3FBE00
		{"_meth_8565", 0x8565}, // SP 0x273A90 MP 0x000000
		{"_meth_8566", 0x8566}, // SP 0x2DC6F0 MP 0x000000
		{"_meth_8567", 0x8567}, // SP 0x2751C0 MP 0x000000
		{"_meth_8568", 0x8568}, // SP 0x2C47A0 MP 0x447140
		{"_meth_8569", 0x8569}, // SP 0x2CBF80 MP 0x435540
		{"_meth_856a", 0x856A}, // SP 0x2CC140 MP 0x435910
		{"_meth_856b", 0x856B}, // SP 0x2CAC40 MP 0x44C760
		{"_meth_856c", 0x856C}, // SP 0x2CAE30 MP 0x44C8C0
		{"_meth_856d", 0x856D}, // SP 0x2CB090 MP 0x44CB50
		{"_meth_856e", 0x856E}, // SP 0x2B85C0 MP 0x000000
		{"_meth_856f", 0x856F}, // SP 0x2B85E0 MP 0x000000
		{"_meth_8570", 0x8570}, // SP 0x2B8630 MP 0x000000
		{"_meth_8571", 0x8571}, // SP 0x275270 MP 0x000000
		{"_meth_8572", 0x8572}, // SP 0x2770F0 MP 0x000000
		{"_meth_8573", 0x8573}, // SP 0x28E620 MP 0x409A00
		{"_meth_8574", 0x8574}, // SP 0x293950 MP 0x406F30
		{"_meth_8575", 0x8575}, // SP 0x293B10 MP 0x407240
		{"_meth_8576", 0x8576}, // SP 0x000000 MP 0x545330
		{"_meth_8577", 0x8577}, // SP 0x000000 MP 0x40EED0
		{"_meth_8578", 0x8578}, // SP 0x2CBBF0 MP 0x44A7B0
		{"_meth_8579", 0x8579}, // SP 0x2CBC40 MP 0x437840
		{"_meth_857a", 0x857A}, // SP 0x000000 MP 0x40F300
		{"_meth_857b", 0x857B}, // SP 0x000000 MP 0x4084D0
		{"_meth_857c", 0x857C}, // SP 0x2BEA80 MP 0x4462E0
		{"_meth_857d", 0x857D}, // SP 0x000000 MP 0x410750
		{"_meth_857e", 0x857E}, // SP 0x000000 MP 0x410810
		{"_meth_857f", 0x857F}, // SP 0x000000 MP 0x411D70
		{"_meth_8580", 0x8580}, // SP 0x000000 MP 0x40F3B0
		{"_meth_8581", 0x8581}, // SP 0x2C67C0 MP 0x448180
		{"_meth_8582", 0x8582}, // SP 0x000000 MP 0x40FBA0
		{"_meth_8583", 0x8583}, // SP 0x000000 MP 0x40FC70
		{"_meth_8584", 0x8584}, // SP 0x000000 MP 0x40FCD0
		{"_meth_8585", 0x8585}, // SP 0x2C4A50 MP 0x447500
		{"_meth_8586", 0x8586}, // SP 0x2BD780 MP 0x43F2A0
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
		{"_not_owner", 0x2E2}, // was "owner"
		{"pacifist", 0x2E3},
		{"pacifistwait", 0x2E4},
		{"owner", 0x2E5}, // was "pain"
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
		{"_not_pers", 0x305}, // was "pers"
		{"persistentperksunlocked", 0x306},
		{"persistentweaponsunlocked", 0x307},
		{"pers", 0x308}, // was "phone_off"
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
		{"_not_primaryweapon", 0x33D},
		{"privatematchactivesquadmember", 0x33E},
		{"privatematchcustomclasses", 0x33F},
		{"primaryweapon", 0x340}, // was privatematchsquadmembers
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
		{"_not_secondaryweapon", 0x3B0}, // was secondaryweapon
		{"sentry", 0x3B1},
		{"sentry_manual", 0x3B2},
		{"secondaryweapon", 0x3B3}, // was sentry_offline
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
		{"_not_tag_stowed_back", 0x488}, // was "tag_stowed_back"
		{"tag_stowed_hip_rear", 0x489},
		{"tag_sync", 0x48A},
		{"tag_stowed_back", 0x48B}, // was "tag_tip"
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
		{"_not_team", 0x4A4}, // was "team"
		{"team3", 0x4A5},
		{"teambalanced", 0x4A6},
		{"team", 0x4A7}, // was "teammode_axisallies"
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
		{"_not_trigger", 0x4C2}, // was "trigger"
		{"trigger_damage", 0x4C3},
		{"trigger_use", 0x4C4},
		{"trigger", 0x4C5}, // was "trigger_use_touch"
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
		{"_createfx", 0x575},
		{"_effect", 0x58f},
		{"ac130", 0x6ce},
		{"ac130player", 0x6d1},
		{"atBrinkOfDeath", 0xD82},
		{"blockWeaponDrops", 0x14B5},
		{"QuickMessageToAll", 0x70a2},
		{"SetupCallbacks", 0x8301},
		{"_effect", 0x58f},
		{"_hasPerk", 0x5cb},
		{"_objective_delete", 0x603},
		{"_unsetPerk", 0x67b},
		{"addSpawnPoints", 0x82f},
		{"addStartSpawnPoints", 0x831},
		{"addToCharactersArray", 0x848},
		{"addToTeam", 0x851},
		{"allowUse", 0xab2},
		{"anim_prop_models", 0xbec},
		{"applyLoadout", 0xcae}, // has applyLoadout notify like IW6's giveLoadout does at the end + similar logic
		{"audio", 0x10f0},
		{"baseFontScale", 0x1309},
		{"beingRevived", 0x13ab},
		{"bombSquadIds", 0x154d},
		{"bombSquadIcons", 0x154c},
		{"bot_funcs", 0x161b},
		{"breathingStopTime", 0x17e6},
		{"brinkOfDeathKillStreak", 0x1818},
		{"c4explodethisframe", 0x1974},
		{"Callback_PlayerDamage", 0x19f5},
		{"callbackPlayerDamage", 0x19fe},
		{"canDoCombat", 0x1ac6},
		{"carryFlag", 0x1bb4},
		{"challengeInfo", 0x1c62},
		{"changingWeapon", 0x1c87},
		{"characters", 0x1c8e},
		{"checkDynamicSpawns", 0x1cfa},
		{"chopper", 0x1d48},
		{"claymoreDetectionDot", 0x1e44},
		{"claymoreDetectionMinDist", 0x1e46},
		{"clearOnVictimDisconnect", 0x1ef9},
		{"clientid", 0x1f0f},
		{"clientMatchDataId", 0x1f10},
		{"combatHigh", 0x20ab},
		{"conf_fx", 0x20e9},
		{"console", 0x2153},
		{"createUseObject", 0x244c},
		{"curOrigin", 0x24c8},
		{"curProgress", 0x24C9},
		{"defaultvalue", 0x27a1},
		{"deleteObjPoint", 0x2859},
		{"detectedExploit", 0x2991},
		{"detectExplosives", 0x2992},
		{"detectId", 0x2994},
		{"disabled", 0x2afd},
		{"disableSpawning", 0x2b28},
		{"dogtags", 0x2cdf},
		{"doingSplash", 0x2ce9},
		{"dont_delete_grenades_on_next_spawn",0x2d1e},
		{"droppedDeathWeapon",0x2f74},
		{"entityHeadIcons", 0x331a},
		{"finalKill", 0x373e},
		{"findBoxCenter", 0x3779},
		{"flashDuration", 0x38ae},
		{"flashEndTime", 0x38b1},
		{"flashRumbleDuration", 0x38c4},
		{"forfeitInProgress", 0x39df},
		{"friendlydamage", 0x3a97},
		{"freeplayers", 0x3a2a},
		{"fx", 0x3b23},
		{"gameended", 0x3bda},
		{"gamemodeModifyPlayerDamage", 0x3bf6},
		{"getNextObjID", 0x4041},
		{"getOtherTeam", 0x4067},
		{"getSpawnPoint", 0x40d2},
		{"getSpawnpoint_FreeForAll", 0x40d5},
		{"getTeamSpawnPoints", 0x411f},
		{"getWeaponClass", 0x4167},
		{"giveLoadout", 0x41e0}, // this may not even be giveLoadout but it's a wrapper for it and it does the same logic so
		{"gotPullbackNotify", 0x428b},
		{"guid", 0x4450},
		{"gunner", 0x4473},
		{"hardcoreMode", 0x46ca},
		{"healthRegenerationStreak", 0x4791},
		{"healthOverlayCutoff", 0x478d},
		{"healthRegenDisabled", 0x478e},
		{"hits", 0x4926},
		{"hitsThisMag", 0x4929},
		{"hud", 0x4ab3},
		{"iDFlags", 0x4b56},
		{"iDFLAGS_NO_KNOCKBACK", 0x4b58},
		{"iDFLAGS_SHIELD_EXPLOSIVE_SPLASH ", 0x4b60 },
		{"iDFLAGS_SHIELD_EXPLOSIVE_IMPACT", 0x4b5e},
		{"iDFLAGS_SHIELD_EXPLOSIVE_IMPACT_HUGE", 0x4b5f},
		{"iDFLAGS_STUN", 0x4b61},
		{"iDFlagsTime", 0x4b62},
		{"initedEntityHeadIcons", 0x4Db6},
		{"inc", 0x4c13},
		{"inFrames", 0x4c67},
		{"inGracePeriod", 0x4c6d},
		{"init_animatedmodels", 0x4c77},
		{"initSpawns", 0x4e26},
		{"inPlayerSmokeScreen", 0x4e42},
		{"inlaststand ", 0x4e3c},
		{"initializeMatchRules", 0x4de0},
		{"initializeTagPathVariables", 0x4de3},
		{"iscooked", 0x50e8},
		{"isKillstreakWeapon", 0x513d},
		{"isRocketCorpse", 0x5193},
		{"joining_team", 0x528c},
		{"kill_streak", 0x533c},
		{"killstreakRoundDelay", 0x53a2},
		{"killstreakSpawnShield", 0x53a8},
		{"largeProjectileDamage", 0x54c0},
		{"lastDamageWasFromEnemy", 0x5591},
		{"lastDeathIcon", 0x5592},
		{"lastDroppableWeapon", 0x5598},
		{"lastFlashedTime", 0x55a1},
		{"lastConcussedTime", 0x558b},
		{"lastGrenadeSuicideTime", 0x55a5},
		{"lastHitTime", 0x55ab},
		{"lastPrimaryWeaponSwapTime", 0x55cd},
		{"lastShotFiredTime", 0x55d9},
		{"lastWave", 0x560e},
		{"leaving_team", 0x566d},
		{"lowerTextFontSize", 0x58a8},
		{"lowerTextY", 0x58a9},
		{"lowerTextYAlign", 0x58aa},
		{"plantedLethalEquip",0x5979},
		{"mapCenter", 0x5986},
		{"mapSize", 0x5990},
		{"matchbonus", 0x59df},
		{"matchRules_damageMultiplier", 0x59e6},
		{"matchRules_vampirism", 0x59eb},
		{"maxClients", 0x5a29},
		{"MaxEvents", 0x5a37},
		{"maxFontScale", 0x5a39},
		{"MaxKillstreaks", 0x5a3c},
		{"MaxLives", 0x5a40},
		{"MaxNumAwardsPerPlayer", 0x5a41},
		{"MaxLogClients", 0x5a42},
		{"MaxNumChallengesPerPlayer", 0x5a45},
		{"modifyPlayerDamage", 0x5d51},
		{"missionCallbacks", 0x5cdc },
		{"modifyPlayerDamage", 0x5d51},
		{"moveSpeedScaler", 0x5f7b},
		{"mp_createfx", 0x5fac},
		{"multiTeamBased", 0x5fec},
		{"objId", 0x6304},
		{"teamObjIds", 0x6305},
		{"OMAClassChanged", 0x6436},
		{"onForfeit", 0x64af},
		{"onNormalDeath", 0x64bf},
		{"onPlayerConnectAudioInit", 0x64c9},
		{"onPlayerScore", 0x64d5},
		{"onStartGameType", 0x64ec},
		{"onUse", 0x64f8},
		{"outFrames", 0x65c4},
		{"participants", 0x669d},
		{"tookWeaponFrom", 0x680b},
		{"placement", 0x6861},
		{"planeModel", 0x687c},
		{"player_speed", 0x6c19},
		{"playerHealth_RegularRegenDelay", 0x6cc3},
		{"bonusUpdateTotal",0x6e8a},
		{"primaryProgressBarFontSize", 0x6f85},
		{"primaryProgressBarHeight", 0x6f86},
		{"primaryProgressBarTextX", 0x6f87},
		{"primaryProgressBarTextY", 0xf88},
		{"primaryProgressBarWidth", 0x6f89},
		{"primaryProgressBarX", 0x6f8a},
		{"primaryProgressBarY", 0x6f8b},
		{"rankTable", 0x713a},
		{"recoilScale", 0x7268},
		{"regenSpeed", 0x72d4},
		{"registerHalfTimeDvar", 0x72ef},
		{"registerNumLivesDvar", 0x72f4},
		{"registerRoundLimitDvar", 0x72f6},
		{"registerRoundSwitchDvar", 0x72f7},
		{"registerScoreLimitDvar", 0x72f8},
		{"registerTimeLimitDvar", 0x72f9},
		{"registerWinLimitDvar", 0x72fe},
		{"reInitializeMatchRulesOnMigration", 0x7307},
		{"removeFromCharactersArray", 0x73a7},
		{"requiredMapAspectRatio", 0x740c},
		{"reverb_settings", 0x74f1},
		{"riotShieldXPBullets", 0x7446},
		{"script_accumulate", 0x792B},
		{"script_destructable_area", 0x799b},
		{"script_fxid", 0x79f1},
		{"script_threshold", 0x7af4},
		{"setClass", 0x7f3b},
		{"setCommonRulesFromMatchRulesData", 0x7f3f},
		{"setObjectiveHintText", 0x7fc3},
		{"setObjectiveScoreText", 0x7fc4},
		{"setObjectiveText", 0x7fc5},
		{"setUseTime", 0x834c},
		{"setupMiniMap", 0x8324},
		{"shieldBulletHits", 0x84c5},
		{"shieldDamage", 0x84c6},
		{"showToTeam", 0x8535},
		{"softLanding", 0x885f},
		{"softLandingTriggers", 0x8860},
		{"spawnDogTags", 0x899e},
		{"spawnPoints", 0x8a01},
		{"splashQueue", 0x8a6b},
		{"splitscreen", 0x8a7c},
		{"stingerLockStartTime", 0x8e48},
		{"stingerLostSightlineTime", 0x8e49},
		{"stingerStage", 0x8e59},
		{"stingerTarget", 0x8e5a},
		{"stingerUseEntered", 0x8e5c},
		{"stunScaler", 0x8f77},
		{"spawnMaxs", 0x89f3},
		{"spawnMins", 0x89f6},
		{"stuckEnemyEntity", 0x8f6c},
		{"suicides", 0x8faf},
		{"switching_teams", 0x907e},
		{"tag_stowed_hip", 0x90d3},
		{"tagTeamUpdater", 0x910a},
		{"teamBalance", 0x91e9},
		{"teamBased", 0x91eb },
		{"teamkillsThisRound", 0x91f3},
		{"teamNameList", 0x91f7 },
		{"teamProgressBarFontSize", 0x91fb},
		{"teamProgressBarHeight", 0x91fc},
		{"teamProgressBarTextY", 0x91fd},
		{"teamProgressBarWidth", 0x91fe},
		{"teamProgressBarY", 0x91ff},
		{"throwingGrenade", 0x933e},
		{"timePlayed", 0x9372},
		{"teamSpawnPoints", 0x9201},
		{"useRate", 0x9c10},
		{"useStartSpawns", 0x9c14},
		{"usingRemote", 0x9c34},
		{"v", 0x9c42},
		{"visuals", 0x9e9c },
		{"teamNameList", 0x91f7},
		{"whizby_settings", 0xa315},
		{"wasAliveAtMatchStart", 0xa1be },
		{"wasChained", 0xa1c0},
		{"wasDamaged", 0xa1c3},
		{"wasDamagedFromBulletPenetration", 0xa1c5},
		{"waveDelay", 0xa2ab},
		{"scoreInfo", 0xa3a5},
		{"xpUpdateTotal", 0xa3ab},
		// additional findings from gametype/map scripts - mikey (6/26/2022)
		{"common_scripts/_fx", 0xa4fb },
		{"common_scripts/_pipes", 0xa4f9 },
		{"common_scripts/utility", 0xa4fa },
		{"maps/mp/_animatedmodels", 0xa72b},
		{"maps/mo/_areas", 0xa72c}, 
		{"maps/mp/_audio", 0xa72e},
		{"maps/mp/_compass", 0xa731},
		{"masp/mp/_createfx", 0xa732},
		{"maps/mp/_crib", 0xa733}, // sp files, missing on mikey dump
		{"maps/mp/_destructables", 0xa734},
		{"maps/mp/_entityheadicons", 0xa737},
		{"maps/mp/_matchdata", 0xa74e},
		{"maps/mp/_flashgranades", 0xa747},
		{"maps/mp/_global_fx", 0xa749},
		{"maps/mp/_global_fx_code", 0xa74a},
		{"maps/mp/_load", 0xa74c},
		{"maps/mp/_scoreboard", 0xa758},
		{"maps/mp/_shutter", 0xa759},
		{"maps/mp/_stinger", 0xa75b},
		{"maps/mp/_utility", 0xa764},
		{"maps/mp/gametypes/_class", 0xa78b},
		{"maps/mp/gametypes/_damage", 0xa78d},
		{"maps/mp/gametypes/_damagefeedback", 0xa78e},
		{"maps/mp/gametypes/_deathicons", 0xa78f},
		{"maps/mp/gametypes/_gamelogic", 0xa793},
		{"maps/mp/gametypes/_gameobjects", 0xa794},
		{"maps/mp/gametypes/_globallogic", 0xa797},
		{"maps/mp/gametypes/_healthoverlay", 0xa799},
		{"maps/mp/gametypes/_hostmigration", 0xa7a4},
		{"maps/mp/gametypes/_hud", 0xa7a5 },
		{"maps/mp/gametypes/_hud_message", 0xa7a6},
		{"maps/mp/gametypes/_hud_util", 0xa7a7 },
		{"maps/mp/gametypes/_menus", 0xa7a9},
		{"maps/mp/gametypes/_objpoints", 0xa7ac},
		{"maps/mp/gametypes/_presistence", 0xa7ae},
		{"maps/mp/gametypes/_playerlogic", 0xa7b1},
		{"maps/mp/gametypes/_rank", 0xa7b4},
		{"maps/mp/gametypes/_spawnlogic", 0xa7b9},
		{"maps/mp/gametypes/_spawnscoring", 0xa7ba},
		{"maps/mp/gametypes/_teams", 0xa7bc},
		{"maps/mp/gametypes/_perks", 0xa7e4},
		{"maps/mp/gametypes/_perkfunctions", 0xa7e5}
	};
}
