#pragma once
#include <d3d11.h>

#define PROTOCOL 2

#include "database.hpp"

typedef float vec_t;
typedef vec_t vec3_t[3];
#define ANIM_TOGGLEBIT 0x800

#ifndef __cplusplus
#define IDA
#endif

#ifndef IDA
namespace game
{
#endif
	using namespace database;

	// * scripting

	struct scr_entref_t
	{
		unsigned short entnum;
		unsigned short classnum;
	};

	enum VariableType
	{
		VAR_UNDEFINED = 0x0,
		VAR_BEGIN_REF = 0x1,
		VAR_POINTER = 0x1,
		VAR_STRING = 0x2,
		VAR_ISTRING = 0x3,
		VAR_VECTOR = 0x4,
		VAR_END_REF = 0x5,
		VAR_FLOAT = 0x5,
		VAR_INTEGER = 0x6,
		VAR_CODEPOS = 0x7,
		VAR_PRECODEPOS = 0x8,
		VAR_FUNCTION = 0x9,
		VAR_BUILTIN_FUNCTION = 0xA,
		VAR_BUILTIN_METHOD = 0xB,
		VAR_STACK = 0xC,
		VAR_ANIMATION = 0xD,
		VAR_PRE_ANIMATION = 0xE,
		VAR_THREAD = 0xF,
		VAR_NOTIFY_THREAD = 0x10,
		VAR_TIME_THREAD = 0x11,
		VAR_CHILD_THREAD = 0x12,
		VAR_OBJECT = 0x13,
		VAR_DEAD_ENTITY = 0x14,
		VAR_ENTITY = 0x15,
		VAR_ARRAY = 0x16,
		VAR_DEAD_THREAD = 0x17,
		VAR_COUNT = 0x18,
		VAR_FREE = 0x18,
		VAR_THREAD_LIST = 0x19,
		VAR_ENDON_LIST = 0x1A,
		VAR_TOTAL_COUNT = 0x1B,
	};

	struct VariableStackBuffer
	{
		const char* pos;
		unsigned __int16 size;
		unsigned __int16 bufLen;
		unsigned __int16 localId;
		char time;
		char buf[1];
	};

	union VariableUnion
	{
		int intValue;
		unsigned int uintValue;
		float floatValue;
		unsigned int stringValue;
		const float* vectorValue;
		const char* codePosValue;
		unsigned int pointerValue;
		VariableStackBuffer* stackValue;
		unsigned int entityOffset;
	};

	struct VariableValue
	{
		VariableUnion u;
		int type;
	};

	struct function_stack_t
	{
		const char* pos;
		unsigned int localId;
		unsigned int localVarCount;
		VariableValue* top;
		VariableValue* startTop;
	};

	struct function_frame_t
	{
		function_stack_t fs;
		int topType;
	};

	struct scrVmPub_t
	{
		unsigned int* localVars;
		VariableValue* maxstack;
		int function_count;
		function_frame_t* function_frame;
		VariableValue* top;
		unsigned int inparamcount;
		unsigned int outparamcount;
		function_frame_t function_frame_start[32];
		VariableValue stack[2048];
	};

	struct scr_classStruct_t
	{
		unsigned __int16 id;
		unsigned __int16 entArrayId;
		char charId;
		const char* name;
	};

	struct ObjectVariableChildren
	{
		unsigned __int16 firstChild;
		unsigned __int16 lastChild;
	};

	struct ObjectVariableValue_u_f
	{
		unsigned __int16 prev;
		unsigned __int16 next;
	};

	union ObjectVariableValue_u_o_u
	{
		unsigned __int16 size;
		unsigned __int16 entnum;
		unsigned __int16 nextEntId;
		unsigned __int16 self;
	};

	struct	ObjectVariableValue_u_o
	{
		unsigned __int16 refCount;
		ObjectVariableValue_u_o_u u;
	};

	union ObjectVariableValue_w
	{
		unsigned int type;
		unsigned int classnum;
		unsigned int notifyName;
		unsigned int waitTime;
		unsigned int parentLocalId;
	};

	struct ChildVariableValue_u_f
	{
		unsigned __int16 prev;
		unsigned __int16 next;
	};

	union ChildVariableValue_u
	{
		ChildVariableValue_u_f f;
		VariableUnion u;
	};

	struct ChildBucketMatchKeys_keys
	{
		unsigned __int16 name_hi;
		unsigned __int16 parentId;
	};

	union ChildBucketMatchKeys
	{
		ChildBucketMatchKeys_keys keys;
		unsigned int match;
	};

	struct	ChildVariableValue
	{
		ChildVariableValue_u u;
		unsigned __int16 next;
		char type;
		char name_lo;
		ChildBucketMatchKeys k;
		unsigned __int16 nextSibling;
		unsigned __int16 prevSibling;
	};

	union ObjectVariableValue_u
	{
		ObjectVariableValue_u_f f;
		ObjectVariableValue_u_o o;
	};

	struct ObjectVariableValue
	{
		ObjectVariableValue_u u;
		ObjectVariableValue_w w;
	};

	struct scrVarGlob_t
	{
		ObjectVariableValue objectVariableValue[40960];
		ObjectVariableChildren objectVariableChildren[40960];
		unsigned __int16 childVariableBucket[65536];
		ChildVariableValue childVariableValue[384000];
	};

	enum Sys_Folder
	{
		SF_ZONE = 0x0,
		SF_ZONE_LOC = 0x1,
		SF_VIDEO = 0x2,
		SF_VIDEO_LOC = 0x3,
		SF_PAKFILE = 0x4,
		SF_PAKFILE_LOC = 0x5,
		SF_COUNT = 0x6,
	};

	enum FileSysResult : std::int32_t
	{
		FILESYSRESULT_SUCCESS = 0x0,
		FILESYSRESULT_EOF = 0x1,
		FILESYSRESULT_ERROR = 0x2,
	};

	struct DB_IFileSysFile
	{
		void* file;
		uint64_t last_read;
		uint64_t bytes_read;
	};

	static_assert(sizeof(DB_IFileSysFile) == 24);

	struct DB_FileSysInterface;

	// this is a best guess, interface doesn't match up exactly w/other games (IW8, T9)
	struct DB_FileSysInterface_vtbl
	{
		DB_IFileSysFile* (__fastcall* OpenFile)(DB_FileSysInterface* _this, Sys_Folder folder, const char* filename);
		FileSysResult(__fastcall* Read)(DB_FileSysInterface* _this, DB_IFileSysFile* handle, unsigned __int64 offset, unsigned __int64 size, void* dest);
		FileSysResult(__fastcall* Tell)(DB_FileSysInterface* _this, DB_IFileSysFile* handle, unsigned __int64* bytesRead);
		__int64(__fastcall* Size)(DB_FileSysInterface* _this, DB_IFileSysFile* handle);
		void(__fastcall* Close)(DB_FileSysInterface* _this, DB_IFileSysFile* handle);
		bool(__fastcall* Exists)(DB_FileSysInterface* _this, Sys_Folder folder, const char* filename);
	};

	struct DB_FileSysInterface
	{
		DB_FileSysInterface_vtbl* vftbl;
	};

	struct StreamFile
	{
		DB_IFileSysFile* handle;
		std::uint64_t length;
		std::uint64_t startOffset;
		bool isPak;
		char __pad0[7];
	};

	static_assert(sizeof(StreamFile) == 0x20);

	struct PackedLoadedSound
	{
		char __pad0[8];
		std::uint8_t isLocalized;
		char __pad1;
		std::uint16_t fileIndex;
		char __pad2[4];
		std::uint64_t offset;
		std::uint64_t length;
		char* dest;
	};

	static_assert(offsetof(PackedLoadedSound, isLocalized) == 0x8);
	static_assert(offsetof(PackedLoadedSound, fileIndex) == 0xA);
	static_assert(offsetof(PackedLoadedSound, offset) == 0x10);
	static_assert(offsetof(PackedLoadedSound, length) == 0x18);
	static_assert(offsetof(PackedLoadedSound, dest) == 0x20);

	struct PMemRange
	{
		char* base;
		std::uint32_t blocks;
	};

	struct TransientMemPool
	{
		char name[0x40];
		std::uint64_t slotSize;
		std::uint32_t slotCount;
		std::uint32_t fileCount;
		char __pad0[0x20];
	};

	static_assert(sizeof(TransientMemPool) == 0x70);

	struct XModelMaterialList
	{
		std::uint32_t count;
		std::uint32_t* indices;
		char __pad0[8];
	};

	static_assert(sizeof(XModelMaterialList) == 0x18);

	enum CodPlayMode
	{
		CODPLAYMODE_NONE = 0x0,
		CODPLAYMODE_SP = 0x1,
		CODPLAYMODE_CORE = 0x2,
		CODPLAYMODE_SURVIVAL = 0x5,
		CODPLAYMODE_ZOMBIES = 0x6,
	};

	enum DWOnlineStatus
	{
		DW_LIVE_DISCONNECTED = 0x0,
		DW_LIVE_CONNECTING = 0x1,
		DW_LIVE_CONNECTED = 0x2,
	};

	enum DWNetStatus
	{
		DW_NET_ERROR_START_FAILED = 0x0,
		DW_NET_ERROR_NO_LOCAL_IP = 0x1,
		DW_NET_NOT_STARTED = 0x2,
		DW_NET_STARTING_LAN = 0x3,
		DW_NET_STARTED_LAN = 0x4,
		DW_NET_STARTING_ONLINE = 0x5,
		DW_NET_STARTED_ONLINE = 0x6,
	};

	enum DWLogonStatus
	{
		DW_LOGON_ERROR = 0x0,
		DW_NP_CONNECTING = 0x1,
		DW_NO_ACCOUNT_SIGNED_IN = 0x2,
		DW_DNS_NOT_RESOLVED = 0x3,
		DW_PC_STEAM_ACQUIRE_DEDICATED_LICENSE = 0x4,
		DW_PC_STEAM_ACQUIRING_DEDICATED_LICENSE = 0x5,
		DW_PC_STEAM_AUTHORIZING_DEDICATED_LICENSE = 0x6,
		DW_PC_STATIC_LICENSE = 0x7,
		DW_PC_AUTHORIZING_STATIC_LICENSE = 0x8,
		DW_LOBBY_CONNECT = 0x9,
		DW_LOBBY_CONNECTING = 0xA,
		DW_LOGON_COMPLETE = 0xB,
	};

	enum bdLobbyErrorCode : uint32_t
	{
		BD_NO_ERROR = 0x0,
		BD_TOO_MANY_TASKS = 0x1,
		BD_NOT_CONNECTED = 0x2,
		BD_SEND_FAILED = 0x3,
		BD_HANDLE_TASK_FAILED = 0x4,
		BD_START_TASK_FAILED = 0x5,
		BD_RESULT_EXCEEDS_BUFFER_SIZE = 0x64,
		BD_ACCESS_DENIED = 0x65,
		BD_EXCEPTION_IN_DB = 0x66,
		BD_MALFORMED_TASK_HEADER = 0x67,
		BD_INVALID_ROW = 0x68,
		BD_EMPTY_ARG_LIST = 0x69,
		BD_PARAM_PARSE_ERROR = 0x6A,
		BD_PARAM_MISMATCHED_TYPE = 0x6B,
		BD_SERVICE_NOT_AVAILABLE = 0x6C,
		BD_CONNECTION_RESET = 0x6D,
		BD_INVALID_USER_ID = 0x6E,
		BD_LOBBY_PROTOCOL_VERSION_FAILURE = 0x6F,
		BD_LOBBY_INTERNAL_FAILURE = 0x70,
		BD_LOBBY_PROTOCOL_ERROR = 0x71,
		BD_LOBBY_FAILED_TO_DECODE_UTF8 = 0x72,
		BD_LOBBY_ASCII_EXPECTED = 0x73,
		BD_ASYNCHRONOUS_ERROR = 0xC8,
		BD_STREAMING_COMPLETE = 0xC9,
		BD_MEMBER_NO_PROPOSAL = 0x12C,
		BD_TEAMNAME_ALREADY_EXISTS = 0x12D,
		BD_MAX_TEAM_MEMBERSHIPS_LIMITED = 0x12E,
		BD_MAX_TEAM_OWNERSHIPS_LIMITED = 0x12F,
		BD_NOT_A_TEAM_MEMBER = 0x130,
		BD_INVALID_TEAM_ID = 0x131,
		BD_INVALID_TEAM_NAME = 0x132,
		BD_NOT_A_TEAM_OWNER = 0x133,
		BD_NOT_AN_ADMIN_OR_OWNER = 0x134,
		BD_MEMBER_PROPOSAL_EXISTS = 0x135,
		BD_MEMBER_EXISTS = 0x136,
		BD_TEAM_FULL = 0x137,
		BD_VULGAR_TEAM_NAME = 0x138,
		BD_TEAM_USERID_BANNED = 0x139,
		BD_TEAM_EMPTY = 0x13A,
		BD_INVALID_TEAM_PROFILE_QUERY_ID = 0x13B,
		BD_TEAMNAME_TOO_SHORT = 0x13C,
		BD_UNIQUE_PROFILE_DATA_EXISTS_ALREADY = 0x13D,
		BD_INVALID_LEADERBOARD_ID = 0x190,
		BD_INVALID_STATS_SET = 0x191,
		BD_EMPTY_STATS_SET_IGNORED = 0x193,
		BD_NO_DIRECT_ACCESS_TO_ARBITRATED_LBS = 0x194,
		BD_STATS_WRITE_PERMISSION_DENIED = 0x195,
		BD_STATS_WRITE_TYPE_DATA_TYPE_MISMATCH = 0x196,
		BD_NO_STATS_FOR_USER = 0x197,
		BD_INVALID_ACCESS_TO_UNRANKED_LB = 0x198,
		BD_INVALID_EXTERNAL_TITLE_ID = 0x199,
		BD_DIFFERENT_LEADERBOARD_SCHEMAS = 0x19A,
		BD_TOO_MANY_LEADERBOARDS_REQUESTED = 0x19B,
		BD_ENTITLEMENTS_ERROR = 0x19C,
		BD_ENTITLEMENTS_INVALID_TITLEID = 0x19D,
		BD_ENTITLEMENTS_INVALID_LEADERBOARDID = 0x19E,
		BD_ENTITLEMENTS_INVALID_GET_MODE_FOR_TITLE = 0x19F,
		BD_ENTITLEMENTS_URL_CONNECTION_ERROR = 0x1A0,
		BD_ENTITLEMENTS_CONFIG_ERROR = 0x1A1,
		BD_ENTITLEMENTS_NAMED_PARENT_ERROR = 0x1A2,
		BD_ENTITLEMENTS_NAMED_KEY_ERROR = 0x1A3,
		BD_TOO_MANY_ENTITY_IDS_REQUESTED = 0x1A4,
		BD_STATS_READ_FAILED = 0x1A5,
		BD_INVALID_TITLE_ID = 0x1F4,
		BD_MESSAGING_INVALID_MAIL_ID = 0x258,
		BD_SELF_BLOCK_NOT_ALLOWED = 0x259,
		BD_GLOBAL_MESSAGE_ACCESS_DENIED = 0x25A,
		BD_GLOBAL_MESSAGES_USER_LIMIT_EXCEEDED = 0x25B,
		BD_MESSAGING_SENDER_DOES_NOT_EXIST = 0x25C,
		BD_AUTH_NO_ERROR = 0x2BC,
		BD_AUTH_BAD_REQUEST = 0x2BD,
		BD_AUTH_SERVER_CONFIG_ERROR = 0x2BE,
		BD_AUTH_BAD_TITLE_ID = 0x2BF,
		BD_AUTH_BAD_ACCOUNT = 0x2C0,
		BD_AUTH_ILLEGAL_OPERATION = 0x2C1,
		BD_AUTH_INCORRECT_LICENSE_CODE = 0x2C2,
		BD_AUTH_CREATE_USERNAME_EXISTS = 0x2C3,
		BD_AUTH_CREATE_USERNAME_ILLEGAL = 0x2C4,
		BD_AUTH_CREATE_USERNAME_VULGAR = 0x2C5,
		BD_AUTH_CREATE_MAX_ACC_EXCEEDED = 0x2C6,
		BD_AUTH_MIGRATE_NOT_SUPPORTED = 0x2C7,
		BD_AUTH_TITLE_DISABLED = 0x2C8,
		BD_AUTH_ACCOUNT_EXPIRED = 0x2C9,
		BD_AUTH_ACCOUNT_LOCKED = 0x2CA,
		BD_AUTH_UNKNOWN_ERROR = 0x2CB,
		BD_AUTH_INCORRECT_PASSWORD = 0x2CC,
		BD_AUTH_IP_NOT_IN_ALLOWED_RANGE = 0x2CD,
		BD_AUTH_WII_TOKEN_VERIFICATION_FAILED = 0x2CE,
		BD_AUTH_WII_AUTHENTICATION_FAILED = 0x2CF,
		BD_AUTH_IP_KEY_LIMIT_REACHED = 0x2D0,
		BD_AUTH_INVALID_GSPID = 0x2D1,
		BD_AUTH_INVALID_IP_RANGE_ID = 0x2D2,
		BD_AUTH_3DS_TOKEN_VERIFICATION_FAILED = 0x2D1,
		BD_AUTH_3DS_AUTHENTICATION_FAILED = 0x2D2,
		BD_AUTH_STEAM_APP_ID_MISMATCH = 0x2D3,
		BD_AUTH_ABACCOUNTS_APP_ID_MISMATCH = 0x2D4,
		BD_AUTH_CODO_USERNAME_NOT_SET = 0x2D5,
		BD_AUTH_WIIU_TOKEN_VERIFICATION_FAILED = 0x2D6,
		BD_AUTH_WIIU_AUTHENTICATION_FAILED = 0x2D7,
		BD_AUTH_CODO_USERNAME_NOT_BASE64 = 0x2D8,
		BD_AUTH_CODO_USERNAME_NOT_UTF8 = 0x2D9,
		BD_AUTH_TENCENT_TICKET_EXPIRED = 0x2DA,
		BD_AUTH_PS3_SERVICE_ID_MISMATCH = 0x2DB,
		BD_AUTH_CODOID_NOT_WHITELISTED = 0x2DC,
		BD_AUTH_PLATFORM_TOKEN_ERROR = 0x2DD,
		BD_AUTH_JSON_FORMAT_ERROR = 0x2DE,
		BD_AUTH_REPLY_CONTENT_ERROR = 0x2DF,
		BD_AUTH_THIRD_PARTY_TOKEN_EXPIRED = 0x2E0,
		BD_AUTH_CONTINUING = 0x2E1,
		BD_AUTH_PLATFORM_DEVICE_ID_ERROR = 0x2E4,
		BD_NO_PROFILE_INFO_EXISTS = 0x320,
		BD_FRIENDSHIP_NOT_REQUSTED = 0x384,
		BD_NOT_A_FRIEND = 0x385,
		BD_SELF_FRIENDSHIP_NOT_ALLOWED = 0x387,
		BD_FRIENDSHIP_EXISTS = 0x388,
		BD_PENDING_FRIENDSHIP_EXISTS = 0x389,
		BD_USERID_BANNED = 0x38A,
		BD_FRIENDS_FULL = 0x38C,
		BD_FRIENDS_NO_RICH_PRESENCE = 0x38D,
		BD_RICH_PRESENCE_TOO_LARGE = 0x38E,
		BD_NO_FILE = 0x3E8,
		BD_PERMISSION_DENIED = 0x3E9,
		BD_FILESIZE_LIMIT_EXCEEDED = 0x3EA,
		BD_FILENAME_MAX_LENGTH_EXCEEDED = 0x3EB,
		BD_EXTERNAL_STORAGE_SERVICE_ERROR = 0x3EC,
		BD_CHANNEL_DOES_NOT_EXIST = 0x44D,
		BD_CHANNEL_ALREADY_SUBSCRIBED = 0x44E,
		BD_CHANNEL_NOT_SUBSCRIBED = 0x44F,
		BD_CHANNEL_FULL = 0x450,
		BD_CHANNEL_SUBSCRIPTIONS_FULL = 0x451,
		BD_CHANNEL_NO_SELF_WHISPERING = 0x452,
		BD_CHANNEL_ADMIN_REQUIRED = 0x453,
		BD_CHANNEL_TARGET_NOT_SUBSCRIBED = 0x454,
		BD_CHANNEL_REQUIRES_PASSWORD = 0x455,
		BD_CHANNEL_TARGET_IS_SELF = 0x456,
		BD_CHANNEL_PUBLIC_BAN_NOT_ALLOWED = 0x457,
		BD_CHANNEL_USER_BANNED = 0x458,
		BD_CHANNEL_PUBLIC_PASSWORD_NOT_ALLOWED = 0x459,
		BD_CHANNEL_PUBLIC_KICK_NOT_ALLOWED = 0x45A,
		BD_CHANNEL_MUTED = 0x45B,
		BD_EVENT_DESC_TRUNCATED = 0x4B0,
		BD_CONTENT_UNLOCK_UNKNOWN_ERROR = 0x514,
		BD_UNLOCK_KEY_INVALID = 0x515,
		BD_UNLOCK_KEY_ALREADY_USED_UP = 0x516,
		BD_SHARED_UNLOCK_LIMIT_REACHED = 0x517,
		BD_DIFFERENT_HARDWARE_ID = 0x518,
		BD_INVALID_CONTENT_OWNER = 0x519,
		BD_CONTENT_UNLOCK_INVALID_USER = 0x51A,
		BD_CONTENT_UNLOCK_INVALID_CATEGORY = 0x51B,
		BD_KEY_ARCHIVE_INVALID_WRITE_TYPE = 0x5DC,
		BD_KEY_ARCHIVE_EXCEEDED_MAX_IDS_PER_REQUEST = 0x5DD,
		BD_BANDWIDTH_TEST_TRY_AGAIN = 0x712,
		BD_BANDWIDTH_TEST_STILL_IN_PROGRESS = 0x713,
		BD_BANDWIDTH_TEST_NOT_PROGRESS = 0x714,
		BD_BANDWIDTH_TEST_SOCKET_ERROR = 0x715,
		BD_INVALID_SESSION_NONCE = 0x76D,
		BD_ARBITRATION_FAILURE = 0x76F,
		BD_ARBITRATION_USER_NOT_REGISTERED = 0x771,
		BD_ARBITRATION_NOT_CONFIGURED = 0x772,
		BD_CONTENTSTREAMING_FILE_NOT_AVAILABLE = 0x7D0,
		BD_CONTENTSTREAMING_STORAGE_SPACE_EXCEEDED = 0x7D1,
		BD_CONTENTSTREAMING_NUM_FILES_EXCEEDED = 0x7D2,
		BD_CONTENTSTREAMING_UPLOAD_BANDWIDTH_EXCEEDED = 0x7D3,
		BD_CONTENTSTREAMING_FILENAME_MAX_LENGTH_EXCEEDED = 0x7D4,
		BD_CONTENTSTREAMING_MAX_THUMB_DATA_SIZE_EXCEEDED = 0x7D5,
		BD_CONTENTSTREAMING_DOWNLOAD_BANDWIDTH_EXCEEDED = 0x7D6,
		BD_CONTENTSTREAMING_NOT_ENOUGH_DOWNLOAD_BUFFER_SPACE = 0x7D7,
		BD_CONTENTSTREAMING_SERVER_NOT_CONFIGURED = 0x7D8,
		BD_CONTENTSTREAMING_INVALID_APPLE_RECEIPT = 0x7DA,
		BD_CONTENTSTREAMING_APPLE_STORE_NOT_AVAILABLE = 0x7DB,
		BD_CONTENTSTREAMING_APPLE_RECEIPT_FILENAME_MISMATCH = 0x7DC,
		BD_CONTENTSTREAMING_HTTP_ERROR = 0x7E4,
		BD_CONTENTSTREAMING_FAILED_TO_START_HTTP = 0x7E5,
		BD_CONTENTSTREAMING_LOCALE_INVALID = 0x7E6,
		BD_CONTENTSTREAMING_LOCALE_MISSING = 0x7E7,
		BD_VOTERANK_ERROR_EMPTY_RATING_SUBMISSION = 0x7EE,
		BD_VOTERANK_ERROR_MAX_VOTES_EXCEEDED = 0x7EF,
		BD_VOTERANK_ERROR_INVALID_RATING = 0x7F0,
		BD_MAX_NUM_TAGS_EXCEEDED = 0x82A,
		BD_TAGGED_COLLECTION_DOES_NOT_EXIST = 0x82B,
		BD_EMPTY_TAG_ARRAY = 0x82C,
		BD_INVALID_QUERY_ID = 0x834,
		BD_NO_ENTRY_TO_UPDATE = 0x835,
		BD_SESSION_INVITE_EXISTS = 0x836,
		BD_INVALID_SESSION_ID = 0x837,
		BD_ATTACHMENT_TOO_LARGE = 0x838,
		BD_INVALID_GROUP_ID = 0xAF0,
		BD_MAIL_INVALID_MAIL_ID_ERROR = 0xB55,
		BD_UCD_SERVICE_ERROR = 0xC80,
		BD_UCD_SERVICE_DISABLED = 0xC81,
		BD_UCD_UNINTIALIZED_ERROR = 0xC82,
		BD_UCD_ACCOUNT_ALREADY_REGISTERED = 0xC83,
		BD_UCD_ACCOUNT_NOT_REGISTERED = 0xC84,
		BD_UCD_AUTH_ATTEMPT_FAILED = 0xC85,
		BD_UCD_ACCOUNT_LINKING_ERROR = 0xC86,
		BD_UCD_ENCRYPTION_ERROR = 0xC87,
		BD_UCD_ACCOUNT_DATA_INVALID = 0xC88,
		BD_UCD_ACCOUNT_DATA_INVALID_FIRSTNAME = 0xC89,
		BD_UCD_ACCOUNT_DATA_INVALID_LASTNAME = 0xC8A,
		BD_UCD_ACCOUNT_DATA_INVALID_DOB = 0xC8B,
		BD_UCD_ACCOUNT_DATA_INVALID_EMAIL = 0xC8C,
		BD_UCD_ACCOUNT_DATA_INVALID_COUNTRY = 0xC8D,
		BD_UCD_ACCOUNT_DATA_INVALID_POSTCODE = 0xC8E,
		BD_UCD_ACCOUNT_DATA_INVALID_PASSWORD = 0xC8F,
		BD_UCD_ACCOUNT_NAME_ALREADY_RESISTERED = 0xC94,
		BD_UCD_ACCOUNT_EMAIL_ALREADY_RESISTERED = 0xC95,
		BD_UCD_GUEST_ACCOUNT_AUTH_CONFLICT = 0xC96,
		BD_TWITCH_SERVICE_ERROR = 0xC1D,
		BD_TWITCH_ACCOUNT_ALREADY_LINKED = 0xC1E,
		BD_TWITCH_NO_LINKED_ACCOUNT = 0xC1F,
		BD_YOUTUBE_SERVICE_ERROR = 0xCE5,
		BD_YOUTUBE_SERVICE_COMMUNICATION_ERROR = 0xCE6,
		BD_YOUTUBE_USER_DENIED_AUTHORIZATION = 0xCE7,
		BD_YOUTUBE_AUTH_MAX_TIME_EXCEEDED = 0xCE8,
		BD_YOUTUBE_USER_UNAUTHORIZED = 0xCE9,
		BD_YOUTUBE_UPLOAD_MAX_TIME_EXCEEDED = 0xCEA,
		BD_YOUTUBE_DUPLICATE_UPLOAD = 0xCEB,
		BD_YOUTUBE_FAILED_UPLOAD = 0xCEC,
		BD_YOUTUBE_ACCOUNT_ALREADY_REGISTERED = 0xCED,
		BD_YOUTUBE_ACCOUNT_NOT_REGISTERED = 0xCEE,
		BD_YOUTUBE_CONTENT_SERVER_ERROR = 0xCEF,
		BD_YOUTUBE_UPLOAD_DOES_NOT_EXIST = 0xCF0,
		BD_YOUTUBE_NO_LINKED_ACCOUNT = 0xCF1,
		BD_YOUTUBE_DEVELOPER_TAGS_INVALID = 0xCF2,
		BD_TWITTER_AUTH_ATTEMPT_FAILED = 0xDAD,
		BD_TWITTER_AUTH_TOKEN_INVALID = 0xDAE,
		BD_TWITTER_UPDATE_LIMIT_REACHED = 0xDAF,
		BD_TWITTER_UNAVAILABLE = 0xDB0,
		BD_TWITTER_ERROR = 0xDB1,
		BD_TWITTER_TIMED_OUT = 0xDB2,
		BD_TWITTER_DISABLED_FOR_USER = 0xDB3,
		BD_TWITTER_ACCOUNT_AMBIGUOUS = 0xDB4,
		BD_TWITTER_MAXIMUM_ACCOUNTS_REACHED = 0xDB5,
		BD_TWITTER_ACCOUNT_NOT_REGISTERED = 0xDB6,
		BD_TWITTER_DUPLICATE_STATUS = 0xDB7,
		BD_TWITTER_ACCOUNT_ALREADY_REGISTERED = 0xE1C,
		BD_FACEBOOK_AUTH_ATTEMPT_FAILED = 0xE11,
		BD_FACEBOOK_AUTH_TOKEN_INVALID = 0xE12,
		BD_FACEBOOK_PHOTO_DOES_NOT_EXIST = 0xE13,
		BD_FACEBOOK_PHOTO_INVALID = 0xE14,
		BD_FACEBOOK_PHOTO_ALBUM_FULL = 0xE15,
		BD_FACEBOOK_UNAVAILABLE = 0xE16,
		BD_FACEBOOK_ERROR = 0xE17,
		BD_FACEBOOK_TIMED_OUT = 0xE18,
		BD_FACEBOOK_DISABLED_FOR_USER = 0xE19,
		BD_FACEBOOK_ACCOUNT_AMBIGUOUS = 0xE1A,
		BD_FACEBOOK_MAXIMUM_ACCOUNTS_REACHED = 0xE1B,
		BD_FACEBOOK_INVALID_NUM_PICTURES_REQUESTED = 0xE1C,
		BD_FACEBOOK_VIDEO_DOES_NOT_EXIST = 0xE1D,
		BD_FACEBOOK_ACCOUNT_ALREADY_REGISTERED = 0xE1E,
		BD_APNS_INVALID_PAYLOAD = 0xE74,
		BD_APNS_INVALID_TOKEN_LENGTH_ERROR = 0xE76,
		BD_MAX_CONSOLEID_LENGTH_EXCEEDED = 0xEE1,
		BD_MAX_WHITELIST_LENGTH_EXCEEDED = 0xEE2,
		BD_USERGROUP_NAME_ALREADY_EXISTS = 0x1770,
		BD_INVALID_USERGROUP_ID = 0x1771,
		BD_USER_ALREADY_IN_USERGROUP = 0x1772,
		BD_USER_NOT_IN_USERGROUP = 0x1773,
		BD_INVALID_USERGROUP_MEMBER_TYPE = 0x1774,
		BD_TOO_MANY_MEMBERS_REQUESTED = 0x1775,
		BD_USERGROUP_NAME_TOO_SHORT = 0x1776,
		BD_RICH_PRESENCE_DATA_TOO_LARGE = 0x1A90,
		BD_RICH_PRESENCE_TOO_MANY_USERS = 0x1A91,
		BD_PRESENCE_DATA_TOO_LARGE = 0x283C,
		BD_PRESENCE_TOO_MANY_USERS = 0x283D,
		BD_USER_LOGGED_IN_OTHER_TITLE = 0x283E,
		BD_USER_NOT_LOGGED_IN = 0x283F,
		BD_SUBSCRIPTION_TOO_MANY_USERS = 0x1B58,
		BD_SUBSCRIPTION_TICKET_PARSE_ERROR = 0x1B59,
		BD_CODO_ID_INVALID_DATA = 0x1BBC,
		BD_INVALID_MESSAGE_FORMAT = 0x1BBD,
		BD_TLOG_TOO_MANY_MESSAGES = 0x1BBE,
		BD_CODO_ID_NOT_IN_WHITELIST = 0x1BBF,
		BD_TLOG_MESSAGE_TRANSFORMATION_ERROR = 0x1BC0,
		BD_REWARDS_NOT_ENABLED = 0x1BC1,
		BD_MARKETPLACE_ERROR = 0x1F40,
		BD_MARKETPLACE_RESOURCE_NOT_FOUND = 0x1F41,
		BD_MARKETPLACE_INVALID_CURRENCY = 0x1F42,
		BD_MARKETPLACE_INVALID_PARAMETER = 0x1F43,
		BD_MARKETPLACE_RESOURCE_CONFLICT = 0x1F44,
		BD_MARKETPLACE_STORAGE_ERROR = 0x1F45,
		BD_MARKETPLACE_INTEGRITY_ERROR = 0x1F46,
		BD_MARKETPLACE_INSUFFICIENT_FUNDS_ERROR = 0x1F47,
		BD_MARKETPLACE_MMP_SERVICE_ERROR = 0x1F48,
		BD_MARKETPLACE_PRECONDITION_REQUIRED = 0x1F49,
		BD_MARKETPLACE_ITEM_MULTIPLE_PURCHASE_ERROR = 0x1F4A,
		BD_MARKETPLACE_MISSING_REQUIRED_ENTITLEMENT = 0x1F4B,
		BD_MARKETPLACE_VALIDATION_ERROR = 0x1F4C,
		BD_MARKETPLACE_TENCENT_PAYMENT_ERROR = 0x1F4D,
		BD_MARKETPLACE_SKU_NOT_COUPON_ENABLED_ERROR = 0x1F4E,
		BD_LEAGUE_INVALID_TEAM_SIZE = 0x1FA4,
		BD_LEAGUE_INVALID_TEAM = 0x1FA5,
		BD_LEAGUE_INVALID_SUBDIVISION = 0x1FA6,
		BD_LEAGUE_INVALID_LEAGUE = 0x1FA7,
		BD_LEAGUE_TOO_MANY_RESULTS_REQUESTED = 0x1FA8,
		BD_LEAGUE_METADATA_TOO_LARGE = 0x1FA9,
		BD_LEAGUE_TEAM_ICON_TOO_LARGE = 0x1FAA,
		BD_LEAGUE_TEAM_NAME_TOO_LONG = 0x1FAB,
		BD_LEAGUE_ARRAY_SIZE_MISMATCH = 0x1FAC,
		BD_LEAGUE_SUBDIVISION_MISMATCH = 0x2008,
		BD_LEAGUE_INVALID_WRITE_TYPE = 0x2009,
		BD_LEAGUE_INVALID_STATS_DATA = 0x200A,
		BD_LEAGUE_SUBDIVISION_UNRANKED = 0x200B,
		BD_LEAGUE_CROSS_TEAM_STATS_WRITE_PREVENTED = 0x200C,
		BD_LEAGUE_INVALID_STATS_SEASON = 0x200D,
		BD_COMMERCE_ERROR = 0x206C,
		BD_COMMERCE_RESOURCE_NOT_FOUND = 0x206D,
		BD_COMMERCE_STORAGE_INVALID_PARAMETER = 0x206E,
		BD_COMMERCE_APPLICATION_INVALID_PARAMETER = 0x206F,
		BD_COMMERCE_RESOURCE_CONFLICT = 0x2070,
		BD_COMMERCE_STORAGE_ERROR = 0x2071,
		BD_COMMERCE_INTEGRITY_ERROR = 0x2072,
		BD_COMMERCE_MMP_SERVICE_ERROR = 0x2073,
		BD_COMMERCE_PERMISSION_DENIED = 0x2074,
		BD_COMMERCE_INSUFFICIENT_FUNDS_ERROR = 0x2075,
		BD_COMMERCE_UNKNOWN_CURRENCY = 0x2076,
		BD_COMMERCE_INVALID_RECEIPT = 0x2077,
		BD_COMMERCE_RECEIPT_USED = 0x2078,
		BD_COMMERCE_TRANSACTION_ALREADY_APPLIED = 0x2079,
		BD_COMMERCE_INVALID_CURRENCY_TYPE = 0x207A,
		BD_CONNECTION_COUNTER_ERROR = 0x20D0,
		BD_LINKED_ACCOUNTS_INVALID_CONTEXT = 0x2198,
		BD_LINKED_ACCOUNTS_INVALID_PLATFORM = 0x2199,
		BD_LINKED_ACCOUNTS_LINKED_ACCOUNTS_FETCH_ERROR = 0x219A,
		BD_LINKED_ACCOUNTS_INVALID_ACCOUNT = 0x219B,
		BD_GMSG_INVALID_CATEGORY_ID = 0x27D8,
		BD_GMSG_CATEGORY_MEMBERSHIPS_LIMIT = 0x27D9,
		BD_GMSG_NONMEMBER_POST_DISALLOWED = 0x27DA,
		BD_GMSG_CATEGORY_DISALLOWS_CLIENT_TYPE = 0x27DB,
		BD_GMSG_PAYLOAD_TOO_BIG = 0x27DC,
		BD_GMSG_MEMBER_POST_DISALLOWED = 0x27DD,
		BD_GMSG_OVERLOADED = 0x27DE,
		BD_GMSG_USER_PERCATEGORY_POST_RATE_EXCEEDED = 0x27DF,
		BD_GMSG_USER_GLOBAL_POST_RATE_EXCEEDED = 0x27E0,
		BD_GMSG_GROUP_POST_RATE_EXCEEDED = 0x27E1,
		BD_MAX_ERROR_CODE = 0x27E2,
	};

	enum bdNATType : uint8_t
	{
		BD_NAT_UNKNOWN = 0x0,
		BD_NAT_OPEN = 0x1,
		BD_NAT_MODERATE = 0x2,
		BD_NAT_STRICT = 0x3,
	};

#pragma pack(push, 1)
	struct bdAuthTicket
	{
		unsigned int m_magicNumber;
		char m_type;
		unsigned int m_titleID;
		unsigned int m_timeIssued;
		unsigned int m_timeExpires;
		unsigned long long m_licenseID;
		unsigned long long m_userID;
		char m_username[64];
		char m_sessionKey[24];
		char m_usingHashMagicNumber[3];
		char m_hash[4];
	};
#pragma pack(pop)

	enum keyNum_t
	{
		K_NONE = 0x0,
		K_FIRSTGAMEPADBUTTON_RANGE_1 = 0x1,
		K_BUTTON_A = 0x1,
		K_BUTTON_B = 0x2,
		K_BUTTON_X = 0x3,
		K_BUTTON_Y = 0x4,
		K_BUTTON_LSHLDR = 0x5,
		K_BUTTON_RSHLDR = 0x6,
		K_LASTGAMEPADBUTTON_RANGE_1 = 0x6,
		K_BS = 0x8,
		K_TAB = 0x9,
		K_ENTER = 0xD,
		K_FIRSTGAMEPADBUTTON_RANGE_2 = 0xE,
		K_BUTTON_START = 0xE,
		K_BUTTON_BACK = 0xF,
		K_BUTTON_LSTICK = 0x10,
		K_BUTTON_RSTICK = 0x11,
		K_BUTTON_LTRIG = 0x12,
		K_BUTTON_RTRIG = 0x13,
		K_DPAD_UP = 0x14,
		K_FIRSTDPAD = 0x14,
		K_DPAD_DOWN = 0x15,
		K_DPAD_LEFT = 0x16,
		K_DPAD_RIGHT = 0x17,
		K_BUTTON_LSTICK_ALTIMAGE2 = 0x10,
		K_BUTTON_RSTICK_ALTIMAGE2 = 0x11,
		K_BUTTON_LSTICK_ALTIMAGE = 0xBC,
		K_BUTTON_RSTICK_ALTIMAGE = 0xBD,
		K_LASTDPAD = 0x17,
		K_LASTGAMEPADBUTTON_RANGE_2 = 0x17,
		K_ESCAPE = 0x1B,
		K_FIRSTGAMEPADBUTTON_RANGE_3 = 0x1C,
		K_APAD_UP = 0x1C,
		K_FIRSTAPAD = 0x1C,
		K_APAD_DOWN = 0x1D,
		K_APAD_LEFT = 0x1E,
		K_APAD_RIGHT = 0x1F,
		K_LASTAPAD = 0x1F,
		K_LASTGAMEPADBUTTON_RANGE_3 = 0x1F,
		K_SPACE = 0x20,
		K_GRAVE = 0x60,
		K_TILDE = 0x7E,
		K_BACKSPACE = 0x7F,
		K_ASCII_FIRST = 0x80,
		K_ASCII_181 = 0x80,
		K_ASCII_191 = 0x81,
		K_ASCII_223 = 0x82,
		K_ASCII_224 = 0x83,
		K_ASCII_225 = 0x84,
		K_ASCII_228 = 0x85,
		K_ASCII_229 = 0x86,
		K_ASCII_230 = 0x87,
		K_ASCII_231 = 0x88,
		K_ASCII_232 = 0x89,
		K_ASCII_233 = 0x8A,
		K_ASCII_236 = 0x8B,
		K_ASCII_241 = 0x8C,
		K_ASCII_242 = 0x8D,
		K_ASCII_243 = 0x8E,
		K_ASCII_246 = 0x8F,
		K_ASCII_248 = 0x90,
		K_ASCII_249 = 0x91,
		K_ASCII_250 = 0x92,
		K_ASCII_252 = 0x93,
		K_END_ASCII_CHARS = 0x94,
		K_COMMAND = 0x96,
		K_CAPSLOCK = 0x97,
		K_POWER = 0x98,
		K_PAUSE = 0x99,
		K_UPARROW = 0x9A,
		K_DOWNARROW = 0x9B,
		K_LEFTARROW = 0x9C,
		K_RIGHTARROW = 0x9D,
		K_ALT = 0x9E,
		K_CTRL = 0x9F,
		K_SHIFT = 0xA0,
		K_INS = 0xA1,
		K_DEL = 0xA2,
		K_PGDN = 0xA3,
		K_PGUP = 0xA4,
		K_HOME = 0xA5,
		K_END = 0xA6,
		K_F1 = 0xA7,
		K_F2 = 0xA8,
		K_F3 = 0xA9,
		K_F4 = 0xAA,
		K_F5 = 0xAB,
		K_F6 = 0xAC,
		K_F7 = 0xAD,
		K_F8 = 0xAE,
		K_F9 = 0xAF,
		K_F10 = 0xB0,
		K_F11 = 0xB1,
		K_F12 = 0xB2,
		K_F13 = 0xB3,
		K_F14 = 0xB4,
		K_F15 = 0xB5,
		K_KP_HOME = 0xB6,
		K_KP_UPARROW = 0xB7,
		K_KP_PGUP = 0xB8,
		K_KP_LEFTARROW = 0xB9,
		K_KP_5 = 0xBA,
		K_KP_RIGHTARROW = 0xBB,
		K_KP_END = 0xBC,
		K_KP_DOWNARROW = 0xBD,
		K_KP_PGDN = 0xBE,
		K_KP_ENTER = 0xBF,
		K_KP_INS = 0xC0,
		K_KP_DEL = 0xC1,
		K_KP_SLASH = 0xC2,
		K_KP_MINUS = 0xC3,
		K_KP_PLUS = 0xC4,
		K_KP_NUMLOCK = 0xC5,
		K_KP_STAR = 0xC6,
		K_KP_EQUALS = 0xC7,
		K_MOUSE1 = 0xC8,
		K_MOUSE2 = 0xC9,
		K_MOUSE3 = 0xCA,
		K_MOUSE4 = 0xCB,
		K_MOUSE5 = 0xCC,
		K_MWHEELDOWN = 0xCD,
		K_MWHEELUP = 0xCE,
		K_AUX1 = 0xCF,
		K_AUX2 = 0xD0,
		K_AUX3 = 0xD1,
		K_AUX4 = 0xD2,
		K_AUX5 = 0xD3,
		K_AUX6 = 0xD4,
		K_AUX7 = 0xD5,
		K_AUX8 = 0xD6,
		K_AUX9 = 0xD7,
		K_AUX10 = 0xD8,
		K_AUX11 = 0xD9,
		K_AUX12 = 0xDA,
		K_AUX13 = 0xDB,
		K_AUX14 = 0xDC,
		K_AUX15 = 0xDD,
		K_AUX16 = 0xDE,
		K_LAST_KEY = 0xDF
	};

	struct KeyState
	{
		int down;
		int repeats;
		int binding;
	};

	struct PlayerKeyState
	{
		int overstrikeMode;
		int anyKeyDown;
		KeyState keys[256];
	};

	struct ScreenPlacement
	{
		vec2_t scaleVirtualToReal;
		vec2_t scaleVirtualToFull;
		vec2_t scaleRealToVirtual;
		vec2_t realViewportPosition;
		vec2_t realViewportSize;
		vec2_t virtualViewableMin;
		vec2_t virtualViewableMax;
		vec2_t realViewableMin;
		vec2_t realViewableMax;
		vec2_t virtualAdjustableMin;
		vec2_t virtualAdjustableMax;
		vec2_t realAdjustableMin;
		vec2_t realAdjustableMax;
		vec2_t subScreenLeft;
	};

	enum netadrtype_t
	{
		NA_BOT = 0x0,
		NA_BAD = 0x1,
		NA_LOOPBACK = 0x2,
		NA_BROADCAST = 0x3,
		NA_IP = 0x4,
	};

	enum netsrc_t
	{
		NS_CLIENT1 = 0x0,
		NS_MAXCLIENTS = 0x1,
		NS_SERVER = 0x2,
		NS_PACKET = 0x3,
		NS_INVALID_NETSRC = 0x4,
	};

	struct netadr_s
	{
		netadrtype_t type;
		unsigned char ip[4];
		unsigned __int16 port;
		netsrc_t localNetID;
		unsigned int addrHandleIndex;
	};

	struct msg_t
	{
		int overflowed;
		int readOnly;
		char* data;
		char* splitData;
		int maxsize;
		int cursize;
		int splitSize;
		int readcount;
		int bit;
		int lastEntityRef;
		netsrc_t targetLocalNetID;
		int useZlib;
	};

	enum errorParm
	{
		ERR_FATAL = 0,
		ERR_DROP = 1,
		ERR_SERVERDISCONNECT = 2,
		ERR_DISCONNECT = 3,
		ERR_SCRIPT = 4,
		ERR_SCRIPT_DROP = 5,
		ERR_LOCALIZATION = 6,
		ERR_MAPLOADERRORSUMMARY = 7,
	};

	struct CmdArgs
	{
		int nesting;
		int localClientNum[8];
		int controllerIndex[8];
		int argc[8];
		const char** argv[8];
	};

	struct CmdArgsPrivate
	{
		char textPool[8192];
		const char* argvPool[512];
		int usedTextPool[8];
		int totalUsedArgvPool;
		int totalUsedTextPool;
	};

	struct cmd_function_s
	{
		cmd_function_s* next;
		const char* name;
		void(__cdecl* function)();
	};

	enum DvarSetSource : std::uint32_t
	{
		DVAR_SOURCE_INTERNAL = 0x0,
		DVAR_SOURCE_EXTERNAL = 0x1,
		DVAR_SOURCE_SCRIPT = 0x2,
		DVAR_SOURCE_UISCRIPT = 0x3,
		DVAR_SOURCE_SERVERCMD = 0x4,
		DVAR_SOURCE_NUM = 0x5,
	};

	enum DvarFlags : std::uint32_t
	{
		DVAR_NOFLAG = 0,						// no flags
		DVAR_ARCHIVE = 0x1,						// will be saved to config_mp.cfg of the client
		DVAR_LATCH = 0x2,						// will only change when C code next does a Dvar_Get(), so it can't be changed
		DVAR_CHEAT = 0x4,						// can not be changed if cheats are disabled
		DVAR_CODINFO = 0x8,						// on change, this is sent to all clients (if you are host)
		DVAR_SCRIPTINFO = 0x10,					// (unused in client)
		DVAR_NETWORK = 0x18,					// NetConstStrings_InitNetworkDvars_Callback & DVAR_SOURCE_SERVERCMD, best guess
		DVAR_TEMP = 0x20,						// ^
		DVAR_SAVED = 0x40,						// ^ not to be confused with DVAR_ARCHIVE (old "save" flags for config)
		DVAR_INTERNAL = 0x80,					// ^
		DVAR_EXTERNAL = 0x100,					// created by set or setclientdvar
		DVAR_USERINFO = 0x200,					// (unused in client)
		DVAR_SERVERINFO = 0x400,				// ^
		DVAR_ROM = 0x800,						// display only, cannot be set by user at all
		DVAR_SYSTEMINFO = 0x1000,				// (unused in client)
		DVAR_INIT = 0x2000,						// don't allow change from console at all
		DVAR_CHANGEABLE_RESET = 0x4000,
		DVAR_AUTOEXEC = 0x8000,
		DVAR_UNADDABLE_FLAGS = DVAR_LATCH | DVAR_CHEAT | DVAR_EXTERNAL | DVAR_ROM | DVAR_INIT
	};

	enum dvar_type : std::int8_t
	{
		boolean = 0,
		boolean_hashed = 10,
		value = 1,
		value_hashed = 11,
		vec2 = 2,
		vec3 = 3,
		vec4 = 4,
		integer = 5,
		integer_hashed = 12,
		enumeration = 6,
		string = 7,
		color = 8,
		rgb = 9 // Color without alpha
	};

	union dvar_value
	{
		bool enabled;
		int integer;
		unsigned int unsignedInt;
		float value;
		float vector[4];
		const char* string;
		char color[4];
	};

	struct $A37BA207B3DDD6345C554D4661813EDD
	{
		int stringCount;
		const char* const* strings;
	};

	struct $9CA192F9DB66A3CB7E01DE78A0DEA53D
	{
		int min;
		int max;
	};

	struct $251C2428A496074035CACA7AAF3D55BD
	{
		float min;
		float max;
	};

	union dvar_limits
	{
		$A37BA207B3DDD6345C554D4661813EDD enumeration;
		$9CA192F9DB66A3CB7E01DE78A0DEA53D integer;
		$251C2428A496074035CACA7AAF3D55BD value;
		$251C2428A496074035CACA7AAF3D55BD vector;
	};

	struct dvar_t;
	
	struct dvar_t
	{
		int hash;
		DvarFlags flags;
		dvar_type type;
		bool modified;
		dvar_value current;
		dvar_value latched;
		dvar_value reset;
		dvar_limits domain;
		bool (__fastcall *domainFunc)(dvar_t*, dvar_value);
		dvar_t* hashNext;
	};

	static_assert(sizeof(dvar_t) == 96);

	enum connstate_t
	{
		CA_DISCONNECTED = 0x0,
		CA_CINEMATIC = 0x1,
		CA_LOGO = 0x2,
		CA_CONNECTING = 0x3,
		CA_CHALLENGING = 0x4,
		CA_CONNECTED = 0x5,
		CA_SENDINGSTATS = 0x6,
		CA_SYNCHRONIZING_DATA = 0x7,
		CA_LOADING = 0x8,
		CA_PRIMED = 0x9,
		CA_ACTIVE = 0xA,
	};

	enum svscmd_type
	{
		SV_CMD_CAN_IGNORE = 0x0,
		SV_CMD_RELIABLE = 0x1,
	};

	enum threadType
	{
		THREAD_CONTEXT_MAIN = 0x0,
		THREAD_CONTEXT_BACKEND = 0x1,
		THREAD_CONTEXT_WORKER0 = 0x2,
		THREAD_CONTEXT_WORKER1 = 0x3,
		THREAD_CONTEXT_WORKER2 = 0x4,
		THREAD_CONTEXT_WORKER3 = 0x5,
		THREAD_CONTEXT_WORKER4 = 0x6,
		THREAD_CONTEXT_WORKER5 = 0x7,
		THREAD_CONTEXT_WORKER6 = 0x8,
		THREAD_CONTEXT_WORKER7 = 0x9,
		THREAD_CONTEXT_SERVER = 0xA,
		THREAD_CONTEXT_TRACE_COUNT = 0xB,
		THREAD_CONTEXT_TRACE_LAST = 0xA,
		THREAD_CONTEXT_CINEMATIC = 0xB,
		THREAD_CONTEXT_DATABASE = 0xC,
		THREAD_CONTEXT_STREAM = 0xD,
		THREAD_CONTEXT_SNDSTREAMPACKETCALLBACK = 0xE,
		THREAD_CONTEXT_STATS_WRITE = 0xF,
		THREAD_CONTEXT_COUNT = 0x10,
	};

	enum GfxDrawSceneMethod
	{
		GFX_DRAW_SCENE_STANDARD = 0x0,
	};

	struct GfxDrawMethod_s
	{
		int drawScene;
		int baseTechType;
		int emissiveTechType;
		int forceTechType;
	};

	enum TestClientType
	{
		TC_NONE = 0x0,
		TC_TEST_CLIENT = 0x1,
		TC_BOT = 0x2,
		TC_COUNT = 0x3,
	};

	enum LiveClientDropType
	{
		SV_LIVE_DROP_NONE = 0x0,
		SV_LIVE_DROP_DISCONNECT = 0x1,
	};

	enum scriptType_e
	{
		SCRIPT_NONE = 0,
		SCRIPT_OBJECT = 1,
		SCRIPT_STRING = 2,
		SCRIPT_ISTRING = 3,
		SCRIPT_VECTOR = 4,
		SCRIPT_FLOAT = 5,
		SCRIPT_INTEGER = 6,
		SCRIPT_END = 8,
		SCRIPT_FUNCTION = 9,
		SCRIPT_STRUCT = 19,
		SCRIPT_ENTITY = 21,
		SCRIPT_ARRAY = 22,
	};

	struct Bounds
	{
		float midPoint[3];
		float halfSize[3];
	};

	struct rectDef_s
	{
		float x;
		float y;
		float w;
		float h;
		char horzAlign;
		char vertAlign;
	};

	// made up
	struct client_state_t
	{
		char __pad0[0x4A40];
		int ping;
		char __pad1[0x8];
		int num_players;
		char __pad2[48];
		int serverTime;
	};

	static_assert(offsetof(client_state_t, ping) == 0x4A40);
	static_assert(offsetof(client_state_t, num_players) == 0x4A4C);
	static_assert(offsetof(client_state_t, serverTime) == 0x4A80);

	// made up
	struct connect_state_t
	{
		char __pad0[0xC];
		netadr_s address;
	};

	enum PlayerHandIndex
	{
		WEAPON_HAND_DEFAULT = 0x0,
		WEAPON_HAND_RIGHT = 0x0,
		WEAPON_HAND_LEFT = 0x1,
		NUM_WEAPON_HANDS = 0x2,
	};

	union Weapon
	{
		unsigned int data;
	};

	struct map_t
	{
		const char* name;
		int id;
		int unk;
	};

	struct UISpawnPos
	{
		float allySpawnPos[2];
		float axisSpawnPos[2];
		float objectives[5][2];
	};

	struct mapInfo
	{
		char mapName[32];
		char mapLoadName[16];
		char mapDescription[32];
		char mapLoadImage[32];
		char mapCustomKey[32][16];
		char mapCustomValue[32][64];
		int mapCustomCount;
		char mapCamoTypes[2][16];
		int isAliensMap;
		int mapPack;
		int unk1;
		int gametype;
		char __pad0[132];
		UISpawnPos mapSpawnPos[32];
	};

	static_assert(sizeof(mapInfo) == 4648);

	struct ui_info
	{
		char __pad0[16];
		float cursor_x;
		float cursor_y;
		int cursor_time;
		int ingame_cursor_visible;
	};

	enum PMem_Direction
	{
		PHYS_ALLOC_LOW = 0x0,
		PHYS_ALLOC_HIGH = 0x1,
		PHYS_ALLOC_COUNT = 0x2,
	};

	enum PMem_Source
	{
		PMEM_SOURCE_EXTERNAL = 0x0,
		PMEM_SOURCE_DATABASE = 0x1,
		PMEM_SOURCE_DEFAULT_LOW = 0x2,
		PMEM_SOURCE_DEFAULT_HIGH = 0x3,
		PMEM_SOURCE_MOVIE = 0x4,
		PMEM_SOURCE_SCRIPT = 0x5,
		PMEM_SOURCE_UNK5 = 0x5,
		PMEM_SOURCE_UNK6 = 0x6,
		PMEM_SOURCE_UNK7 = 0x7,
		PMEM_SOURCE_UNK8 = 0x8,
		PMEM_SOURCE_CUSTOMIZATION = 0x9,
	};

	struct PhysicalMemoryAllocation
	{
		const char* name;
		char __pad0[16];
		unsigned __int64 pos;
		char __pad1[8];
	}; static_assert(sizeof(PhysicalMemoryAllocation) == 40);

	struct PhysicalMemoryPrim
	{
		const char* name;
		unsigned int allocListCount;
		char __pad0[4];
		unsigned char* buf;
		char __pad1[8];
		int unk1;
		char __pad2[4];
		unsigned __int64 pos;
		PhysicalMemoryAllocation allocList[32];
	}; static_assert(sizeof(PhysicalMemoryPrim) == 1328);

	struct PhysicalMemory
	{
		PhysicalMemoryPrim prim[2];
	}; static_assert(sizeof(PhysicalMemory) == 0xA60);

	struct cachedSnapshot_t
	{
		int archivedFrame;
		int time;
		int num_entities;
		int first_entity;
		int num_clients;
		int first_client;
		int num_agents;
		int first_agent;
		unsigned int scriptableCount;
		unsigned int scriptableFirstIndex;
		int usesDelta;
	};

	enum sessionState_t
	{
		SESS_STATE_PLAYING = 0x0,
		SESS_STATE_DEAD = 0x1,
		SESS_STATE_SPECTATOR = 0x2,
		SESS_STATE_INTERMISSION = 0x3,
	};

	enum team_t
	{
		TEAM_FREE		= 0x0,
		TEAM_BAD		= 0x0,
		TEAM_AXIS		= 0x1,
		TEAM_ALLIES		= 0x2,
		TEAM_SPECTATOR	= 0x3,
		TEAM_HOSTILE	= 0x4,
		TEAM_NEUTRAL	= 0x5,
		TEAM_NUM_TEAMS	= 0x6,
	};

	struct SprintState
	{
		int sprintButtonUpRequired;
		int sprintDelay;
		int lastSprintStart;
		int lastSprintEnd;
		int sprintStartMaxLength;
	};

	struct PlayerActiveWeaponState
	{
		int weapAnim;
		int weaponTime;
		int weaponDelay;
		int weaponRestrictKickTime;
		int weaponState;
		int weapHandFlags;
		unsigned int weaponShotCount;
	};

	struct GlobalAmmo
	{
		ammoindex_t ammoType;
		int ammoCount;
	};

	struct ClipAmmo
	{
		clipindex_t clipIndex;
		int ammoCount[2];
		int chargeAmmoCount[2];
	};

	struct PlayerWeaponAnimArrays
	{
		XAnimParts* normalAnimArray[190];
		XAnimParts* altAnimArray[190];
		XAnimParts* leftHandedAnimArray[190];
		XAnimParts* leftHandedAltAnimArray[190];
	};

	struct PlayerWeaponCommonState
	{
		Weapon offHand;
		Weapon lethalWeapon;
		Weapon tacticalWeapon;
		Weapon weapon;
		int weapFlags;
		float fWeaponPosFrac;
		float fPreviousWeaponPosFrac;
		float aimSpreadScale;
		int adsDelayTime;
		int spreadOverride;
		int spreadOverrideState;
		float fAimSpreadMovementScale;
		PlayerHandIndex lastWeaponHand;
		GlobalAmmo ammoNotInClip[15];
		ClipAmmo ammoInClip[15];
		int weapLockFlags;
		short weapLockedEntnum;
		float weapLockedPos[3];
		int weaponIdleTime;
		Weapon lastStowedWeapon;
		PlayerWeaponAnimArrays weaponAnimArrays;
	};

	static_assert(sizeof(PlayerWeaponCommonState) == 6704);
	static_assert(offsetof(PlayerWeaponCommonState, weapon) == 12);
	static_assert(offsetof(PlayerWeaponCommonState, lastWeaponHand) == 48);

	struct PlayerEquippedWeaponState
	{
		bool usedBefore;
		bool dualWielding;
		bool inAltMode;
		bool needsRechamber[2];
		int zoomLevelIndex;
		bool thermalEnabled;
		bool hybridScope;
	};

	struct $BEFE9800A1314C6B12F8C3CE18A08083
	{
		unsigned __int32 iHeadIcon : 4;
		unsigned __int32 iHeadIconTeam : 2;
		unsigned __int32 hudOutlineInfo : 5;
		unsigned __int32 padding : 21;
	};

	struct ClientOutlineData
	{
		unsigned int bits[4];
	};

	union HudData
	{
		$BEFE9800A1314C6B12F8C3CE18A08083 __s0;
		unsigned int data;
	};

	struct EntityEvent
	{
		int eventType;
		int eventParm;
	};

	struct playerEvents_t
	{
		int eventSequence;
		EntityEvent events[4];
		int oldEventSequence;
		//int timeADSCameUp;
	};

	enum ViewLockTypes : int32_t
	{
		PLAYERVIEWLOCK_NONE = 0x0,
		PLAYERVIEWLOCK_FULL = 0x1,
		PLAYERVIEWLOCK_WEAPONJITTER = 0x2,
		PLAYERVIEWLOCKCOUNT = 0x3,
	};

	struct compressedAnimData_s
	{
		int flags;
		int animRate;
		int distanceIn2D;
		int distanceOut2D;
		int distanceInZ;
		int distanceOutZ;
		int endScriptAnimTableIndex;
	};

	struct MantleState
	{
		float yaw;
		int startPitch;
		int transIndex;
		int flags;
		int startTime;
		float startPosition[3];
		compressedAnimData_s compressedAnimData;
	};

	enum he_type_t : int32_t
	{
		HE_TYPE_FREE = 0x0,
		HE_TYPE_TEXT = 0x1,
		HE_TYPE_VALUE = 0x2,
		HE_TYPE_PLAYERNAME = 0x3,
		HE_TYPE_MATERIAL = 0x4,
		HE_TYPE_TIMER_DOWN = 0x5,
		HE_TYPE_TIMER_UP = 0x6,
		HE_TYPE_TIMER_STATIC = 0x7,
		HE_TYPE_TENTHS_TIMER_DOWN = 0x8,
		HE_TYPE_TENTHS_TIMER_UP = 0x9,
		HE_TYPE_TENTHS_TIMER_STATIC = 0xA,
		HE_TYPE_CLOCK_DOWN = 0xB,
		HE_TYPE_CLOCK_UP = 0xC,
		HE_TYPE_WAYPOINT = 0xD,
		HE_TYPE_RADAR_PING = 0xE,
		HE_TYPE_RADAR_HIGHLIGHT = 0xF,
		HE_TYPE_SONARVISION = 0x10,
		HE_TYPE_HARMONIC_BREACH = 0x11,
		HE_TYPE_COUNT = 0x12,
	};

	struct $0D0CB43DF22755AD856C77DD3F304010
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	union hudelem_color_t
	{
		$0D0CB43DF22755AD856C77DD3F304010 __s0;
		int rgba;
	};

	struct hudelem_s
	{
		__int16 targetEntNum;
		__int16 triggerEntNum;
		int font;
		int alignOrg;
		int alignScreen;
		float x;
		float y;
		float z;
		he_type_t type;
		float fontScale;
		float fromFontScale;
		int fontScaleStartTime;
		int fontScaleTime;
		hudelem_color_t color;
		hudelem_color_t fromColor;
		int fadeStartTime;
		int fadeTime;
		int label;
		int width;
		int height;
		int materialIndex;
		int fromWidth;
		int fromHeight;
		int scaleStartTime;
		int scaleTime;
		float fromX;
		float fromY;
		int fromAlignOrg;
		int fromAlignScreen;
		int moveStartTime;
		int moveTime;
		int time;
		int duration;
		float value;
		int text;
		float sort;
		hudelem_color_t glowColor;
		int fxBirthTime;
		int fxLetterTime;
		int fxDecayStartTime;
		int fxDecayDuration;
		int soundID;
		int boneIndex;
		int flags;
		int unk;
		char unk2[16];
	};

	struct playerState_s_hud_struct
	{
		hudelem_s current[30];
		hudelem_s archival[15];
	};

	union OmnvarValue
	{
		bool enabled;
		int integer;
		unsigned int time;
		float value;
		unsigned int ncsString;
	};

	struct OmnvarData
	{
		unsigned int timeModified;
		OmnvarValue current;
	};

	enum OmnvarType : int32_t
	{
		OMNVAR_TYPE_INVALID = 0x0,
		OMNVAR_TYPE_BOOL = 0x1,
		OMNVAR_TYPE_FLOAT = 0x2,
		OMNVAR_TYPE_INT = 0x3,
		OMNVAR_TYPE_TIME = 0x4,
		OMNVAR_TYPE_NCS_LUI = 0x5,
	};

	struct OmnvarDef
	{
		const char* name;
		unsigned __int8 flags;
		OmnvarType type;
		OmnvarValue initial;
		const char* ncsString;
		int offset;
		int numbits;
		int minvalue;
		int maxvalue;
		int row;
	};

	enum ActionSlotType : int32_t
	{
		ACTIONSLOTTYPE_DONOTHING = 0x0,
		ACTIONSLOTTYPE_SPECIFYWEAPON = 0x1,
		ACTIONSLOTTYPE_ALTWEAPONTOGGLE = 0x2,
		ACTIONSLOTTYPE_NIGHTVISION = 0x3,
		ACTIONSLOTTYPECOUNT = 0x4,
	};

	struct ActionSlotParam_SpecifyWeapon
	{
		Weapon weapon;
	};

	struct ActionSlotParam
	{
		ActionSlotParam_SpecifyWeapon specifyWeapon;
	};

	enum DofPhysicalScriptingState : int32_t
	{
		DOF_PHYSICAL_SCRIPTING_DISABLED = 0x0,
		DOF_PHYSICAL_SCRIPTING_HIP = 0x1,
		DOF_PHYSICAL_SCRIPTING_ADS = 0x2,
		DOF_PHYSICAL_SCRIPTING_BOTH = 0x3,
		DOF_PHYSICAL_SCRIPTING_COUNT = 0x4,
	};

	enum objectiveState_t : int32_t
	{
		OBJST_EMPTY = 0x0,
		OBJST_ACTIVE = 0x1,
		OBJST_INVISIBLE = 0x2,
		OBJST_DONE = 0x3,
		OBJST_CURRENT = 0x4,
		OBJST_FAILED = 0x5,
		OBJST_NUMSTATES = 0x6,
	};

	enum ObjectiveVisMode : int32_t
	{
		OBJVIS_BYCLIENT = 0x0,
		OBJVIS_BYTEAM = 0x1,
		OBJVIS_BYMASK = 0x2,
	};

	union objective_t_visData_union
	{
		struct
		{
			char clientNum;
			int invert;
		} byClient;
		int teamNum;
		int clientMask;
	};

	struct objective_s
	{
		objectiveState_t state;
		__int16 entNum;
		__int16 icon;
		float origin[3];
		ObjectiveVisMode visMode;
		objective_t_visData_union visData;
		bool shouldRotate;
	};

	struct playerState_s
	{
		char clientNum;
		bool cursorHintDualWield;
		char pm_type;
		unsigned char remoteEyesTagname;
		unsigned char shellshockIndex;
		unsigned char damageEvent;
		unsigned char damageYaw;
		unsigned char damagePitch;
		unsigned char damageCount;
		unsigned char damageFlags;
		unsigned char cursorHint;
		unsigned char meleeChargeDist;
		unsigned char meleeServerResult;
		unsigned char laserIndex;
		char corpseIndex;
		bool radarMode;
		bool enemyRadarMode;
		unsigned char perkSlots[9];
		short remoteEyesEnt;
		short remoteControlEnt;
		short throwbackGrenadeOwner;
		short viewlocked_entNum;
		short groundEntityNum;
		short linkWeaponEnt;
		short cursorHintEntIndex;
		short meleeChargeEnt;
		short movingPlatformEntity;
		short groundRefEnt;
		unsigned short loopSound;
		short linkFlags;
		short remoteTurretEnt;
		short gravity;
		short speed;
		unsigned short shellshockFlashDuration;
		unsigned short shellshockMoveDuration;
		unsigned short viewmodelIndex;
		unsigned int cursorHintString;
		int meleeChargeTime;
		int shellshockTime;
		int commandTime;
		int pm_time;
		int pm_flags;
		int eFlags;
		int otherFlags;
		int foliageSoundTime;
		int grenadeTimeLeft;
		int throwbackGrenadeTimeLeft;
		//int varGrenadeSwitchTime;
		int jumpTime;
		float jumpOriginZ;
		float unk1;
		vec3_t origin;
		vec3_t velocity;
		vec3_t delta_angles;
		float vLadderVec[3];
		Weapon throwbackWeapon;
		Weapon cursorHintWeapon;
		int legsTimer;
		int legsAnim;
		int torsoTimer;
		int torsoAnim;
		int damageTimer;
		int damageDuration;
		int movementDir;
		int turnStartTime;
		int turnDirection;
		int turnRemaining;
		int flinch;
		playerEvents_t pe;
		int unpredictableEventSequence;
		int unpredictableEventSequenceOld;
		EntityEvent unpredictableEvents[4];
		vec3_t viewangles;
		int viewHeightTarget;
		float viewHeightCurrent;
		int viewHeightLerpTime;
		int viewHeightLerpTarget;
		int viewHeightLerpDown;
		float viewAngleClampBase[2];
		float viewAngleClampRange[2];
		int stats[4];
		float proneDirection;
		float proneDirectionPitch;
		float proneTorsoPitch;
		ViewLockTypes viewlocked;
		float linkAngles[3];
		float linkWeaponAngles[3];
		unsigned int cursorHintString2;
		unsigned int cursorHintString3;
		int iCompassPlayerInfo;
		int radarEnabled;
		int enemyRadarEnabled;
		int radarBlocked;
		int radarStrength;
		int radarShowEnemyDirection;
		//int sightedEnemyPlayersMask;
		int locationSelectionInfo;
		SprintState sprintState;
		float holdBreathScale;
		int holdBreathTimer;
		int stationaryZoomTimer;
		float stationaryZoomScale;
		float moveSpeedScaleMultiplier;
		float grenadeCookScale;
		MantleState mantleState;
		float leanf;
		PlayerActiveWeaponState weaponState[2];
		Weapon weaponsEquipped[15];
		PlayerEquippedWeaponState weapEquippedData[15];
		PlayerWeaponCommonState weapCommon;
		unsigned int perks[4];
		ActionSlotType actionSlotType[8];
		ActionSlotParam actionSlotParam[8];
		short weaponHudIconOverrides[10];
		float viewKickScale;
		//int chargeTimer;
		float dofNearStart;
		float dofNearEnd;
		float dofFarStart;
		float dofFarEnd;
		float dofNearBlur;
		float dofFarBlur;
		float dofViewmodelStart;
		float dofViewmodelEnd;
		DofPhysicalScriptingState dofPhysicalScriptingState;
		float dofPhysicalFstop;
		float dofPhysicalFocusDistance;
		float dofPhysicalFocusSpeed;
		float dofPhysicalApertureSpeed;
		float dofPhysicalViewModelFstop;
		float dofPhysicalViewModelFocusDistance;
		float dofPhysicalAdsStart;
		float dofPhysicalAdsEnd;
		objective_s objective[36];
		int deltaTime;
		short killCamEntity;
		short killCamLookAtEntity;
		char killCamClientNum;
		unsigned char recoilScale;
		playerState_s_hud_struct hud;
		OmnvarData rxvOmnvars[96];
		int partBits[8];
		int stunTime;
		int isLeftFoot;
		int proneFootstepTime;
		int proneStanceSoundTime;
		int duckedStanceSoundTime;
		int ammoGenerationTimer;
		// ClientOutlineData outlineData; // missing in 1.04
		HudData hudData;
		bool autoMantleEnabled;
		char __pad_end[3];
	};

	static_assert(sizeof(playerState_s) == 18560);

	static_assert(offsetof(playerState_s, pm_type) == 2);
	static_assert(offsetof(playerState_s, damageCount) == 8);
	static_assert(offsetof(playerState_s, groundEntityNum) == 34);
	static_assert(offsetof(playerState_s, linkFlags) == 48);
	static_assert(offsetof(playerState_s, gravity) == 52);
	static_assert(offsetof(playerState_s, pm_time) == 80);
	static_assert(offsetof(playerState_s, pm_flags) == 84);
	static_assert(offsetof(playerState_s, eFlags) == 88);
	static_assert(offsetof(playerState_s, otherFlags) == 92);
	static_assert(offsetof(playerState_s, origin) == 120);
	static_assert(offsetof(playerState_s, velocity) == 132);
	static_assert(offsetof(playerState_s, torsoAnim) == 188);
	static_assert(offsetof(playerState_s, viewangles) == 300);
	static_assert(offsetof(playerState_s, linkAngles) == 380);
	static_assert(offsetof(playerState_s, sprintState) == 440);
	static_assert(offsetof(playerState_s, moveSpeedScaleMultiplier) == 476);
	static_assert(offsetof(playerState_s, weaponState) == 548);
	static_assert(offsetof(playerState_s, weaponsEquipped) == 604);
	static_assert(offsetof(playerState_s, weapEquippedData) == 664);
	static_assert(offsetof(playerState_s, weapCommon) == 904);
	static_assert(offsetof(playerState_s, ammoGenerationTimer) == 18548);
	static_assert(offsetof(playerState_s, hudData) == 18552);

	struct usercmd_s
	{
		int serverTime;
		unsigned int buttons;
		int angles[3];
		Weapon weapon;
		Weapon offHand;
		char forwardmove;
		char rightmove;
		unsigned short airburstMarkDistance;
		unsigned short meleeChargeEnt;
		unsigned char meleeChargeDist;
		char selectedLoc[2];
		unsigned char selectedLocAngle;
		char remoteControlAngles[2];
		char remoteControlMove[3];
		unsigned int sightedClientsMask;
		unsigned short spawnTraceEntIndex;
		unsigned int sightedSpawnsMask[2];
		unsigned int partialSightedSpawnsMask;
	};

	enum clientConnected_t : int32_t
	{
		CON_DISCONNECTED = 0x0,
		CON_CONNECTING = 0x1,
		CON_CONNECTED = 0x2,
	};
	
	struct clientSession_t
	{
		sessionState_t sessionState;
		char forceSpectatorClient;
		int isInKillcam;
		short killCamEntity;
		short killCamLookAtEntity;
		int archiveTime;
		unsigned short scriptPersId;
		clientConnected_t connected;
		usercmd_s cmd;
		usercmd_s oldcmd;
		int localClient;
		char newnetname[32];
		int maxHealth;
		char __pad2[20];
		char clientIndex;
		char __pad6[6];
		team_t team;
		char __pad3[72];
		char name[32];
		char __pad4[118];
		bool autoMantleEnabled;
		char __pad5[61];
		int weaponHudIconOverrides[10];
		unsigned int unusableEntFlags[64];
		float spectateDefaultPos[3];
		float spectateDefaultAngles[3];
		float mlgCameraPos[4][3];
		float mlgCameraAngles[4][3];
		team_t mlgSpectatingTeam;
		float clanWarsXPModifier;
		OmnvarData* omnvars;
	};

	static_assert(sizeof(clientSession_t) == 944);
	static_assert(offsetof(clientSession_t, maxHealth) == 192);
	static_assert(offsetof(clientSession_t, spectateDefaultPos) == 808);

	struct AntilagAimHistoryFrame
	{
		float offset[3];
		float angleOffset;
	};

	struct viewClamp
	{
		float start[2];
		float current[2];
		float goal[2];
	};

	struct viewClampState
	{
		viewClamp min;
		viewClamp max;
		float accelTime;
		float decelTime;
		float totalTime;
		float startTime;
	};

	struct EntHandle
	{
		uint16_t number;
		uint16_t infoIndex;
	};

	enum hintType_t : int32_t
	{
		HINT_NONE = 0x0,
		HINT_NOICON = 0x1,
		HINT_ACTIVATE = 0x2,
		HINT_HEALTH = 0x3,
		HINT_FRIENDLY = 0x4,
		HINT_WEAPON = 0x5,
		HINT_NUM_HINTS = 0x6,
	};

	struct gclient_s
	{
		playerState_s ps;
		clientSession_t sess;
		int flags;
		char spectatorClient;
		char unk1;
		char cycleSpectatorClient;
		int cycleSpectatorTime;
		int lastCmdTime;
		int unk2;
		int buttons;
		int oldbuttons;
		int latched_buttons;
		int buttonsSinceLastFrame;
		float oldOrigin[3];
		float fGunPitch;
		float fGunYaw;
		AntilagAimHistoryFrame antilagAimHistory[8];
		int damage_blood;
		int damage_stun;
		float damage_from[3];
		int damage_fromWorld;
		int accurateCount;
		int accuracy_shots;
		int accuracy_hits;
		int inactivityTime;
		int inactivityWarning;
		int lastVoiceTime;
		int switchTeamTime;
		float currentAimSpreadScale;
		float prevLinkedInvQuat[4];
		bool prevLinkAnglesSet;
		bool link_rotationMovesEyePos;
		bool link_doCollision;
		bool link_useTagAnglesForViewAngles;
		bool link_useBaseAnglesForViewClamp;
		float linkAnglesFrac;
		viewClampState link_viewClamp;
		int portalID;
		int dropWeaponTime;
		int sniperRifleFiredTime;
		float sniperRifleMuzzleYaw;
		//int PCSpecialPickedUpCount;
		int unk4;
		EntHandle useHoldEntity;
		int useHoldTime;
		int useButtonDone;
		int iLastCompassPlayerInfoEnt;
		int compassPingTime;
		int damageTime;
		float v_dmg_roll;
		float v_dmg_pitch;
		float baseAngles[3];
		float baseOrigin[3];
		float swayViewAngles[3];
		float swayOffset[3];
		float swayAngles[3];
		float recoilAngles[3];
		float recoilSpeed[3];
		float fLastIdleFactor;
		int lastServerTime;
		Weapon lastWeapon;
		unsigned __int32 : 26;
		unsigned __int32 previouslySprintSliding : 1;
		unsigned __int32 previouslySprinting : 1;
		unsigned __int32 previouslyUsingNightVision : 1;
		unsigned __int32 previouslyFiringLeftHand : 1;
		unsigned __int32 previouslyFiring : 1;
		unsigned __int32 lastWeaponAltStatus : 1;
		int visionDuration[6];
		char visionName[6][64];
		int contactEntity;
		int lastStand;
		int lastStandTime;
		int hudElemLastAssignedSoundID;
		float lockedTargetOffset[3];
		Weapon attachShieldWeapon[2];
		hintType_t hintForcedType;
		unsigned int hintForcedString;
		float grenadeThrowSpeedScale;
		float grenadeThrowSpeedUpScale;
	};

	static_assert(sizeof(gclient_s) == 20448);

	enum trType_t : __int32
	{
		TR_STATIONARY = 0x0,
		TR_INTERPOLATE = 0x1,
		TR_LINEAR = 0x2,
		TR_LINEAR_STOP = 0x3,
		TR_SINE = 0x4,
		TR_GRAVITY = 0x5,
		TR_LOW_GRAVITY = 0x6,
		TR_ACCELERATE = 0x7,
		TR_DECELERATE = 0x8,
		TR_FIRST_PHYSICS = 0x9,
		TR_PHYSICS_SPAWN_BULLET = 0x9,
		TR_PHYSICS_SPAWN_IMPULSE = 0xA,
		TR_LAST_PHYSICS = 0xA,
		TR_ANIMATED_MOVER = 0xB,
		TR_FIRST_RAGDOLL = 0xC,
		TR_RAGDOLL = 0xC,
		TR_RAGDOLL_GRAVITY = 0xD,
		TR_RAGDOLL_INTERPOLATE = 0xE,
		TR_LAST_RAGDOLL = 0xE,
		NUM_TRTYPES = 0xF,
	};

	struct trajectory_t
	{
		trType_t trType;
		int trTime;
		int trDuration;
		vec3_t trBase;
		vec3_t trDelta;
	};

	struct agentState_s
	{
		int entityNum;
		char pad[112];
		team_t team;
		char pad2[44];
		int name;
	};

	struct gagent_s
	{
		playerState_s ps;
		agentState_s agentState;
		usercmd_s cmd;
		usercmd_s oldcmd;
		float lockedTargetOffset[3];
		float baseAngles[3];
		float baseOrigin[3];
		float swayViewAngles[3];
		float swayOffset[3];
		float swayAngles[3];
		float recoilAngles[3];
		float recoilSpeed[3];
		float lastIdleFactor;
		float currentAimSpreadScale;
		float gunPitch;
		float gunYaw;
		float damagePitch;
		float damageRoll;
		int damageTime;
		int damageBlood;
		int damageStun;
		float damageFrom[3];
		int damageFromWorld;
		int maxHealth;
		int lastServerTime;
		int flags;
		char agentGuid[7];
		AntilagAimHistoryFrame antilagAimHistory[8];
		Weapon attachShieldWeapon[2];
		int buttons;
		int oldbuttons;
		int latched_buttons;
		EntHandle useHoldEntity;
		int useHoldTime;
		int useButtonDone;
		unsigned short useEntsEnabled[2];
		Weapon lastWeapon;
		unsigned __int32 lastWeaponAltStatus : 1;
		unsigned __int32 previouslyFiring : 1;
		unsigned __int32 previouslyFiringLeftHand : 1;
		unsigned __int32 previouslyUsingNightVision : 1;
		unsigned __int32 previouslySprinting : 1;
		unsigned __int32 previouslySprintSliding : 1;
		float grenadeThrowSpeedScale;
		float grenadeThrowSpeedUpScale;
		int padding;
	};

	static_assert(sizeof(gagent_s) == 19208);

	enum ModelType : int8_t
	{
		MODELTYPE_CAPSULE = 0x0,
		MODELTYPE_CYLINDER = 0x1,
		MODELTYPE_DISK = 0x2,
		MODELTYPE_TRIGGER = 0x3,
		MODELTYPE_BRUSH = 0x4,
	};

	struct LerpEntityStateAnonymous
	{
		int data[8];
	};

	union LerpEntityStateTypeUnion
	{
		LerpEntityStateAnonymous anonymous;
	};

	struct LerpEntityState
	{
		int eFlags;
		trajectory_t pos;
		trajectory_t apos;
		LerpEntityStateTypeUnion u;
	};

	enum entityType_t : uint8_t
	{
		ET_GENERAL = 0x0,
		ET_PLAYER = 0x1,
		ET_PLAYER_CORPSE = 0x2,
		ET_ITEM = 0x3,
		ET_MISSILE = 0x4,
		ET_INVISIBLE = 0x5,
		ET_SCRIPTMOVER = 0x6,
		ET_SOUND_BLEND = 0x7,
		ET_FX = 0x8,
		ET_LOOP_FX = 0x9,
		ET_PRIMARY_LIGHT = 0xA,
		ET_TURRET = 0xB,
		ET_HELICOPTER = 0xC,
		ET_PLANE = 0xD,
		ET_VEHICLE = 0xE,
		ET_VEHICLE_COLLMAP = 0xF,
		ET_VEHICLE_CORPSE = 0x10,
		ET_VEHICLE_SPAWNER = 0x11,
		ET_AGENT = 0x12,
		ET_AGENT_CORPSE = 0x13,
		ET_EVENTS = 0x14,
	};

	struct animInfo_t
	{
		int animTime;
		unsigned int animData;
	};

	union entityState_s_un1_union
	{
		int eventParm2;
		unsigned __int8 characterFlags;
	};

	struct $6ED5A76A3BBCE2DDEE3C4B7BAC6D0D67
	{
		__int8 hintVisibleToOwnerOnly : 1;
		__int8 hintType : 7;
		unsigned __int8 hintString : 8;
		unsigned __int8 hintString2 : 8;
		unsigned __int8 hintString3 : 8;
	};

	union entityState_s_un2_union
	{
		$6ED5A76A3BBCE2DDEE3C4B7BAC6D0D67 __s0;
		int vehicleXModel;
		int actorFlags;
		float animRate;
	};

	struct entityState_s
	{
		uint16_t number;
		uint16_t otherEntityNum;
		uint16_t groundEntityNum;
		bool inAltWeaponMode;
		char pad;
		unsigned short loopSound;
		entityType_t eType;
		unsigned char surfType;
		char clientNum;
		unsigned char laserIndex;
		char _padding01[2];
		union
		{
			int brushModel;
			int triggerModel;
			int xmodel;
			int primaryLight;
		} index;
		HudData hudData;
		int time2;
		int unk;
		int solid;
		unsigned int eventParm;
		int eventSequence;
		EntityEvent events[4];
		Weapon weapon;
		animInfo_t animInfo;
		char _padding04[4];
		LerpEntityState lerp;
		char _padding05[4];
		entityState_s_un1_union un1;
		entityState_s_un2_union un2;
		int clientLinkInfo;
		int partBits[8];
		int clientMask[1];
		int threatMask[1];
	};

	static_assert(sizeof(entityState_s) == 256);

	struct entityShared_t
	{
		char isLinked;
		ModelType modelType;
		char svFlags;
		char isInUse;
		Bounds box;
		int contents;
		Bounds absBox;
		float currentOrigin[3];
		float currentAngles[3];
		EntHandle ownerNum;
		int eventTime;
	};

	struct item_ent_t
	{
		int ammoCount;
		int clipAmmoCount[2];
		Weapon weapon;
		int itemFlags;
	};

	struct spawner_ent_t
	{
		int team;
		int timestamp;
		int index;
	};

	struct trigger_ent_t
	{
		int threshold;
		int accumulate;
		int timestamp;
		int singleUserEntIndex;
		int damage;
		bool requireLookAt;
	};

	struct mover_position_points_t
	{
		float pos1[3];
		float pos2[3];
		float pos3[3];
	};

	struct mover_positions_t
	{
		float decelTime;
		float midTime;
		float speed;
		mover_position_points_t setup;
	};

	struct mover_slidedata_t
	{
		Bounds bounds;
		float velocity[3];
	};

	union $3A602DD4751FFC5F9B0F47F5CC5BB642
	{
		mover_positions_t pos;
		mover_slidedata_t slide;
	};

	struct mover_ent_t
	{
		$3A602DD4751FFC5F9B0F47F5CC5BB642 ___u0;
		mover_positions_t angle;
	};

	struct corpse_ent_t
	{
		int deathAnimStartTime;
	};

	struct missile_fields_grenade
	{
		float predictLandPos[3];
		int predictLandTime;
		float wobbleCycle;
		float curve;
	};

	enum MissileStage : int32_t
	{
		MISSILESTAGE_SOFTLAUNCH = 0x0,
		MISSILESTAGE_ASCENT = 0x1,
		MISSILESTAGE_DESCENT = 0x2,
	};

	struct missile_fields_nonGrenade
	{
		float curvature[3];
		float targetEntOffset[3];
		float targetPos[3];
		float launchOrigin[3];
		MissileStage stage;
	};

	union $53847FC87150DFC7FD0EBA7785353E10
	{
		missile_fields_grenade grenade;
		missile_fields_nonGrenade nonGrenade;
	};

	struct missile_ent_t
	{
		int timestamp;
		float time;
		int timeOfBirth;
		float travelDist;
		float surfaceNormal[3];
		team_t team;
		int flags;
		int antilagTimeOffset;
		int explodeCount;
		$53847FC87150DFC7FD0EBA7785353E10 ___u9;
	};

	struct blend_ent_t
	{
		float pos[3];
		float vel[3];
		float viewQuat[4];
		bool changed;
		float accelTime;
		float decelTime;
		float startTime;
		float totalTime;
	};

	union $1A5AFDD77C191B30F66652D606D5F59E
	{
		item_ent_t item[2];
		spawner_ent_t spawner;
		trigger_ent_t trigger;
		mover_ent_t mover;
		corpse_ent_t corpse;
		missile_ent_t missile;
		blend_ent_t blend;
	};

	struct gentity_s;
	struct tagInfo_s
	{
		gentity_s* parent;
		gentity_s* next;
		scr_string_t name;
		bool blendToParent;
		int index;
		float axis[4][3];
		float parentInvAxis[4][3];
	};

	struct gentity_s
	{
		entityState_s s;
		entityShared_t r;
		gclient_s* client;
		void* turret;
		gagent_s* agent;
		void* sentient;
		void* vehicle;
		__int64 physObjId;
		unsigned short model;
		unsigned char physicsObject;
		unsigned char takedamage;
		unsigned char active;
		unsigned char handler;
		unsigned char team;
		bool freeAfterEvent;
		unsigned short disconnectedLinks;
		scr_string_t classname;
		scr_string_t script_classname;
		scr_string_t script_linkName;
		scr_string_t target;
		scr_string_t targetname;
		unsigned int attachIgnoreCollision;
		int spawnflags;
		int flags;
		int eventTime;
		int clipmask;
		int processedFrame;
		EntHandle parent;
		EntHandle owner;
		EntHandle trackMovingPlatformEntity;
		int health;
		int maxHealth;
		char __pad6[4];
		$1A5AFDD77C191B30F66652D606D5F59E ___u33;
		EntHandle missileTargetEnt;
		EntHandle remoteControlledOwner;
		char __pad7[4];
		tagInfo_s* tagInfo;
		gentity_s* tagChildren;
		unsigned short attachModelNames[20];
		scr_string_t attachTagNames[16];
		unsigned int variants[4];
		int useCount;
		int birthTime;
		gentity_s* nextFree;
	};

	static_assert(sizeof(gentity_s) == 736);
	static_assert(offsetof(gentity_s, s.index) == 16);
	static_assert(offsetof(gentity_s, s.hudData) == 20);
	static_assert(offsetof(gentity_s, s.solid) == 32);
	static_assert(offsetof(gentity_s, s.lerp.eFlags) == 92);
	static_assert(offsetof(gentity_s, r.svFlags) == 258);
	static_assert(offsetof(gentity_s, model) == 392);
	static_assert(offsetof(gentity_s, attachModelNames) == 600);
	static_assert(offsetof(gentity_s, ___u33.missile.flags) == 504);

	struct snapshot_s
	{
		playerState_s ps;
	};

	enum CubemapShot : std::int32_t
	{
		CUBEMAPSHOT_NONE = 0x0,
		CUBEMAPSHOT_RIGHT = 0x1,
		CUBEMAPSHOT_LEFT = 0x2,
		CUBEMAPSHOT_BACK = 0x3,
		CUBEMAPSHOT_FRONT = 0x4,
		CUBEMAPSHOT_UP = 0x5,
		CUBEMAPSHOT_DOWN = 0x6,
		CUBEMAPSHOT_COUNT = 0x7,
	};

	struct GfxViewport
	{
		__int16 x;
		__int16 y;
		__int16 width;
		__int16 height;
	};

	struct RefdefView
	{
		float tanHalfFovX;
		float tanHalfFovY;
		float unk[2];
		float org[3];
		float axis[3][3];
	};

	struct refdef_t
	{
		GfxViewport displayViewport;
		int time;
		int frameTime;
		RefdefView view;
		float viewOffset[3];
		float viewOffsetPrev[3];
	};

	struct playerEntity_t
	{
		int bPositionToADS;
		float fLastIdleFactor;
		float baseMoveOrigin[3];
		float baseMoveAngles[3];
	};

	enum DemoType : std::int32_t
	{
		DEMO_TYPE_NONE = 0x0,
		DEMO_TYPE_CLIENT = 0x1,
		DEMO_TYPE_SERVER = 0x2,
	};

	struct cpose_t
	{
		unsigned __int16 lightingHandle;
		unsigned __int8 eType;
		unsigned __int8 cullIn;
		unsigned __int8 isRagdoll;
		int ragdollHandle;
		int killcamRagdollHandle;
		int physObjId;
		float origin[3];
		float angles[3];
		char __pad0[0x40];
		char __pad1[100];
	};

	struct XAnimParent
	{
		unsigned __int16 flags;
		unsigned __int16 children;
	};

	union $1A6660B292B883AB62F4E15A2C35B0BF
	{
		XAnimParts* parts;
		XAnimParent animParent;
	};

	struct XAnimEntry
	{
		unsigned __int16 numAnims;
		unsigned __int16 parent;
		$1A6660B292B883AB62F4E15A2C35B0BF ___u2;
	};

	struct XAnim_s
	{
		unsigned int size;
		XAnimEntry entries[1];
	};

	struct XAnimTree
	{
		XAnim_s* anims;
		unsigned __int16 children;
	};

	struct centity_s
	{
		cpose_t pose;
		LerpEntityState prevState;
		entityState_s nextState;
		int flags;
		unsigned __int8 tracerDrawRateCounter;
		unsigned __int8 weaponVisTestCounter;
		unsigned __int8 surfaceType;
		unsigned __int8 shotsFired;
		char prevClientNum;
		int previousEventSequence;
		int pickupPredictionTime;
		float lightingOrigin[3];
		XAnimTree* tree;
		centity_s* updateDelayedNext;
		int grenadeExplodeTime;
		int cloakTransitionStartTime;
		Weapon launcherWeapon;
	};

	static_assert(offsetof(centity_s, nextState.number) == 316);

	// 1.15 cg_s +0xE9A18
	struct ViewModelInfo
	{
		XModel* handModel;
		XModel* unk08;
		XModel* knifeModel;
		Weapon weapon;
		int unk1C;
		int numExtraModels;
		char __pad0[0x68];
		bool hideWeapon; // drops gun models from the viewmodel
	};
	static_assert(offsetof(ViewModelInfo, knifeModel) == 0x10);
	static_assert(offsetof(ViewModelInfo, weapon) == 0x18);
	static_assert(offsetof(ViewModelInfo, numExtraModels) == 0x20);
	static_assert(offsetof(ViewModelInfo, hideWeapon) == 0x8C);

	struct cg_s
	{
		playerState_s predictedPlayerState;
		centity_s* predictedPlayerEntity;
		playerEntity_t playerEntity;
		int predictedErrorTime;
		float predictedError[3];
		char clientNum;
		int localClientNum;
		DemoType demoType;
		CubemapShot cubemapShot;
		int cubemapSize;
		int hiResShotMode;
		int renderScreen;
		int latestSnapshotNum;
		int latestSnapshotTime;
		snapshot_s* snap;
		snapshot_s* nextSnap;
		char __pad1[582400];
		int spectatingThirdPerson;
		int renderingThirdPerson;
		char __pad5[24];
		refdef_t refdef;
		char __pad2[358452];
		int unk959660;
		char __pad3[16];
		int unk959680;
		char __pad4[136];
		char crosshairPreviousClientNum;
		char crosshairClientNum;
		unsigned int crosshairTraceDistance;
		int crosshairClientType;
	};
	
	static_assert(offsetof(cg_s, nextSnap) == 18664);
	//static_assert(offsetof(cg_s, time) == 151022);
	static_assert(offsetof(cg_s, renderingThirdPerson) == 601076);

	/*
	static_assert(offsetof(cg_s, cubemapShot) == 18644);
	static_assert(offsetof(cg_s, cubemapSize) == 18648);
	static_assert(offsetof(cg_s, nextSnap) == 18680);
	static_assert(offsetof(cg_s, unk_601088) == 601088);
	static_assert(offsetof(cg_s, renderingThirdPerson) == 601092);
	static_assert(offsetof(cg_s, refdef) == 601120);
	static_assert(offsetof(cg_s, unk_979676) == 979676);
	static_assert(offsetof(cg_s, unk_979696) == 979696);
	*/

	struct pmove_t
	{
		playerState_s* ps;
		usercmd_s cmd;
		usercmd_s oldcmd;
		int tracemask;
		int numtouch;
		int touchents[32];
		Bounds bounds;
		float speed;
		short contactEntity;
		int proneChange;
		bool mantleStarted;
		float mantleEndPos[3];
		int mantleDuration;
		float meleeEntOrigin[3];
		float meleeEntVelocity[3];
		int viewChangeTime;
		float viewChange;
		float fTorsoPitch;
		float fWaistPitch;
		int remoteTurretFireTime;
		int remoteTurretShotCount;
		int lastUpdateCMDServerTime;
		bool boostEventPending;
		unsigned int groundSurfaceType;
		unsigned char handler;
	};

	static_assert(offsetof(pmove_t, touchents) == 144);

	enum TraceHitType : std::int32_t
	{
		TRACE_HITTYPE_NONE = 0x0,
		TRACE_HITTYPE_ENTITY = 0x1,
		TRACE_HITTYPE_DYNENT_MODEL = 0x2,
		TRACE_HITTYPE_DYNENT_BRUSH = 0x3,
		TRACE_HITTYPE_GLASS = 0x4,
	};

	struct trace_t
	{
		float fraction;
		float normal[3];
		int surfaceFlags;
		int contents;
		TraceHitType hitType;
		unsigned __int16 hitId;
		unsigned __int16 unused;
		scr_string_t partName;
		unsigned __int16 modelIndex;
		unsigned __int16 partGroup;
		unsigned __int8 localBoneIndex;
		bool allsolid;
		bool startsolid;
		bool walkable;
		bool getPenetration;
		bool removePitchAndRollRotations;
	};
		
	struct pml_t
	{
		float forward[3];
		float right[3];
		float up[3];
		float frametime;
		int msec;
		int walking;
		int groundPlane;
		int almostGroundPlane;
		trace_t groundTrace;
		float impactSpeed;
		float previous_origin[3];
		float previous_velocity[3];
		float wishdir[3];
		unsigned int holdrand;
		float platformUp[3];
		int flinch;
		int turning;
		int airborne;
	};

	struct clientHeader_t
	{
		int state;
		char __pad0[44];
		netadr_s remoteAddress;
		char __pad1[264132];
	}; // size = 264200

	struct client_t
	{
		clientHeader_t header;
		const char* dropReason;
		char userinfo[1024];
		int reliableSequence;
		int reliableAcknowledge;
		char __pad1[265832];		// 265240
		gentity_s* gentity;			// 531072
		char name[32];				// 531080
		char __pad2[8];				// 531112
		int nextSnapshotTime; // 269024
		char __pad3[544];
		LiveClientDropType liveDropRequest; //269572
		char __pad4[24];
		TestClientType testClient; // 269600
		char __pad5[347912];
		int lastPacketTime; // 531112
	}; // size = 879616
	//lastPacketTime
	static_assert(sizeof(client_t) == 879616);
	static_assert(offsetof(client_t, dropReason) == 264200);
	static_assert(offsetof(client_t, reliableSequence) == 265232);

	struct DObj
	{
		char __pad0[15];
		unsigned char numModels;
		char __pad1[199];
		XModel* models;
	}; 
		
	static_assert(offsetof(DObj, models) == 216);

	struct Glyph
	{
		unsigned short letter;
		char x0;
		char y0;
		char dx;
		char pixelWidth;
		char pixelHeight;
		float s0;
		float t0;
		float s1;
		float t1;
	};
	
	struct Font_s
	{
		const char* fontName;
		int pixelHeight;
		TTFDef* ttfDef;
	};

	struct DB_AuthSignature
	{
		unsigned char bytes[256];
	};

	struct DB_AuthHash
	{
		unsigned char bytes[32];
	};

	struct XPakHeader
	{
		char header[8];
		std::int32_t version;
		unsigned char unknown[16];
		DB_AuthHash hash;
		DB_AuthSignature signature;
	};

	struct DBFile
	{
		char __pad0[32];
		char name[64];
	};

	namespace hks
	{
		struct lua_State;
		struct HashTable;
		struct cclosure;

		struct GenericChunkHeader
		{
			unsigned __int64 m_flags;
		};

		struct ChunkHeader : GenericChunkHeader
		{
			ChunkHeader* m_next;
		};

		struct UserData : ChunkHeader
		{
			unsigned __int64 m_envAndSizeOffsetHighBits;
			unsigned __int64 m_metaAndSizeOffsetLowBits;
			char m_data[8];
		};

		struct InternString
		{
			unsigned __int64 m_flags;
			unsigned __int64 m_lengthbits;
			unsigned int m_hash;
			char m_data[30];
		};

		union HksValue
		{
			cclosure* cClosure;
			void* closure;
			UserData* userData;
			HashTable* table;
			void* tstruct;
			InternString* str;
			void* thread;
			void* ptr;
			float number;
			long long i64;
			unsigned long long ui64;
			unsigned int native;
			bool boolean;
		};

		enum HksObjectType
		{
			TANY = 0xFFFFFFFE,
			TNONE = 0xFFFFFFFF,
			TNIL = 0x0,
			TBOOLEAN = 0x1,
			TLIGHTUSERDATA = 0x2,
			TNUMBER = 0x3,
			TSTRING = 0x4,
			TTABLE = 0x5,
			TFUNCTION = 0x6,  // idk
			TUSERDATA = 0x7,
			TTHREAD = 0x8,
			TIFUNCTION = 0x9, // Lua function
			TCFUNCTION = 0xA, // C function
			TUI64 = 0xB,
			TSTRUCT = 0xC,
			NUM_TYPE_OBJECTS = 0xE,
		};

		struct HksObject
		{
			HksObjectType t;
			HksValue v;
		};

		const struct hksInstruction
		{
			unsigned int code;
		};

		struct ActivationRecord
		{
			HksObject* m_base;
			const hksInstruction* m_returnAddress;
			__int16 m_tailCallDepth;
			__int16 m_numVarargs;
			int m_numExpectedReturns;
		};

		struct CallStack
		{
			ActivationRecord* m_records;
			ActivationRecord* m_lastrecord;
			ActivationRecord* m_current;
			const hksInstruction* m_current_lua_pc;
			const hksInstruction* m_hook_return_addr;
			int m_hook_level;
		};

		struct ApiStack
		{
			HksObject* top;
			HksObject* base;
			HksObject* alloc_top;
			HksObject* bottom;
		};

		struct UpValue : ChunkHeader
		{
			HksObject m_storage;
			HksObject* loc;
			UpValue* m_next;
		};

		struct CallSite
		{
			_SETJMP_FLOAT128 m_jumpBuffer[16];
			CallSite* m_prev;
		};

		enum Status
		{
			NEW = 0x1,
			RUNNING = 0x2,
			YIELDED = 0x3,
			DEAD_ERROR = 0x4,
		};

		enum HksError
		{
			HKS_NO_ERROR = 0x0,
			HKS_ERRSYNTAX = 0xFFFFFFFC,
			HKS_ERRFILE = 0xFFFFFFFB,
			HKS_ERRRUN = 0xFFFFFF9C,
			HKS_ERRMEM = 0xFFFFFF38,
			HKS_ERRERR = 0xFFFFFED4,
			HKS_THROWING_ERROR = 0xFFFFFE0C,
			HKS_GC_YIELD = 0x1,
		};

		struct lua_Debug
		{
			int event;
			const char* name;
			const char* namewhat;
			const char* what;
			const char* source;
			int currentline;
			int nups;
			int nparams;
			int ishksfunc;
			int linedefined;
			int lastlinedefined;
			char short_src[512];
			int callstack_level;
			int is_tail_call;
		};

		using lua_function = int(__fastcall*)(lua_State*);

		struct luaL_Reg
		{
			const char* name;
			lua_function function;
		};

		struct Node
		{
			HksObject m_key;
			HksObject m_value;
		};

		struct Metatable
		{
		};

		struct HashTable : ChunkHeader
		{
			Metatable* m_meta;
			unsigned int m_version;
			unsigned int m_mask;
			Node* m_hashPart;
			HksObject* m_arrayPart;
			unsigned int m_arraySize;
			Node* m_freeNode;
		};

		struct cclosure : ChunkHeader
		{
			lua_function m_function;
			HashTable* m_env;
			__int16 m_numUpvalues;
			__int16 m_flags;
			InternString* m_name;
			HksObject m_upvalues[1];
		};

		enum HksCompilerSettings_BytecodeSharingFormat
		{
			BYTECODE_DEFAULT = 0x0,
			BYTECODE_INPLACE = 0x1,
			BYTECODE_REFERENCED = 0x2,
		};

		enum HksCompilerSettings_IntLiteralOptions
		{
			INT_LITERALS_NONE = 0x0,
			INT_LITERALS_LUD = 0x1,
			INT_LITERALS_32BIT = 0x1,
			INT_LITERALS_UI64 = 0x2,
			INT_LITERALS_64BIT = 0x2,
			INT_LITERALS_ALL = 0x3,
		};

		struct HksCompilerSettings
		{
			int m_emitStructCode;
			const char** m_stripNames;
			int m_emitGlobalMemoization;
			int _m_isHksGlobalMemoTestingMode;
			HksCompilerSettings_BytecodeSharingFormat m_bytecodeSharingFormat;
			HksCompilerSettings_IntLiteralOptions m_enableIntLiterals;
			int(__fastcall* m_debugMap)(const char*, int);
		};

		enum HksBytecodeSharingMode
		{
			HKS_BYTECODE_SHARING_OFF = 0x0,
			HKS_BYTECODE_SHARING_ON = 0x1,
			HKS_BYTECODE_SHARING_SECURE = 0x2,
		};

		struct HksGcWeights
		{
			int m_removeString;
			int m_finalizeUserdataNoMM;
			int m_finalizeUserdataGcMM;
			int m_cleanCoroutine;
			int m_removeWeak;
			int m_markObject;
			int m_traverseString;
			int m_traverseUserdata;
			int m_traverseCoroutine;
			int m_traverseWeakTable;
			int m_freeChunk;
			int m_sweepTraverse;
		};

		struct GarbageCollector_Stack
		{
			void* m_storage;
			unsigned int m_numEntries;
			unsigned int m_numAllocated;
		};

		struct ProtoList
		{
			void** m_protoList;
			unsigned __int16 m_protoSize;
			unsigned __int16 m_protoAllocSize;
		};

		struct GarbageCollector
		{
			int m_target;
			int m_stepsLeft;
			int m_stepLimit;
			HksGcWeights m_costs;
			int m_unit;
			_SETJMP_FLOAT128(*m_jumpPoint)[16];
			lua_State* m_mainState;
			lua_State* m_finalizerState;
			void* m_memory;
			int m_phase;
			GarbageCollector_Stack m_resumeStack;
			GarbageCollector_Stack m_greyStack;
			GarbageCollector_Stack m_remarkStack;
			GarbageCollector_Stack m_weakStack;
			int m_finalizing;
			HksObject m_safeTableValue;
			lua_State* m_startOfStateStackList;
			lua_State* m_endOfStateStackList;
			lua_State* m_currentState;
			HksObject m_safeValue;
			void* m_compiler;
			void* m_bytecodeReader;
			void* m_bytecodeWriter;
			int m_pauseMultiplier;
			int m_stepMultiplier;
			bool m_stopped;
			int(__fastcall* m_gcPolicy)(lua_State*);
			unsigned __int64 m_pauseTriggerMemoryUsage;
			int m_stepTriggerCountdown;
			unsigned int m_stringTableIndex;
			unsigned int m_stringTableSize;
			UserData* m_lastBlackUD;
			UserData* m_activeUD;
		};

		enum MemoryManager_ChunkColor
		{
			RED = 0x0,
			BLACK = 0x1,
		};

		struct ChunkList
		{
			ChunkHeader m_prevToStart;
		};

		enum Hks_DeleteCheckingMode
		{
			HKS_DELETE_CHECKING_OFF = 0x0,
			HKS_DELETE_CHECKING_ACCURATE = 0x1,
			HKS_DELETE_CHECKING_SAFE = 0x2,
		};

		struct MemoryManager
		{
			void* (__fastcall* m_allocator)(void*, void*, unsigned __int64, unsigned __int64);
			void* m_allocatorUd;
			MemoryManager_ChunkColor m_chunkColor;
			unsigned __int64 m_used;
			unsigned __int64 m_highwatermark;
			ChunkList m_allocationList;
			ChunkList m_sweepList;
			ChunkHeader* m_lastKeptChunk;
			lua_State* m_state;
			ChunkList m_deletedList;
			int m_deleteMode;
			Hks_DeleteCheckingMode m_deleteCheckingMode;
		};

		struct StaticStringCache
		{
			HksObject m_objects[41];
		};

		enum HksBytecodeEndianness
		{
			HKS_BYTECODE_DEFAULT_ENDIAN = 0x0,
			HKS_BYTECODE_BIG_ENDIAN = 0x1,
			HKS_BYTECODE_LITTLE_ENDIAN = 0x2,
		};

		struct RuntimeProfileData_Stats
		{
			unsigned __int64 hksTime;
			unsigned __int64 callbackTime;
			unsigned __int64 gcTime;
			unsigned __int64 cFinalizerTime;
			unsigned __int64 compilerTime;
			unsigned int hkssTimeSamples;
			unsigned int callbackTimeSamples;
			unsigned int gcTimeSamples;
			unsigned int compilerTimeSamples;
			unsigned int num_newuserdata;
			unsigned int num_tablerehash;
			unsigned int num_pushstring;
			unsigned int num_pushcfunction;
			unsigned int num_newtables;
		};

		struct RuntimeProfileData
		{
			__int64 stackDepth;
			__int64 callbackDepth;
			unsigned __int64 lastTimer;
			RuntimeProfileData_Stats frameStats;
			unsigned __int64 gcStartTime;
			unsigned __int64 finalizerStartTime;
			unsigned __int64 compilerStartTime;
			unsigned __int64 compilerStartGCTime;
			unsigned __int64 compilerStartGCFinalizerTime;
			unsigned __int64 compilerCallbackStartTime;
			__int64 compilerDepth;
			void* outFile;
			lua_State* rootState;
		};

		struct HksGlobal
		{
			MemoryManager m_memory;
			GarbageCollector m_collector;
			StringTable m_stringTable;
			HksBytecodeSharingMode m_bytecodeSharingMode;
			unsigned int m_tableVersionInitializer;
			HksObject m_registry;
			ProtoList m_protoList;
			HashTable* m_structProtoByName;
			ChunkList m_userDataList;
			lua_State* m_root;
			StaticStringCache m_staticStringCache;
			void* m_debugger;
			void* m_profiler;
			RuntimeProfileData m_runProfilerData;
			HksCompilerSettings m_compilerSettings;
			int(__fastcall* m_panicFunction)(lua_State*);
			void* m_luaplusObjectList;
			int m_heapAssertionFrequency;
			int m_heapAssertionCount;
			void (*m_logFunction)(lua_State*, const char*, ...);
			HksBytecodeEndianness m_bytecodeDumpEndianness;
		};

		struct lua_State : ChunkHeader
		{
			HksGlobal* m_global;
			CallStack m_callStack;
			ApiStack m_apistack;
			UpValue* pending;
			HksObject globals;
			HksObject m_cEnv;
			CallSite* m_callsites;
			int m_numberOfCCalls;
			void* m_context;
			InternString* m_name;
			lua_State* m_nextState;
			lua_State* m_nextStateStack;
			Status m_status;
			HksError m_error;
		};
	}

	struct GfxPlacement
	{
		float quat[4];
		float origin[3];
	};

	struct GfxScaledPlacement
	{
		GfxPlacement base;
		float scale;
	};

	struct XModelDrawInfo
	{
		char hasGfxEntIndex;
		char lod;
		unsigned __int16 surfId;
	};
    
	struct GfxSceneModel
	{
	    XModelDrawInfo info;
		XModel* model;
		void* obj;
		GfxScaledPlacement placement;
		char _pad1[80];
	};

	struct __declspec(align(64)) GfxScene
	{
	    char _pad0[1544324];
		volatile int sceneModelCount;
		int sceneModelCountAtMark;
		int sceneDObjModelCount;
		GfxSceneModel sceneModel[2046];
	};

	enum CustomizationType
	{
		GENDER = 0,
		SHIRT,
		HEAD,
		GLOVES
	};

	// pulled from IW4
	struct DrawClipAmmoParams
	{
		Material* image;
	};

	struct __declspec(align(8)) TempPriority
	{
		void* threadHandle;
		int oldPriority;
	};

	struct FastCriticalSection
	{
		volatile int readCount;
		volatile int writeCount;
		TempPriority tempPriority;
	};

	struct TransientPoolSlot
	{
		unsigned __int64 usedSize[1];
		char parentPoolIndex;
		char nextFreeSlotIndex;
	};

	struct TransientPool
	{
		char name[64];				// 0
		unsigned int entrySize[1];	// 64
		unsigned __int64 numSlots;	// 72
		unsigned __int64 numFiles;	// 80
		unsigned __int8* poolMemStart[1];	// 88
		TransientPoolSlot* firstSlot;		// 96
		unsigned __int16 freeSlotIndex;		// 104
		//AssetSize* anchorSizes;
	};
	static_assert(offsetof(TransientPool, name) == 0, "name offset");
	static_assert(offsetof(TransientPool, entrySize) == 64, "entrySize offset");
	static_assert(offsetof(TransientPool, poolMemStart) == 88);
	static_assert(offsetof(TransientPool, firstSlot) == 96);

	struct LargeLocal
	{
		unsigned __int64 startPos;
		unsigned __int64 size;
	};

	enum playerOtherFlags_t
	{
		POF_INVULNERABLE = 0x1,
		POF_REMOTE_EYES = 0x2,
		POF_BALL_PASS_ALLOWED = 0x4,
		POF_THERMAL_VISION = 0x8,
		POF_THERMAL_VISION_OVERLAY_FOF = 0x10,
		POF_REMOTE_CAMERA_SOUNDS = 0x20,
		POF_ALT_SCENE_REAR_VIEW = 0x40,
		POF_ALT_SCENE_TAG_VIEW = 0x80,
		POF_SHIELD_ATTACHED_TO_WORLD_MODEL = 0x100,
		POF_DONT_LERP_VIEWANGLES = 0x200,
		POF_EMP_JAMMED_EQUIPMENT = 0x800,
		POF_LASTSTAND = 0x1000,
		POF_SHADOW_OFF = 0x2000,
		POF_FOLLOW = 0x4000,
		POF_PLAYER = 0x8000,
		POF_SPEC_ALLOW_CYCLE = 0x10000,
		POF_SPEC_ALLOW_FREELOOK = 0x20000,
		POF_SPEC_CYCLE_LOADING = 0x40000,
		POF_COMPASS_PING = 0x80000,
		POF_ADS_THIRD_PERSON_TOGGLE = 0x100000,
		POF_AUTOSPOT_OVERLAY = 0x200000,
		POF_REMOTE_TURRET = 0x400000,
		POF_KILLCAM_THERMAL_OFF = 0x800000,
		POF_AGENT = 0x1000000,
		POF_PLATFORM_PUSH = 0x2000000,
		POF_PLATFORM_ALTERNATE_COLLISION = 0x4000000,
		POF_COMPASS_EYES_ON = 0x8000000,
		POF_FOLLOW_FORCE_THIRD = 0x10000000,
		POF_FOLLOW_FORCE_FIRST = 0x20000000,
		POF_AC130 = 0x40000000,
		POF_VIEWMODEL_UFO = 0x80000000
	};

	enum pmtype_t
	{
		PM_NORMAL = 0x0,
		PM_NORMAL_LINKED = 0x1,
		PM_NOCLIP = 0x2,
		PM_UFO = 0x3,
		PM_SPECTATOR = 0x4,
		PM_INTERMISSION = 0x5,
		PM_LASTSTAND = 0x6,
		PM_DEAD = 0x7,
		PM_DEAD_LINKED = 0x8,
	};

	enum playerMoveFlags_t
	{
		PMF_PRONE = 0x1,
		PMF_DUCKED = 0x2,
		PMF_MANTLE = 0x4,
		PMF_LADDER = 0x8,
		PMF_SIGHT_AIMING = 0x10,
		PMF_BACKWARDS_RUN = 0x20,
		PMF_WALKING = 0x40,
		PMF_TIME_HARDLANDING = 0x80,
		PMF_TIME_KNOCKBACK = 0x100,
		PMF_PRONEMOVE_OVERRIDDEN = 0x200,
		PMF_RESPAWNED = 0x400,
		PMF_FROZEN = 0x800,
		PMF_LADDER_FALL = 0x1000,
		PMF_JUMPING = 0x2000,
		PMF_SPRINTING = 0x4000,
		PMF_SHELLSHOCKED = 0x8000,
		PMF_MELEE_CHARGE = 0x10000,
		PMF_NO_SPRINT = 0x20000,
		PMF_NO_JUMP = 0x40000,
		PMF_REMOTE_CONTROLLING = 0x80000,
		PMF_SLIDE = 0x100000, // reimplemented
		PMF_NO_STAND = 0x800000,
		PMF_NO_CROUCH = 0x1000000,
		PMF_NO_PRONE = 0x2000000,
		PMF_NO_LEAN = 0x4000000,
		PMF_NO_MELEE = 0x8000000,
		PMF_NO_FIRE = 0x10000000,
		PMF_NO_LADDER = 0x20000000,
		PMF_NO_MANTLE = 0x40000000,
		PMF_DIVE_TO_PRONE = 0x50000000, // reimplemented
	};

	enum weaponstate_t : std::uint32_t
	{
		WEAPON_READY = 0,
		WEAPON_RAISING = 1,
		WEAPON_RAISING_ALTSWITCH = 2,
		WEAPON_DROPPING = 3,
		WEAPON_DROPPING_QUICK = 4,
		WEAPON_DROPPING_ALT = 5,
		WEAPON_FIRING = 6,
		WEAPON_FIRING_BALL_PASS = 7,
		WEAPON_RECHAMBERING = 8,
		WEAPON_RELOADING = 9,
		WEAPON_RELOADING_INTERUPT = 10,
		WEAPON_RELOAD_START = 11,
		WEAPON_RELOAD_START_INTERUPT = 12,
		WEAPON_RELOAD_END = 13,
		WEAPON_MELEE_WAIT_FOR_RESULT = 14,
		WEAPON_MELEE_FIRE = 15,
		WEAPON_MELEE_END = 16,
		WEAPON_OFFHAND_INIT = 17,
		WEAPON_OFFHAND_PREPARE = 18,
		WEAPON_OFFHAND_HOLD = 19,
		WEAPON_OFFHAND_HOLD_PRIMED = 20,
		WEAPON_OFFHAND_FIRE = 21,
		WEAPON_OFFHAND_SWITCH = 22,
		WEAPON_OFFHAND_DETONATE = 23,
		WEAPON_OFFHAND_END = 24,
		WEAPON_DETONATING = 25,
		WEAPON_SPRINT_RAISE = 26,
		WEAPON_SPRINT_LOOP = 27,
		WEAPON_SPRINT_DROP = 28,
		WEAPON_STUNNED_START = 29,
		WEAPON_STUNNED_LOOP = 30,
		WEAPON_STUNNED_END = 31,
		WEAPON_NIGHTVISION_WEAR = 32,
		WEAPON_NIGHTVISION_REMOVE = 33,
		WEAPON_MANTLE_UP = 34,
		WEAPON_MANTLE_OVER = 35,
		WEAPON_BLAST_IMPACT = 36,
		WEAPON_HYBRID_SIGHT_IN = 37,
		WEAPON_HYBRID_SIGHT_OUT = 38,
		WEAPON_HEAT_COOLDOWN_START = 39,
		WEAPON_HEAT_COOLDOWN_END = 40,
		WEAPON_HEAT_COOLDOWN_READY = 41,
		WEAPON_OVERHEAT_END = 42,
		WEAPON_OVERHEAT_READY = 43,
		WEAPON_RIOTSHIELD_PREPARE = 44,
		WEAPON_RIOTSHIELD_HOLD = 45,
		WEAPON_RIOTSHIELD_START = 46,
		WEAPON_RIOTSHIELD_END = 47,
		WEAPON_INSPECTION_ANIM = 48,
		WEAPONSTATES_NUM = 49
	};

	enum weaponstate_t_extended
	{
		WEAPON_SLIDE_IN			= WEAPONSTATES_NUM + 1,
		WEAPON_SLIDE_LOOP		= WEAPONSTATES_NUM + 2,
		WEAPON_SLIDE_OUT		= WEAPONSTATES_NUM + 3,

		WEAPON_DTP_IN = WEAPONSTATES_NUM + 4,
		WEAPON_DTP_LOOP = WEAPONSTATES_NUM + 5,
		WEAPON_DTP_OUT = WEAPONSTATES_NUM + 6,
		
		WEAPON_HYBRID2_SIGHT_IN = WEAPONSTATES_NUM + 7,
                WEAPON_HYBRID2_SIGHT_OUT = WEAPONSTATES_NUM + 8,

		/*
		WEAPON_SLIDE_AND_RECHAMBER_IN	= WEAPON_SLIDE_OUT + 1,
		WEAPON_SLIDE_AND_RECHAMBER_LOOP = WEAPON_SLIDE_OUT + 2,
		WEAPON_SLIDE_AND_RECHAMBER_OUT	= WEAPON_SLIDE_OUT + 3,
		WEAPON_SLIDE_AND_FIRE_IN		= WEAPON_SLIDE_OUT + 4,
		WEAPON_SLIDE_AND_FIRE_LOOP		= WEAPON_SLIDE_OUT + 5,
		WEAPON_SLIDE_AND_FIRE_OUT		= WEAPON_SLIDE_OUT + 6
		*/
	};

	enum userbuttons_t
	{
		BUTTON_ATTACK = (1 << 0),
		BUTTON_SPRINT = (1 << 1),
		BUTTON_MELEE = (1 << 2),
		BUTTON_ACTIVATE = (1 << 3),
		BUTTON_RELOAD = (1 << 4),
		BUTTON_USE_RELOAD = (1 << 5),
		BUTTON_LEANLEFT = (1 << 6),
		BUTTON_LEANRIGHT = (1 << 7),
		BUTTON_PRONE = (1 << 8),
		BUTTON_CROUCH = (1 << 9),
		BUTTON_JUMP = (1 << 10),
		BUTTON_ADS = (1 << 11),
		BUTTON_TEMPSTANCE = (1 << 12),
		BUTTON_BREATH = (1 << 13),
		BUTTON_FRAG = (1 << 14),
		BUTTON_OFFHANDSECONDARY = (1 << 15),
		BUTTON_CONFIRM_LOCATION = (1 << 16),
		BUTTON_CANCEL_LOCATION = (1 << 17),
		BUTTON_NIGHTVISION = (1 << 18),
		BUTTON_THROW = (1 << 19),
		BUTTON_REMOTECONTROL = (1 << 20),
		BUTTON_UNK13 = (1 << 21), // ??
		BUTTON_UNK14 = (1 << 22), // ??
		BUTTON_UNK15 = (1 << 23), // ??
		BUTTON_UNK16 = (1 << 24), // ??
		BUTTON_UNK17 = (1 << 25), // ??
		BUTTON_UNK18 = (1 << 26), // ??
		BUTTON_UNK19 = (1 << 27), // ??
		BUTTON_UNK20 = (1 << 28), // ??
		BUTTON_SLIDE = (1 << 29),
		BUTTON_BIT_COUNT = 29
	};

	struct SlideState
	{
		int flags;
		int noFricTime;
		int startTime;
	};

	enum aistateEnum_t
	{
		AISTATE_COMBAT = 0x0,
		MAX_AISTATES = 0x1,
	};
	
	enum eventStates_t
	{
		EV_NONE,
		EV_FOLIAGE_SOUND,
		EV_STOP_WEAPON_SOUND,
		EV_SOUND_ALIAS,
		EV_SOUND_ALIAS_AS_MASTER,
		EV_SOUND_ALIAS_SET_PITCH,
		EV_SOUND_ALIAS_SCALE_PITCH,
		EV_SOUND_ALIAS_SET_VOLUME,
		EV_SOUND_ALIAS_SCALE_VOLUME,
		EV_STOPSOUND,
		EV_STOPSOUNDS,
		EV_STANCE_FORCE_STAND,
		EV_STANCE_FORCE_CROUCH,
		EV_STANCE_FORCE_PRONE,
		EV_STANCE_INVALID,
		EV_ITEM_PICKUP,
		EV_AMMO_PICKUP,
		EV_NOAMMO,
		EV_NOAMMO_FIREATTEMPT,
		EV_ADSREQUIRED_FIREATTEMPT,
		EV_EMPTY_OFFHAND_PRIMARY,
		EV_EMPTY_OFFHAND_SECONDARY,
		EV_EMP_OFFHAND,
		EV_OFFHAND_END_NOTIFY,
		EV_RESET_ADS,
		EV_RELOAD,
		EV_RELOAD_FROM_EMPTY,
		EV_RELOAD_START,
		EV_RELOAD_END,
		EV_RELOAD_START_NOTIFY,
		EV_RELOAD_ADDAMMO,
		EV_RAISE_WEAPON,
		EV_FIRST_RAISE_WEAPON,
		EV_PUTAWAY_WEAPON,
		EV_WEAPON_ALT,
		EV_WEAPON_SWITCH_STARTED,
		EV_WEAPON_SWITCH_STARTED_OFFHAND,
		EV_PULLBACK_WEAPON,
		EV_FIRE_WEAPON_END,
		EV_FIRE_WEAPON,
		EV_FIRE_WEAPON_LASTSHOT,
		EV_FIRE_RICOCHET,
		EV_RECHAMBER_WEAPON,
		EV_EJECT_BRASS,
		EV_FIRE_WEAPON_LEFT,
		EV_FIRE_WEAPON_LASTSHOT_LEFT,
		EV_EJECT_BRASS_LEFT,
		EV_HITCLIENT_FIRE_WEAPON,
		EV_HITCLIENT_FIRE_WEAPON_LASTSHOT,
		EV_HITCLIENT_FIRE_WEAPON_LEFT,
		EV_HITCLIENT_FIRE_WEAPON_LASTSHOT_LEFT,
		EV_SV_FIRE_WEAPON,
		EV_SV_FIRE_WEAPON_LASTSHOT,
		EV_SV_FIRE_WEAPON_LEFT,
		EV_SV_FIRE_WEAPON_LASTSHOT_LEFT,
		EV_SV_FIRE_WEAPON_BALL_PASS,
		EV_FIRE_MELEE_SWIPE,
		EV_FIRE_MELEE_STAB_START,
		EV_FIRE_MELEE_STAB_END,
		EV_PREP_OFFHAND,
		EV_USE_OFFHAND,
		EV_USE_OFFHAND_THROWBACK,
		EV_SWITCH_OFFHAND,
		EV_PREP_RIOTSHIELD,
		EV_LOWER_RIOTSHIELD,
		EV_STARTDEPLOY_RIOTSHIELD,
		EV_DEPLOY_RIOTSHIELD,
		EV_CANNOTPLANT,
		EV_MELEE_HIT,
		EV_MELEE_MISS,
		EV_MELEE_BLOOD,
		EV_SV_FIRE_TURRET,
		EV_FIRE_VEH_TURRET,
		EV_FIRE_TURRET,
		EV_FIRE_SENTRY,
		EV_FIRE_QUADBARREL_1,
		EV_FIRE_QUADBARREL_2,
		EV_BULLET_HIT,
		EV_BULLET_HIT_EXPLODE,
		EV_BULLET_HIT_SHIELD,
		EV_BULLET_HIT_CLIENT_SMALL,
		EV_BULLET_HIT_CLIENT_LARGE,
		EV_BULLET_HIT_CLIENT_EXPLODE,
		EV_BULLET_HIT_CLIENT_SMALL_FATAL,
		EV_BULLET_HIT_CLIENT_LARGE_FATAL,
		EV_BULLET_HIT_CLIENT_EXPLODE_FATAL,
		EV_BULLET_HIT_CLIENT_SHIELD,
		EV_BEAM_HIT,
		EV_BEAM_HIT_SHIELD,
		EV_BULLET_DESTROY_CLIENT_SHIELD,
		EV_DROP_CLIENT_SHIELD,
		EV_EXPLOSIVE_IMPACT_ON_SHIELD,
		EV_EXPLOSIVE_SPLASH_ON_SHIELD,
		EV_GRENADE_THROW,
		EV_GRENADE_BOUNCE,
		EV_GRENADE_STICK,
		EV_GRENADE_REST,
		EV_GRENADE_ROLL,
		EV_GRENADE_EXPLODE,
		EV_GRENADE_PICKUP,
		EV_GRENADE_LETGO,
		EV_ROCKET_EXPLODE,
		EV_TRAIL_DESTROY,
		EV_FLASHBANG_EXPLODE,
		EV_CUSTOM_EXPLODE,
		EV_CHANGE_TO_DUD,
		EV_DUD_EXPLODE,
		EV_DUD_EXPLODE_ONLY,
		EV_DUD_IMPACT,
		EV_TROPHY_EXPLODE,
		EV_BULLET,
		EV_PLAY_FX,
		EV_PLAY_FX_ON_TAG,
		EV_STOP_FX_ON_TAG,
		EV_KILL_FX_ON_TAG,
		EV_PLAY_FX_ON_TAG_FOR_CLIENTS,
		EV_STOP_FX_ON_TAG_FOR_CLIENT,
		EV_KILL_FX_ON_TAG_FOR_CLIENT,
		EV_PLAY_FX_ON_WEAPON,
		EV_STOP_FX_ON_WEAPON,
		EV_KILL_FX_ON_WEAPON,
		EV_PLAY_IMPACT_HEAD_FATAL_FX,
		EV_PHYS_EXPLOSION_SPHERE,
		EV_PHYS_EXPLOSION_CYLINDER,
		EV_PHYS_EXPLOSION_JOLT,
		EV_RADIUSDAMAGE,
		EV_PHYS_JITTER,
		EV_EARTHQUAKE,
		EV_GRENADE_SUICIDE,
		EV_DETONATE_BY_EMPTY_THROW,
		EV_DETONATE_BY_DOUBLE_TAP,
		EV_NIGHTVISION_WEAR,
		EV_NIGHTVISION_REMOVE,
		EV_PLAY_RUMBLE_ON_ENT,
		EV_PLAY_RUMBLE_ON_POS,
		EV_PLAY_RUMBLELOOP_ON_ENT,
		EV_PLAY_RUMBLELOOP_ON_POS,
		EV_STOP_RUMBLE,
		EV_STOP_ALL_RUMBLES,
		EV_VARIABLE_ZOOM_CHANGE,
		EV_OBITUARY,
		EV_NO_PRIMARY_GRENADE_HINT,
		EV_NO_SECONDARY_GRENADE_HINT,
		EV_TARGET_TOO_CLOSE_HINT,
		EV_TARGET_NOT_ENOUGH_CLEARANCE_HINT,
		EV_LOCKON_REQUIRED_HINT,
		EV_VEHICLE_COLLISION,
		EV_VEHICLE_SUSPENSION_SOFT,
		EV_VEHICLE_SUSPENSION_HARD,
		EV_FOOTSTEP_PRONE_ENTER,
		EV_FOOTSTEP_PRONE_ENTER_FROM_STAND,
		EV_FOOTSTEP_PRONE_EXIT,
		EV_FOOTSTEP_CROUCH_ENTER,
		EV_FOOTSTEP_CROUCH_EXIT,
		EV_FOOTSTEP_PRONE_STOP,
		EV_FOOTSTEP_PRONE,
		EV_FOOTSTEP_CROUCH_WALK,
		EV_FOOTSTEP_CROUCH_RUN,
		EV_FOOTSTEP_WALK,
		EV_FOOTSTEP_RUN,
		EV_FOOTSTEP_SPRINT,
		EV_JUMP,
		EV_LANDING_LIGHT,
		EV_LANDING_MEDIUM,
		EV_LANDING_HEAVY,
		EV_LANDING_PAIN,
		EV_MANTLE_UP_HIGH,
		EV_MANTLE_UP_MEDIUM,
		EV_MANTLE_UP_LOW,
		EV_MANTLE_OVER_HIGH,
		EV_MANTLE_OVER_MEDIUM,
		EV_MANTLE_OVER_LOW,
		EV_EXPLODER,
		EV_STOP_EXPLODER,
		EV_KILL_EXPLODER,
		EV_PHYSICS_IMPACT_SOUND,
		EV_PORTABLE_RADAR_PING,
		EV_SHOOT_BLANK,
		EV_SOUND_FOCUS,
		EV_PLAY_FX_ON_TAG_OBJ_SPACE,
		EV_PHYS_APPLY_ACCELERATION,
		EV_PHYS_APPLY_IMPULSE,
		EV_MISSILE_PENETRATE_SCRIPTABLE_GLASS,
	};

	enum weapAnimFiles_t : std::int32_t
	{
		WEAP_ANIM_INVALID = -1,
		WEAP_ANIM_ROOT = 0,
		WEAP_ANIM_IDLE = 1,
		WEAP_ANIM_EMPTY_IDLE = 2,
		WEAP_ANIM_FIRE = 3,
		WEAP_ANIM_HOLD_FIRE = 4,
		WEAP_ANIM_LASTSHOT = 5,
		WEAP_ANIM_RECHAMBER = 6,
		WEAP_ANIM_GRENADE_PRIME = 7,
		WEAP_ANIM_GRENADE_PRIME_READY_TO_THROW = 8,
		WEAP_ANIM_MELEE_SWIPE = 9,
		WEAP_ANIM_MELEE_HIT = 10,
		WEAP_ANIM_MELEE_FATAL = 11,
		WEAP_ANIM_MELEE_MISS = 12,
		WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT = 13,
		WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL = 14,
		WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS = 15,
		WEAP_ANIM_MELEE_ALT_STANDING = 16,
		WEAP_ANIM_MELEE_ALT_CROUCHING = 17,
		WEAP_ANIM_MELEE_ALT_PRONE = 18,
		WEAP_ANIM_MELEE_ALT_JUMPING = 19,
		WEAP_ANIM_MELEE_ALT_STANDING_VICTIM_CROUCHING = 20,
		WEAP_ANIM_MELEE_ALT_CROUCHING_VICTIM_CROUCHING = 21,
		WEAP_ANIM_MELEE_ALT_PRONE_VICTIM_CROUCHING = 22,
		WEAP_ANIM_MELEE_ALT_JUMPING_VICTIM_CROUCHING = 23,
		WEAP_ANIM_RELOAD = 24,
		WEAP_ANIM_RELOAD_EMPTY = 25,
		WEAP_ANIM_RELOAD_START = 26,
		WEAP_ANIM_RELOAD_END = 27,
		WEAP_ANIM_FAST_RELOAD = 28,
		WEAP_ANIM_FAST_RELOAD_EMPTY = 29,
		WEAP_ANIM_FAST_RELOAD_START = 30,
		WEAP_ANIM_FAST_RELOAD_END = 31,
		WEAP_ANIM_DUALMAG_RELOAD = 32,
		WEAP_ANIM_DUALMAG_RELOAD_EMPTY = 33,
		WEAP_ANIM_SPEED_RELOAD = 34,
		WEAP_ANIM_RELOAD_FROM_ALT = 35,
		WEAP_ANIM_RAISE = 36,
		WEAP_ANIM_FIRST_RAISE = 37,
		WEAP_ANIM_BREACH_RAISE = 38,
		WEAP_ANIM_DROP = 39,
		WEAP_ANIM_ALT_RAISE = 40,
		WEAP_ANIM_ALT_DROP = 41,
		WEAP_ANIM_ALT_OVERRIDE = 42,
		WEAP_ANIM_QUICK_RAISE = 43,
		WEAP_ANIM_QUICK_DROP = 44,
		WEAP_ANIM_EMPTY_RAISE = 45,
		WEAP_ANIM_EMPTY_DROP = 46,
		WEAP_ANIM_HYBRID_SIGHT_ON = 47,
		WEAP_ANIM_HYBRID_SIGHT_OFF = 48,
		WEAP_ANIM_SPRINT_IN = 49,
		WEAP_ANIM_SPRINT_IN_FROM_SLIDE = 50,
		WEAP_ANIM_SPRINT_IN_CANCEL = 51,
		WEAP_ANIM_SPRINT_LOOP = 52,
		WEAP_ANIM_SPRINT_OUT = 53,
		WEAP_ANIM_SPRINTANDFIRE_IN = 54,
		WEAP_ANIM_SPRINTANDFIRE_LOOP = 55,
		WEAP_ANIM_SPRINTANDFIRE_OUT = 56,
		WEAP_ANIM_STUNNED_START = 57,
		WEAP_ANIM_STUNNED_LOOP = 58,
		WEAP_ANIM_STUNNED_END = 59,
		WEAP_ANIM_THROWBACK = 60,
		WEAP_ANIM_DETONATE = 61,
		WEAP_ANIM_NIGHTVISION_WEAR = 62,
		WEAP_ANIM_NIGHTVISION_REMOVE = 63,
		WEAP_ANIM_ADS_FIRE = 64,
		WEAP_ANIM_ADS_LASTSHOT = 65,
		WEAP_ANIM_ADS_RECHAMBER = 66,
		WEAP_ANIM_BLAST_FRONT = 67,
		WEAP_ANIM_BLAST_RIGHT = 68,
		WEAP_ANIM_BLAST_BACK = 69,
		WEAP_ANIM_BLAST_LEFT = 70,
		WEAP_ANIM_SLIDE_IN = 71,
		WEAP_ANIM_SLIDE_LOOP = 72,
		WEAP_ANIM_SLIDE_OUT_TO_SPRINT = 73,
		WEAP_ANIM_SLIDE_OUT = 74,
		WEAP_ANIM_SLIDE_AND_FIRE_IN = 75,
		WEAP_ANIM_SLIDE_AND_FIRE_LOOP = 76,
		WEAP_ANIM_SLIDE_AND_FIRE_OUT = 77,
		WEAP_ANIM_HIGH_JUMP_IN = 78,
		WEAP_ANIM_HIGH_JUMP_DROP_IN = 79,
		WEAP_ANIM_HIGH_JUMP_DROP_LOOP = 80,
		WEAP_ANIM_HIGH_JUMP_DROP_LAND = 81,
		WEAP_ANIM_DODGE_GROUND_BACK = 82,
		WEAP_ANIM_DODGE_GROUND_LEFT = 83,
		WEAP_ANIM_DODGE_GROUND_RIGHT = 84,
		WEAP_ANIM_DODGE_AIR_FORWARD = 85,
		WEAP_ANIM_DODGE_AIR_BACK = 86,
		WEAP_ANIM_DODGE_AIR_LEFT = 87,
		WEAP_ANIM_DODGE_AIR_RIGHT = 88,
		WEAP_ANIM_LAND_DIP = 89,
		WEAP_ANIM_RECOIL_SETTLE = 90,
		WEAP_ANIM_SWIM_LOOP = 91,
		WEAP_ANIM_MANTLE_UP_64 = 92,
		WEAP_ANIM_MANTLE_UP_56 = 93,
		WEAP_ANIM_MANTLE_UP_48 = 94,
		WEAP_ANIM_MANTLE_UP_40 = 95,
		WEAP_ANIM_MANTLE_UP_32 = 96,
		WEAP_ANIM_MANTLE_UP_24 = 97,
		WEAP_ANIM_MANTLE_UP_16 = 98,
		WEAP_ANIM_MANTLE_OVER_64 = 99,
		WEAP_ANIM_MANTLE_OVER_56 = 100,
		WEAP_ANIM_MANTLE_OVER_48 = 101,
		WEAP_ANIM_MANTLE_OVER_40 = 102,
		WEAP_ANIM_MANTLE_OVER_32 = 103,
		WEAP_ANIM_MANTLE_OVER_24 = 104,
		WEAP_ANIM_MANTLE_OVER_16 = 105,
		WEAP_ANIM_GOLIATH_ENTRY = 106,
		WEAP_ANIM_OFFHAND_SWITCH = 107,
		WEAP_ANIM_HEAT_COOLDOWN_IN = 108,
		WEAP_ANIM_HEAT_COOLDOWN_OUT = 109,
		WEAP_ANIM_OVERHEAT_OUT = 110,
		WEAP_ANIM_SCRIPTED = 111,
		WEAP_ANIM_INSPECTION = 112,
		WEAP_ANIM_RELOAD_MULTIPLE_1 = 113,
		WEAP_ANIM_RELOAD_MULTIPLE_2 = 114,
		WEAP_ANIM_RELOAD_MULTIPLE_3 = 115,
		WEAP_ANIM_RELOAD_MULTIPLE_4 = 116,
		WEAP_ANIM_RELOAD_MULTIPLE_5 = 117,
		WEAP_ANIM_RELOAD_MULTIPLE_6 = 118,
		WEAP_ANIM_RELOAD_MULTIPLE_7 = 119,
		WEAP_ANIM_RELOAD_MULTIPLE_8 = 120,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_1 = 121,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_2 = 122,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_3 = 123,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_4 = 124,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_5 = 125,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_6 = 126,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_7 = 127,
		WEAP_ANIM_RELOAD_MULTIPLE_FAST_8 = 128,
		WEAP_ANIM_ADS_UP = 129,
		WEAP_ANIM_ADS_DOWN = 130,
		WEAP_ANIM_RECOIL = 131,
		WEAP_ALT_ANIM_ADJUST = 132,
		WEAP_ANIM_ADDITIVE_ADS_ROOT = 133,
		WEAP_ANIM_ADDITIVE_ADS_UP = 134,
		WEAP_ANIM_ADDITIVE_HYBRID_SIGHT_UP_ROOT = 135,
		WEAP_ANIM_ADDITIVE_HYBRID_SIGHT_UP = 136,
		WEAP_ANIM_ADDITIVE_DRAG_LEFT_ROOT = 137,
		WEAP_ANIM_ADDITIVE_DRAG_LEFT = 138,
		WEAP_ANIM_ADDITIVE_DRAG_RIGHT_ROOT = 139,
		WEAP_ANIM_ADDITIVE_DRAG_RIGHT = 140,
		WEAP_ANIM_ADDITIVE_DRAG_UP_ROOT = 141,
		WEAP_ANIM_ADDITIVE_DRAG_UP = 142,
		WEAP_ANIM_ADDITIVE_DRAG_DOWN_ROOT = 143,
		WEAP_ANIM_ADDITIVE_DRAG_DOWN = 144,
		WEAP_ANIM_ADDITIVE_SWIM_FORWARD_ROOT = 145,
		WEAP_ANIM_ADDITIVE_SWIM_FORWARD = 146,
		WEAP_ANIM_ADDITIVE_SWIM_BACKWARD_ROOT = 147,
		WEAP_ANIM_ADDITIVE_SWIM_BACKWARD = 148,
		WEAP_ANIM_ADDITIVE_JUMP_ROOT = 149,
		WEAP_ANIM_ADDITIVE_JUMP = 150,
		WEAP_ANIM_ADDITIVE_JUMP_BOOST = 151,
		WEAP_ANIM_ADDITIVE_JUMP_LAND_ROOT = 152,
		WEAP_ANIM_ADDITIVE_JUMP_LAND = 153,
		WEAP_ANIM_ADDITIVE_JUMP_LAND_HEAVY = 154,
		WEAP_ANIM_ADDITIVE_WALK_ROOT = 155,
		WEAP_ANIM_ADDITIVE_WALK = 156,
		WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP_ROOT = 157,
		WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP = 158,
		WEAP_ANIM_ADDITIVE_OVERHEAT_IN_ROOT = 159, // GUN_DOWN_ROOT
		WEAP_ANIM_ADDITIVE_OVERHEAT_IN = 160, // GUN_DOWN
		WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP_ROOT = 161,
		WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP = 162,
		WEAP_ANIM_ADDITIVE_CRAWL_IN_ROOT = 163,
		WEAP_ANIM_ADDITIVE_CRAWL_IN = 164,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_ROOT = 165,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP = 166,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_BACK_ROOT = 167,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_BACK = 168,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT_ROOT = 169,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT = 170,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT_ROOT = 171,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT = 172,
		WEAP_ANIM_ADDITIVE_PRONE_DROP_ROOT = 173,
		WEAP_ANIM_ADDITIVE_PRONE_DROP = 174,
		WEAP_ANIM_ADDITIVE_EMPTY_ROOT = 175,
		WEAP_ANIM_ADDITIVE_EMPTY = 176,
		WEAP_ANIM_ADDITIVE_MANTLE_ROOT = 177,
		WEAP_ANIM_ADDITIVE_MANTLE = 178,
		WEAP_ANIM_ADDITIVE_LOW_MANTLE_ROOT = 179,
		WEAP_ANIM_ADDITIVE_MANTLE_UP_24 = 180,
		WEAP_ANIM_ADDITIVE_MANTLE_UP_16 = 181,
		WEAP_ANIM_ADDITIVE_MANTLE_OVER_24 = 182,
		WEAP_ANIM_ADDITIVE_MANTLE_OVER_16 = 183,
		WEAP_ANIM_ADDITIVE_SHOT_CHARGE_IN_ROOT = 184,
		WEAP_ANIM_ADDITIVE_SHOT_CHARGE_IN = 185,
		WEAP_ANIM_ADDITIVE_SHOT_CHARGE_LOOP_ROOT = 186,
		WEAP_ANIM_ADDITIVE_SHOT_CHARGE_LOOP = 187,
		WEAP_ANIM_ADDITIVE_SCRIPTED_ROOT = 188,
		WEAP_ANIM_ADDITIVE_SCRIPTED = 189,
		NUM_WEAP_ANIMS = 190,
	};
  
	enum StanceState : __int32
	{
		CL_STANCE_STAND = 0x0,
		CL_STANCE_CROUCH = 0x1,
		CL_STANCE_PRONE = 0x2,
		CL_STANCE_SLIDE = 0x3,
	};

	enum WeaponAnimNumber : std::int32_t
	{
		WEAP_IDLE = 0,
		WEAP_FORCE_IDLE = 1,
		WEAP_ATTACK = 2,
		WEAP_ATTACK_LASTSHOT = 3,
		WEAP_RECHAMBER = 4,
		WEAP_ADS_ATTACK = 5,
		WEAP_ADS_ATTACK_LASTSHOT = 6,
		WEAP_ADS_RECHAMBER = 7,
		WEAP_GRENADE_PRIME = 8,
		WEAP_GRENADE_PRIME_READY_TO_THROW = 9,
		WEAP_MELEE_SWIPE = 10,
		WEAP_MELEE_HIT = 11,
		WEAP_MELEE_FATAL = 12,
		WEAP_MELEE_MISS = 13,
		WEAP_MELEE_VICTIM_CROUCHING_HIT = 14,
		WEAP_MELEE_VICTIM_CROUCHING_FATAL = 15,
		WEAP_MELEE_VICTIM_CROUCHING_MISS = 16,
		WEAP_DROP = 17,
		WEAP_RAISE = 18,
		WEAP_FIRST_RAISE = 19,
		WEAP_RELOAD = 20,
		WEAP_RELOAD_EMPTY = 21,
		WEAP_RELOAD_START = 22,
		WEAP_RELOAD_END = 23,
		WEAP_RELOAD_ALT = 24,
		WEAP_ALTSWITCHFROM = 25,
		WEAP_ALTSWITCHTO = 26,
		WEAP_QUICK_DROP = 27,
		WEAP_QUICK_RAISE = 28,
		WEAP_EMPTY_DROP = 29,
		WEAP_EMPTY_RAISE = 30,
		WEAP_SPRINT_IN = 31,
		WEAP_SPRINT_IN_CANCEL = 32,
		WEAP_SPRINT_LOOP = 33,
		WEAP_SPRINT_OUT = 34,
		WEAP_STUNNED_START = 35,
		WEAP_STUNNED_LOOP = 36,
		WEAP_STUNNED_END = 37,
		WEAP_HOLD_FIRE = 38,
		WEAP_THROWBACK = 39,
		WEAP_DETONATE = 40,
		WEAP_NIGHTVISION_WEAR = 41,
		WEAP_NIGHTVISION_REMOVE = 42,
		WEAP_MANTLE_UP_64 = 43,
		WEAP_MANTLE_UP_56 = 44,
		WEAP_MANTLE_UP_48 = 45,
		WEAP_MANTLE_UP_40 = 46,
		WEAP_MANTLE_UP_32 = 47,
		WEAP_MANTLE_UP_24 = 48,
		WEAP_MANTLE_UP_16 = 49,
		WEAP_MANTLE_OVER_64 = 50,
		WEAP_MANTLE_OVER_56 = 51,
		WEAP_MANTLE_OVER_48 = 52,
		WEAP_MANTLE_OVER_40 = 53,
		WEAP_MANTLE_OVER_32 = 54,
		WEAP_MANTLE_OVER_24 = 55,
		WEAP_MANTLE_OVER_16 = 56,
		WEAP_OFFHAND_SWITCH = 57,
		WEAP_INSPECTION = 58,
		MAX_WP_ANIMATIONS = 59,
	};

	enum WeaponAnimNumber_extended : std::int32_t
	{
		WEAP_IDLE_EXTENDED = 0,
		
		WEAP_SLIDE_IN	= MAX_WP_ANIMATIONS + 1,
		WEAP_SLIDE_LOOP = MAX_WP_ANIMATIONS + 2,
		WEAP_SLIDE_OUT	= MAX_WP_ANIMATIONS + 3,

		WEAP_DTP_IN	= MAX_WP_ANIMATIONS + 4,
		WEAP_DTP_LOOP = MAX_WP_ANIMATIONS + 5,
		WEAP_DTP_OUT	= MAX_WP_ANIMATIONS + 6,
		
		WEAP_HYBRID_ON	= MAX_WP_ANIMATIONS + 7,
		WEAP_HYBRID_OFF = MAX_WP_ANIMATIONS + 8,
		WEAP_HYBRID_TOGGLE = MAX_WP_ANIMATIONS + 9
	};
  
	struct outline_data_t
	{
		int refCount;
		char colorIndexForClient[18];
		bool depthEnableForClient[18];
		unsigned int enabledForClientMask;
	};

	struct CompassPlaneMedia
	{
		Material* friendly;
		Material* enemy;
	};

	/* 8746 */
	struct CompassPlane
	{
		int entityNum;
		int lastUpdateTime;
		float lastPos[3];
		float lastYaw;
		int ownerNum;
		CompassPlaneMedia planeMedia[2];
	};

	struct VehicleDef
	{
		char pad0[1512];
		Material* compassFriendlyIcon;
		Material* compassEnemyIcon;
		Material* compassFriendlyAltIcon;
		Material* compassEnemyAltIcon;
	};

	struct LevelLoad
	{
		XZoneInfo info[24];
		unsigned __int64 sizeEstimate[24];
		char names[24][64];
		unsigned int numZones;
		unsigned int loadPhaseCount[3];
		unsigned int numPhases;
	};

	struct sentient_s
	{
		char pad[16];
		team_t eTeam; // 16
	};

	struct ClientVoicePacket_t
	{
		char data[256];
		int dataSize;
	};

	struct voiceCommunication_t
	{
		ClientVoicePacket_t voicePackets[10];	// 0
		int voicePacketCount;					// 2600
		int voicePacketLastTransmit;			// 2604
		int packetsPerSec;
		int packetsPerSecStart;
	};

	struct VoicePacket_t
	{
		char talker;
		char data[256];
		int dataSize;
	};

	enum clientState_t
	{
		CS_FREE = 0x0,
		CS_ZOMBIE = 0x1,
		CS_RECONNECTING = 0x2,
		CS_CONNECTED = 0x3,
		CS_CLIENTLOADING = 0x4,
		CS_ACTIVE = 0x5,
	};

	struct netProfilePacket_t
	{
		int iTime;
		int iSize;
		int bFragment;
	};

	struct netProfileStream_t
	{
		netProfilePacket_t packets[60];
		int iCurrPacket;
		int iBytesPerSecond;
		int iLastBPSCalcTime;
		int iCountedPackets;
		int iCountedFragments;
		int iFragmentPercentage;
		int iLargestPacket;
		int iSmallestPacket;
	};

	struct netProfileInfo_t
	{
		netProfileStream_t send;
		netProfileStream_t recieve;
	};

	struct netchan_t
	{
		int outgoingSequence;
		netsrc_t sock;
		int dropped;
		int incomingSequence;
		netadr_s remoteAddress;
		int fragmentSequence;
		int fragmentLength;
		char* fragmentBuffer;
		int fragmentBufferSize;
		int unsentFragments;
		int unsentFragmentStart;
		int unsentLength;
		char* unsentBuffer;
		int unsentBufferSize;
	};
	
	struct clientConnection_t
	{
		int qport;
		int clientNum;
		int lastPacketTime;
		netadr_s serverAddress;
		int dword_14318C668; // what is here? 0x14318C668 
		int connectLastSendTime;
		int connectPacketCount;
		char serverMessage[256];
		int challenge;
		int checksumFeed;
		int reliableSequence;
		int reliableAcknowledge;
		char reliableCommands[128][1024];
		int serverMessageSequence;
		int serverCommandSequence;
		int lastExecutedServerCommand;
		char serverCommands[128][1024];
		bool isServerRestarting;
		netchan_t netchan;
		char netchanOutgoingBuffer[2048];
		char netchanIncomingBuffer[131072];
		netProfileInfo_t OOBProf;
		unsigned int statPacketsToSend;
		int statPacketSendTime[31];
		unsigned int currentGamestatePacket;
	};

	struct FontGlowStyle
	{
		float glowMinDistance;
		float glowMaxDistance;
		vec2_t glowUVOffset;
		vec4_t glowColor;
		float outlineGlowMinDistance;
		float outlineGlowMaxDistance;
		vec4_t outlineGlowColor;
	};

	struct GroupLookup
	{
		unsigned int hash;
		const char *string;
	};

	enum GfxRenderTargetId : int32_t
	{
		R_RENDERTARGET_SAVED_SCREEN,
		R_RENDERTARGET_FRAME_BUFFER,
		R_RENDERTARGET_FRAME_BUFFER_NODEPTH,
		R_RENDERTARGET_SCENE,
		R_RENDERTARGET_SCENE_PINGPONG,
		R_RENDERTARGET_RESOLVED_POST_SUN,
		R_RENDERTARGET_RESOLVED_SCENE,
		R_RENDERTARGET_FLOAT_Z,
		R_RENDERTARGET_FLOAT_Z_COPY,
		R_RENDERTARGET_PINGPONG_0,
		R_RENDERTARGET_PINGPONG_1,
		R_RENDERTARGET_LDR_SCENE_PINGPONG_0,
		R_RENDERTARGET_LDR_SCENE_PINGPONG_1,
		R_RENDERTARGET_SMAA_EDGES,
		R_RENDERTARGET_SMAA_BLEND,
		R_RENDERTARGET_SMAA_TEMPORAL_0,
		R_RENDERTARGET_SMAA_TEMPORAL_1,
		R_RENDERTARGET_SMAA_TEMPORAL_2,
		R_RENDERTARGET_SMAA_TEMPORAL_3,
		R_RENDERTARGET_SMAA_FILMIC_0,
		R_RENDERTARGET_SMAA_FILMIC_1,
		R_RENDERTARGET_SCENE_SPECULAR,
		R_RENDERTARGET_SCENE_ALBEDO,
		R_RENDERTARGET_POST_EFFECT_0,
		R_RENDERTARGET_POST_EFFECT_1,
		R_RENDERTARGET_SHADOWMAP_LARGE,
		R_RENDERTARGET_SHADOWMAP_SMALL,
		R_RENDERTARGET_TRANS_SHADOWMAP_LARGE,
		R_RENDERTARGET_TRANS_SHADOWMAP_SMALL,
		R_RENDERTARGET_MDAO,
		R_RENDERTARGET_MDAO_FLOATZ_SCALED,
		R_RENDERTARGET_SSAO,
		R_RENDERTARGET_SSAO_MIP1,
		R_RENDERTARGET_SSAO_MIP2,
		R_RENDERTARGET_SSAO_MIP3,
		R_RENDERTARGET_SSAO_MIP4,
		R_RENDERTARGET_SSAO_MIP5,
		R_RENDERTARGET_SSAO_BLURRED,
		R_RENDERTARGET_SSAO_BLURRED_MIP1,
		R_RENDERTARGET_SSAO_BLURRED_MIP2,
		R_RENDERTARGET_SSAO_BLURRED_MIP3,
		R_RENDERTARGET_SSAO_BLURRED_MIP4,
		R_RENDERTARGET_SSAO_BLURRED_MIP5,
		R_RENDERTARGET_SSAO_FLOATZ_MIP1,
		R_RENDERTARGET_SSAO_FLOATZ_MIP2,
		R_RENDERTARGET_SSAO_FLOATZ_MIP3,
		R_RENDERTARGET_SSAO_FLOATZ_MIP4,
		R_RENDERTARGET_SSAO_FLOATZ_MIP5,
		R_RENDERTARGET_SSAO_BLURRED_W1H0,
		R_RENDERTARGET_SSAO_BLURRED_W2H0,
		R_RENDERTARGET_SSAO_BLURRED_W3H0,
		R_RENDERTARGET_HDR_POSTFX_0,
		R_RENDERTARGET_HDR_POSTFX_1,
		R_RENDERTARGET_HDR_PINGPONG,
		R_RENDERTARGET_DEPTH_BLUR_0,
		R_RENDERTARGET_DEPTH_BLUR_1,
		R_RENDERTARGET_BLUR_DISTORTION_MIPMAP,
		R_RENDERTARGET_BLUR_DISTORTION_MIP0,
		R_RENDERTARGET_BLUR_DISTORTION_MIP1,
		R_RENDERTARGET_BLUR_DISTORTION_MIP2,
		R_RENDERTARGET_BLUR_DISTORTION_MIP3,
		R_RENDERTARGET_BLUR_DISTORTION_MIP4,
		R_RENDERTARGET_BLUR_DISTORTION_MIP5,
		R_RENDERTARGET_SCENE_VELOCITY,
		R_RENDERTARGET_SCENE_MOTIONBLUR_VELOCITY,
		R_RENDERTARGET_SCENE_VELOCITY_TILE0_PINGPONG,
		R_RENDERTARGET_SCENE_VELOCITY_TILE0,
		R_RENDERTARGET_SCENE_VELOCITY_TILE1,
		R_RENDERTARGET_DOF_FLOATZ,
		R_RENDERTARGET_DOF_HALF_PREPASS,
		R_RENDERTARGET_DOF_HALF_COLOR_PINGPONG,
		R_RENDERTARGET_DOF_HALF_ALPHA_PINGPONG,
		R_RENDERTARGET_DOF_HALF_COLOR,
		R_RENDERTARGET_DOF_HALF_ALPHA,
		R_RENDERTARGET_DOF_TILE0_PINGPONG,
		R_RENDERTARGET_DOF_TILE0_PINGPONG_HALF,
		R_RENDERTARGET_DOF_TILE0,
		R_RENDERTARGET_DOF_TILE1,
		R_RENDERTARGET_SCENE_NODEPTH,
		R_RENDERTARGET_SCENE_PINGPONG_NODEPTH,
		R_RENDERTARGET_UI3D,
		R_RENDERTARGET_UI3D_PING_PONG,
		R_RENDERTARGET_SCENE_MIPMAP,
		R_RENDERTARGET_SCENE_MIP1,
		R_RENDERTARGET_SCENE_MIP2,
		R_RENDERTARGET_SCENE_MIP3,
		R_RENDERTARGET_SCENE_MIP4,
		R_RENDERTARGET_SCENE_MIP5,
		R_RENDERTARGET_SCENE_MIP6,
		R_RENDERTARGET_SCENE_MIP7,
		R_RENDERTARGET_SCENE_MIP8,
		R_RENDERTARGET_SCENE_MIP1_PINGPONG,
		R_RENDERTARGET_SCENE_MIP2_PINGPONG,
		R_RENDERTARGET_SCENE_MIP3_PINGPONG,
		R_RENDERTARGET_SCENE_MIP4_PINGPONG,
		R_RENDERTARGET_SCENE_MIP5_PINGPONG,
		R_RENDERTARGET_SCENE_MIP6_PINGPONG,
		R_RENDERTARGET_SCENE_MIP7_PINGPONG,
		R_RENDERTARGET_SCENE_MIP8_PINGPONG,
		R_RENDERTARGET_VEIL_MIP1,
		R_RENDERTARGET_VEIL_MIP2,
		R_RENDERTARGET_VEIL_MIP3,
		R_RENDERTARGET_VEIL_MIP4,
		R_RENDERTARGET_VEIL_MIP5,
		R_RENDERTARGET_VEIL_MIP6,
		R_RENDERTARGET_VEIL_MIP7,
		R_RENDERTARGET_VEIL_MIP8,
		R_RENDERTARGET_SCENE_HIGHLIGHTDIR,
		R_RENDERTARGET_SCENE_HIGHLIGHTDIR_SCALED,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_0,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_1,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_2,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_3,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_4,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_5,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_6,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_7,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_8,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_9,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_10,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_11,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_12,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_13,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_14,
		R_RENDERTARGET_CACHED_SPOT_BSP_ONLY_15,
		R_RENDERTARGET_CACHED_SPOT_0,
		R_RENDERTARGET_CACHED_SPOT_1,
		R_RENDERTARGET_CACHED_SPOT_2,
		R_RENDERTARGET_CACHED_SPOT_3,
		R_RENDERTARGET_CACHED_SPOT_4,
		R_RENDERTARGET_CACHED_SPOT_5,
		R_RENDERTARGET_CACHED_SPOT_6,
		R_RENDERTARGET_CACHED_SPOT_7,
		R_RENDERTARGET_CACHED_SPOT_8,
		R_RENDERTARGET_CACHED_SPOT_9,
		R_RENDERTARGET_CACHED_SPOT_10,
		R_RENDERTARGET_CACHED_SPOT_11,
		R_RENDERTARGET_CACHED_SPOT_12,
		R_RENDERTARGET_CACHED_SPOT_13,
		R_RENDERTARGET_CACHED_SPOT_14,
		R_RENDERTARGET_CACHED_SPOT_15,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_0,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_1,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_2,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_3,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_4,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_5,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_6,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_7,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_8,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_9,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_10,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_11,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_12,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_13,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_14,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_15,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_16,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_17,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_18,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_19,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_20,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_21,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_22,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_23,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_24,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_25,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_26,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_27,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_28,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_29,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_30,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_31,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_32,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_33,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_34,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_35,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_36,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_37,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_38,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_39,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_40,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_41,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_42,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_43,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_44,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_45,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_46,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_47,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_48,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_49,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_50,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_51,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_52,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_53,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_54,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_55,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_56,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_57,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_58,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_59,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_60,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_61,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_62,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_63,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_64,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_65,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_66,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_67,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_68,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_69,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_70,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_71,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_72,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_73,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_74,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_75,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_76,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_77,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_78,
		R_RENDERTARGET_CACHED_SUN_BSP_ONLY_79,
		R_RENDERTARGET_CACHED_SUN_0,
		R_RENDERTARGET_CACHED_SUN_1,
		R_RENDERTARGET_CACHED_SUN_2,
		R_RENDERTARGET_CACHED_SUN_3,
		R_RENDERTARGET_CACHED_SUN_4,
		R_RENDERTARGET_CACHED_SUN_5,
		R_RENDERTARGET_CACHED_SUN_6,
		R_RENDERTARGET_CACHED_SUN_7,
		R_RENDERTARGET_CACHED_SUN_8,
		R_RENDERTARGET_CACHED_SUN_9,
		R_RENDERTARGET_CACHED_SUN_10,
		R_RENDERTARGET_CACHED_SUN_11,
		R_RENDERTARGET_CACHED_SUN_12,
		R_RENDERTARGET_CACHED_SUN_13,
		R_RENDERTARGET_CACHED_SUN_14,
		R_RENDERTARGET_CACHED_SUN_15,
		R_RENDERTARGET_CACHED_SUN_16,
		R_RENDERTARGET_CACHED_SUN_17,
		R_RENDERTARGET_CACHED_SUN_18,
		R_RENDERTARGET_CACHED_SUN_19,
		R_RENDERTARGET_CACHED_SUN_20,
		R_RENDERTARGET_CACHED_SUN_21,
		R_RENDERTARGET_CACHED_SUN_22,
		R_RENDERTARGET_CACHED_SUN_23,
		R_RENDERTARGET_CACHED_SUN_24,
		R_RENDERTARGET_CACHED_SUN_25,
		R_RENDERTARGET_CACHED_SUN_26,
		R_RENDERTARGET_CACHED_SUN_27,
		R_RENDERTARGET_CACHED_SUN_28,
		R_RENDERTARGET_CACHED_SUN_29,
		R_RENDERTARGET_CACHED_SUN_30,
		R_RENDERTARGET_CACHED_SUN_31,
		R_RENDERTARGET_CACHED_SUN_32,
		R_RENDERTARGET_CACHED_SUN_33,
		R_RENDERTARGET_CACHED_SUN_34,
		R_RENDERTARGET_CACHED_SUN_35,
		R_RENDERTARGET_CACHED_SUN_36,
		R_RENDERTARGET_CACHED_SUN_37,
		R_RENDERTARGET_CACHED_SUN_38,
		R_RENDERTARGET_CACHED_SUN_39,
		R_RENDERTARGET_CACHED_SUN_40,
		R_RENDERTARGET_CACHED_SUN_41,
		R_RENDERTARGET_CACHED_SUN_42,
		R_RENDERTARGET_CACHED_SUN_43,
		R_RENDERTARGET_CACHED_SUN_44,
		R_RENDERTARGET_CACHED_SUN_45,
		R_RENDERTARGET_CACHED_SUN_46,
		R_RENDERTARGET_CACHED_SUN_47,
		R_RENDERTARGET_CACHED_SUN_48,
		R_RENDERTARGET_CACHED_SUN_49,
		R_RENDERTARGET_CACHED_SUN_50,
		R_RENDERTARGET_CACHED_SUN_51,
		R_RENDERTARGET_CACHED_SUN_52,
		R_RENDERTARGET_CACHED_SUN_53,
		R_RENDERTARGET_CACHED_SUN_54,
		R_RENDERTARGET_CACHED_SUN_55,
		R_RENDERTARGET_CACHED_SUN_56,
		R_RENDERTARGET_CACHED_SUN_57,
		R_RENDERTARGET_CACHED_SUN_58,
		R_RENDERTARGET_CACHED_SUN_59,
		R_RENDERTARGET_CACHED_SUN_60,
		R_RENDERTARGET_CACHED_SUN_61,
		R_RENDERTARGET_CACHED_SUN_62,
		R_RENDERTARGET_CACHED_SUN_63,
		R_RENDERTARGET_CACHED_SUN_64,
		R_RENDERTARGET_CACHED_SUN_65,
		R_RENDERTARGET_CACHED_SUN_66,
		R_RENDERTARGET_CACHED_SUN_67,
		R_RENDERTARGET_CACHED_SUN_68,
		R_RENDERTARGET_CACHED_SUN_69,
		R_RENDERTARGET_CACHED_SUN_70,
		R_RENDERTARGET_CACHED_SUN_71,
		R_RENDERTARGET_CACHED_SUN_72,
		R_RENDERTARGET_CACHED_SUN_73,
		R_RENDERTARGET_CACHED_SUN_74,
		R_RENDERTARGET_CACHED_SUN_75,
		R_RENDERTARGET_CACHED_SUN_76,
		R_RENDERTARGET_CACHED_SUN_77,
		R_RENDERTARGET_CACHED_SUN_78,
		R_RENDERTARGET_CACHED_SUN_79,
		R_RENDERTARGET_DEPTH_HACK_DOF,
		R_RENDERTARGET_SUITE_GENERIC,
		R_RENDERTARGET_SUITE_LIT,
		R_RENDERTARGET_SUITE_POST_OPAQUE,
		R_RENDERTARGET_SUITE_MOTIONBLUR,
		R_RENDERTARGET_SUITE_SSS,
		R_RENDERTARGET_SUITE_ZPREPASS,
		R_RENDERTARGET_SUITE_DOF,
		R_RENDERTARGET_COUNT,
		R_RENDERTARGET_NONE
	};

	enum GfxViewStatsTarget : uint32_t
	{
		GFX_VIEW_STATS_INVALID = 0xFFFFFFFF,
		GFX_VIEW_STATS_2D = 0x0,
		GFX_VIEW_STATS_OPAQUE = 0x1,
		GFX_VIEW_STATS_TRANS = 0x2,
		GFX_VIEW_STATS_LIGHTMAP = 0x3,
		GFX_VIEW_STATS_EMISSIVE = 0x4,
		GFX_VIEW_STATS_DEPTH = 0x5,
		GFX_VIEW_STATS_SHADOW_SUN = 0x6,
		GFX_VIEW_STATS_SHADOW_SPOT = 0x7,
		GFX_VIEW_STATS_COUNT = 0x8,
		GFX_VIEW_STATS_VIEW_BEGIN = 0x0,
		GFX_VIEW_STATS_VIEW_END = 0x6,
		GFX_VIEW_STATS_SHADOW_BEGIN = 0x6,
		GFX_VIEW_STATS_SHADOW_END = 0x8,
	};

	struct GfxMatrix
	{
		float m[4][4];
	};

	struct GfxCodeMatrices
	{
		GfxMatrix matrix[72];
	};

	struct GfxRenderTargetSurface
	{
		ID3D11Texture2D* color;
		ID3D11Texture2D* depthStencil;
		ID3D11Texture2D* unk1;
		ID3D11Texture2D* unk2;
		ID3D11Texture2D* unk3;
	};

	struct GfxImageAndSurface
	{
		GfxImage* image;
		GfxRenderTargetSurface surface;
	};

	struct GfxRenderTarget
	{
		GfxImageAndSurface imageAndSurface;
		unsigned short width;
		unsigned short height;
		unsigned short selectedMipSlice;
		int unkRendererTarget;
		int format;
	};

	struct GfxSceneParms
	{
		char __pad0[924];
		GfxViewport ssrSourceSceneViewport;
		char __pad1[184];
		GfxViewport sceneViewport;
		GfxViewport displayViewport;
		GfxViewport scissorViewport;
		char __pad_end[310500];
	};

	struct GfxCamera
	{
		float origin[3];
		float axis[3][3];
		float subWindowMins[2];
		float subWindowMaxs[2];
		float tanHalfFovX;
		float tanHalfFovY;
		float zNear;
		float depthHackNearClip;
		float focalLength;
		unsigned __int8 padding[28];
	};

	struct GfxViewParms
	{
		GfxMatrix viewMatrix;
		GfxMatrix projectionMatrix;
		GfxMatrix viewProjectionMatrix;
		GfxMatrix inverseViewProjectionMatrix;
		GfxCamera camera;
	};

	struct GfxUI3DBackend
	{
		GfxViewport viewport[6];
		GfxViewParms viewParms[6];
		float uvSetup[6][4];
		int renderCmdCount[6];
		int renderCmdThatDrawsSomethingCount[5];
		int totalRenderCmds;
		int totalRenderCmdsThatDrawsSomething;
		float blurRadius;
	};

	enum R_HudOutlinePostMode : uint8_t
	{
		R_HUD_OUTLINE_POST_MODE_PIXEL = 0x0,
		R_HUD_OUTLINE_POST_MODE_DEPRECATED = 0x1,
		R_HUD_OUTLINE_POST_MODE_HALO = 0x2,
		R_HUD_OUTLINE_POST_MODE_CURVY = 0x3,
		R_HUD_OUTLINE_POST_MODE_CLOAK = 0x4,
		R_HUD_OUTLINE_POST_MODE_COUNT = 0x5,
	};

	enum R_HudOutlineWhen : uint8_t
	{
		R_HUD_OUTLINE_WHEN_AFTER_COLORIZE = 0x0,
		R_HUD_OUTLINE_WHEN_AFTER_FXAA = 0x1,
		R_HUD_OUTLINE_WHEN_BEFORE_FXAA = 0x2,
		R_HUD_OUTLINE_WHEN_BEFORE_HUDFX = 0x3,
		R_HUD_OUTLINE_WHEN_COUNT = 0x4,
	};

	struct GfxHudOutlineState
	{
		unsigned char enable;
		unsigned char postMode;
		unsigned char when;
		float width;
		float alpha0;
		float alpha1;
		float haloBlurRadius;
		float haloLumScale;
		float haloDarkenScale;
		float curvyBlurRadius;
		float curvyWidth;
		float curvyDepth;
		float curvyLumScale;
		float curvyDarkenScale;
		float cloakBlurRadius;
		float cloakLumScale;
		float cloakDarkenScale;
	};

	struct GfxSceneDef
	{
		int time;
		float floatTime;
		float viewOffset[3];
		GfxImage* sunShadowImage;
		float sunShadowPixelAdjust[4];
		float sunShadowNearFarPlane[4];
	};

	struct GfxBackEndData;
	struct GfxCmdBufInput
	{
		float consts[349][4];
		char __pad0[16];
		const GfxImage* codeImages[79];
		unsigned __int8 codeImageSamplerStates[79];
		unsigned __int8 : 6;
		__int8 noLightMapPrepass : 1;
		__int8 noLitPrepass : 1;
		char __pad1[88];
		GfxBackEndData* data;
	};

	struct GfxSSR
	{
		float ssrFade;
		float ssrBlendScale;
		float ssrRoughnessMipParameters[3];
		GfxViewport ssrSourceSceneViewport;
	};

	struct GfxBspSurfList
	{
		unsigned int count;
		const unsigned int* stream;
	};

	struct GfxSModelSurfList
	{
		unsigned int surfDataBytes;
		const unsigned __int8* surfData;
		const unsigned __int8* visData;
	};

	struct GfxCmdBuf
	{
		void* device;
		void* buf;
	};

	struct GfxDrawCallOutput
	{
		GfxCmdBuf cmdBuf;
		volatile int cmdCount;
		volatile int inCmdCount;
	};

	struct GfxDrawSurfList
	{
		GfxDrawSurf* array;
		unsigned int count;
	};

	enum GfxCodeSurfListType : uint32_t
	{
		GFX_CODE_SURF_LIST_INVALID = 0xFFFFFFFF,
		GFX_CODE_SURF_LIST_TRANS = 0x0,
		GFX_CODE_SURF_LIST_EMISSIVE = 0x1,
		GFX_CODE_SURF_LIST_TYPE_COUNT = 0x2,
	};

	struct GfxCodeSurfList
	{
		void* surfs;
		unsigned int count;
	};

	struct GfxMarkSurfList
	{
		void* surfs;
		unsigned int count;
	};

	struct GfxGlassSurfList
	{
		void* surfs;
		unsigned int count;
	};

	struct GfxCloudSurfList
	{
		void* particles;
		void* surfs;
		unsigned int count;
	};

	struct GfxSparkSurfList
	{
		void* surfs;
		unsigned int count;
	};

	struct GfxSurfsIterGroup
	{
		unsigned int iteratorBegin;
		unsigned int iteratorCount;
		char __pad0[92];
	};

	struct GfxViewInfo;
	struct GfxDrawListInfo
	{
		MaterialTechniqueType baseTechType;
		GfxViewInfo* viewInfo;
		float eyeOffset[3];
		unsigned int sceneLightIndex;
		int cameraView;
		int isNoSunShadow;
		GfxCodeSurfListType codeSurfListType;
		GfxSurfsIterGroup iterGroup;
	};

	struct GfxDrawList
	{
		GfxBspSurfList bspSurfList;
		GfxBspSurfList bspLightMapSurfList;
		GfxSModelSurfList smodelSurfList[1];
		GfxDrawSurfList drawSurfList;
		GfxCodeSurfList codeSurfList;
		GfxMarkSurfList markSurfList;
		GfxGlassSurfList glassSurfList;
		GfxCloudSurfList cloudSurfList;
		GfxSparkSurfList sparkSurfList;
		GfxDrawListInfo info;
	};

	assert_sizeof(GfxDrawList, 304);

	enum GfxViewportBehavior : int32_t
	{
		GFX_USE_VIEWPORT_FOR_VIEW = 0x0,
		GFX_USE_VIEWPORT_FULL = 0x1,
	};

	struct GfxEntity
	{
		unsigned int renderFlags;
		float materialTime;
		unsigned int genericMaterialData;
		float scriptShaderParam;
		float headRot[4];
		float colorLit[4];
		float colorUnlit[4];
		float colorEmissive[4];
	};

	struct GfxCmdBufSourceState
	{
		GfxCodeMatrices matrices;
		GfxCmdBufInput input;
		char __pad0[664];
		unsigned __int16 constVersions[468];
		unsigned __int8 maxSamplerFilter;
		char __pad5[15];
		GfxScaledPlacement skinnedPlacement;
		GfxScaledPlacement placement2;
		int viewMode;
		char __pad1[4];
		GfxSceneDef sceneDef;
		char __pad2[56];
		GfxViewport sceneViewport;
		GfxDepthStencilMode depthStencilMode;
		GfxViewportBehavior viewportBehavior;
		short renderTargetWidth;
		short renderTargetHeight;
		char __pad3[16];
		char bitFlags[1];
		char __pad4[3];
		GfxEntity gfxEnt;
		float pixelAspect;
		char viewStatsTarget;
		char __pad_end[3];
	};

	struct GfxViewInfo
	{
		GfxViewParms viewParms;
		GfxViewport sceneViewport;
		GfxViewport displayViewport;
		GfxViewport scissorViewport;
		GfxSceneDef sceneDef;
		char __pad1[520];
		GfxCmdBufInput input;
		GfxRenderTargetId renderTargetId;
		char __pad2[268];
		int ssaaSamples;
		short unk;
		char __pad3[610];
		GfxUI3DBackend rbUI3D;
		int padding[2];
		GfxRenderTargetId sceneResolveRenderTargetId;
		GfxRenderTargetId unkTargetId;
		GfxRenderTargetId unkTargetId2;
		GfxRenderTargetId unkTargetId3;
		float unk3;
		GfxSSR ssr;
		GfxViewParms prevFrameViewParms;
		char __pad4[8];
		char motionBlur;
		char __pad5[111];
		//GfxHudOutlineState hudOutline; // 11224 this is missing in 1.04
		char __pad6[280];
		int opaqueUnlit;
		int splitscreen;
		GfxDrawList drawList[66];
		char __pad_end[8];
	};

	struct GfxCmdBufPrimState
	{
		void* device;
		void* indexBuffer;
		int vertDeclType;
		unsigned int primType;
		unsigned int streamStrides[3];
		void* streamBuffers[3];
		unsigned int streamOffsets[3];
		void* vertexDecl;
		unsigned __int8 padding[8];
		__m128 perPrimConstantShadow[80];
		__m128 perObjectConstantShadow[64];
		__m128 stableConstantShadow[88];
		void* perPrimVSPSConstantBuffer;
		void* perPrimHSDSConstantBuffer;
		void* perObjectVSPSConstantBuffer;
		void* perObjectHSDSConstantBuffer;
		void* stableVSPSConstantBuffer;
		void* stableHSDSConstantBuffer;
		void* perMaterialVSConstantBuffer;
		void* perMaterialHSConstantBuffer;
		void* perMaterialDSConstantBuffer;
		void* perMaterialPSConstantBuffer;
	};

	struct GfxDeviceState
	{
		bool srgbWrite;
		bool srgbRenderTarget;
		char pad[2];
		float blendFactor[4];
		unsigned int sampleMask;
		void* rasterizerState;
		void* blendState;
		void* depthStencilState;
	};

	struct GfxCmdBufState
	{
		GfxCmdBufPrimState prim;
		char __pad0[2440];
		int firstVertexSamplerDirty;
		int lastVertexSamplerDirty;
		int firstDomainSamplerDirty;
		int lastDomainSamplerDirty;
		int firstPixelSamplerDirty;
		int lastPixelSamplerDirty;
		int firstVertexTextureDirty;
		int lastVertexTextureDirty;
		int firstDomainTextureDirty;
		int lastDomainTextureDirty;
		int firstPixelTextureDirty;
		int lastPixelTextureDirty;
		Material* material;
		MaterialTechniqueType techType;
		MaterialTechnique* technique;
		MaterialPass* pass;
		unsigned int passIndex;
		int depthRangeType;
		float depthRangeNear;
		float depthRangeFar;
		unsigned __int64 perPrimConstantState[80];
		unsigned __int64 perObjectConstantState[64];
		unsigned __int64 stableConstantState[77];
		unsigned int refStateBits[6];
		MaterialPixelShader* pixelShader;
		MaterialVertexShader* vertexShader;
		MaterialHullShader* hullShader;
		MaterialDomainShader* domainShader;
		ComputeShader* computeShader;
		int shaderMode;
		__int16 scissorX;
		__int16 scissorY;
		__int16 scissorW;
		__int16 scissorH;
		GfxViewport viewport;
		int renderTargetId;
		int renderTargetSuiteId;
		int renderTargetSuite[2];
		GfxDeviceState deviceState;
	};

	struct GfxCmdBufContext
	{
		GfxCmdBufSourceState* source;
		GfxCmdBufState* state;
	};

	struct GfxWindowParms
	{
		HWND hwnd;
		int x;
		int y;
		int flags;
		int mode;
		int aspectRatio;
		float displayAspectRatio;
		uint16_t shadowTileResolutionSmall;
		uint16_t shadowTileResolutionLarge;
		int width;
		int height;
		int renderWidth;
		int renderHeight;
		int displayWidth;
		int displayHeight;
		int unk15;
		int ssaaSamples;
	};

	struct vidConfig_t
	{
		unsigned int renderWidth;
		unsigned int renderHeight;
		unsigned int sceneWidth;
		unsigned int sceneHeight;
		unsigned int displayWidth;
		unsigned int displayHeight;
		int flags;
		uint16_t shadowTileResSmall;
		uint16_t shadowTileResLarge;
		int mode;
		int aspectRatio;
		float displayAspectRatio;
		float renderAspectRatio;
		float aspectRatioDisplayPixel;
	};

	struct GfxBackEndData
	{
		char __pad0[5508472];
		unsigned int viewInfoIndex;
		unsigned int viewInfoCount;
		char __pad1[8];
		GfxViewInfo* viewInfo;
	};

#ifndef IDA

	/***************************************************************
	 * h1-mod types not present in (verify layout for 1.04)
	 **************************************************************/

	struct NetConstStringMapList;

	struct weaponParms
	{
		float forward[3];
		float right[3];
		float up[3];
		float muzzleTrace[3];
		float gunForward[3];
		Weapon weapon;
		bool isAlternate;
		const WeaponDef* weapDef;
		const void* weapCompleteDef;
	};

	struct FxEffect
	{
		const FxEffectDef* def;
		int status;
		unsigned int firstElemHandle[3];
		unsigned int firstSortedElemHandle;
		unsigned int firstTrailHandle;
		unsigned int firstSparkFountainHandle;
		unsigned __int16 occlusionQueryHandle;
		char __pad0[10];
		unsigned __int16 randomSeed;
		unsigned int owner;
		float lighting[3];
		unsigned __int16 updateCount;
		unsigned __int16 markEntnum;
		unsigned __int16 b;
		unsigned __int16 flags;
		char __pad1[4];
		unsigned __int16 bolt;
		char __pad2[1];
		unsigned __int8 markViewmodelClientIndex;
		char __pad4[2];
		unsigned __int8 runnerSortOrder;
		volatile int frameCount;
		int msecBegin;
		int msecLastUpdate;
		float distanceTravelled;
		FxSpatialFrame frameAtSpawn;
		FxSpatialFrame frameNow;
		FxSpatialFrame framePrev;
		unsigned int numVectorFields;
		unsigned int vectorFields[8];
		unsigned int pad[3];
		float occlusionFade;
		char __pad3[32];
	};

	struct FxElem
	{
		unsigned __int8 defIndex : 6;
		__int8 defer : 1;
		__int8 valid : 1;
		unsigned __int8 sequence;
		unsigned __int8 atRestFraction;
		unsigned __int8 emitResidual;
		int msecBegin;
		float baseVel[3];
		float origin[3];
		__declspec(align(8)) float lastFrameColor[4][3];
		int timeLightingLastCalculated;
	};

	struct ElemUpdate
	{
		unsigned int effectHandle;
		unsigned int elemHandle;
	};

	union FxAccessLock
	{
		volatile int data;
		unsigned __int8 pad0[128];
	};

	struct FxSystem
	{
		char __pad0[48];
		FxEffect* effects;
		char __pad1[64];
		FxElem* elems;
		char __pad2[16];
		unsigned int* nextElemHandleInEffect;
		unsigned int* prevElemHandleInEffect;
		char __pad8[48];
		int firstActiveEffect;
		int firstNewEffect;
		int firstFreeEffect;
		char __pad6[4];
		unsigned int* allEffectHandles;
		char __pad7[12];
		int msecNow;
		int msecDelta;
		int msecDraw;
		char __pad3[56];
		FxAccessLock* lock;
		unsigned int systemFlags;
		char __pad5[44];
		ElemUpdate* updateElement;
		int numUpdateElement;
		char __pad4[1972];
	};
	static_assert(sizeof(FxSystem) == 2352);

	struct GfxDrawListArgs
	{
		GfxCmdBufContext context;
		GfxDrawListInfo* listInfo;
		MaterialTechniqueType baseTechType;
	};

	struct pathsort_s
	{
		pathnode_t* node;
		float metric;
	};

	struct NetConstStringMapList
	{
		NetConstStrings* ncs;
		NetConstStringMapList* next;
	};

	struct NetConstStringMap
	{
		NetConstStringMapList* head;
		unsigned int ncsCount;
	};

	struct NetConstStringConfigStringTypeData
	{
		unsigned int csMax;
		int a2;
	};

	enum ConfigString : __int32
	{
		CS_FIRST = 0x0,
		MAX_CONFIGSTRINGS = 0x13F1, // 1.04 0x1371
	};

	namespace sp
	{
		struct gclient_s
		{
			char __pad[59200];
			int flags; // 59200
		};

		struct gentity_s
		{
			char __pad0[280];
			gclient_s* client; // 280
			char __pad1[76];
			int flags; // 364
		};

		struct playerState_s
		{
		};

		struct XZone
		{
			char __pad0[32];
			char name[64];
			char __pad1[128];
		};

		static_assert(sizeof(XZone) == 224);
	}
}
#endif
