#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	/***************************************************************
	 * Functions
	 **************************************************************/

	WEAK symbol<void(int type, VariableUnion u)> AddRefToValue{0x36E600, 0x43C580};
	WEAK symbol<void(int type, VariableUnion u)> RemoveRefToValue{0x3700F0, 0x43E090};
	WEAK symbol<void(unsigned int id)> AddRefToObject{0x36E5F0, 0x43C570};
	WEAK symbol<void(unsigned int id)> RemoveRefToObject{0x36FFE0, 0x43DF80};
	WEAK symbol<unsigned int(unsigned int id)> AllocThread{0x36E960, 0x43C8E0};

	WEAK symbol<void(int localClientNum, const char* text)> Cbuf_AddText{ 0x342EB0, 0x1CF480 }; // H1MP(1.15)
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* buffer,
		void (int, int, const char*))> Cbuf_ExecuteBufferInternal{0x342FC0, 0x4034C0};
	WEAK symbol<void(const char* message)> Conbuf_AppendText{0x3E3300, 0x513FF0};
	WEAK symbol<char*(int start)> ConcatArgs{0x2697F0, 0x335D70};
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* text)> Cmd_ExecuteSingleCommand{0x343980, 0x403F60};
	WEAK symbol<void(const char* cmdName, void(), cmd_function_s* allocedCmd)> Cmd_AddCommandInternal{0x3433E0, 0x403950};
	WEAK symbol<void(const char*)> Cmd_RemoveCommand{0x343FF0, 0x4045D0};
	WEAK symbol<void(const char* text_in)> Cmd_TokenizeString{0x344110, 0x4046F0};
	WEAK symbol<void()> Cmd_EndTokenizeString{0x343630, 0x403C20};

	WEAK symbol<void(void*, void*)> AimAssist_AddToTargetList{0, 0x9D0F0};

	WEAK symbol<DWOnlineStatus()> dwGetLogOnStatus{ 0, 0x14053CCB0 }; // S1

	WEAK symbol<void(unsigned int weapon, bool isAlternate, 
		char* output, unsigned int maxStringLen)> BG_GetWeaponNameComplete{0x430550, 0x1F9670};

	WEAK symbol<void()> Com_Frame_Try_Block_Function{0x1CE8D0, 0xD8310};
	WEAK symbol<CodPlayMode()> Com_GetCurrentCoDPlayMode{0, 0x5039A0};
	WEAK symbol<bool()> Com_InFrontEnd{0xE4B30, 0x176A30};
	WEAK symbol<void(float, float, int)> Com_SetSlowMotion{0, 0xDB790};
	WEAK symbol<void(errorParm code, const char* message, ...)> Com_Error{0x3509C0, 0xD78A0};
	WEAK symbol<void()> Com_Quit_f{0x352BE0, 0xDA830};
	WEAK symbol<void()> Quit{0x352D90, 0xDA830};

	WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessage{0x1389A0, 0x220CC0};
	WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessageBold{0x138750, 0x220620};

	WEAK symbol<bool()> CL_IsCgameInitialized{0x17EE30, 0x245650};

	WEAK symbol<void(int hash, const char* name, const char* buffer)> Dvar_SetCommand{0x3C72B0, 0x4FD0A0};
	WEAK symbol<dvar_t*(const char* name)> Dvar_FindVar{0x3C5D50, 0x4FBB00};
	WEAK symbol<void(char* buffer, int index)> Dvar_GetCombinedString{0x354DF0, 0x41D830};
	WEAK symbol<const char*(dvar_t* dvar, dvar_value value)> Dvar_ValueToString{0x3C8560, 0x4FE660};
	WEAK symbol<dvar_t*(int hash, const char* name, bool value, 
		unsigned int flags)> Dvar_RegisterBool{0x3C47E0, 0x4FA540};
	WEAK symbol<dvar_t*(int hash, const char* name, int value, int min, int max, 
		unsigned int flags)> Dvar_RegisterInt{0x3C4CC0, 0x4FAA20};
	WEAK symbol<dvar_t*(int hash, const char* dvarName, float value, float min, 
		float max, unsigned int flags)> Dvar_RegisterFloat{0x3C4BB0, 0x4FA910};
	WEAK symbol<dvar_t*(int hash, const char* dvarName, const char* value, 
		unsigned int flags)> Dvar_RegisterString{0x3C4DA0, 0x4FAB00};
	WEAK symbol<dvar_t*(int dvarName, const char* a2, float x, float y, float z, 
		float w, float min, float max, unsigned int flags)> Dvar_RegisterVec4{0x3C5220, 0x4FAF40};

	WEAK symbol<long long(const char* qpath, char** buffer)> FS_ReadFile{0x3B9020, 0x4EE720};
	WEAK symbol<void(void* buffer)> FS_FreeFile{0x3B9010, 0x4EE710};
	WEAK symbol<void(const char* gameName)> FS_Startup{0x3B85D0, 0x4EDD30};
	WEAK symbol<void(const char* path, const char* dir)> FS_AddLocalizedGameDirectory{0x3B6030, 0x4EBE20};

	WEAK symbol<unsigned int(unsigned int, unsigned int)> GetVariable{0x36FDD0, 0x3F3730};

	WEAK symbol<void()> GScr_LoadConsts{0x2D13E0, 0x393810};
	WEAK symbol<unsigned int(unsigned int parentId, unsigned int name)> FindVariable{0x36F4B0, 0x43D430};
	WEAK symbol<unsigned int(int entnum, unsigned int classnum)> FindEntityId{0x36F3B0, 0x43D330};
	WEAK symbol<void(unsigned int parentId, unsigned int index)> RemoveVariableValue{0x370190, 0x43E130};
	WEAK symbol<void(VariableValue* result, unsigned int classnum, 
		int entnum, int offset)> GetEntityFieldValue{0x373780, 0x441780};

	WEAK symbol<int(const char* fname)> generateHashValue{ 0xFE8A0, 0x183F80 }; // STEEEEEEEEEEEEAM

	WEAK symbol<void()> G_Glass_Update{0x26C570, 0x33A640};
	WEAK symbol<int(int clientNum)> G_GetClientScore{0, 0x342F90};

	WEAK symbol<char*(char* string)> I_CleanStr{0x3CD230, 0x503D00};

	WEAK symbol<const char* (int, int, int)> Key_KeynumToString{0x187CC0, 0x24FE10};

	WEAK symbol<unsigned int(int)> Live_SyncOnlineDataFlags{0, 0x59A700};

	WEAK symbol<Material* (const char* material)> Material_RegisterHandle{0x4E48B0, 0x5EAB30};

	WEAK symbol<void(netadr_s*, sockaddr*)> NetadrToSockadr{0x3C11C0, 0x4F62F0};
	WEAK symbol<void(netsrc_t, netadr_s*, const char*)> NET_OutOfBandPrint{ 0x357560, 0x4F1DE0 }; // STEEAM
	WEAK symbol<void(netsrc_t sock, int length, const void* data, const netadr_s* to)> NET_SendLoopPacket{0, 0x425790};
	WEAK symbol<bool(const char* s, netadr_s* a)> NET_StringToAdr{0, 0x425870};

	WEAK symbol<void(float x, float y, float width, float height, float s0, float t0, float s1, float t1,
		float* color, Material* material)> R_AddCmdDrawStretchPic{0x17E5C0, 0x2443A0};
	WEAK symbol<Font_s* (const char* font, int size)> R_RegisterFont{ 0x4D4100, 0x67F630 }; // H1MP(1.15)
	WEAK symbol<int(const char* text, int maxChars, Font_s* font)> R_TextWidth{0x4D43B0, 0x5D94A0};
	WEAK symbol<int(void* font)> R_GetFontHeight{0x5EA360, 0x67F710};
	WEAK symbol<void* (int a1)> R_DrawSomething{0x4D37B0, 0x67ECE0};
	WEAK symbol<void()> R_SyncRenderThread{0x4F8240, 0x5FF3A0};
	WEAK symbol<void* (const char* text, int maxChars, void* font, int fontHeight, float x, float y, float xScale, float yScale, float rotation, float* color, int style, int cursor_pos, char cursor_char, void* style_unk)> H1_AddBaseDrawTextCmd{ 0x4F3DC0,0x6A3080 }; // H1MP(1.15)

#define R_AddCmdDrawText(TXT, MC, F, X, Y, XS, YS, R, C, S) \
	H1_AddBaseDrawTextCmd(TXT, MC, F, game::R_GetFontHeight(F), X, Y, XS, YS, R, C, S,-1, 0, game::R_DrawSomething(S))
#define R_AddCmdDrawTextWithCursor(TXT, MC, F, UNK, X, Y, XS, YS, R, C, S, CP, CC) \
	H1_AddBaseDrawTextCmd(TXT, MC, F, game::R_GetFontHeight(F), X, Y, XS, YS, R, C, S, CP, CC, game::R_DrawSomething(S))

	WEAK symbol<unsigned int(unsigned int localId, const char* pos, 
		unsigned int paramcount)> VM_Execute{0x376360, 0x444350};

	WEAK symbol<void(unsigned int id, scr_string_t stringValue, 
		unsigned int paramcount)> Scr_NotifyId{0x375800, 0x4437E0};
	WEAK symbol<const float*(const float* v)> Scr_AllocVector{0x370930, 0x43E7D0};
	WEAK symbol<float(int index)> Scr_GetFloat{0x374D20, 0x442D10};
	WEAK symbol<const char*(int index)> Scr_GetString{0x375210, 0x443150};
	WEAK symbol<int()> Scr_GetNumParam{0x374F30, 0x442E70};
	WEAK symbol<void()> Scr_ClearOutParams{0x374460, 0x442510};
	WEAK symbol<scr_entref_t(unsigned int entId)> Scr_GetEntityIdRef{0x372D50, 0x440D80};
	WEAK symbol<unsigned int(int classnum, unsigned int entnum)> Scr_GetEntityId{0x372CA0, 0x440CD0};

	WEAK symbol<ScreenPlacement* ()> ScrPlace_GetViewPlacement{0x1981F0, 0x288550};

	WEAK symbol<int(XAssetType type)> DB_GetXAssetTypeSize{0x19A3B0, 0x28BE70};

	WEAK symbol<void(int clientNum, const char* menu, 
		int a3, int a4, unsigned int a5)> LUI_OpenMenu{0x39D5F0, 0x4CD210};

	WEAK symbol<bool(int clientNum, const char* menu)> Menu_IsMenuOpenAndVisible{0x4709C0, 0x4C7320};

	WEAK symbol<scr_string_t(const char* str)> SL_FindString{0x36D700, 0x43B470};
	WEAK symbol<scr_string_t(const char* str, unsigned int user)> SL_GetString{0x36D9A0, 0x43B840};
	WEAK symbol<const char* (scr_string_t stringValue)> SL_ConvertToString{0x36D420, 0x43B170};
	WEAK symbol<int(unsigned int classnum, int entnum, int offset)> Scr_SetObjectField{0x2B9F60, 0x385330};

	WEAK symbol<void(netadr_s* from)> SV_DirectConnect{0, 0x480860};
	WEAK symbol<void(int arg, char* buffer, int bufferLength)> SV_Cmd_ArgvBuffer{0x3446C0, 0x404CA0};
	WEAK symbol<void(const char* text_in)> SV_Cmd_TokenizeString{0x344740, 0x404D20};
	WEAK symbol<void()> SV_Cmd_EndTokenizedString{0x344700, 0x404CE0};

	WEAK symbol<mp::gentity_s*(const char* name)> SV_AddBot{0, 0x480190};
	WEAK symbol<bool(int clientNum)> SV_BotIsBot{0, 0x46E6C0};
	WEAK symbol<const char* ()> SV_BotGetRandomName{0, 0x46DBA0};
	WEAK symbol<int(mp::gentity_s* ent)> SV_SpawnTestClient{0, 0x4832A0};

	WEAK symbol<const char* (int clientNum)> SV_GetGuid{0, 0x484B90};
	WEAK symbol<int(int clientNum)> SV_GetClientPing{0, 0x484B70};
	WEAK symbol<playerState_s* (int num)> SV_GetPlayerstateForClientNum{0x4426D0, 0x484C10};
	WEAK symbol<void(int index, const char* string)> SV_SetConfigstring{0, 0x486720};
	WEAK symbol<bool()> SV_Loaded{0x442F60, 0x4864A0};
	WEAK symbol<void(int clientNum, const char* reason)> SV_KickClientNum{0, 0x47ED00};
	WEAK symbol<bool(const char* map)> SV_MapExists{0, 0x47ED60};
	WEAK symbol<void(mp::client_t*, const char*, int)> SV_ExecuteClientCommand{0, 0x481870};
	WEAK symbol<void(int localClientNum)> SV_FastRestart{0, 0x47E990};
	WEAK symbol<void(int clientNum, svscmd_type type, const char* text)> SV_GameSendServerCommand{
		0x3F3A70, 0x484AD0
	};

	WEAK symbol<void()> Sys_ShowConsole{0x3E3B90, 0x514910};
	WEAK symbol<void(const char* error, ...)> Sys_Error{0x3E0C40, 0x511520};
	WEAK symbol<void(char* path, int pathSize, Sys_Folder folder, const char* filename, const char* ext)>
		Sys_BuildAbsPath{0x3CFF90, 0x507010};
	WEAK symbol<int()> Sys_Milliseconds{0x3E2B10, 0x513710};
	WEAK symbol<bool()> Sys_IsDatabaseReady2{0x3580B0, 0x42B090};
	WEAK symbol<bool(int, void const*, const netadr_s*)> Sys_SendPacket{0x3E2820, 0x5133B0};
	WEAK symbol<bool(const char* path)> Sys_FileExists{0x3E0CE0, 0x5115E0};

	WEAK symbol<const char* (const char*)> UI_GetMapDisplayName{0, 0x408CC0};
	WEAK symbol<const char* (const char*)> UI_GetGameTypeDisplayName{0, 0x4086A0};
	WEAK symbol<void(unsigned int localClientNum, const char** args)> UI_RunMenuScript{0x39EFF0, 0x4CFE60};
	WEAK symbol<int(const char* text, int maxChars, Font_s* font, float scale)> UI_TextWidth{0x3A0F20, 0x4D21A0};

	WEAK symbol<const char*(const char* string)> UI_SafeTranslateString{0x350430, 0x41C580};

	WEAK symbol<void* (jmp_buf* Buf, int Value)> longjmp{0x648FD4, 0x779F64};
	WEAK symbol<int(jmp_buf* Buf)> _setjmp{0x6BFD30, 0x7F5F90};

	/***************************************************************
	 * Variables
	 **************************************************************/

	WEAK symbol<CmdArgs> sv_cmd_args{0xAD99A10, 0x946BA20};

	WEAK symbol<int> g_script_error_level{0xA1917A8, 0xA33C824};
	WEAK symbol<jmp_buf> g_script_error{0xA1917B0, 0xA33C940};
	
	WEAK symbol<unsigned int> levelEntityId{0x9AF55B0, 0x9CA0730};
	WEAK symbol<unsigned int> gameEntityId{0x9CA0734, 0xB65E3B4};

	WEAK symbol<const char*> command_whitelist{0x1079A60, 0x120C360};
	WEAK symbol<cmd_function_s*> cmd_functions{ 0xAD99AB8,0x344DF18 }; // H1MP(1.15)
	WEAK symbol<CmdArgs> cmd_args{0xAD99960, 0x946B970};

	WEAK symbol<int> g_poolSize{0, 0xFEADF0};
	WEAK symbol<scr_classStruct_t> g_classMap{0x80A840, 0x12106B0};

	WEAK symbol<scrVarGlob_t> scr_VarGlob{0xB686480, 0x9CC8800};
	WEAK symbol<scrVmPub_t> scr_VmPub{0xA1938C0, 0xA33EA40};
	WEAK symbol<function_stack_t> scr_function_stack{0xBD06C40, 0xA348FC0};

	WEAK symbol<GfxDrawMethod_s> gfxDrawMethod{0xF05CE50, 0xFD21180};

	WEAK symbol<int> dvarCount{0xC217D10, 0xD064CF4};
	WEAK symbol<dvar_t*> dvarPool{0xC217D20, 0xD064D00};

	WEAK symbol<void*> DB_XAssetPool{0xDE8C80, 0xFEB5D0};

	WEAK symbol<int> keyCatchers{0x243DAF0, 0x2D0BA9C};
	WEAK symbol<PlayerKeyState> playerKeys{0x22A873C, 0x2C19AFC};

	WEAK symbol<SOCKET> query_socket{0, 0xDDFBF98};

	WEAK symbol<DWORD> threadIds{0xB19B880, 0x9810E00};

	namespace mp
	{
		WEAK symbol<gentity_s> g_entities{ 0, 0x71F19E0 }; // H1MP(1.15)
		WEAK symbol<client_t> svs_clients{0, 0xB204A10};
		WEAK symbol<int> svs_numclients{0, 0xB204A0C};
		WEAK symbol<int> gameTime{0, 0x621BDBC};

		WEAK symbol<int> sv_serverId_value{0, 0xA3E99B8};

		WEAK symbol<bool> virtualLobby_loaded{0, 0x2D077FD};
	}

	namespace sp
	{
		WEAK symbol<gentity_s> g_entities{0x550DD90, 0};
	}

	namespace hks
	{
		WEAK symbol<lua_State*> lua_state{0x1E2C2F8, 0x26D3D08};
		WEAK symbol<void(lua_State* s, const char* str, unsigned int l)> hksi_lua_pushlstring{0x4DA90, 0x624F0};
		WEAK symbol<HksObject*(HksObject* result, lua_State* s, const HksObject* table, const HksObject* key)> hks_obj_getfield{0x9C0A0, 0x12C600};
		WEAK symbol<void(lua_State* s, const HksObject* tbl, const HksObject* key, const HksObject* val)> hks_obj_settable{0x9D240, 0x12D820};
		WEAK symbol<HksObject* (HksObject* result, lua_State* s, const HksObject* table, const HksObject* key)> hks_obj_gettable{0x9C580, 0x12CAE0};
		WEAK symbol<void(lua_State* s, int nargs, int nresults, const unsigned int* pc)> vm_call_internal{0xC87A0, 0x159EB0};
		WEAK symbol<HashTable*(lua_State* s, unsigned int arraySize, unsigned int hashSize)> Hashtable_Create{0x8B3B0, 0x11B320};
		WEAK symbol<cclosure*(lua_State* s, lua_function function, int num_upvalues, 
			int internal_, int profilerTreatClosureAsFunc)> cclosure_Create{0x8B5D0, 0x11B540};
		WEAK symbol<int(lua_State* s, int t)> hksi_luaL_ref{0xA64D0, 0x136D30};
		WEAK symbol<void(lua_State* s, int t, int ref)> hksi_luaL_unref{0x9EF10, 0x12F610};
	}
}
