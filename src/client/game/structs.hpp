#pragma once
#include <d3d11.h>

#define PROTOCOL 1

#ifdef DEBUG
#define assert_sizeof(__ASSET__, __SIZE__) static_assert(sizeof(__ASSET__) == __SIZE__)
#define assert_offsetof(__ASSET__, __VARIABLE__, __OFFSET__) static_assert(offsetof(__ASSET__, __VARIABLE__) == __OFFSET__)
#else
#define assert_sizeof(__ASSET__, __SIZE__)
#define assert_offsetof(__ASSET__, __VARIABLE__, __OFFSET__)
#endif

namespace game
{
	typedef float vec_t;
	typedef vec_t vec2_t[2];
	typedef vec_t vec3_t[3];
	typedef vec_t vec4_t[4];

	// * scripting
	enum scr_string_t
	{
		scr_string_t_dummy = 0x0,
	};

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
		DVAR_FLAG_NONE = 0,
		DVAR_FLAG_SAVED = 0x1,
		DVAR_FLAG_LATCHED = 0x2,
		DVAR_FLAG_CHEAT = 0x4,
		DVAR_FLAG_EXTERNAL = 0x100,
		DVAR_FLAG_REPLICATED = 0x8,
		DVAR_FLAG_WRITE = 0x800,
		DVAR_FLAG_READ = 0x2000,
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

	struct dvar_t
	{
		int hash;
		unsigned int flags;
		dvar_type type;
		bool modified;
		dvar_value current;
		dvar_value latched;
		dvar_value reset;
		dvar_limits domain;
		char __pad0[0xC];
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

	enum DBSyncMode
	{
		DB_LOAD_ASYNC = 0x0,
		DB_LOAD_SYNC = 0x1,
		DB_LOAD_ASYNC_WAIT_ALLOC = 0x2,
		DB_LOAD_ASYNC_FORCE_FREE = 0x3,
		DB_LOAD_ASYNC_NO_SYNC_THREADS = 0x4,
		DB_LOAD_SYNC_SKIP_ALWAYS_LOADED = 0x5,
	};

	enum DBAllocFlags : std::int32_t
	{
		DB_ZONE_NONE = 0x0,
		DB_ZONE_COMMON = 0x1,
		DB_ZONE_UI = 0x2,
		DB_ZONE_GAME = 0x4,
		DB_ZONE_LOAD = 0x8,
		DB_ZONE_DEV = 0x10,
		DB_ZONE_BASEMAP = 0x20,
		DB_ZONE_TRANSIENT_POOL = 0x40,
		DB_ZONE_TRANSIENT_MASK = 0x40,
		DB_ZONE_CUSTOM = 0x1000 // added for custom zone loading
	};

	struct XZoneInfo
	{
		const char* name;
		int allocFlags;
		int freeFlags;
	};

	struct XZoneInfoInternal
	{
		char name[64];
		int flags;
		int isBaseMap;
	};

	enum XAssetType
	{
		ASSET_TYPE_PHYSPRESET,
		ASSET_TYPE_PHYS_COLLMAP,
		ASSET_TYPE_PHYSWATERPRESET,
		ASSET_TYPE_PHYS_WORLDMAP,
		ASSET_TYPE_PHYSCONSTRAINT,
		ASSET_TYPE_XANIM,
		ASSET_TYPE_XMODELSURFS,
		ASSET_TYPE_XMODEL,
		ASSET_TYPE_MATERIAL,
		ASSET_TYPE_COMPUTESHADER,
		ASSET_TYPE_VERTEXSHADER,
		ASSET_TYPE_HULLSHADER,
		ASSET_TYPE_DOMAINSHADER,
		ASSET_TYPE_PIXELSHADER,
		ASSET_TYPE_VERTEXDECL,
		ASSET_TYPE_TECHSET,
		ASSET_TYPE_IMAGE,
		ASSET_TYPE_SOUND,
		ASSET_TYPE_SOUNDSUBMIX,
		ASSET_TYPE_SNDCURVE,
		ASSET_TYPE_LPFCURVE,
		ASSET_TYPE_REVERBSENDCURVE,
		ASSET_TYPE_SNDCONTEXT,
		ASSET_TYPE_LOADED_SOUND,
		ASSET_TYPE_COL_MAP_MP,
		ASSET_TYPE_COM_MAP,
		ASSET_TYPE_GLASS_MAP,
		ASSET_TYPE_AIPATHS,
		ASSET_TYPE_VEHICLE_TRACK,
		ASSET_TYPE_MAP_ENTS,
		ASSET_TYPE_FX_MAP,
		ASSET_TYPE_GFX_MAP,
		ASSET_TYPE_LIGHTDEF,
		ASSET_TYPE_UI_MAP,
		ASSET_TYPE_MENUFILE,
		ASSET_TYPE_MENU,
		ASSET_TYPE_ANIMCLASS,
		ASSET_TYPE_LOCALIZE,
		ASSET_TYPE_ATTACHMENT,
		ASSET_TYPE_WEAPON,
		ASSET_TYPE_SNDDRIVERGLOBALS,
		ASSET_TYPE_FX,
		ASSET_TYPE_IMPACTFX,
		ASSET_TYPE_SURFACEFX,
		ASSET_TYPE_AITYPE,
		ASSET_TYPE_MPTYPE,
		ASSET_TYPE_CHARACTER,
		ASSET_TYPE_XMODELALIAS,
		ASSET_TYPE_RAWFILE,
		ASSET_TYPE_SCRIPTFILE,
		ASSET_TYPE_STRINGTABLE,
		ASSET_TYPE_LEADERBOARDDEF,
		ASSET_TYPE_VIRTUALLEADERBOARDDEF,
		ASSET_TYPE_STRUCTUREDDATADEF,
		ASSET_TYPE_DDL,
		ASSET_TYPE_PROTO,
		ASSET_TYPE_TRACER,
		ASSET_TYPE_VEHICLE,
		ASSET_TYPE_ADDON_MAP_ENTS,
		ASSET_TYPE_NETCONSTSTRINGS,
		ASSET_TYPE_REVERBPRESET,
		ASSET_TYPE_LUAFILE,
		ASSET_TYPE_SCRIPTABLE,
		ASSET_TYPE_EQUIPSNDTABLE,
		ASSET_TYPE_VECTORFIELD,
		ASSET_TYPE_DOPPLERPRESET,
		ASSET_TYPE_PARTICLESIMANIMATION,
		ASSET_TYPE_LASER,
		ASSET_TYPE_SKELETONSCRIPT,
		ASSET_TYPE_CLUT,
		ASSET_TYPE_TTF,
		ASSET_TYPE_COUNT
	};

	enum GfxDrawSceneMethod
	{
		GFX_DRAW_SCENE_STANDARD = 0x0,
	};

	enum MaterialTechniqueType
	{
		TECHNIQUE_UNLIT = 8,
		TECHNIQUE_EMISSIVE = 9,
		TECHNIQUE_LIT = 13,
	};

	struct GfxDrawMethod_s
	{
		int drawScene;
		int baseTechType;
		int emissiveTechType;
		int forceTechType;
	};

	struct GfxImage;

	union MaterialTextureDefInfo
	{
		GfxImage* image;
		void* water;
	};

	struct MaterialTextureDef
	{
		unsigned int nameHash;
		char nameStart;
		char nameEnd;
		char samplerState;
		char semantic;
		MaterialTextureDefInfo u;
	};

	struct MaterialPass
	{
		void* vertexShader;
		void* vertexDecl;
		void* hullShader;
		void* domainShader;
		void* pixelShader;
		char pixelOutputMask;
		char perPrimArgCount;
		char perObjArgCount;
		char stableArgCount;
		unsigned __int16 perPrimArgSize;
		unsigned __int16 perObjArgSize;
		unsigned __int16 stableArgSize;
		char zone;
		char perPrimConstantBuffer;
		char perObjConstantBuffer;
		char stableConstantBuffer;
		unsigned int customBufferFlags;
		char customSamplerFlags;
		char precompiledIndex;
		char stageConfig;
		void* args;
	};

	struct MaterialTechnique
	{
		const char* name;
		unsigned __int16 flags;
		unsigned __int16 passCount;
		MaterialPass passArray[1];
	};

	struct MaterialTechniqueSet
	{
		const char* name;
		unsigned __int16 flags;
		char worldVertFormat;
		char preDisplacementOnlyCount;
		MaterialTechnique* techniques[309];
	};

	struct GfxStateBits
	{
		unsigned int loadBits[3];
		char zone;
		char depthStencilState[11];
		char blendState;
		char rasterizerState;
	};

	struct Packed128
	{
		std::uint64_t p0;
		std::uint64_t p1;
	};

	struct GfxDrawSurfFields
	{
		unsigned __int64 objectId : 16; // p0 >> 0
		unsigned __int64 pad0 : 20;
		unsigned __int64 reflectionProbeIndex : 8;
		unsigned __int64 hasGfxEntIndex : 1;
		unsigned __int64 customIndex : 5; // p0 >> 45
		unsigned __int64 materialSortedIndex : 14; // p0 >> 50
		unsigned __int64 tessellation : 2; // p1 >> 0
		unsigned __int64 prepass : 2; // p1 >> 2
		unsigned __int64 pad1 : 4;
		unsigned __int64 useHeroLighting : 1;
		unsigned __int64 sceneLightEnvIndex : 16;
		unsigned __int64 viewModelRender : 1;
		unsigned __int64 surfType : 4;
		unsigned __int64 primarySortKey : 6; // p1 >> 30
		unsigned __int64 unused : 28;
	};

	union GfxDrawSurf
	{
		GfxDrawSurfFields fields;
		Packed128 packed;
	};

	enum SurfaceTypeBits : std::uint64_t
	{
		SURFTYPE_BITS_DEFAULT = 0x0,
		SURFTYPE_BITS_BARK = 0x1,
		SURFTYPE_BITS_BRICK = 0x2,
		SURFTYPE_BITS_CARPET = 0x4,
		SURFTYPE_BITS_CLOTH = 0x8,
		SURFTYPE_BITS_CONCRETE = 0x10,
		SURFTYPE_BITS_DIRT = 0x20,
		SURFTYPE_BITS_FLESH = 0x40,
		SURFTYPE_BITS_FOLIAGE_DEBRIS = 0x80,
		SURFTYPE_BITS_GLASS = 0x100,
		SURFTYPE_BITS_GRASS = 0x200,
		SURFTYPE_BITS_GRAVEL = 0x400,
		SURFTYPE_BITS_ICE = 0x800,
		SURFTYPE_BITS_METAL_SOLID = 0x1000,
		SURFTYPE_BITS_METAL_GRATE = 0x2000,
		SURFTYPE_BITS_MUD = 0x4000,
		SURFTYPE_BITS_PAPER = 0x8000,
		SURFTYPE_BITS_PLASTER = 0x10000,
		SURFTYPE_BITS_ROCK = 0x20000,
		SURFTYPE_BITS_SAND = 0x40000,
		SURFTYPE_BITS_SNOW = 0x80000,
		SURFTYPE_BITS_WATER_WAIST = 0x100000,
		SURFTYPE_BITS_WOOD_SOLID = 0x200000,
		SURFTYPE_BITS_ASPHALT = 0x400000,
		SURFTYPE_BITS_CERAMIC = 0x800000,
		SURFTYPE_BITS_PLASTIC_SOLID = 0x1000000,
		SURFTYPE_BITS_RUBBER = 0x2000000,
		SURFTYPE_BITS_FRUIT = 0x4000000,
		SURFTYPE_BITS_PAINTEDMETAL = 0x8000000,
		SURFTYPE_BITS_RIOTSHIELD = 0x10000000,
		SURFTYPE_BITS_SLUSH = 0x20000000,
		SURFTYPE_BITS_ASPHALT_WET = 0x40000000,
		SURFTYPE_BITS_ASPHALT_DEBRIS = 0x80000000,
		SURFTYPE_BITS_CONCRETE_WET = 0x100000000,
		SURFTYPE_BITS_CONCRETE_DEBRIS = 0x200000000,
		SURFTYPE_BITS_FOLIAGE_VEGETATION = 0x400000000,
		SURFTYPE_BITS_FOLIAGE_LEAVES = 0x800000000,
		SURFTYPE_BITS_GRASS_TALL = 0x1000000000,
		SURFTYPE_BITS_METAL_HOLLOW = 0x2000000000,
		SURFTYPE_BITS_METAL_VEHICLE = 0x4000000000,
		SURFTYPE_BITS_METAL_THIN = 0x8000000000,
		SURFTYPE_BITS_METAL_WET = 0x10000000000,
		SURFTYPE_BITS_METAL_DEBRIS = 0x20000000000,
		SURFTYPE_BITS_PLASTIC_HOLLOW = 0x40000000000,
		SURFTYPE_BITS_PLASTIC_TARP = 0x80000000000,
		SURFTYPE_BITS_ROCK_WET = 0x100000000000,
		SURFTYPE_BITS_ROCK_DEBRIS = 0x200000000000,
		SURFTYPE_BITS_WATER_ANKLE = 0x400000000000,
		SURFTYPE_BITS_WATER_KNEE = 0x800000000000,
		SURFTYPE_BITS_WOOD_HOLLOW = 0x1000000000000,
		SURFTYPE_BITS_WOOD_WET = 0x2000000000000,
		SURFTYPE_BITS_WOOD_DEBRIS = 0x4000000000000,
		SURFTYPE_BITS_CUSHION = 0x8000000000000,
	};

	struct MaterialInfo
	{
		const char* name;
		unsigned char gameFlags;
		unsigned char sortKey;
		unsigned char textureAtlasRowCount;
		unsigned char textureAtlasColumnCount;
		unsigned char textureAtlasFrameBlend;
		unsigned char textureAtlasAsArray;
		unsigned char renderFlags;
		GfxDrawSurf drawSurf;
		void* surfaceTypeBits;
		unsigned int hashIndex;
	};

	struct Material
	{
		union
		{
			const char* name;
			MaterialInfo info;
		};
		unsigned char stateBitsEntry[240];
		unsigned char textureCount;
		unsigned char constantCount;
		unsigned char stateBitsCount;
		unsigned char stateFlags;
		unsigned char cameraRegion;
		unsigned char materialType;
		unsigned char layerCount;
		unsigned char assetFlags;
		MaterialTechniqueSet* techniqueSet;
		MaterialTextureDef* textureTable;
		void* constantTable;
		GfxStateBits* stateBitsTable;
		unsigned char constantBufferIndex[240];
		void* constantBufferTable;
		unsigned char constantBufferCount;
		const char** subMaterials;
	};

	static_assert(sizeof(Material) == 0x250);

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
		int glyphCount;
		Material* material;
		Material* glowMaterial;
		Glyph* glyphs;
	};

	struct StreamFileNameRaw
	{
		const char* dir;
		const char* name;
	};

	struct StreamFileNamePacked
	{
		unsigned __int64 offset;
		unsigned __int64 length;
	};

	union StreamFileInfo
	{
		StreamFileNameRaw raw;
		StreamFileNamePacked packed;
	};

	struct StreamFileName
	{
		unsigned __int16 isLocalized;
		unsigned __int16 fileIndex;
		StreamFileInfo info;
	};

	struct StreamedSound
	{
		StreamFileName filename;
		unsigned int totalMsec;
	};

	union SoundFileRef
	{
		StreamedSound streamSnd;
	};

	struct SoundFile
	{
		char type;
		char exists;
		SoundFileRef u;
	};

	struct snd_alias_t
	{
		const char* aliasName;
		char __pad0[24];
		SoundFile* soundFile;
		char __pad1[198];
		// not gonna map this out...
	};

	struct snd_alias_list_t
	{
		const char* aliasName;
		snd_alias_t* head;
		void* unk;
		unsigned char count;
		unsigned char unkCount;
		char __pad0[6];
	};

	struct RawFile
	{
		const char* name;
		int compressedLen;
		int len;
		const char* buffer;
	};

	struct ScriptFile
	{
		const char* name;
		int compressedLen;
		int len;
		int bytecodeLen;
		char* buffer;
		char* bytecode;
	};

	struct StringTableCell
	{
		const char* string;
		int hash;
	};

	struct StringTable
	{
		const char* name;
		int columnCount;
		int rowCount;
		StringTableCell* values;
	};

	struct LuaFile
	{
		const char* name;
		int len;
		char strippingType;
		const char* buffer;
	};

	struct TTF
	{
		const char* name;
		int len;
		const char* buffer;
		int fontFace;
	};

	struct GfxImageLoadDef
	{
		char levelCount;
		char numElements;
		char pad[2];
		int flags;
		int format;
		int resourceSize;
		char data[1];
	};

	union $3FA29451CE6F1FA138A5ABAB84BE9676
	{
		ID3D11Texture1D* linemap;
		ID3D11Texture2D* map;
		ID3D11Texture3D* volmap;
		ID3D11Texture2D* cubemap;
		GfxImageLoadDef* loadDef;
	};

	struct GfxTexture
	{
		$3FA29451CE6F1FA138A5ABAB84BE9676 ___u0;
		ID3D11ShaderResourceView* shaderView;
		ID3D11ShaderResourceView* shaderViewAlternate;
	};

	struct Picmip
	{
		char platform[2];
	};

	struct CardMemory
	{
		int platform[2];
	};

	struct GfxImage
	{
		GfxTexture textures;
		int flags;
		int imageFormat;
		int resourceSize;
		char mapType;
		char semantic;
		char category;
		char flags2;
		Picmip picmip;
		char track;
		//CardMemory cardMemory;
		unsigned short width;
		unsigned short height;
		unsigned short depth;
		unsigned short numElements;
		char pad3[4];
		void* pixelData;
		//GfxImageLoadDef *loadDef;
		uint64_t streams[4];
		const char* name;
	};

	struct XModel
	{
		const char* name;
	};

	struct WeaponDef
	{
		const char* name;
	};

	struct LocalizeEntry
	{
		const char* value;
		const char* name;
	};

	struct DDLMember
	{
		const char* name;
		int index;
		void* parent;
		int bitSize;
		int limitSize;
		int offset;
		int type;
		int externalIndex;
		unsigned int rangeLimit;
		unsigned int serverDelta;
		unsigned int clientDelta;
		int arraySize;
		int enumIndex;
		int permission;
	};

	struct DDLHash
	{
		unsigned int hash;
		int index;
	};

	struct DDLHashTable
	{
		DDLHash* list;
		int count;
		int max;
	};

	struct DDLStruct
	{
		const char* name;
		int bitSize;
		int memberCount;
		DDLMember* members;
		DDLHashTable hashTableUpper;
		DDLHashTable hashTableLower;
	};

	struct DDLEnum
	{
		const char* name;
		int memberCount;
		const char** members;
		DDLHashTable hashTable;
	};

	struct DDLDef
	{
		char* name;
		unsigned short version;
		unsigned int checksum;
		unsigned char flags;
		int bitSize;
		int byteSize;
		DDLStruct* structList;
		int structCount;
		DDLEnum* enumList;
		int enumCount;
		DDLDef* next;
		int headerBitSize;
		int headerByteSize;
		int reserveSize;
		int userFlagsSize;
		bool paddingUsed;
	};

	struct DDLRoot
	{
		const char* name;
		DDLDef* ddlDef;
	};

	union XAssetHeader
	{
		void* data;
		Material* material;
		Font_s* font;
		RawFile* rawfile;
		ScriptFile* scriptfile;
		StringTable* stringTable;
		LuaFile* luaFile;
		GfxImage* image;
		TTF* ttf;
		XModel* model;
		WeaponDef* weapon;
		LocalizeEntry* localize;
	};

	struct XAsset
	{
		XAssetType type;
		XAssetHeader header;
	};

	struct XAssetEntry
	{
		XAsset asset;
		char zoneIndex;
		volatile char inuseMask;
		unsigned int nextHash;
		unsigned int nextOverride;
		unsigned int nextPoolEntry;
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

	struct trace_t
	{
		float fraction;
		float normal[3];
		char __pad0[25];
		bool allsolid;
		bool startsolid;
		char __pad1[0x2C]; // not correct
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
		int horzAlign;
		int vertAlign;
	};

	// made up
	struct client_state_t
	{
		char __pad0[0x4A50];
		int ping;
		char __pad1[0x8];
		int num_players;
	};

	// made up
	struct connect_state_t
	{
		char __pad0[0xC];
		netadr_s address;
	};

	static_assert(offsetof(client_state_t, ping) == 0x4A50);
	static_assert(offsetof(client_state_t, num_players) == 0x4A5C);

	struct pmove_t
	{
		unsigned char __pad0[0x190];
	};

	static_assert(sizeof(pmove_t) == 0x190);

	struct pml_t
	{
		unsigned char __pad0[0x130];
	};

	static_assert(sizeof(pml_t) == 0x130);

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

	namespace mp
	{
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
			TEAM_FREE = 0x0,
			TEAM_BAD = 0x0,
			TEAM_AXIS = 0x1,
			TEAM_ALLIES = 0x2,
			TEAM_SPECTATOR = 0x3,
			TEAM_NUM_TEAMS = 0x4,
		};

		struct gclient_s
		{
			char __pad0[2];
			char pm_type; // 2
			char __pad1[18573];
			sessionState_t sessionState;
			char __pad2[220]; // 254
			team_t team;
			char __pad3[30];
			char name[32]; // 18834
			char __pad4[622];
			int flags; // 19488 
		}; // size = ?

		static_assert(offsetof(gclient_s, team) == 18800);
		static_assert(offsetof(gclient_s, name) == 18834);
		static_assert(offsetof(gclient_s, flags) == 19488);

		struct usercmd_s
		{
			char __pad0[28];
			char forwardmove;
			char rightmove;
			char __pad1[34];
		};

		struct EntityState
		{
			uint16_t number;
		}; // size = ?

#pragma pack(push, 1)
		struct gentity_s
		{
			EntityState s;
			char __pad0[342];
			gclient_s* client;
			char __pad1[80];
			int flags;
			char __pad2[300];
		}; // size = 736
#pragma pack(pop)

		static_assert(sizeof(gentity_s) == 736);

		struct playerState_s
		{
			int clientNum;
			char __pad0[116];
			vec3_t origin;
			vec3_t velocity;
			char __pad1[312];
			int sprintButtonUpRequired;
		};

		struct pmove_t
		{
			playerState_s* ps;
			usercmd_s cmd;
			usercmd_s oldcmd;
			int tracemask;
			int numtouch;
			int touchents[32];
			Bounds bounds;
		};

		static_assert(offsetof(pmove_t, touchents) == 144);

		struct pml_t
		{
			float forward[3];
			float right[3];
			float up[3];
			float frametime;
		};

		struct clientHeader_t
		{
			int state;
			char __pad0[44];
			netadr_s remoteAddress;
		}; // size = ?

		struct client_t
		{
			clientHeader_t header;
			char __pad0[265164];
			int reliableSequence;
			int reliableAcknowledge;
			char __pad1[397928];
			gentity_s* gentity;
			char name[32];
			char __pad5[348752];
		}; // size = 1011960

		static_assert(sizeof(client_t) == 1011960);

		struct XZone
		{
			char __pad0[32];
			char name[64];
			char __pad1[408];
		};

		static_assert(sizeof(XZone) == 504);
	}

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

	union playerState_s
	{
		sp::playerState_s* sp;
		mp::playerState_s* mp;
	};

	struct GfxWorldDpvsPlanes
	{
		int cellCount;
		void* planes;
		unsigned short* nodes;
		unsigned int* sceneEntCellBits;
	}; assert_sizeof(GfxWorldDpvsPlanes, 32);

	struct sunflare_t
	{
		bool hasValidData;
		Material* spriteMaterial;
		Material* flareMaterial;
		float spriteSize;
		float flareMinSize;
		float flareMinDot;
		float flareMaxSize;
		float flareMaxDot;
		float flareMaxAlpha;
		int flareFadeInTime;
		int flareFadeOutTime;
		float blindMinDot;
		float blindMaxDot;
		float blindMaxDarken;
		int blindFadeInTime;
		int blindFadeOutTime;
		float glareMinDot;
		float glareMaxDot;
		float glareMaxLighten;
		int glareFadeInTime;
		int glareFadeOutTime;
		float sunFxPosition[3];
	}; assert_sizeof(sunflare_t, 112);

	typedef void* umbraTomePtr_t;

	struct GfxBuildInfo
	{
		const char* args0;
		const char* args1;
		const char* buildStartTime;
		const char* buildEndTime;
	}; assert_sizeof(GfxBuildInfo, 32);

	struct GfxStaticModelDrawInst
	{
		char __pad0[56];
		XModel* __ptr64 model;
		unsigned short cullDist;
		unsigned short flags;
		unsigned short lightingHandle;
		unsigned short staticModelId;
		unsigned short primaryLightEnvIndex;
		short unk0;
		char unk1; // lod related
		unsigned char reflectionProbeIndex;
		unsigned char firstMtlSkinIndex;
		unsigned char sunShadowFlags;
	}; assert_sizeof(GfxStaticModelDrawInst, 80);
	assert_offsetof(GfxStaticModelDrawInst, model, 56);
	assert_offsetof(GfxStaticModelDrawInst, cullDist, 64);
	assert_offsetof(GfxStaticModelDrawInst, flags, 66);
	assert_offsetof(GfxStaticModelDrawInst, lightingHandle, 68);
	assert_offsetof(GfxStaticModelDrawInst, primaryLightEnvIndex, 72);
	assert_offsetof(GfxStaticModelDrawInst, reflectionProbeIndex, 77); // maybe wrong
	assert_offsetof(GfxStaticModelDrawInst, firstMtlSkinIndex, 78);

	struct GfxStaticModelVertexLighting
	{
		unsigned char visibility[4];
		unsigned short ambientColorFloat16[4];
		unsigned short highlightColorFloat16[4];
	}; assert_sizeof(GfxStaticModelVertexLighting, 20);

	struct GfxStaticModelVertexLightingInfo
	{
		GfxStaticModelVertexLighting* lightingValues;
		ID3D11Buffer* lightingValuesVb;
		int numLightingValues;
	};

	struct GfxStaticModelLightmapInfo
	{
		float offset[2];
		float scale[2];
		unsigned int lightmapIndex;
	};

	struct GfxStaticModelGroundLightingInfo
	{
		unsigned short groundLighting[4]; // float16
	};

	struct GfxStaticModelLightGridLightingInfo
	{
		unsigned short colorFloat16[4];
		int a;
		float b;
		char __pad1[8];
	};

	union GfxStaticModelLighting
	{
		GfxStaticModelVertexLightingInfo vertexLightingInfo;
		GfxStaticModelLightmapInfo modelLightmapInfo;
		GfxStaticModelGroundLightingInfo modelGroundLightingInfo;
		GfxStaticModelLightGridLightingInfo modelLightGridLightingInfo;
	}; assert_sizeof(GfxStaticModelLighting, 24);

	struct GfxWorldDpvsStatic
	{
		unsigned int smodelCount; // 0
		unsigned int subdivVertexLightingInfoCount; // 4
		unsigned int staticSurfaceCount; // 8
		unsigned int litOpaqueSurfsBegin; // 12
		unsigned int litOpaqueSurfsEnd; // 16
		unsigned int unkSurfsBegin;
		unsigned int unkSurfsEnd;
		unsigned int litDecalSurfsBegin; // 28
		unsigned int litDecalSurfsEnd; // 32
		unsigned int litTransSurfsBegin; // 36
		unsigned int litTransSurfsEnd; // 40
		unsigned int shadowCasterSurfsBegin; // 44
		unsigned int shadowCasterSurfsEnd; // 48
		unsigned int emissiveSurfsBegin; // 52
		unsigned int emissiveSurfsEnd; // 56
		unsigned int smodelVisDataCount; // 60
		unsigned int surfaceVisDataCount; // 64
		unsigned int* smodelVisData[4]; // 72 80 88 96
		unsigned int* smodelUnknownVisData[27];
		unsigned int* surfaceVisData[4]; // 320 328 336 344
		unsigned int* surfaceUnknownVisData[27];
		unsigned int* smodelUmbraVisData[4]; // 568 576 584 592
		unsigned int* surfaceUmbraVisData[4]; // 600 608 616 624
		unsigned int* lodData; // 632
		unsigned int* tessellationCutoffVisData; // 640
		unsigned int* sortedSurfIndex; // 648
		void* smodelInsts; // 656
		void* surfaces; // 664
		void* surfacesBounds; // 672
		GfxStaticModelDrawInst* smodelDrawInsts; // 680
		unsigned int* unknownSModelVisData1; // 688
		unsigned int* unknownSModelVisData2; // 696
		GfxStaticModelLighting* smodelLighting; // 704 (array)
		void* subdivVertexLighting; // 712 (array)
		GfxDrawSurf* surfaceMaterials; // 720
		unsigned int* surfaceCastsSunShadow; // 728
		unsigned int sunShadowOptCount; // 736
		unsigned int sunSurfVisDataCount; // 740
		unsigned int* surfaceCastsSunShadowOpt; // 744
		void* surfaceDeptAndSurf; // 752
		void* constantBuffersLit; // 760
		void* constantBuffersAmbient; // 768
		int usageCount; // 776
	}; assert_sizeof(GfxWorldDpvsStatic, 784);
	assert_offsetof(GfxWorldDpvsStatic, smodelVisData[0], 72);
	assert_offsetof(GfxWorldDpvsStatic, surfaceVisData[0], 320);
	assert_offsetof(GfxWorldDpvsStatic, smodelUmbraVisData[0], 568);
	assert_offsetof(GfxWorldDpvsStatic, tessellationCutoffVisData, 640);
	assert_offsetof(GfxWorldDpvsStatic, smodelDrawInsts, 680);
	assert_offsetof(GfxWorldDpvsStatic, smodelLighting, 704);
	assert_offsetof(GfxWorldDpvsStatic, sunSurfVisDataCount, 740);
	assert_offsetof(GfxWorldDpvsStatic, constantBuffersAmbient, 768);

	struct GfxWorld
	{
		const char* name; // 0
		const char* baseName; // 8
		unsigned int bspVersion; // 16
		int planeCount; // 20
		int nodeCount; // 24
		unsigned int surfaceCount; // 28
		int skyCount; // 32
		void* skies; // 40
		unsigned int portalGroupCount; // 48
		unsigned int lastSunPrimaryLightIndex; // 52
		unsigned int primaryLightCount; // 56
		unsigned int primaryLightEnvCount; // 60
		unsigned int sortKeyLitDecal; // 64
		unsigned int sortKeyEffectDecal; // 68
		unsigned int sortKeyTopDecal; // 72
		unsigned int sortKeyEffectAuto; // 76
		unsigned int sortKeyDistortion; // 80
		unsigned int sortKeyUnknown; // 84
		unsigned int sortKeyUnknown2; // 88
		char __pad0[4]; // 92
		GfxWorldDpvsPlanes dpvsPlanes; // 96
		void* aabbTreeCounts; // 128
		void* aabbTrees; // 136
		void* cells; // 144
		void* portalGroup; // 152
		int unk_vec4_count_0; // 160
		char __pad1[4];
		vec4_t* unk_vec4_0; // 168

		//GfxWorldDraw draw; // 176
		//GfxLightGrid lightGrid; // 432
		char __pad2[1336]; // 176

		int modelCount; // 1512
		void* models; // 1520
		vec3_t mins1;
		vec3_t maxs1;
		vec3_t mins2;
		vec3_t maxs2;
		unsigned int checksum;
		int materialMemoryCount; // 1580
		void* materialMemory; // 1584
		sunflare_t sun; // 1592
		float outdoorLookupMatrix[4][4];
		GfxImage* outdoorImage; // 1768
		unsigned int* cellCasterBits; // 1776
		unsigned int* cellHasSunLitSurfsBits; // 1784
		void* sceneDynModel; // 1792
		void* sceneDynBrush; // 1800
		unsigned int* primaryLightEntityShadowVis; // 1808
		unsigned int* primaryLightDynEntShadowVis[2]; // 1816 1824
		unsigned short* nonSunPrimaryLightForModelDynEnt; // 1832
		void* shadowGeom; // 1840
		void* shadowGeomOptimized; // 1848
		void* lightRegion; // 1856

		GfxWorldDpvsStatic dpvs; // 1864
		//GfxWorldDpvsDynamic dpvsDyn; // 2648
		char __pad3[96]; // 2648

		unsigned int mapVtxChecksum; // 2744
		unsigned int heroOnlyLightCount; // 2748
		void* heroOnlyLights; // 2752
		unsigned char fogTypesAllowed; // 2760
		unsigned int umbraTomeSize; // 2764
		char* umbraTomeData; // 2768
		umbraTomePtr_t umbraTomePtr; // 2776
		unsigned int mdaoVolumesCount; // 2784
		void* mdaoVolumes; // 2792
		char __pad4[32];
		GfxBuildInfo buildInfo; // 2832
	}; assert_sizeof(GfxWorld, 0xB30);
	assert_offsetof(GfxWorld, skyCount, 32);
	assert_offsetof(GfxWorld, skies, 40);
	assert_offsetof(GfxWorld, dpvsPlanes, 96);
	assert_offsetof(GfxWorld, aabbTreeCounts, 128);
	assert_offsetof(GfxWorld, cells, 144);
	assert_offsetof(GfxWorld, portalGroup, 152);
	assert_offsetof(GfxWorld, unk_vec4_count_0, 160);
	assert_offsetof(GfxWorld, unk_vec4_0, 168);
	assert_offsetof(GfxWorld, __pad2, 176);
	//assert_offsetof(GfxWorld, lightGrid, 432);
	assert_offsetof(GfxWorld, modelCount, 1512);
	assert_offsetof(GfxWorld, models, 1520);
	assert_offsetof(GfxWorld, materialMemoryCount, 1580);
	assert_offsetof(GfxWorld, materialMemory, 1584);
	assert_offsetof(GfxWorld, sun, 1592);
	assert_offsetof(GfxWorld, outdoorImage, 1768);
	assert_offsetof(GfxWorld, cellCasterBits, 1776);
	assert_offsetof(GfxWorld, cellHasSunLitSurfsBits, 1784);
	assert_offsetof(GfxWorld, dpvs, 1864);
	assert_offsetof(GfxWorld, __pad3, 2648);
	assert_offsetof(GfxWorld, heroOnlyLightCount, 2748);
	assert_offsetof(GfxWorld, heroOnlyLights, 2752);
	assert_offsetof(GfxWorld, umbraTomeSize, 2764);
	assert_offsetof(GfxWorld, umbraTomeData, 2768);
	assert_offsetof(GfxWorld, umbraTomePtr, 2776);
	assert_offsetof(GfxWorld, mdaoVolumesCount, 2784);
	assert_offsetof(GfxWorld, mdaoVolumes, 2792);
	assert_offsetof(GfxWorld, mdaoVolumes, 2792);
	assert_offsetof(GfxWorld, buildInfo, 2832);

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
}
