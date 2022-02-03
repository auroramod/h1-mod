#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	/***************************************************************
	 * Functions
	 **************************************************************/

	WEAK symbol<void(int localClientNum, const char* text)> Cbuf_AddText{ 0x140342EB0, 0x1404033B0 }; // H1MP64[AYRIA]
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* text)> Cmd_ExecuteSingleCommand{ 0x140343980, 0x140343980 }; // H1SP64[CODEX]

	WEAK symbol<void(const char* cmdName, void(), cmd_function_s* allocedCmd)> Cmd_AddCommandInternal{ 0x1403433E0, 0x140403950 }; // H1MP64[AYRIA]

	WEAK symbol<void()> Com_Frame_Try_Block_Function{ 0, 0x1400D8310 }; //H1MP MWR TEST

	WEAK symbol<void(unsigned int weapon, bool isAlternate, char* output, unsigned int maxStringLen)> BG_GetWeaponNameComplete{ 0x0, 0x140165580 };

	// wip
	WEAK symbol<void()> GScr_LoadConsts{ 0x140283970, 0x1403479C0 };
	WEAK symbol<unsigned int(unsigned int, unsigned int)> GetVariable{ 0x0, 0x1403F3730 };
	WEAK symbol<scr_string_t(unsigned int parentId, unsigned int id)> GetVariableName{ 0x1403170E0, 0x1403F37F0 };


	WEAK symbol<void()> Com_Quit_f{ 0, 0x1400DA640 }; //H1MP64[AYRIA]

	WEAK symbol<void(const char* text_in)> Cmd_TokenizeString{ 0, 0x1404046F0 }; //H1MP64[AYRIA]

	WEAK symbol<void(const char* dvar, const char* buffer)> Dvar_SetCommand{ 0x1403C72B0, 0x1404FD0A0 }; //H1MP64[AYRIA] NOT_SURE

	WEAK symbol<void(errorParm code, const char* message, ...)> Com_Error{ 0, 0x1400D78A0 }; //H1MP64[AYRIA]

	// WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessage{ 0x1389A0 }; // H1SP64[CODEX]
	// WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessageBold{ 0x138750 }; // H1SP64[CODEX]

	WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessage{ 0x1401389A0, 0x140220CC0 }; // H1MP64[AYRIA]
	WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessageBold{ 0x140138750, 0x140220620 }; // H1MP64[AYRIA]
	WEAK symbol<void(const char* message)> Conbuf_AppendText{ 0, 0x140513FF0 }; // H1MP64[AYRIA]

	WEAK symbol<char* (int start)> ConcatArgs{ 0x14021A7E0, 0x1402E9670 };

	WEAK symbol<void()> Cmd_EndTokenizeString{ 0, 0x140403C20 }; // H1MP64[AYRIA]

	WEAK symbol<dvar_t* (const char* name)> Dvar_FindVar{ 0x1403C5D50, 0x1404FBB00 }; // H1MP64[AYRIA]
	WEAK symbol<const char* (const dvar_t* dvar, dvar_value value, __int64 a3)> Dvar_ValueToString{ 0x1403C8560,0x1404FE660 };  // H1MP64[AYRIA]; different typedef from previous titles
	//WEAK symbol<void(int hash, const char* name, const char* buffer)> Dvar_SetCommand{ 0, 0x1404FD0A0 }; // H1MP64[AYRIA]

	WEAK symbol<dvar_t* (int hash, const char* name, bool value, unsigned int flags)> Dvar_RegisterBool{ 0x1403C47E0,0x1404FA540 }; // H1
	//WEAK symbol<dvar_t* (const char* dvarName, bool value, unsigned int flags, const char* description)>
		//Dvar_RegisterBool{ 0, 0x1404FA540 };
	WEAK symbol<dvar_t* (int hash, const char* name, int value, int min, int max, unsigned int flags)> Dvar_RegisterInt{ 0, 0x1404FAA20 }; // H1
	//WEAK symbol<dvar_t* (const char* dvarName, int value, int min, int max, unsigned int flags, const char* desc)>
		//Dvar_RegisterInt{ 0, 0x1404FAA20 };
	WEAK symbol<dvar_t* (int hash, const char* dvarName, float value, float min, float max, unsigned int flags)> Dvar_RegisterFloat{ 0,0x1404FA910 }; // H1MP64[AYRIA]
	WEAK symbol<dvar_t* (int hash, const char* dvarName, const char* value, unsigned int flags)> Dvar_RegisterString{ 0,0x1404FAB00 }; // H1MP64[AYRIA]
	WEAK symbol<dvar_t* (int dvarName, const char* a2, float x, float y, float z, float w, float min, float max, unsigned int flags)> Dvar_RegisterVec4{ 0, 0x1404FAF40 }; // H1MP64[AYRIA]

	WEAK symbol<int(const char* fname)> generateHashValue{ 0x1400FE8A0, 0x1401B1010 }; // H1MP64[AYRIA]

	//WEAK symbol<bool()> CL_IsCgameInitialized{ 0x17EE30 }; // H1SP64[CODEX]
	WEAK symbol<bool()> CL_IsCgameInitialized{ 0, 0x140245650 }; //H1MP64[AYRIA]
	WEAK symbol<unsigned int(int)> Live_SyncOnlineDataFlags{ 0, 0x14059A700 }; //H1MP64[AYRIA]

	WEAK symbol<void()> Sys_Milliseconds{ 0, 0x140513710 };
	WEAK symbol<bool()> Sys_IsDatabaseReady2{ 0, 0x14042B090 }; //H1MP64[AYRIA]

	WEAK symbol<void(netadr_s* from)> SV_DirectConnect{ 0, 0x140480860 };
	WEAK symbol<void(const char* text_in)> SV_Cmd_TokenizeString{ 0, 0x140404D20 };
	WEAK symbol<void()> SV_Cmd_EndTokenizedString{ 0, 0x140404CE0 };
	WEAK symbol<bool(int, void const*, const netadr_s*)> Sys_SendPacket{ 0, 0x1405133B0 };
	WEAK symbol<void(netadr_s*, sockaddr*)> NetadrToSockadr{ 0, 0x1404F62F0 };
	WEAK symbol<void(netsrc_t, netadr_s*, const char*)> NET_OutOfBandPrint{ 0, 0x1404255D0 };
	WEAK symbol<SOCKET> query_socket{ 0, 0x14DDFBF98 };
	WEAK symbol<void(netsrc_t sock, int length, const void* data, const netadr_s* to)> NET_SendLoopPacket{ 0, 0x140425790 };

	//WEAK symbol<dvar_t* (const char* dvarName, int value, int min, int max, unsigned int flags, const char* desc)>
		//Dvar_RegisterInt{ 0, 0x140 };

	WEAK symbol<bool(int clientNum)> SV_BotIsBot{ 0, 0x14046E6C0 };

	WEAK symbol<char* (char* string)> I_CleanStr{ 0, 0x1403CD230 }; // H1SP64[CODEX]
	WEAK symbol<ScreenPlacement* ()> ScrPlace_GetViewPlacement{ 0, 0x140288550 }; // H1MP64[AYRIA]

	WEAK symbol<Material* (const char* material)> Material_RegisterHandle{ 0, 0x1405EAB30 };  // H1MP64[AYRIA]

	WEAK symbol<void(float x, float y, float width, float height, float s0, float t0, float s1, float t1,
		float* color, Material* material)> R_AddCmdDrawStretchPic{ 0, 0x1402443A0 }; // H1MP64[AYRIA]

	WEAK symbol<Font_s* (const char* font, int size)> R_RegisterFont{ 0, 0x1405D91E0 }; // H1MP64[AYRIA]
	WEAK symbol<int(void* font)> R_GetFontHeight{ 0, 0x1405D92C0 }; // H1MP64[AYRIA]
	WEAK symbol<void* (int a1)> JUST_DO_OUR_DIRTY_WORK{ 0, 0x1405D8890 }; // H1MP64[AYRIA]

	WEAK symbol<int(int clientNum)> G_GetClientScore{ 0, 0x140342F90 }; // H1MP64[AYRIA]
	WEAK symbol<const char* (int clientNum)> SV_GetGuid{ 0, 0x140484B90 }; // H1MP64[AYRIA]
	WEAK symbol<int(int clientNum)> SV_GetClientPing{ 0, 0x140484B70 }; // H1MP64[AYRIA]

	WEAK symbol<void* (const char* text, int maxChars, void* font, int fontHeight, float x, float y, float xScale, float yScale,
		float rotation, float* color, int style, int cursor_pos, char cursor_char, void* style_unk)> H1_AddBaseDrawTextCmd{ 0,0x1405FB1F0 }; // H1MP64[AYRIA]

#define R_AddCmdDrawText(TXT,MC,F,X,Y,XS,YS,R,C,S) H1_AddBaseDrawTextCmd(TXT,MC,F,game::R_GetFontHeight(F),X,Y,XS,YS,R,C,S,-1,0,game::JUST_DO_OUR_DIRTY_WORK(S))

#define R_AddCmdDrawTextWithCursor(TXT,MC,F,UNK,X,Y,XS,YS,R,C,S,CP,CC) H1_AddBaseDrawTextCmd(TXT,MC,F,game::R_GetFontHeight(F),X,Y,XS,YS,R,C,S,CP,CC,game::JUST_DO_OUR_DIRTY_WORK(S))

	WEAK symbol<int(const char* text, int maxChars, Font_s* font)> R_TextWidth{ 0, 0x1405D94A0 }; // H1MP64[AYRIA]

	//WEAK symbol<void()> Com_Quit_f{ 0x352BE0 }; //H1SP64[CODEX]

	WEAK symbol<cmd_function_s*> cmd_functions{ 0,0x14946BAC8 }; //H1MP64[AYRIA]
	WEAK symbol<int> keyCatchers{ 0,0x142D0BA9C }; //H1MP64[AYRIA]
	WEAK symbol<PlayerKeyState> playerKeys{ 0,0x142C19AFC }; //H1MP64[AYRIA]
	WEAK symbol<CmdArgs> cmd_args{ 0, 0x140000000 }; //H1SP64[CODEX]

	//WEAK symbol<gentity_s> g_entities{ 0, 0x621E530 }; //H1MP64[ARYIA]
	//WEAK symbol<gentity_s> g_entities{ 0x550DD90 }; //H1SP64[CODEX]

	//###########################################################################################################################################################################
	//###########################################################################################################################################################################
	//###########################################################################################################################################################################

	WEAK symbol<void()> G_Glass_Update{ 0, 0x14033A640 }; // H1MP64[AYRIA]

	WEAK symbol<void(int type, VariableUnion u)> AddRefToValue{ 0, 0x1405C0EB0 };
	WEAK symbol<void(unsigned int id)> AddRefToObject{ 0,0x1405C0EA0 };
	WEAK symbol<unsigned int(unsigned int id)> AllocThread{ 0,0x1405C1200 };
	WEAK symbol<void(int type, VariableUnion u)> RemoveRefToValue{ 0, 0x1405C29B0 };
	WEAK symbol<void(unsigned int id)> RemoveRefToObject{ 0,0x1405C28A0 };


	WEAK symbol<void(XAssetType type, void(__cdecl* func)(game::XAssetHeader, void*), const void* inData, bool includeOverride)>
		DB_EnumXAssets_Internal{ 0, 0x1404129F0 };
	WEAK symbol<const char* (const XAsset* asset)> DB_GetXAssetName{ 0,0x1403E4090 };
	WEAK symbol<void(XZoneInfo* zoneInfo, unsigned int zoneCount, DBSyncMode syncMode)> DB_LoadXAssets{ 0,0x140414FF0 };
	WEAK symbol<XAssetHeader(XAssetType type, const char* name, int allowCreateDefault)> DB_FindXAssetHeader{ 0, 0x140412F60 };
	WEAK symbol<int(const RawFile* rawfile)> DB_GetRawFileLen{ 0,0x140413D80 };
	WEAK symbol<int(const RawFile* rawfile, char* buf, int size)> DB_GetRawBuffer{ 0,0x140413C40 };

	WEAK symbol<unsigned int(unsigned int parentId, unsigned int name)> FindVariable{ 0,0x1405C1D50 };
	WEAK symbol<unsigned int(int entnum, unsigned int classnum)> FindEntityId{ 0, 0x1405C1C50 };
	WEAK symbol<void(VariableValue* result, unsigned int classnum, int entnum, int offset)> GetEntityFieldValue{ 0, 0x1405C6100 };

	WEAK symbol<unsigned int(const char* name)> G_GetWeaponForName{ 0, 0x14051B260 };
	WEAK symbol<int(void* ps, unsigned int weapon, int a3, int a4, __int64 a5, int a6)>
		G_GivePlayerWeapon{ 0, 0x14051B660 };
	WEAK symbol<void(void* ps, const unsigned int weapon, int hadWeapon)> G_InitializeAmmo{ 0, 0x1404C4110 };
	WEAK symbol<void(int clientNum, const unsigned int weapon)> G_SelectWeapon{ 0,0x14051C0D0 };

	WEAK symbol<char* (GfxImage* image, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipCount,
		uint32_t imageFlags, DXGI_FORMAT imageFormat, int a8, const char* name, const void* initData)> Image_Setup{ 0, 0x14074B2A0 };

	WEAK symbol<void(int clientNum, const char* menu, int a3, int a4, unsigned int a5)> LUI_OpenMenu{ 0, 0x1405F0EE0 };
	WEAK symbol<bool(int clientNum, const char* menu)> Menu_IsMenuOpenAndVisible{ 0, 0x1405EE1A0 };

	WEAK symbol<const float* (const float* v)> Scr_AllocVector{ 0, 0x1405C3220 };
	WEAK symbol<void()> Scr_ClearOutParams{ 0, 0x1405C6E50 };
	WEAK symbol<scr_entref_t(unsigned int entId)> Scr_GetEntityIdRef{ 0, 0x1405C56C0 };
	WEAK symbol<int(unsigned int classnum, int entnum, int offset)> Scr_SetObjectField{ 0,0x140512190 };
	WEAK symbol<void(unsigned int id, scr_string_t stringValue, unsigned int paramcount)> Scr_NotifyId{ 0,0x1405C8240 };

	WEAK symbol<unsigned int(unsigned int localId, const char* pos, unsigned int paramcount)> VM_Execute{ 0, 0x1405C8DB0 };

	WEAK symbol<void()> R_SyncRenderThread{ 0,0x14076E7D0 };
	WEAK symbol<void(const void* obj, void* pose, unsigned int entnum, unsigned int renderFxFlags, float* lightingOrigin,
		float materialTime, __int64 a7, __int64 a8)> R_AddDObjToScene{ 0, 0x140775C40 };

	WEAK symbol<const char* (scr_string_t stringValue)> SL_ConvertToString{ 0,0x1405BFBB0 };
	WEAK symbol<scr_string_t(const char* str, unsigned int user)> SL_GetString{ 0, 0x1405C0170 };

	WEAK symbol<bool()> SV_Loaded{ 0,0x140442F60 };

	WEAK symbol<void()> Sys_ShowConsole{ 0,0x140514910 };

	WEAK symbol<const char* (const char* string)> UI_SafeTranslateString{ 0, 0x5A2930 };

	WEAK symbol<void* (jmp_buf* Buf, int Value)> longjmp{ 0, 0x14089EED0 };
	WEAK symbol<int(jmp_buf* Buf)> _setjmp{ 0, 0x1408EC2E0 };

	WEAK symbol<void(int arg, char* buffer, int bufferLength)> SV_Cmd_ArgvBuffer{ 0x1402EEFD0, 0x1403B05C0 };

	// Variables
	WEAK symbol<CmdArgs> sv_cmd_args{ 0, 0x14946BA20 }; // mwr maybe


	WEAK symbol<const char*> g_assetNames{ 0, 0x140BEF280 };
	WEAK symbol<int> g_poolSize{ 0, 0x140BF2E40 };

	WEAK symbol<GfxDrawMethod_s> gfxDrawMethod{ 0,0x14EDF9E00 };

	WEAK symbol<int> dvarCount{ 0, 0x14BFBB310 };
	WEAK symbol<dvar_t*> sortedDvars{ 0,0x14BFBB320 };

	WEAK symbol<unsigned int> levelEntityId{ 0,0x14B5E0B30 };
	WEAK symbol<int> g_script_error_level{ 0,0x14BA9CC24 };
	WEAK symbol<jmp_buf> g_script_error{ 0,0x14BA9CD40 };
	WEAK symbol<scr_classStruct_t> g_classMap{ 0, 0x140BF95C0 };

	WEAK symbol<scrVarGlob_t> scr_VarGlob{ 0, 0x14B617C00 };
	WEAK symbol<scrVmPub_t> scr_VmPub{ 0,0x14BA9EE40 };
	WEAK symbol<function_stack_t> scr_function_stack{ 0,0x14BAA93C0 };

	namespace mp
	{
		WEAK symbol<gentity_s> g_entities{ 0, 0x14621E530 }; // H1MP64[AYRIA]
		WEAK symbol<client_t> svs_clients{ 0, 0x14B204A10 }; // H1MP64[AYRIA]
	}

	namespace sp
	{
		WEAK symbol<gentity_s> g_entities{ 0x14550DD90 , 0 };
	}

	namespace hks
	{
		WEAK symbol<lua_State*> lua_state{0, 0x1412E2B50};
		WEAK symbol<void(lua_State* s, const char* str, unsigned int l)> hksi_lua_pushlstring{0, 0x1400290B0};
		WEAK symbol<HksObject*(HksObject* result, lua_State* s, const HksObject* table, const HksObject* key)> hks_obj_getfield{0, 0x14009D3C0};
		WEAK symbol<void(lua_State* s, const HksObject* tbl, const HksObject* key, const HksObject* val)> hks_obj_settable{0, 0x14009E480};
		WEAK symbol<HksObject* (HksObject* result, lua_State* s, const HksObject* table, const HksObject* key)> hks_obj_gettable{0, 0x14009D800};
		WEAK symbol<void(lua_State* s, int nargs, int nresults, const unsigned int* pc)> vm_call_internal{0, 0x1400C9EC0};
		WEAK symbol<HashTable*(lua_State* s, unsigned int arraySize, unsigned int hashSize)> Hashtable_Create{0, 0x14008AAE0};
		WEAK symbol<cclosure*(lua_State* s, lua_function function, int num_upvalues,
			int internal_, int profilerTreatClosureAsFunc)> cclosure_Create{0, 0x14008AD00};
		WEAK symbol<int(lua_State* s, int t)> hksi_luaL_ref{0, 0x1400A7D60};
		WEAK symbol<void(lua_State* s, int t, int ref)> hksi_luaL_unref{0, 0x1400A0660};
	}
}
