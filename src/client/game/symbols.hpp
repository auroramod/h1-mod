#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	/***************************************************************
	 * Functions
	 **************************************************************/
	
	namespace mp
	{
		WEAK symbol<void(int localClientNum, const char* text)> Cbuf_AddText{0x0, 0x1404033B0};
		WEAK symbol<void(const char* text_in, int limit)> Cmd_TokenizeStringWithLimit{0x0, 0x140404990};
		WEAK symbol<void(const char* text_in)> Cmd_TokenizeString{0x0, 0x1404046F0};
		WEAK symbol<void()> Cmd_EndTokenizeString{0x0, 0x140403C20};
		
		WEAK symbol<const char*(dvar_t* dvar, dvar_value* value)> Dvar_ValueToString{0x0, 0x1404FE660};
		WEAK symbol<char*(size_t size, size_t alignment)> Hunk_AllocAlignInternal{0x0, 0x1404F5220};

		WEAK symbol<void(const char* name, void* zone, void* memory, bool flag)> DB_LoadXFile{0x0, 0x14028D740};
		WEAK symbol<unsigned int(unsigned char* pos, std::uint64_t size, int imageCopy)> DB_ReadXFile{0x0, 0x14028E050};
		WEAK symbol<void()> DB_WaitXFileStage{0x0, 0x14028E4B0};
		WEAK symbol<void()> DB_ParseImageFileHeaders{0x0, 0x1402C6060};
		WEAK symbol<int(const void* src, void* dst, int srcLen, int dstLen)> DB_Block_DecompressZlib{0x0, 0x14028D2F0};
		WEAK symbol<int(const void* src, void* dst, int srcLen, int dstLen)> DB_Block_DecompressLZ4{0x0, 0x14028D2D0};
		WEAK symbol<void()> DB_LoadPackedLoadedSounds{0x0, 0x1402C6000};
		WEAK symbol<void(int unused)> DB_ReadPackedLoadedSounds{0x0, 0x1402C6590};

		WEAK symbol<void(StreamFile* file, std::int16_t* name)> StreamFileOpen{0x0, 0x140506E70};
		WEAK symbol<void(StreamFile* file)> StreamFileClose{0x0, 0x140506C40};
		WEAK symbol<DB_IFileSysFile*(DB_IFileSysFile** cache, int count, int isImageFile, int index,
			std::uint64_t isLocalized)> StreamFile_OpenPak{0x0, 0x140506A00};

		WEAK symbol<std::uint64_t(void* channel, int index, unsigned int startMsec)> SD_StartAlias{0x0, 0x1404A8BD0};

		WEAK symbol<std::uint64_t(PMemRange* range)> PMem_CommitMemory{0x0, 0x140501BF0};
		WEAK symbol<std::uint64_t(std::uint64_t a1, std::uint64_t a2, std::uint64_t a3, std::uint64_t a4)> PMem_DecommitMemory{0x0, 0x140501E00};

		WEAK symbol<void()> SV_ClearServer{0x0, 0x140485840};

		WEAK symbol<std::uint32_t(void* model)> DB_GetXModelIndex{0x0, 0x1402BBDB0};
		WEAK symbol<void*(std::uint32_t index)> DB_GetMaterialAtIndex{0x0, 0x1402BBB00};
		WEAK symbol<char(const char* zone, int flags)> DB_IsFileLoaded{0x0, 0x1402BC110};

		WEAK symbol<void(void* state)> sha256_init{0x0, 0x1406ECF00};
		WEAK symbol<int(void* state, const std::uint8_t* data, unsigned long size)> sha256_process{0x0, 0x1406ECF40};
		WEAK symbol<int(void* state, std::uint8_t* digest)> sha256_done{0x0, 0x1406ED020};
	}

	namespace sp
	{
		WEAK symbol<void(int localClientNum, const char* text)> Cbuf_AddText{0x1403764A0, 0x0};
		WEAK symbol<const char*(dvar_t* dvar, bool is_hashed, dvar_value value)> Dvar_ValueToString{0x14041CE00, 0x0};
		WEAK symbol<void(const char* text_in)> Cmd_TokenizeString{0x140377790, 0x0};
		WEAK symbol<void()> Cmd_EndTokenizeString{0x140376C90, 0x0};
	}

	WEAK symbol<void(int type, VariableUnion u)> AddRefToValue{0x1403C1F50, 0x14043C580};
	WEAK symbol<void(int type, VariableUnion u)> RemoveRefToValue{0x1403C3A60, 0x14043E090};
	WEAK symbol<void(unsigned int id)> AddRefToObject{0x1403C1F40, 0x14043C570};
	WEAK symbol<void(unsigned int id)> RemoveRefToObject{0x1403C3950, 0x14043DF80};
	WEAK symbol<unsigned int(unsigned int id)> AllocThread{0x1403C22B0, 0x14043C8E0};
	WEAK symbol<ObjectVariableValue*(unsigned int* id)> AllocVariable{0x1403C2310, 0x14043C940};

	WEAK symbol<int(int entnum)> Agent_IsScripted{0x0, 0x14044FBA0};

	WEAK symbol<void(float (*axis)[3], float* angles)> AxisToAngles{0x0, 0x1404EF210};
	WEAK symbol<void(float* angles, float* forward, float* right, float* up)> AngleVectors{0x0, 0x1404F4390};

	WEAK symbol<void(int localClientNum, int controllerIndex, const char* buffer,
		void (int, int, const char*))> Cbuf_ExecuteBufferInternal{0x1403765B0, 0x1404034C0};
	WEAK symbol<void(const char* message)> Conbuf_AppendText{0x0, 0x0};
	WEAK symbol<char*(int start)> ConcatArgs{0x140296420, 0x140335D70};
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* text)> Cmd_ExecuteSingleCommand{0x140376FF0, 0x140403F60};
	WEAK symbol<void(const char* cmdName, void(), cmd_function_s* allocedCmd)> Cmd_AddCommandInternal{0x140376A40, 0x140403950};
	WEAK symbol<void(const char*)> Cmd_RemoveCommand{0x140377670, 0x1404045D0};

	WEAK symbol<void(void*, void*)> AimAssist_AddToTargetList{0x0, 0x14009D0F0};

	WEAK symbol<int(game::playerState_s* ps)> BG_GetMaxSprintTime{0x0, 0x1401DB1F0};
	WEAK symbol<int(Weapon weapon, bool isAlternate)> BG_SegmentedReload{0x0, 0x140201490};
	WEAK symbol<bool(Weapon weapon)> BG_HasUnderbarrelAmmo{0x0, 0x1401FEED0};
	WEAK symbol<XModel*(Weapon weapon, bool isAlternate)> BG_GetKnifeModel{0x0, 0x1401FFAE0};
	WEAK symbol<XModel*(Weapon weapon, bool isAlternate, int modelIndex)> BG_GetGunModel{0x0, 0x1401FE7E0};
	WEAK symbol<bool(Weapon weapon)> BG_HasAttachmentCombo{0x0, 0x1401FA510};
	WEAK symbol<Weapon(const char* name)> BG_FindWeaponForName{0x0, 0x1401F7C50};
	WEAK symbol<int(playerState_s* ps, int hand)> PM_Weapon_AllowReload{0x0, 0x1401EFB30};

	WEAK symbol<void(unsigned int weapon, bool isAlternate, 
		char* output, unsigned int maxStringLen)> BG_GetWeaponNameComplete{0x1404B19C0, 0x1401F9670};
	WEAK symbol<int(playerState_s* ps)> BG_PlayerLastWeaponHand{0x0, 0x140200190};
	WEAK symbol<int(Weapon weapIdx, bool isAlternate, bool isDualWielding)> BG_SprintInTime{0x0, 0x1402021A0};
	WEAK symbol<int(Weapon weapIdx, bool isAlternate, bool isDualWielding)> BG_SprintOutTime{0x0, 0x140202220};
	WEAK symbol<void(playerState_s* ps, PlayerHandIndex hand)> PM_SetReloadingState{0x0, 0x1401EDC00};

	WEAK symbol<void()> Com_Frame_Try_Block_Function{0x140385280, 0x0};
	WEAK symbol<CodPlayMode()> Com_GetCurrentCoDPlayMode{0x0, 0x1405039A0};
	WEAK symbol<bool()> Com_InFrontend{0x1400F6430, 0x14005F870};
	WEAK symbol<void(float, float, int)> Com_SetSlowMotion{0x0, 0x1400DB790};
	WEAK symbol<void(errorParm code, const char* message, ...)> Com_Error{0x140384820, 0x1400D78A0};
	WEAK symbol<void(char const* finalMessage)> Com_Shutdown{0x1403A6A50, 0x140486C40};
	WEAK symbol<bool(const char* mapname, const char** base_mapname)> Com_IsAddonMap{0x14040AED0, 0x1404EBB10};
	WEAK symbol<int(char* dest, int size, const char* fmt, ...)> Com_sprintf{0x140429200, 0x140503B10};

	WEAK symbol<snd_alias_t* (const char* aliasname, int entNum)> Com_PickSoundAlias{0x0, 0x1404F7BE0};

	WEAK symbol<void()> Quit{0x1403A5A20, 0x1400DA830};

	WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessage{0x14015B3B0, 0x140220CC0};
	WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessageBold{0x14015B110, 0x140220620};
	WEAK symbol<void(int localClientNum, /*cg_s**/void* cg, 
		const char* dvar, const char* value)> CG_SetClientDvarFromServer{0x0, 0x0};
	WEAK symbol<char*(const unsigned int weapon, 
		bool isAlternate, char* outputBuffer, int bufferLen)> CG_GetWeaponDisplayName{0x140192B80, 0x1400B5840};
	WEAK symbol<cg_s* ()> CG_GetLocalClientGlobals{0x0, 0x140214280};
	WEAK symbol<int(int localClientNum, int serverTime, int demoType, int cubemapShot, int cubemapSize, int renderScreen, 
		unsigned int a7)> CG_DrawActiveFrame{0x0, 0x1400A96D0};

	WEAK symbol<bool()> CL_IsCgameInitialized{0x1401A3210, 0x140245650};
	WEAK symbol<void(int a1)> CL_VirtualLobbyShutdown{0x0, 0x140256D40};
	WEAK symbol<const char* (int configStringIndex)> CL_GetConfigString{0x0, 0x1402448F0};

	WEAK symbol<void(int a1)> CL_ShowSystemCursor{0x0, 0x14050F0B0};
	WEAK symbol<void(tagPOINT* position)> CL_GetCursorPos{0x0, 0x14050EE50};

	WEAK symbol<void(int hash, const char* name, const char* buffer)> Dvar_SetCommand{0x14041BAD0, 0x1404FD0A0};
	WEAK symbol<dvar_t*(const char* name)> Dvar_FindVar{0x14041A600, 0x1404FBB00};
	WEAK symbol<dvar_t*(int hash)> Dvar_FindMalleableVar{0x14041A570, 0x1404FBA70};
	WEAK symbol<void(const dvar_t* dvar)> Dvar_ClearModified{0x14041A4F0, 0x1404FB930};
	WEAK symbol<void(char* buffer, int index)> Dvar_GetCombinedString{0x1403A7D60, 0x14041D830};
	WEAK symbol<void(dvar_t* dvar, DvarSetSource source)> Dvar_Reset{0x14041B5F0, 0x1404FCC40};
	WEAK symbol<void(const char*, const char*, 
		DvarSetSource)> Dvar_SetFromStringByNameFromSource{0x14041BD90, 0x1404FD490};
	WEAK symbol<void(dvar_t* dvar, const char* string, DvarSetSource source)> Dvar_SetFromStringFromSource{0x0, 0x1404FD520};
	WEAK symbol<void(dvar_t* dvar, dvar_value* value, DvarSetSource source)> Dvar_SetVariant{0x14041C190, 0x1404FD970};

	WEAK symbol<dvar_t*(int hash, const char* name, bool value, 
		unsigned int flags)> Dvar_RegisterBool{0x140419220, 0x1404FA540};
	WEAK symbol<dvar_t*(int hash, const char* name, int value, int min, int max, 
		unsigned int flags)> Dvar_RegisterInt{0x140419700, 0x1404FAA20};
	WEAK symbol<dvar_t*(int hash, const char* dvarName, float value, float min, 
		float max, unsigned int flags)> Dvar_RegisterFloat{0x1404195F0, 0x1404FA910};
	WEAK symbol<dvar_t*(int hash, const char* dvarName, float value, float min, 
		float max, unsigned int flags)> Dvar_RegisterFloatHashed{0x0, 0x1404FA910};
	WEAK symbol<dvar_t*(int hash, const char* dvarName, const char* value, 
		unsigned int flags)> Dvar_RegisterString{0x1404197E0, 0x1404FAB00};
	WEAK symbol<dvar_t*(int dvarName, const char* a2, float x, float y, float z, 
		float w, float min, float max, unsigned int flags)> Dvar_RegisterVec4{0x140419C60, 0x1404FAF40};
	WEAK symbol<dvar_t*(int hash, const char* dvarName, const char** valueList, int defaultIndex, unsigned int flags)> Dvar_RegisterEnum{0x140419500, 0x1404FA820};

	WEAK symbol<long long(const char* qpath, char** buffer)> FS_ReadFile{0x14040E280, 0x1404EE720};
	WEAK symbol<void(void* buffer)> FS_FreeFile{0x14040E270, 0x1404F6060};
	WEAK symbol<void(const char* gameName)> FS_Startup{0x14040D890, 0x0};
	WEAK symbol<void(const char* path, const char* dir)> FS_AddLocalizedGameDirectory{0x14040B1E0, 0x1404EBE20};

	WEAK symbol<FxSystem*()> Fx_GetSystem{0x0, 0x1402FF7F0};
	WEAK symbol<void(FxAccessLock* lock)> FX_WaitEnterReadSystemLock{0x0, 0x1402FFF70};
	WEAK symbol<void(FxAccessLock* lock)> FX_ExitReadSystemLock{0x0, 0x1402FFE80};

	WEAK symbol<unsigned int(unsigned int)> GetObjectType{0x1403C3680, 0x14043DCB0};
	WEAK symbol<unsigned int(unsigned int, unsigned int)> GetVariable{0x1403C3740, 0x14043DD70};
	WEAK symbol<unsigned int(unsigned int parentId, unsigned int unsignedValue)> GetNewVariable{0x1403C3360, 0x14043D990};
	WEAK symbol<unsigned int(unsigned int parentId, unsigned int unsignedValue)> GetNewArrayVariable{0x1403C31E0, 0x14043D810};
	WEAK symbol<unsigned int(unsigned int parentId, unsigned int name)> FindVariable{0x1403C2E00, 0x14043D430};
	WEAK symbol<unsigned int(int entnum, unsigned int classnum)> FindEntityId{0x1403C2D00, 0x14043D330};
	WEAK symbol<void(unsigned int parentId, unsigned int index)> RemoveVariableValue{0x1403C3B00, 0x14043E130};
	WEAK symbol<void(VariableValue* result, unsigned int classnum, 
		int entnum, int offset)> GetEntityFieldValue{0x1403C71A0, 0x140441780};

	WEAK symbol<int(const char* fname)> generateHashValue{0x14011FEA0, 0x1401B1010};

	WEAK symbol<void()> G_Glass_Update{0x1402992E0, 0x14033A640};
	WEAK symbol<int(int clientNum)> G_GetClientScore{0x0, 0x140342F90};
	WEAK symbol<unsigned int(const char* name)> G_GetWeaponForName{0x1402F20F0, 0x14038C300};
	WEAK symbol<int(playerState_s* ps, unsigned int weapon, int dualWield, 
		int startInAltMode, int, int, int, char, ...)> G_GivePlayerWeapon{0x1402F24F0, 0x14038C750};
	WEAK symbol<void(playerState_s* ps, unsigned int weapon, int hadWeapon)> G_InitializeAmmo{0x14029D9E0, 0x14033EDD0};
	WEAK symbol<void(const char* fmt, ...)> G_LogPrintf{0x14005FEF0, 0x140344100};
	WEAK symbol<void(int clientNum, unsigned int weapon)> G_SelectWeapon{0x1402F2EA0, 0x14038D1B0};
	WEAK symbol<int(const char* buffer, int entity, int type)> G_SetFog{0x0, 0x140335E80};
	WEAK symbol<int(playerState_s* ps, unsigned int weapon)> G_TakePlayerWeapon{0x1402F3050, 0x14038D370};
	WEAK symbol<void(gentity_s*, float* origin)> G_SetOrigin{0x0, 0x14038A810};

	WEAK symbol<int(const char* buf, int max, char** infos)> GameInfo_ParseArenas{0x0, 0x140408E90};

	WEAK symbol<char*(const size_t size)> Hunk_AllocateTempMemoryHigh{0x140415DB0, 0x1404F5C30};

	WEAK symbol<char*(char* string)> I_CleanStr{0x1404293E0, 0x140503D00};
	WEAK symbol<char*(char* dest, const char* src, int dest_size)> I_strncpyz{0x0, 0x140504270};

	WEAK symbol<const char*(int, int, int)> Key_KeynumToString{0x1401AC410, 0x14024FE10};
	WEAK symbol<int(const char* cmd)> Key_GetBindingForCmd{0x140377280, 0x1404041E0};
	WEAK symbol<void(int local_client_num, int keynum, int binding)> Key_SetBinding{0x1401AC570, 0x14024FF60};

	WEAK symbol<unsigned int(int)> Live_SyncOnlineDataFlags{0x0, 0x14059A700};

	WEAK symbol<Material*(const char* material)> Material_RegisterHandle{0x14056EA20, 0x1405EAB30};

	WEAK symbol<char*(msg_t* msg, char* buffer, unsigned int max_chars)> MSG_ReadStringLine{0x0, 0x14041F100};
	WEAK symbol<int(msg_t* msg)> MSG_ReadBit{0x0, 0x14005CF60};
	WEAK symbol<int(msg_t* msg)> MSG_ReadLong{0x0, 0x14041EF00};
	WEAK symbol<void(msg_t* msg)> MSG_WriteBit0{0x0, 0x14041F3E0};
	WEAK symbol<void(msg_t* msg)> MSG_WriteBit1{0x0, 0x14041F420};
	WEAK symbol<void(msg_t* msg, int c)> MSG_WriteLong{0x0, 0x14041F720};

	WEAK symbol<void(netadr_s*, sockaddr*)> NetadrToSockadr{0x140416580, 0x1404F62F0};
	WEAK symbol<void(netsrc_t, netadr_s*, const char*)> NET_OutOfBandPrint{0x1403AA550, 0x1404255D0};
	WEAK symbol<void(netsrc_t sock, int length, const void* data, const netadr_s* to)> NET_SendLoopPacket{0x0, 0x140425790};
	WEAK symbol<bool(const char* s, netadr_s* a)> NET_StringToAdr{0x0, 0x140425870};

	WEAK symbol<void(float x, float y, float width, float height, float s0, float t0, float s1, float t1,
		float* color, Material* material)> R_AddCmdDrawStretchPic{0x1401A29A0, 0x1402443A0};
	WEAK symbol<Font_s*(const char* font, int size)> R_RegisterFont{0x14055C4E0, 0x1405D91E0};
	WEAK symbol<int(const char* text, int maxChars, Font_s* font)> R_TextWidth{0x14055C7A0, 0x1405D94A0};
	WEAK symbol<int(void* font)> R_GetFontHeight{0x14055C5C0, 0x1405D92C0};
	WEAK symbol<void*(int a1)> R_GetSomething{0x14055BB90, 0x1405D8890};
	WEAK symbol<void()> R_SyncRenderThread{0x140582F30, 0x1405FF3A0};

	WEAK symbol<void(GfxCmdBufContext* context, GfxRenderTargetId newTargetId)> R_SetRenderTarget{0x0, 0x14060AEC0};
	WEAK symbol<void(GfxCmdBufSourceState* source, GfxRenderTargetId newTargetId)> R_SetRenderTargetSize{0x0, 0x14060AFD0};
	WEAK symbol<void(GfxCmdBufSourceState* source, GfxViewport* viewport)> R_SetViewportStruct{0x0, 0x14060B780};
	WEAK symbol<void(GfxCmdBufSourceState* source, GfxCmdBufState* state)> R_InitLocalCmdBufState{0x0, 0x14007A940};
	WEAK symbol<void(GfxCmdBufSourceState* source, GfxCmdBufState* state)> R_ShutdownCmdBufState{0x0, 0x14007AA80};
	WEAK symbol<void(GfxCmdBufState* state, unsigned char whichToClear, float* color, double depth,
		unsigned char stencil, GfxViewport* viewport)> R_ClearScreen{0x0, 0x140608B20};
	WEAK symbol<void(Material* material, GfxViewport* viewport, int isFullScreen)> RB_ViewportFilter{0x0, 0x140622DF0};
	WEAK symbol<bool(const GfxViewInfo* viewInfo)> RB_IsSceneViewportFull{0x0, 0x140620630};
	WEAK symbol<void(float radius, GfxRenderTargetId srcRenderTargetId, GfxRenderTargetId dstRenderTargetId,
		GfxViewInfo* viewInfo, unsigned int filterOptions)> RB_GaussianFilterImageWithOptions{0x0, 0x14062ADC0};
	WEAK symbol<void*(const char* text, int maxChars, void* font, int fontHeight, float x, 
		float y, float xScale, float yScale, float rotation, float* color, 
		int style, int cursor_pos, char cursor_char, 
		void* style_unk)> H1_AddBaseDrawTextCmd{0x14057EA60, 0x1405FB1F0};

#define R_AddCmdDrawText(TXT, MC, F, X, Y, XS, YS, R, C, S) \
	H1_AddBaseDrawTextCmd(TXT, MC, F, game::R_GetFontHeight(F), X, Y, XS, YS, R, C, S, -1, 0, game::R_GetSomething(S))
#define R_AddCmdDrawTextWithCursor(TXT, MC, F, UNK, X, Y, XS, YS, R, C, S, CP, CC) \
	H1_AddBaseDrawTextCmd(TXT, MC, F, game::R_GetFontHeight(F), X, Y, XS, YS, R, C, S, CP, CC, game::R_GetSomething(S))

	WEAK symbol<int()> R_PopRemoteScreenUpdate{0x0, 0x1405FEE90};
	WEAK symbol<void(int)> R_PushRemoteScreenUpdate{0x0, 0x1405FEF90};

	WEAK symbol<void()> R_BeginFrame{0x0, 0x1405FE360};
	WEAK symbol<void()> R_EndFrame{0x0, 0x1405FE470};
	WEAK symbol<void(int)> R_IssueRenderCommands{0x0, 0x1405FEC30};
	WEAK symbol<bool(__int64 a1, int a2, int a3, int a4, int width, int height, int numChannels, void* buffer)> R_TakeScreenshot{0x0, 0x1400937F0};
	WEAK symbol<void(XModel* model, game::GfxScaledPlacement* placement, unsigned int renderFlags, 
		unsigned __int16* cachedLightingHandle, float* colorLit, float* colorUnlit, float* colorEmissive)> R_FilterXModelIntoScene{0x0, 0x1405B99E0};

	WEAK symbol<char* (GfxImage* image, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipCount,
		uint32_t imageFlags, DXGI_FORMAT imageFormat, const char* name, const D3D11_SUBRESOURCE_DATA* initData)> Image_Setup{0x140560740, 0x1405DCF90};

	WEAK symbol<char*(const size_t size, unsigned int alignment, 
		unsigned int type, PMem_Source source)> PMem_AllocFromSource_NoDebug{0x14041FB50, 0x140501920};
	WEAK symbol<void(char* buf, const size_t size, unsigned int alignment, 
		unsigned int type, PMem_Source source)> PMem_PopFromSource_NoDebug{0x1404203D0, 0x140502230};

	WEAK symbol<unsigned int(unsigned int localId, const char* pos, 
		unsigned int paramcount)> VM_Execute{0x1403C9E50, 0x140444350};

	WEAK symbol<void(const char* value)> Scr_AddString{0x1403C7B20, 0x1404420F0};
	WEAK symbol<void(int value)> Scr_AddInt{0x1403C7A40, 0x140441D90};
	WEAK symbol<void(int value)> Scr_AddBool{0x1403C77C0, 0x140442010};

	WEAK symbol<void(unsigned int id, scr_string_t stringValue, 
		unsigned int paramcount)> Scr_NotifyId{0x1403C92E0, 0x1404437E0};
	WEAK symbol<const float*(const float* v)> Scr_AllocVector{0x1403C42D0, 0x14043E7D0};
	WEAK symbol<float(int index)> Scr_GetFloat{0x1403C87D0, 0x140442D10};
	WEAK symbol<const char*(int index)> Scr_GetString{0x1403C8CC0, 0x140443150};
	WEAK symbol<int()> Scr_GetNumParam{0x1403C89E0, 0x140442E70};
	WEAK symbol<bool(VariableValue* value)> Scr_CastString{0x1403C4450, 0x14043E950};
	WEAK symbol<void()> Scr_ClearOutParams{0x1403C7EF0, 0x140442510};
	WEAK symbol<scr_entref_t(unsigned int entId)> Scr_GetEntityIdRef{0x1403C6760, 0x140440D80};
	WEAK symbol<unsigned int(int classnum, unsigned int entnum)> Scr_GetEntityId{0x1403C66B0, 0x140440CD0};
	WEAK symbol<int(unsigned int classnum, int entnum, int offset)> Scr_SetObjectField{0x1402E8FC0, 0x140385330};
	WEAK symbol<void()> Scr_ErrorInternal{0x1403C7F60, 0x140442570};

	WEAK symbol<unsigned int(const char* filename)> Scr_LoadScript{0x1403BDF70, 0x140438440};
	WEAK symbol<unsigned int(const char* filename, unsigned int handle)> Scr_GetFunctionHandle{0x1403BDE00, 0x1404382D0};
	WEAK symbol<unsigned int(int handle, int num_param)> Scr_ExecThread{0x1403C7FE0, 0x1404425F0};
	WEAK symbol<unsigned int(void* func, int type, unsigned int name)> Scr_RegisterFunction{0x1403BD860, 0x140437CE0};

	WEAK symbol<ScreenPlacement* ()> ScrPlace_GetActivePlacement{0x0, 0x140288520};
	WEAK symbol<ScreenPlacement*()> ScrPlace_GetViewPlacement{0x1401BCED0, 0x140288550};
	WEAK symbol<float()> ScrPlace_HiResGetScaleX{0x0, 0x140288620};
	WEAK symbol<float()> ScrPlace_HiResGetScaleY{0x0, 0x140288640};

	WEAK symbol<char*(StringTable*, int, int)> StringTable_GetColumnValueForRow{0x0, 0x1404F8830};
	WEAK symbol<void(const char* filename, StringTable** table)> StringTable_GetAsset{0x0, 0x1404F87F0};
	WEAK symbol<int(const StringTable* table)> StringTable_GetRowCount{0x0, 0x1404F8870};

	WEAK symbol<bool(int localClient, ScreenPlacement* scrPlace, vec3_t& WorldLocation, vec2_t& Screen)> CG_WorldPosToScreenPosReal{0x0, 0x14021CE50};

	WEAK symbol<void(XAssetType type, void(__cdecl* func)(XAssetHeader, void*), const void* inData, bool includeOverride)>
	DB_EnumXAssets_Internal{0x1401F0BF0, 0x1402BA7C0};
	WEAK symbol<const char*(const XAsset* asset)> DB_GetXAssetName{0x1401BF890, 0x14028BE50};
	WEAK symbol<int(XAssetType type)> DB_GetXAssetTypeSize{0x1401BF8D0, 0x14028BE70};
	WEAK symbol<XAssetHeader(XAssetType type, const char* name, 
		int createDefault)> DB_FindXAssetHeader{0x1401F1120, 0x1402BAC70};
	WEAK symbol<void(void* levelLoad, const char* name, 
		const unsigned int allocFlags, const unsigned __int64 sizeEst)> DB_LevelLoadAddZone{0x0, 0x1402BC600};

	WEAK symbol<int(XAssetType type, const char* name)> DB_IsXAssetDefault{0x1401F25A0, 0x1402BC370};
	WEAK symbol<int(XAssetType type, const char* name)> DB_XAssetExists{0x1401F6290, 0x1402C0FB0};

	WEAK symbol<int(const RawFile* rawfile)> DB_GetRawFileLen{0x1401F1F40, 0x1402BBCD0};
	WEAK symbol<int(const RawFile* rawfile, char* buf, int size)> DB_GetRawBuffer{0x1401F1E00, 0x1402BBBA0};
	WEAK symbol<char*(const char* filename, char* buf, int size)> DB_ReadRawFile{0x1401F4D00, 0x1402BEF10};

	WEAK symbol<bool(const char* zone, int source)> DB_FileExists{0x1401F0D50, 0x1402BA970};
	WEAK symbol<void(XZoneInfo* zoneInfo, unsigned int zoneCount, DBSyncMode syncMode)> DB_LoadXAssets{0x1401F31E0, 0x1402BCF90};
	WEAK symbol<bool(const char* zoneName)> DB_IsLocalized{0x1401F23C0, 0x1402BC290};

	WEAK symbol<void(int client_num, const char* menu, 
		int is_popup, int is_modal, unsigned int is_exclusive)> LUI_OpenMenu{0x1403F20A0, 0x1404CD210};
	WEAK symbol<void(int clientNum, const char* menu, int immediate,
		hks::lua_State* state)> LUI_LeaveMenuByName{0x1400F6D00, 0x140168210};
	WEAK symbol<void()> LUI_EnterCriticalSection{0x1400F19A0, 0x140162DC0};
	WEAK symbol<void()> LUI_LeaveCriticalSection{0x1400F6C40, 0x140168150};

	WEAK symbol<bool(int clientNum, const char* menu)> Menu_IsMenuOpenAndVisible{0x1404F43C0, 0x1404C7320};
	WEAK symbol<void(int clientNum, const char* menu)> Menus_OpenByName{0x0, 0x1404CD2B0};
	WEAK symbol<void(int clientNum, const char* menu)> Menus_CloseByName{0x0, 0x1404C7C70};
	WEAK symbol<void*(void* dc, const char* name)> Menus_FindByName{0x0, 0x1404E46A0};
	WEAK symbol<void(void* dc, void* menu, int a3)> Menus_Open{0x0, 0x1404E4B00};
	WEAK symbol<void(void* dc)> Display_MouseMove{0x0, 0x1404DA010};

	WEAK symbol<int(const float* origin, const float* enemyPos, float maxDist, float maxHeight,
		pathsort_s* nodes, int maxNodes, int typeFlags)> Path_NodesInCylinder{0x0, 0x14031D540};

	WEAK symbol<scr_string_t(const char* str)> SL_FindString{0x1403C0F50, 0x14043B470};
	WEAK symbol<scr_string_t(const char* str, unsigned int user)> SL_GetString{0x1403C1210, 0x14043B840};
	WEAK symbol<const char*(scr_string_t stringValue)> SL_ConvertToString{0x1403C0C50, 0x14043B170};
	WEAK symbol<unsigned int(const char* str)> SL_GetCanonicalString{0x1403BDA20, 0x140437EA0};

	WEAK symbol<void(int index, char* buffer, int bufferSize)> SV_GetConfigstring{0x0, 0x140485B20};

	WEAK symbol<void(netadr_s* from)> SV_DirectConnect{0x0, 0x140480860};
	WEAK symbol<void(int arg, char* buffer, int bufferLength)> SV_Cmd_ArgvBuffer{0x140377D40, 0x140404CA0};
	WEAK symbol<void(const char* text_in)> SV_Cmd_TokenizeString{0x140377DC0, 0x140404D20};
	WEAK symbol<void()> SV_Cmd_EndTokenizedString{0x140377D80, 0x140404CE0};

	WEAK symbol<gentity_s*(const char* name)> SV_AddBot{0x0, 0x140480190};
	WEAK symbol<bool(int clientNum)> SV_BotIsBot{0x0, 0x14046E6C0};
	WEAK symbol<const char*()> SV_BotGetRandomName{0x0, 0x14046DBA0};
	WEAK symbol<int(gentity_s* ent)> SV_SpawnTestClient{0x0, 0x1404832A0};

	WEAK symbol<const char*(int clientNum)> SV_GetGuid{0x0, 0x140484B90};
	WEAK symbol<int(int clientNum)> SV_GetClientPing{0x0, 0x140484B70};
	WEAK symbol<playerState_s* (int num)> SV_GetPlayerstateForClientNum{0x1404C3F10, 0x140484C10};
	WEAK symbol<void(int index, const char* string)> SV_SetConfigstring{0x0, 0x140486720};
	WEAK symbol<bool()> SV_Loaded{0x1404C4810, 0x1404864A0};
	WEAK symbol<void(int clientNum, const char* reason)> SV_KickClientNum{0x0, 0x14047ED00};
	WEAK symbol<bool(const char* map)> SV_MapExists{0x0, 0x14047ED60};
	WEAK symbol<void(int localClientNum, const char* map, bool mapIsPreloaded, bool migrate)> SV_StartMapForParty{0x0, 0x14047F930};

	WEAK symbol<void(client_t*, const char*, int)> SV_ExecuteClientCommand{0x0, 0x0};
	WEAK symbol<void(int localClientNum)> SV_FastRestart{0x0, 0x14047E990};
	WEAK symbol<void(void* cl, int type, const char* fmt, ...)> SV_SendServerCommand{0x0, 0x140489C20};
	WEAK symbol<void(client_t* drop, const char* reason, bool tellThem)> SV_DropClient_Internal{0x0, 0x140481470};

	WEAK symbol<void(const float* origin, float radius, int dangerous)> SV_BotMarkNodesAsDangerous{0x0, 0x14046EC00};

	WEAK symbol<void(void* entity)> SV_LinkEntity{0x0, 0x14049DC30};

	WEAK symbol<void()> Sys_ShowConsole{0x0, 0x0};
	WEAK symbol<void(const char* error, ...)> Sys_Error{0x0, 0x140511520};
	WEAK symbol<void(char* path, int pathSize, Sys_Folder folder, const char* filename, const char* ext)>
		Sys_BuildAbsPath{0x14042C330, 0x0};
	WEAK symbol<int()> Sys_Milliseconds{0x140462B30, 0x140513710};
	WEAK symbol<bool()> Sys_IsDatabaseReady2{0x1403AB100, 0x14042B090};
	WEAK symbol<bool(int, void const*, const netadr_s*)> Sys_SendPacket{0x0, 0x1405133B0};
	WEAK symbol<bool(const char* path)> Sys_FileExists{0x0, 0x1405115E0};
	WEAK symbol<HANDLE(Sys_Folder, const char* baseFilename)> Sys_CreateFile{0x14042C430, 0x140507110};

	WEAK symbol<const char*()> SEH_GetCurrentLanguageCode{0x1403E5FB0, 0x1404BA5F0};
	WEAK symbol<const char*()> SEH_GetCurrentLanguageName{0x1403E6030, 0x1404BA650};

	WEAK symbol<const char*(const char*)> UI_GetMapDisplayName{0x0, 0x140408CC0};
	WEAK symbol<const char*(const char*)> UI_GetGameTypeDisplayName{0x0, 0x1404086A0};
	WEAK symbol<const char*(const char* key, const char* mapname)> UI_GetMapCustomField{0x0, 0x140408AD0};
	WEAK symbol<void(unsigned int localClientNum, const char** args)> UI_RunMenuScript{0x1403F3AA0, 0x1404CFE60};
	WEAK symbol<int(const char* text, int maxChars, Font_s* font, float scale)> UI_TextWidth{0x1403F5D90, 0x0};
	WEAK symbol<void(void* dc, void* menuList, int close)> UI_AddMenuList{0x0, 0x1404E6E30};
	WEAK symbol<void*(const char* name)> UI_LoadMenus{0x0, 0x1404E98A0};
	WEAK symbol<void(int, const char*, int)> UI_PlayLocalSoundAliasByName{0x0, 0x140658790};

	WEAK symbol<const char*(const char* string)> UI_SafeTranslateString{0x1403840A0, 0x14041C580};
	WEAK symbol<void(ScreenPlacement* scrPlace, const char* text, rectDef_s* rect, Font_s* font, float x, float y,
		float scale, const float* color, int style, int textAlignMode, rectDef_s* textRect, char a12)> UI_DrawWrappedText{0x140406CD0, 0x1404E7270};

	WEAK symbol<int(int local_client_num, int menu)> UI_SetActiveMenu{0x0, 0x1404D1320};

	WEAK symbol<void*(jmp_buf* Buf, int Value)> longjmp{0x1406DCA90, 0x140779F64};
	WEAK symbol<int(jmp_buf* Buf)> _setjmp{0x140758980, 0x1407F6030};

	/***************************************************************
	 * Variables
	 **************************************************************/

	WEAK symbol<CmdArgs> sv_cmd_args{0x14B48FF90, 0x14946BA20};

	WEAK symbol<int> g_script_error_level{0x14C3FD358, 0x14A33C824};
	WEAK symbol<jmp_buf> g_script_error{0x14C3FD470, 0x14A33C940};
	
	WEAK symbol<unsigned int> levelEntityId{0x14BD58DA0, 0x149CA0730};
	WEAK symbol<unsigned int> gameEntityId{0x14BD58DA4, 0x149CA0734};

	WEAK symbol<const char*> command_whitelist{0x14115ADF0, 0x14120C360};
	WEAK symbol<cmd_function_s*> cmd_functions{0x14B490038, 0x14946BAC8};
	WEAK symbol<CmdArgs> cmd_args{0x14B48FEE0, 0x14946B970};

	WEAK symbol<int> connectionState{0x0, 0x142D0BA9C};
	WEAK symbol<snapshot_s*> next_snap{0x0, 0x1429398E8};
	WEAK symbol<cg_s> cgameGlob{0x0, 0x142935000};
	WEAK symbol<ViewModelInfo> viewModelInfo{0x0, 0x142A1F5D8};

	WEAK symbol<GfxCmdBufState> gfxCmdBufState{0x0, 0x1525C0320};
	WEAK symbol<GfxCmdBufSourceState> gfxCmdBufSourceState{0x0, 0x1525C23D0};
	WEAK symbol<GfxCmdBufContext> gfxCmdBufContext{0x0, 0x1408973F0};
	WEAK symbol<GfxRenderTarget> gfxRenderTargets{0x0, 0x151E90880};
	WEAK symbol<GfxBackEndData*> backEndData{0x0, 0x1524CBCF8};
	WEAK symbol<GfxBackEndData*> frontEndDataOut{0x0, 0x14FEAFB00};
	WEAK symbol<vidConfig_t> vidConfig{0x0, 0x14FE70A88};
	WEAK symbol<int> is_rendering_thirdperson{0x0, 0x142A24ED4};
	WEAK symbol<int> is_rendering_thirdperson1{0x0, 0x142A24EE8};

	WEAK symbol<int> g_poolSize{0x140EC97D0, 0x140FEADF0};
	WEAK symbol<int> g_compressor{0x142574804, 0x143227F04};

	WEAK symbol<scrVarGlob_t> scr_VarGlob{0x14BD80E00, 0x149CC8800};
	WEAK symbol<scrVmPub_t> scr_VmPub{0x14C3F4E20, 0x14A33EA40};
	WEAK symbol<function_stack_t> scr_function_stack{0x14C4015C0, 0x14A348FC0};

	WEAK symbol<unsigned __int64> pmem_size{0x14D5F26D8, 0x14DD4A2D8};
	WEAK symbol<unsigned char*> pmem_buffer{0x14D5F26D0, 0x14DD4A2D0};

	WEAK symbol<PhysicalMemory> g_mem{0x14D5F26E0, 0x14DD4A2E0};
	WEAK symbol<PhysicalMemory> g_scriptmem{0x14D5F3140, 0x14DD4AD40};
	WEAK symbol<PhysicalMemory> g_physmem{0x14D5F3BA0, 0x14DD4B7A0};

	WEAK symbol<unsigned __int64> stream_size{0x141DAD810, 0x14207CE90};
	WEAK symbol<unsigned char*> stream_buffer{0x141DAD808, 0x14207CE88};

	WEAK symbol<GfxDrawMethod_s> gfxDrawMethod{0x14F7530B0, 0x14FD21180};

	WEAK symbol<int> dvarCount{0x14C90E550, 0x14D064CF4};
	WEAK symbol<dvar_t> dvarPool{0x14C90E560, 0x14D064D00};

	WEAK symbol<void*> g_assetPool{0x140EC9FB0, 0x140FEB5D0};
	WEAK symbol<const char*> g_assetNames{0x140991BA0, 0x140FEA240};
	WEAK symbol<XZoneInfoInternal> g_zoneInfo{0x0, 0x145122460};
	WEAK symbol<unsigned short> g_zoneIndex{0x0, 0x1434A9B68};

	WEAK symbol<DB_FileSysInterface*> db_fs{0x1425C1168, 0x1413F8F78};

	WEAK symbol<int> keyCatchers{0x14252AF70, 0x142D0BA9C};
	WEAK symbol<PlayerKeyState> playerKeys{0x142395B0C, 0x142C19AFC};

	WEAK symbol<SOCKET> query_socket{0x14D64D3F8, 0x14DDFBF98};

	WEAK symbol<DWORD> threadIds{0x14B896210, 0x149810E00};

	WEAK symbol<int> ui_num_arenas{0x0, 0x149560E78};
	WEAK symbol<int> ui_arena_buf_pos{0x0, 0x149560E7C};
	WEAK symbol<char*> ui_arena_infos{0x0, 0x149560E80};
	WEAK symbol<ui_info> ui_info_array{0x0, 0x14CF1E220};

	WEAK symbol<int> level_time{0x1456DBAA0, 0x14621BDBC};
	WEAK symbol<int> com_frameTime{0x0, 0x1421DE388};

	WEAK symbol<map_t> maps{0x1407CE5A0, 0x1408676B0};

	WEAK symbol<GfxWorld*> s_world{0x0, 0x14FD70160};

	WEAK symbol<ID3D11Device*> d3d11_device{0x141163B98, 0x1412185F8};

	WEAK symbol<ComWorld> comWorld{0x0, 0x149487488};
	
	WEAK symbol<NetConstStringMap> s_netConstStringMaps{0x0, 0x1428B2BB0};
	WEAK symbol<NetConstStringMapList> s_netConstStringMapLists{0x0, 0x1401CA240};
	WEAK symbol<NetConstStringConfigStringTypeData> s_oldConfigStringToNetStringMap{0x0, 0x14082FBB4};

	namespace mp
	{
		WEAK symbol<gentity_s> g_entities{0x0, 0x14621E530};
		WEAK symbol<gclient_s*> clients{0x0, 0x14621B840};
		WEAK symbol<client_t> svs_clients{0x0, 0x14B204A10};
		WEAK symbol<int> svs_numclients{0x0, 0x14B204A0C};
		WEAK symbol<int> gameTime{0x0, 0x14621BDBC};
		WEAK symbol<int> num_entities{0x0, 0x14621B860};

		WEAK symbol<int> sv_serverId_value{0x0, 0x14A3E99B8};

		WEAK symbol<bool> virtualLobby_loaded{0x0, 0x142D077FD};


		WEAK symbol<client_state_t> client_state{0x0, 0x142D0BCB0};
		WEAK symbol<connect_state_t> connect_state{0x0, 0x14318C650};

		WEAK symbol<XZone> g_zones{0x0, 0x1450F56D0};
		WEAK symbol<unsigned int> g_zoneCount{0x0, 0x143498F0C};

		WEAK symbol<int> db_hashTable{0x0, 0x143411FA0};
		WEAK symbol<XAssetEntry> g_assetEntryPool{0x0, 0x144CFDCD0};

		WEAK symbol<PackedLoadedSound*> db_packedLoadedSounds{0x0, 0x14533C9F0};
		WEAK symbol<int> db_packedLoadedSoundCount{0x0, 0x14534C3F0};
		WEAK symbol<std::uint64_t> db_packedLoadedSoundSize{0x0, 0x14534C400};
		WEAK symbol<std::uint64_t> db_xfileStageReadSize{0x0, 0x14338E038};
		WEAK symbol<XModelMaterialList> db_xmodelMaterialLists{0x0, 0x1451BAB00};

		WEAK symbol<std::uint32_t> streamImageCount{0x0, 0x141CD3580};
		WEAK symbol<void*> streamSortFile{0x0, 0x14534C418};

		WEAK symbol<TransientMemPool> s_transientPools{0x0, 0x1413B8320};
		WEAK symbol<std::uint32_t> s_transientPoolCount{0x0, 0x1413B88D0};
		WEAK symbol<std::uint8_t*> s_transientCurrentFile{0x0, 0x1413D2F70};
		WEAK symbol<void*> s_transientTempBuffer{0x0, 0x1413D2F78};
		WEAK symbol<std::uint16_t> s_transientFileHashTable{0x0, 0x1413D1F60};
		WEAK symbol<std::uint8_t*(std::uint32_t hash)> CL_TransientMem_FindFileByHash{0x0, 0x14005DCC0};

		WEAK symbol<playerState_s*> playerState{0x0, 0x14CAAC780};
	}

	WEAK symbol<int64_t(ID3D11Buffer**)> R_DestroyComputeBuffers{0x0, 0x140091AD0};
	WEAK symbol<refdef_t*> refdef{0x0, 0x142917A28};
	WEAK symbol<PathData> pathdata{0x0, 0x1461D9090};
	WEAK symbol<unsigned __int64(pathnode_t* node, float* pos)> WorldifyPosFromParent{0x0, 0x140320FB0};
	WEAK symbol<GfxScene> scene{0x0, 0x14243AD80};

	namespace sp
	{
		WEAK symbol<gentity_s> g_entities{0x1456E74D0, 0x0};

		WEAK symbol<XZone> g_zones{0x1445FE990, 0x0};
	}

	namespace hks
	{
		WEAK symbol<lua_State*> lua_state{0x141F0E408, 0x1426D3D08};
		WEAK symbol<void(lua_State* s, const char* str, unsigned int l)> hksi_lua_pushlstring{0x140062E60, 0x1400624F0};
		WEAK symbol<HksObject*(HksObject* result, lua_State* s, const HksObject* table, const HksObject* key)> hks_obj_getfield{0x1400B5100, 0x14012C600};
		WEAK symbol<void(lua_State* s, const HksObject* tbl, const HksObject* key, const HksObject* val)> hks_obj_settable{0x1400B6320, 0x14012D820};
		WEAK symbol<HksObject* (HksObject* result, lua_State* s, const HksObject* table, const HksObject* key)> hks_obj_gettable{0x1400B55E0, 0x14012CAE0};
		WEAK symbol<void(lua_State* s, int nargs, int nresults, const unsigned int* pc)> vm_call_internal{0x1400E5E40, 0x140159EB0};
		WEAK symbol<HashTable*(lua_State* s, unsigned int arraySize, unsigned int hashSize)> Hashtable_Create{0x1400A3570, 0x14011B320};
		WEAK symbol<cclosure*(lua_State* s, lua_function function, int num_upvalues, 
			int internal_, int profilerTreatClosureAsFunc)> cclosure_Create{0x1400A3790, 0x14011B540};
		WEAK symbol<int(lua_State* s, int t)> hksi_luaL_ref{0x1400B7F90, 0x140136D30};
		WEAK symbol<void(lua_State* s, int t, int ref)> hksi_luaL_unref{0x1400B8130, 0x14012F610};
		WEAK symbol<int(lua_State* s, const HksCompilerSettings* options, const char* buff, 
			unsigned __int64 sz, const char* name)> hksi_hksL_loadbuffer{0x1400B6B90, 0x14012E070};
		WEAK symbol<int(lua_State* s, const char* what, lua_Debug* ar)> hksi_lua_getinfo{0x1400B84D0, 0x14012FA70};
		WEAK symbol<int(lua_State* s, int level, lua_Debug* ar)> hksi_lua_getstack{0x1400B87A0, 0x14012FD40};
		WEAK symbol<void(lua_State* s, const char* fmt, ...)> hksi_luaL_error{0x1400BF120, 0x14012F300};
		WEAK symbol<void(lua_State* s, int what, int data)> hksi_lua_gc{0x0, 0x140136F30};
		WEAK symbol<const char*> s_compilerTypeName{0x14098CD20, 0x140FE3B30};
	}
}
