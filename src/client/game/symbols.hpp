#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	/***************************************************************
	 * Functions
	 **************************************************************/

	WEAK symbol<void(int type, VariableUnion u)> AddRefToValue{0x14036E600, 0x14043C580};
	WEAK symbol<void(int type, VariableUnion u)> RemoveRefToValue{0x1403700F0, 0x14043E090};
	WEAK symbol<void(unsigned int id)> AddRefToObject{0x14036E5F0, 0x14043C570};
	WEAK symbol<void(unsigned int id)> RemoveRefToObject{0x14036FFE0, 0x14043DF80};
	WEAK symbol<unsigned int(unsigned int id)> AllocThread{0x14036E960, 0x14043C8E0};

	WEAK symbol<void(int localClientNum, const char* text)> Cbuf_AddText{0x140342EB0, 0x1404033B0};
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* buffer,
		void (int, int, const char*))> Cbuf_ExecuteBufferInternal{0x140342FC0, 0x1404034C0};
	WEAK symbol<void(const char* message)> Conbuf_AppendText{0x1403E3300, 0x140513FF0};
	WEAK symbol<char*(int start)> ConcatArgs{0x1402697F0, 0x140335D70};
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* text)> Cmd_ExecuteSingleCommand{0x140343980, 0x140403F60};
	WEAK symbol<void(const char* cmdName, void(), cmd_function_s* allocedCmd)> Cmd_AddCommandInternal{0x1403433E0, 0x140403950};
	WEAK symbol<void(const char*)> Cmd_RemoveCommand{0x140343FF0, 0x1404045D0};
	WEAK symbol<void(const char* text_in)> Cmd_TokenizeString{0x140344110, 0x1404046F0};
	WEAK symbol<void()> Cmd_EndTokenizeString{0x140343630, 0x140403C20};

	WEAK symbol<void(void*, void*)> AimAssist_AddToTargetList{0, 0x14009D0F0};

	WEAK symbol<void(unsigned int weapon, bool isAlternate, 
		char* output, unsigned int maxStringLen)> BG_GetWeaponNameComplete{0x140430550, 0x1401F9670};

	WEAK symbol<void()> Com_Frame_Try_Block_Function{0x1401CE8D0, 0x1400D8310};
	WEAK symbol<CodPlayMode()> Com_GetCurrentCoDPlayMode{0, 0x1405039A0};
	WEAK symbol<bool()> Com_InFrontEnd{0x1400E4B30, 0x140176A30};
	WEAK symbol<void(float, float, int)> Com_SetSlowMotion{0, 0x1400DB790};
	WEAK symbol<void(errorParm code, const char* message, ...)> Com_Error{0x1403509C0, 0x1400D78A0};
	WEAK symbol<void()> Com_Quit_f{0x140352BE0, 0x1400DA830};
	WEAK symbol<void()> Quit{0x140352D90, 0x1400DA830};

	WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessage{0x1401389A0, 0x140220CC0};
	WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessageBold{0x140138750, 0x140220620};

	WEAK symbol<bool()> CL_IsCgameInitialized{0x14017EE30, 0x140245650};

	WEAK symbol<void(int hash, const char* name, const char* buffer)> Dvar_SetCommand{0x1403C72B0, 0x1404FD0A0};
	WEAK symbol<dvar_t*(const char* name)> Dvar_FindVar{0x1403C5D50, 0x1404FBB00};
	WEAK symbol<void(char* buffer, int index)> Dvar_GetCombinedString{0x140354DF0, 0x14041D830};
	WEAK symbol<const char*(dvar_t* dvar, dvar_value value)> Dvar_ValueToString{0x1403C8560, 0x1404FE660};
	WEAK symbol<dvar_t*(int hash, const char* name, bool value, 
		unsigned int flags)> Dvar_RegisterBool{0x1403C47E0, 0x1404FA540};
	WEAK symbol<dvar_t*(int hash, const char* name, int value, int min, int max, 
		unsigned int flags)> Dvar_RegisterInt{0x1403C4CC0, 0x1404FAA20};
	WEAK symbol<dvar_t*(int hash, const char* dvarName, float value, float min, 
		float max, unsigned int flags)> Dvar_RegisterFloat{0x1403C4BB0, 0x1404FA910};
	WEAK symbol<dvar_t*(int hash, const char* dvarName, const char* value, 
		unsigned int flags)> Dvar_RegisterString{0x1403C4DA0, 0x1404FAB00};
	WEAK symbol<dvar_t*(int dvarName, const char* a2, float x, float y, float z, 
		float w, float min, float max, unsigned int flags)> Dvar_RegisterVec4{0x1403C5220, 0x1404FAF40};

	WEAK symbol<long long(const char* qpath, char** buffer)> FS_ReadFile{0x1403B9020, 0x1404EE720};
	WEAK symbol<void(void* buffer)> FS_FreeFile{0x1403B9010, 0x1404EE710};
	WEAK symbol<void(const char* gameName)> FS_Startup{0x1403B85D0, 0x1404EDD30};
	WEAK symbol<void(const char* path, const char* dir)> FS_AddLocalizedGameDirectory{0x1403B6030, 0x1404EBE20};

	WEAK symbol<unsigned int(unsigned int, unsigned int)> GetVariable{0x14036FDD0, 0x1403F3730};

	WEAK symbol<void()> GScr_LoadConsts{0x1402D13E0, 0x140393810};
	WEAK symbol<unsigned int(unsigned int parentId, unsigned int name)> FindVariable{0x14036F4B0, 0x14043D430};
	WEAK symbol<unsigned int(int entnum, unsigned int classnum)> FindEntityId{0x14036F3B0, 0x14043D330};
	WEAK symbol<void(unsigned int parentId, unsigned int index)> RemoveVariableValue{0x140370190, 0x14043E130};
	WEAK symbol<void(VariableValue* result, unsigned int classnum, 
		int entnum, int offset)> GetEntityFieldValue{0x140373780, 0x140441780};

	WEAK symbol<int(const char* fname)> generateHashValue{0x1400FE8A0, 0x1401B1010};

	WEAK symbol<void()> G_Glass_Update{0x14026C570, 0x14033A640};
	WEAK symbol<int(int clientNum)> G_GetClientScore{0, 0x140342F90};

	WEAK symbol<char*(char* string)> I_CleanStr{0x1403CD230, 0x140503D00};

	WEAK symbol<const char* (int, int, int)> Key_KeynumToString{0x140187CC0, 0x14024FE10};

	WEAK symbol<unsigned int(int)> Live_SyncOnlineDataFlags{0, 0x14059A700};

	WEAK symbol<Material* (const char* material)> Material_RegisterHandle{0x1404E48B0, 0x1405EAB30};

	WEAK symbol<void(netadr_s*, sockaddr*)> NetadrToSockadr{0x1403C11C0, 0x1404F62F0};
	WEAK symbol<void(netsrc_t, netadr_s*, const char*)> NET_OutOfBandPrint{0x140357560, 0x1404255D0};
	WEAK symbol<void(netsrc_t sock, int length, const void* data, const netadr_s* to)> NET_SendLoopPacket{0, 0x140425790};
	WEAK symbol<bool(const char* s, netadr_s* a)> NET_StringToAdr{0, 0x140425870};

	WEAK symbol<void(float x, float y, float width, float height, float s0, float t0, float s1, float t1,
		float* color, Material* material)> R_AddCmdDrawStretchPic{0x14017E5C0, 0x1402443A0};
	WEAK symbol<Font_s* (const char* font, int size)> R_RegisterFont{0x1404D4100, 0x1405D91E0};
	WEAK symbol<int(const char* text, int maxChars, Font_s* font)> R_TextWidth{0x1404D43B0, 0x1405D94A0};
	WEAK symbol<int(void* font)> R_GetFontHeight{0x1405EA360, 0x1405D92C0};
	WEAK symbol<void* (int a1)> R_DrawSomething{0x1404D37B0, 0x1405D8890};
	WEAK symbol<void()> R_SyncRenderThread{0x1404F8240, 0x1405FF3A0};
	WEAK symbol<void* (const char* text, int maxChars, void* font, int fontHeight, float x, 
		float y, float xScale, float yScale, float rotation, float* color, 
		int style, int cursor_pos, char cursor_char, 
		void* style_unk)> H1_AddBaseDrawTextCmd{0x1404F3DC0, 0x1405FB1F0};

#define R_AddCmdDrawText(TXT, MC, F, X, Y, XS, YS, R, C, S) \
	H1_AddBaseDrawTextCmd(TXT, MC, F, game::R_GetFontHeight(F), X, Y, XS, YS, R, C, S,-1, 0, game::R_DrawSomething(S))
#define R_AddCmdDrawTextWithCursor(TXT, MC, F, UNK, X, Y, XS, YS, R, C, S, CP, CC) \
	H1_AddBaseDrawTextCmd(TXT, MC, F, game::R_GetFontHeight(F), X, Y, XS, YS, R, C, S, CP, CC, game::R_DrawSomething(S))

	WEAK symbol<unsigned int(unsigned int localId, const char* pos, 
		unsigned int paramcount)> VM_Execute{0x140376360, 0x140444350};

	WEAK symbol<void(unsigned int id, scr_string_t stringValue, 
		unsigned int paramcount)> Scr_NotifyId{0x140375800, 0x1404437E0};
	WEAK symbol<const float*(const float* v)> Scr_AllocVector{0x140370930, 0x14043E7D0};
	WEAK symbol<float(int index)> Scr_GetFloat{0x140374D20, 0x140442D10};
	WEAK symbol<const char*(int index)> Scr_GetString{0x140375210, 0x140443150};
	WEAK symbol<int()> Scr_GetNumParam{0x140374F30, 0x140442E70};
	WEAK symbol<void()> Scr_ClearOutParams{0x140374460, 0x140442510};
	WEAK symbol<scr_entref_t(unsigned int entId)> Scr_GetEntityIdRef{0x140372D50, 0x140440D80};
	WEAK symbol<unsigned int(int classnum, unsigned int entnum)> Scr_GetEntityId{0x140372CA0, 0x140440CD0};

	WEAK symbol<ScreenPlacement* ()> ScrPlace_GetViewPlacement{0x1401981F0, 0x140288550};

	WEAK symbol<int(XAssetType type)> DB_GetXAssetTypeSize{0x14019A3B0, 0x14028BE70};

	WEAK symbol<void(int clientNum, const char* menu, 
		int a3, int a4, unsigned int a5)> LUI_OpenMenu{0x14039D5F0, 0x1404CD210};

	WEAK symbol<bool(int clientNum, const char* menu)> Menu_IsMenuOpenAndVisible{0x1404709C0, 0x1404C7320};

	WEAK symbol<scr_string_t(const char* str)> SL_FindString{0x14036D700, 0x14043B470};
	WEAK symbol<scr_string_t(const char* str, unsigned int user)> SL_GetString{0x14036D9A0, 0x14043B840};
	WEAK symbol<const char* (scr_string_t stringValue)> SL_ConvertToString{0x14036D420, 0x14043B170};
	WEAK symbol<int(unsigned int classnum, int entnum, int offset)> Scr_SetObjectField{0x1402B9F60, 0x140385330};

	WEAK symbol<void(netadr_s* from)> SV_DirectConnect{0, 0x140480860};
	WEAK symbol<void(int arg, char* buffer, int bufferLength)> SV_Cmd_ArgvBuffer{0x1403446C0, 0x140404CA0};
	WEAK symbol<void(const char* text_in)> SV_Cmd_TokenizeString{0x140344740, 0x140404D20};
	WEAK symbol<void()> SV_Cmd_EndTokenizedString{0x140344700, 0x140404CE0};

	WEAK symbol<mp::gentity_s*(const char* name)> SV_AddBot{0, 0x140480190};
	WEAK symbol<bool(int clientNum)> SV_BotIsBot{0, 0x14046E6C0};
	WEAK symbol<const char* ()> SV_BotGetRandomName{0, 0x14046DBA0};
	WEAK symbol<int(mp::gentity_s* ent)> SV_SpawnTestClient{0, 0x1404832A0};

	WEAK symbol<const char* (int clientNum)> SV_GetGuid{0, 0x140484B90};
	WEAK symbol<int(int clientNum)> SV_GetClientPing{0, 0x140484B70};
	WEAK symbol<playerState_s* (int num)> SV_GetPlayerstateForClientNum{0x1404426D0, 0x140484C10};
	WEAK symbol<void(int index, const char* string)> SV_SetConfigstring{0, 0x140486720};
	WEAK symbol<bool()> SV_Loaded{0x140442F60, 0x1404864A0};
	WEAK symbol<void(int clientNum, const char* reason)> SV_KickClientNum{0, 0x14047ED00};
	WEAK symbol<bool(const char* map)> SV_MapExists{0, 0x14047ED60};
	WEAK symbol<void(mp::client_t*, const char*, int)> SV_ExecuteClientCommand{0, 0x140481870};
	WEAK symbol<void(int localClientNum)> SV_FastRestart{0, 0x14047E990};
	WEAK symbol<void(int clientNum, svscmd_type type, const char* text)> SV_GameSendServerCommand{
		0x1403F3A70, 0x140484AD0
	};

	WEAK symbol<void()> Sys_ShowConsole{0x1403E3B90, 0x140514910};
	WEAK symbol<void(const char* error, ...)> Sys_Error{0x1403E0C40, 0x140511520};
	WEAK symbol<void(char* path, int pathSize, Sys_Folder folder, const char* filename, const char* ext)>
		Sys_BuildAbsPath{0x1403CFF90, 0x140507010};
	WEAK symbol<int()> Sys_Milliseconds{0x1403E2B10, 0x140513710};
	WEAK symbol<bool()> Sys_IsDatabaseReady2{0x1403580B0, 0x14042B090};
	WEAK symbol<bool(int, void const*, const netadr_s*)> Sys_SendPacket{0x1403E2820, 0x1405133B0};
	WEAK symbol<bool(const char* path)> Sys_FileExists{0x1403E0CE0, 0x1405115E0};

	WEAK symbol<const char* (const char*)> UI_GetMapDisplayName{0, 0x140408CC0};
	WEAK symbol<const char* (const char*)> UI_GetGameTypeDisplayName{0, 0x1404086A0};
	WEAK symbol<void(unsigned int localClientNum, const char** args)> UI_RunMenuScript{0x14039EFF0, 0x1404CFE60};
	WEAK symbol<int(const char* text, int maxChars, Font_s* font, float scale)> UI_TextWidth{0x1403A0F20, 0x1404D21A0};

	WEAK symbol<const char*(const char* string)> UI_SafeTranslateString{0x140350430, 0x14041C580};

	WEAK symbol<void* (jmp_buf* Buf, int Value)> longjmp{0x140648FD4, 0x140779F64};
	WEAK symbol<int(jmp_buf* Buf)> _setjmp{0x1406BFD30, 0x1407F5F90};

	/***************************************************************
	 * Variables
	 **************************************************************/

	WEAK symbol<CmdArgs> sv_cmd_args{0, 0x14946BA20};

	WEAK symbol<int> g_script_error_level{0x14A1917A8, 0x14A33C824};
	WEAK symbol<jmp_buf> g_script_error{0x14A1917B0, 0x14A33C940};
	
	WEAK symbol<unsigned int> levelEntityId{0x149AF55B0, 0x149CA0730};
	WEAK symbol<unsigned int> gameEntityId{0x149CA0734, 0x14B65E3B4};

	WEAK symbol<const char*> command_whitelist{0x141079A60, 0x14120C360};
	WEAK symbol<cmd_function_s*> cmd_functions{0x14AD99AB8, 0x14946BAC8};
	WEAK symbol<CmdArgs> cmd_args{0x14AD99960, 0x14946B970};

	WEAK symbol<int> g_poolSize{0, 0x140FEADF0};
	WEAK symbol<scr_classStruct_t> g_classMap{0x14080A840, 0x1412106B0};

	WEAK symbol<scrVarGlob_t> scr_VarGlob{0x14B686480, 0x149CC8800};
	WEAK symbol<scrVmPub_t> scr_VmPub{0x14A1938C0, 0x14A33EA40};
	WEAK symbol<function_stack_t> scr_function_stack{0x14BD06C40, 0x14A348FC0};

	WEAK symbol<GfxDrawMethod_s> gfxDrawMethod{0x14F05CE50, 0x14FD21180};

	WEAK symbol<int> dvarCount{0x14C217D10, 0x14D064CF4};
	WEAK symbol<dvar_t*> dvarPool{0x14C217D20, 0x14D064D00};

	WEAK symbol<void*> DB_XAssetPool{0x140DE8C80, 0x140FEB5D0};

	WEAK symbol<int> keyCatchers{0x14243DAF0, 0x142D0BA9C};
	WEAK symbol<PlayerKeyState> playerKeys{0x1422A873C, 0x142C19AFC};

	WEAK symbol<SOCKET> query_socket{0, 0x14DDFBF98};

	WEAK symbol<DWORD> threadIds{0x14B19B880, 0x149810E00};

	namespace mp
	{
		WEAK symbol<gentity_s> g_entities{0, 0x14621E530};
		WEAK symbol<client_t> svs_clients{0, 0x14B204A10};
		WEAK symbol<int> svs_numclients{0, 0x14B204A0C};
		WEAK symbol<int> gameTime{0, 0x14621BDBC};

		WEAK symbol<int> sv_serverId_value{0, 0x14A3E99B8};

		WEAK symbol<bool> virtualLobby_loaded{0, 0x142D077FD};
	}

	namespace sp
	{
		WEAK symbol<gentity_s> g_entities{0x14550DD90, 0};
	}

	namespace hks
	{
		WEAK symbol<lua_State*> lua_state{0x141E2C2F8, 0x1426D3D08};
		WEAK symbol<void(lua_State* s, const char* str, unsigned int l)> hksi_lua_pushlstring{0x14004DA90, 0x1400624F0};
		WEAK symbol<HksObject*(HksObject* result, lua_State* s, const HksObject* table, const HksObject* key)> hks_obj_getfield{0x14009C0A0, 0x14012C600};
		WEAK symbol<void(lua_State* s, const HksObject* tbl, const HksObject* key, const HksObject* val)> hks_obj_settable{0x14009D240, 0x14012D820};
		WEAK symbol<HksObject* (HksObject* result, lua_State* s, const HksObject* table, const HksObject* key)> hks_obj_gettable{0x14009C580, 0x14012CAE0};
		WEAK symbol<void(lua_State* s, int nargs, int nresults, const unsigned int* pc)> vm_call_internal{0x1400C87A0, 0x140159EB0};
		WEAK symbol<HashTable*(lua_State* s, unsigned int arraySize, unsigned int hashSize)> Hashtable_Create{0x14008B3B0, 0x14011B320};
		WEAK symbol<cclosure*(lua_State* s, lua_function function, int num_upvalues, 
			int internal_, int profilerTreatClosureAsFunc)> cclosure_Create{0x14008B5D0, 0x14011B540};
		WEAK symbol<int(lua_State* s, int t)> hksi_luaL_ref{0x1400A64D0, 0x140136D30};
		WEAK symbol<void(lua_State* s, int t, int ref)> hksi_luaL_unref{0x14009EF10, 0x14012F610};
	}
}
