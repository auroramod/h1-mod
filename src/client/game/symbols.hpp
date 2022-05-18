#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	/***************************************************************
	 * Functions
	 **************************************************************/

	WEAK symbol<void(int type, VariableUnion u)> AddRefToValue{0x0, 0x5090E0};
	WEAK symbol<void(int type, VariableUnion u)> RemoveRefToValue{0x0, 0x50ABF0};
	WEAK symbol<void(unsigned int id)> AddRefToObject{0x0, 0x5090D0};
	WEAK symbol<void(unsigned int id)> RemoveRefToObject{0x0, 0x50AAE0};
	WEAK symbol<unsigned int(unsigned int id)> AllocThread{0x0, 0x509440};
	WEAK symbol<ObjectVariableValue*(unsigned int* id)> AllocVariable{0x0, 0x0};

	WEAK symbol<void(int localClientNum, int controllerIndex, const char* text)> Cbuf_AddText{0x0, 0x1CF480};
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* buffer,
		void (int, int, const char*))> Cbuf_ExecuteBufferInternal{0x0, 0x155BC0};
	WEAK symbol<void(const char* message)> Conbuf_AppendText{0x0, 0x0};
	WEAK symbol<char*(int start)> ConcatArgs{0x0, 0x413050};
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* text)> Cmd_ExecuteSingleCommand{0x0, 0x156E90};
	WEAK symbol<void(const char* cmdName, void(), cmd_function_s* allocedCmd)> Cmd_AddCommandInternal{0x0, 0x156880};
	WEAK symbol<void(const char*)> Cmd_RemoveCommand{0x0, 0x157690};
	WEAK symbol<void(const char* text_in)> Cmd_TokenizeString{0x0, 0x0}; // not a function
	WEAK symbol<void()> Cmd_EndTokenizeString{0x0, 0x0}; // not a function

	WEAK symbol<void(void*, void*)> AimAssist_AddToTargetList{0x0, 0xE66C0};

	WEAK symbol<void(unsigned int weapon, bool isAlternate, 
		char* output, unsigned int maxStringLen)> BG_GetWeaponNameComplete{0x0, 0x2E2500};

	WEAK symbol<void()> Com_Frame_Try_Block_Function{0x0, 0x0};
	WEAK symbol<CodPlayMode()> Com_GetCurrentCoDPlayMode{0x0, 0x5AEF80};
	WEAK symbol<bool()> Com_InFrontend{0x0, 0x76A40};
	WEAK symbol<void(float, float, int)> Com_SetSlowMotion{0x0, 0x17E5F0};
	WEAK symbol<void(errorParm code, const char* message, ...)> Com_Error{0x0, 0x0};
	WEAK symbol<void()> Com_Quit_f{0x0, 0x1F9280};
	WEAK symbol<void(char const* finalMessage)> Com_Shutdown{0x0, 0x0};

	WEAK symbol<void()> Quit{0x0, 0x17CF50};

	WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessage{0x0, 0x316210};
	WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessageBold{0x0, 0x3122F0};
	WEAK symbol<void(int localClientNum, /*mp::cg_s**/void* cg, 
		const char* dvar, const char* value)> CG_SetClientDvarFromServer{0x0, 0x0};
	WEAK symbol<char*(const unsigned int weapon, 
		bool isAlternate, char* outputBuffer, int bufferLen)> CG_GetWeaponDisplayName{0x0, 0x0};

	WEAK symbol<bool()> CL_IsCgameInitialized{0x0, 0x76A40};
	WEAK symbol<void(int a1)> CL_VirtualLobbyShutdown{0x0, 0x0};

	WEAK symbol<void(int hash, const char* name, const char* buffer)> Dvar_SetCommand{0x0, 0x1857D0};
	WEAK symbol<dvar_t*(const char* name)> Dvar_FindVar{0x0, 0x183EB0};
	WEAK symbol<void(const dvar_t* dvar)> Dvar_ClearModified{0x0, 0x0};
	WEAK symbol<void(char* buffer, int index)> Dvar_GetCombinedString{0x0, 0x4EA020};
	WEAK symbol<const char*(dvar_t* dvar, bool is_hashed, dvar_value value)> Dvar_ValueToString{0x0, 0x187000};
	WEAK symbol<void(dvar_t* dvar, DvarSetSource source)> Dvar_Reset{0x0, 0x185390};
	WEAK symbol<void(const char*, const char*, 
		DvarSetSource)> Dvar_SetFromStringByNameFromSource{0x0, 0x185BD0};

	WEAK symbol<dvar_t*(int hash, const char* name, bool value, 
		unsigned int flags)> Dvar_RegisterBool{0x0, 0x182340};
	WEAK symbol<dvar_t*(int hash, const char* name, int value, int min, int max, 
		unsigned int flags)> Dvar_RegisterInt{0x0, 0x182A10};
	WEAK symbol<dvar_t*(int hash, const char* dvarName, float value, float min, 
		float max, unsigned int flags)> Dvar_RegisterFloat{0x0, 0x182900};
	WEAK symbol<dvar_t*(int hash, const char* dvarName, const char* value, 
		unsigned int flags)> Dvar_RegisterString{0x0, 0x182AF0};
	WEAK symbol<dvar_t*(int dvarName, const char* a2, float x, float y, float z, 
		float w, float min, float max, unsigned int flags)> Dvar_RegisterVec4{0x0, 0x0};

	WEAK symbol<long long(const char* qpath, char** buffer)> FS_ReadFile{0x0, 0x0};
	WEAK symbol<void(void* buffer)> FS_FreeFile{0x0, 0x0};
	WEAK symbol<void(const char* gameName)> FS_Startup{0x0, 0x0};
	WEAK symbol<void(const char* path, const char* dir)> FS_AddLocalizedGameDirectory{0x0, 0x0};

	WEAK symbol<unsigned int(unsigned int, unsigned int)> GetVariable{0x0, 0x0};
	WEAK symbol<unsigned int(unsigned int parentId, unsigned int unsignedValue)> GetNewVariable{0x0, 0x0};
	WEAK symbol<unsigned int(unsigned int parentId, unsigned int unsignedValue)> GetNewArrayVariable{0x0, 0x0};
	WEAK symbol<void()> GScr_LoadConsts{0x0, 0x0};
	WEAK symbol<unsigned int(unsigned int parentId, unsigned int name)> FindVariable{0x0, 0x0};
	WEAK symbol<unsigned int(int entnum, unsigned int classnum)> FindEntityId{0x0, 0x0};
	WEAK symbol<void(unsigned int parentId, unsigned int index)> RemoveVariableValue{0x0, 0x0};
	WEAK symbol<void(VariableValue* result, unsigned int classnum, 
		int entnum, int offset)> GetEntityFieldValue{0x0, 0x0};

	WEAK symbol<int(const char* fname)> generateHashValue{0x0, 0x183F80};

	WEAK symbol<void()> G_Glass_Update{0x0, 0x0};
	WEAK symbol<int(int clientNum)> G_GetClientScore{0x0, 0x0};
	WEAK symbol<unsigned int(const char* name)> G_GetWeaponForName{0x0, 0x0};
	WEAK symbol<int(playerState_s* ps, unsigned int weapon, int dualWield, 
		int startInAltMode, int, int, int, char, ...)> G_GivePlayerWeapon{0x0, 0x0};
	WEAK symbol<void(playerState_s* ps, unsigned int weapon, int hadWeapon)> G_InitializeAmmo{0x0, 0x0};
	WEAK symbol<void(int clientNum, unsigned int weapon)> G_SelectWeapon{0x0, 0x0};
	WEAK symbol<int(playerState_s* ps, unsigned int weapon)> G_TakePlayerWeapon{0x0, 0x0};

	WEAK symbol<char*(char* string)> I_CleanStr{0x0, 0x0};

	WEAK symbol<const char* (int, int, int)> Key_KeynumToString{0x0, 0x0};

	WEAK symbol<unsigned int(int)> Live_SyncOnlineDataFlags{0x0, 0x0};

	WEAK symbol<Material* (const char* material)> Material_RegisterHandle{0x0, 0x692360};

	WEAK symbol<void(netadr_s*, sockaddr*)> NetadrToSockadr{0x0, 0x0};
	WEAK symbol<void(netsrc_t, netadr_s*, const char*)> NET_OutOfBandPrint{0x0, 0x0};
	WEAK symbol<void(netsrc_t sock, int length, const void* data, const netadr_s* to)> NET_SendLoopPacket{0x0, 0x0};
	WEAK symbol<bool(const char* s, netadr_s* a)> NET_StringToAdr{0x0, 0x0};

	WEAK symbol<void(float x, float y, float width, float height, float s0, float t0, float s1, float t1,
		float* color, Material* material)> R_AddCmdDrawStretchPic{0x0, 0x0};
	WEAK symbol<Font_s* (const char* font, int size)> R_RegisterFont{0x0, 0x67F630};
	WEAK symbol<int(const char* text, int maxChars, Font_s* font)> R_TextWidth{0x0, 0x0};
	WEAK symbol<int(void* font)> R_GetFontHeight{0x0, 0x67F710};
	WEAK symbol<void* (int a1)> R_DrawSomething{0x0, 0x67ECE0};
	WEAK symbol<void()> R_SyncRenderThread{0x0, 0x0};
	WEAK symbol<void* (const char* text, int maxChars, void* font, int fontHeight, float x, 
		float y, float xScale, float yScale, float rotation, float* color, 
		int style, int cursor_pos, char cursor_char, 
		void* style_unk)> H1_AddBaseDrawTextCmd{0x0, 0x6A3080};

#define R_AddCmdDrawText(TXT, MC, F, X, Y, XS, YS, R, C, S) \
	H1_AddBaseDrawTextCmd(TXT, MC, F, game::R_GetFontHeight(F), X, Y, XS, YS, R, C, S,-1, 0, game::R_DrawSomething(S))
#define R_AddCmdDrawTextWithCursor(TXT, MC, F, UNK, X, Y, XS, YS, R, C, S, CP, CC) \
	H1_AddBaseDrawTextCmd(TXT, MC, F, game::R_GetFontHeight(F), X, Y, XS, YS, R, C, S, CP, CC, game::R_DrawSomething(S))

	WEAK symbol<char* (GfxImage* image, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipCount,
		uint32_t imageFlags, DXGI_FORMAT imageFormat, const char* name, const D3D11_SUBRESOURCE_DATA* initData)> Image_Setup{0x0, 0x0};

	WEAK symbol<unsigned int(unsigned int localId, const char* pos, 
		unsigned int paramcount)> VM_Execute{0x0, 0x0};

	WEAK symbol<void(unsigned int id, scr_string_t stringValue, 
		unsigned int paramcount)> Scr_NotifyId{0x0, 0x0};
	WEAK symbol<const float*(const float* v)> Scr_AllocVector{0x0, 0x0};
	WEAK symbol<float(int index)> Scr_GetFloat{0x0, 0x0};
	WEAK symbol<const char*(int index)> Scr_GetString{0x0, 0x0};
	WEAK symbol<int()> Scr_GetNumParam{0x0, 0x0};
	WEAK symbol<void()> Scr_ClearOutParams{0x0, 0x0};
	WEAK symbol<scr_entref_t(unsigned int entId)> Scr_GetEntityIdRef{0x0, 0x0};
	WEAK symbol<unsigned int(int classnum, unsigned int entnum)> Scr_GetEntityId{0x0, 0x0};
	WEAK symbol<int(unsigned int classnum, int entnum, int offset)> Scr_SetObjectField{0x0, 0x0};

	WEAK symbol<ScreenPlacement* ()> ScrPlace_GetViewPlacement{0x0, 0x0};

	WEAK symbol<void(XAssetType type, void(__cdecl* func)(XAssetHeader, void*), const void* inData, bool includeOverride)>
	DB_EnumXAssets_Internal{0x0, 0x0};
	WEAK symbol<const char*(const XAsset* asset)> DB_GetXAssetName{0x0, 0x0};
	WEAK symbol<int(XAssetType type)> DB_GetXAssetTypeSize{0x0, 0x0};
	WEAK symbol<XAssetHeader(XAssetType type, const char* name, 
		int createDefault)> DB_FindXAssetHeader{0x0, 0x0};

	WEAK symbol<void(int clientNum, const char* menu, 
		int a3, int a4, unsigned int a5)> LUI_OpenMenu{0x0, 0x0};
	WEAK symbol<bool(int clientNum, const char* name, hks::lua_State* s)> LUI_BeginEvent{0x0, 0x0};
	WEAK symbol<void(hks::lua_State* s)> LUI_EndEvent{0x0, 0x0};
	WEAK symbol<void()> LUI_EnterCriticalSection{0x0, 0x0};
	WEAK symbol<void()> LUI_LeaveCriticalSection{0x0, 0x0};

	WEAK symbol<bool(int clientNum, const char* menu)> Menu_IsMenuOpenAndVisible{0x0, 0x0};

	WEAK symbol<scr_string_t(const char* str)> SL_FindString{0x0, 0x0};
	WEAK symbol<scr_string_t(const char* str, unsigned int user)> SL_GetString{0x0, 0x0};
	WEAK symbol<const char*(scr_string_t stringValue)> SL_ConvertToString{0x0, 0x0};
	WEAK symbol<unsigned int(const char* str)> SL_GetCanonicalString{0x0, 0x0};

	WEAK symbol<void(netadr_s* from)> SV_DirectConnect{0x0, 0x0};
	WEAK symbol<void(int arg, char* buffer, int bufferLength)> SV_Cmd_ArgvBuffer{0x0, 0x0};
	WEAK symbol<void(const char* text_in)> SV_Cmd_TokenizeString{0x0, 0x0};
	WEAK symbol<void()> SV_Cmd_EndTokenizedString{0x0, 0x0};

	WEAK symbol<mp::gentity_s*(const char* name)> SV_AddBot{0x0, 0x0};
	WEAK symbol<bool(int clientNum)> SV_BotIsBot{0x0, 0x0};
	WEAK symbol<const char* ()> SV_BotGetRandomName{0x0, 0x0};
	WEAK symbol<int(mp::gentity_s* ent)> SV_SpawnTestClient{0x0, 0x0};

	WEAK symbol<const char* (int clientNum)> SV_GetGuid{0x0, 0x0};
	WEAK symbol<int(int clientNum)> SV_GetClientPing{0x0, 0x0};
	WEAK symbol<playerState_s* (int num)> SV_GetPlayerstateForClientNum{0x0, 0x0};
	WEAK symbol<void(int index, const char* string)> SV_SetConfigstring{0x0, 0x0};
	WEAK symbol<bool()> SV_Loaded{0x0, 0x0};
	WEAK symbol<void(int clientNum, const char* reason)> SV_KickClientNum{0x0, 0x0};
	WEAK symbol<bool(const char* map)> SV_MapExists{0x0, 0x0};
	WEAK symbol<void(mp::client_t*, const char*, int)> SV_ExecuteClientCommand{0x0, 0x0};
	WEAK symbol<void(int localClientNum)> SV_FastRestart{0x0, 0x0};
	WEAK symbol<void(void* cl, int type, const char* fmt, ...)> SV_SendServerCommand{0x0, 0x1CC040};

	WEAK symbol<void()> Sys_ShowConsole{0x0, 0x0};
	WEAK symbol<void(const char* error, ...)> Sys_Error{0x0, 0x0};
	WEAK symbol<void(char* path, int pathSize, Sys_Folder folder, const char* filename, const char* ext)>
		Sys_BuildAbsPath{0x0, 0x0};
	WEAK symbol<int()> Sys_Milliseconds{0x0, 0x0};
	WEAK symbol<bool()> Sys_IsDatabaseReady2{0x0, 0x0};
	WEAK symbol<bool(int, void const*, const netadr_s*)> Sys_SendPacket{0x0, 0x0};
	WEAK symbol<bool(const char* path)> Sys_FileExists{0x0, 0x0};

	WEAK symbol<const char* (const char*)> UI_GetMapDisplayName{0x0, 0x0};
	WEAK symbol<const char* (const char*)> UI_GetGameTypeDisplayName{0x0, 0x0};
	WEAK symbol<void(unsigned int localClientNum, const char** args)> UI_RunMenuScript{0x0, 0x0};
	WEAK symbol<int(const char* text, int maxChars, Font_s* font, float scale)> UI_TextWidth{0x0, 0x0};

	WEAK symbol<const char*(const char* string)> UI_SafeTranslateString{0x0, 0x0};

	WEAK symbol<void* (jmp_buf* Buf, int Value)> longjmp{0x0, 0x0};
	WEAK symbol<int(jmp_buf* Buf)> _setjmp{0x0, 0x0};

	/***************************************************************
	 * Variables
	 **************************************************************/

	WEAK symbol<CmdArgs> sv_cmd_args{0x0, 0x0};

	WEAK symbol<int> g_script_error_level{0x0, 0x0};
	WEAK symbol<jmp_buf> g_script_error{0x0, 0x0};
	
	WEAK symbol<unsigned int> levelEntityId{0x0, 0x0};
	WEAK symbol<unsigned int> gameEntityId{0x0, 0x0};

	WEAK symbol<const char*> command_whitelist{0x0, 0x0};
	WEAK symbol<cmd_function_s*> cmd_functions{0x0, 0x344DF18};
	WEAK symbol<CmdArgs> cmd_args{0x0, 0x2ED1E00};

	WEAK symbol<int> g_poolSize{0x0, 0x0};
	WEAK symbol<scr_classStruct_t> g_classMap{0x0, 0x0};

	WEAK symbol<scrVarGlob_t> scr_VarGlob{0x0, 0x0};
	WEAK symbol<scrVmPub_t> scr_VmPub{0x0, 0x0};
	WEAK symbol<function_stack_t> scr_function_stack{0x0, 0x0};

	WEAK symbol<GfxDrawMethod_s> gfxDrawMethod{0x0, 0x0};

	WEAK symbol<int> dvarCount{0x0, 0x0};
	WEAK symbol<dvar_t> dvarPool{0x0, 0x0};

	WEAK symbol<void*> DB_XAssetPool{0x0, 0x0};
	WEAK symbol<const char*> g_assetNames{0x0, 0x0};

	WEAK symbol<int> keyCatchers{0x0, 0x2EC82C4};
	WEAK symbol<PlayerKeyState> playerKeys{0x0, 0x0};

	WEAK symbol<SOCKET> query_socket{0x0, 0x0};

	WEAK symbol<DWORD> threadIds{0x0, 0x0};

	namespace mp
	{
		WEAK symbol<gentity_s> g_entities{0x0, 0x71F19E0};
		WEAK symbol<client_t> svs_clients{0x0, 0x2DC3390};
		WEAK symbol<int> svs_numclients{0x0, 0x2DC338C};
		WEAK symbol<int> gameTime{0x0, 0x0};

		WEAK symbol<int> ping{0x0, 0x0};

		WEAK symbol<int> sv_serverId_value{0x0, 0x0};

		WEAK symbol<bool> virtualLobby_loaded{0x0, 0x2E6EC9D};
	}

	namespace sp
	{
		WEAK symbol<gentity_s> g_entities{0x0, 0x0};
	}

	namespace hks
	{
		WEAK symbol<lua_State*> lua_state{0x0, 0x0};
		WEAK symbol<void(lua_State* s, const char* str, unsigned int l)> hksi_lua_pushlstring{0x0, 0x0};
		WEAK symbol<HksObject*(HksObject* result, lua_State* s, const HksObject* table, const HksObject* key)> hks_obj_getfield{0x0, 0x0};
		WEAK symbol<void(lua_State* s, const HksObject* tbl, const HksObject* key, const HksObject* val)> hks_obj_settable{0x0, 0x0};
		WEAK symbol<HksObject* (HksObject* result, lua_State* s, const HksObject* table, const HksObject* key)> hks_obj_gettable{0x0, 0x0};
		WEAK symbol<void(lua_State* s, int nargs, int nresults, const unsigned int* pc)> vm_call_internal{0x0, 0x0};
		WEAK symbol<HashTable*(lua_State* s, unsigned int arraySize, unsigned int hashSize)> Hashtable_Create{0x0, 0x0};
		WEAK symbol<cclosure*(lua_State* s, lua_function function, int num_upvalues, 
			int internal_, int profilerTreatClosureAsFunc)> cclosure_Create{0x0, 0x0};
		WEAK symbol<int(lua_State* s, int t)> hksi_luaL_ref{0x0, 0x0};
		WEAK symbol<void(lua_State* s, int t, int ref)> hksi_luaL_unref{0x0, 0x0};
		WEAK symbol<void(lua_State* s, HksObject* lfp)> closePendingUpvalues{0x0, 0x0};
	}
}
