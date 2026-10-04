Store = Store or {}
Depot = Depot or {}
Matchmaking = Matchmaking or {}
LobbyMember = LobbyMember or {}
Cac = Cac or {}

local dvar_defaults = {
	mm_dlc_map_ignore_chance = 0,
	mm_search_max_tier = 0,
	loot_taskSupplyDropTimeout = 30,
	vlDepotEnabled = false,
	vlDepotLoaded = false,
	extendedLoadoutsKillswitch = 1,
	csdRewardMasterKillswitch = 1,
	ui_enable_set_rewards = 0,
	scr_depotcreditscale = 1,
	scr_xpscalewithparty = 1,
	inventory_contentPromo = "",
	inventory_bundlePromo = "",
	inventory_contentPromoExpireTime = "0",
	ui_mouse_char_rot = 0
}

local dvar_fallbacks = {
	GetDvarFloat = 0,
	GetDvarInt = 0,
	GetDvarBool = false,
	GetDvarString = ""
}

for name, fallback in pairs(dvar_fallbacks) do
	local original = Engine[name]
	if original then
		Engine[name] = function(dvar, ...)
			local value = original(dvar, ...)
			if value ~= nil then
				return value
			end

			value = dvar_defaults[dvar]
			if value == nil then
				return fallback
			end

			if name == "GetDvarString" then
				return tostring(value)
			end

			if name == "GetDvarBool" then
				return value == true or value == 1
			end

			return tonumber(value) or fallback
		end
	end
end

local function defer(callback)
	local root = Engine.GetLuiRoot()
	if not root then
		callback()
		return
	end

	local holder = LUI.UIElement.new()
	holder:registerEventHandler("compat_deferred", function(element)
		element:close()
		callback()
	end)
	holder:addElement(LUI.UITimer.new(10, "compat_deferred", nil, true))
	root:addElement(holder)
end

local function dispatch(event)
	defer(function()
		local root = Engine.GetLuiRoot()
		if root then
			root:processEvent(event)
		end
	end)
end

compat_mp = {
	defer = defer,
	dispatch = dispatch
}

local function stub(module, name, value)
	if module and module[name] == nil then
		module[name] = value
	end
end

local function returns(value)
	return function()
		return value
	end
end

local noop = function()
end

stub(Engine, "GetDisplayDriverMeetsMinVer", returns(true))
stub(Engine, "IsDepotEnabled", returns(true))
stub(Engine, "IsCODAccountEnabled", returns(false))
stub(Engine, "SetCODAccountDebounce", noop)
stub(Engine, "CacheCharacterCamos", noop)
stub(Engine, "ControllerHasMap", returns(true))
stub(Engine, "IsControllerInUse", returns(true))
stub(Engine, "IsPS4SCEA", returns(false))
stub(Engine, "IsPS4SCEE", returns(false))
stub(Engine, "JoinSplitScreenParty", noop)
stub(Engine, "LeaveSplitScreenParty", noop)
stub(Engine, "MarketingRefreshMessages", noop)
stub(Engine, "SubtitlesEnabled", returns(false))
stub(Engine, "SetSubtitlesEnabled", noop)
stub(Engine, "ShaderUploadFrontendSystemIsAvailable", returns(false))
stub(Engine, "PlayStreamingVideo", noop)
stub(Engine, "StopStreamingVideo", noop)
stub(Engine, "SetViewport", noop)
stub(Engine, "FlashFade", noop)
stub(Engine, "TableLookupFromStart", Engine.TableLookup)
stub(Engine, "GetPlayerDataRecentUnlocks", returns(""))
stub(Engine, "HasExtendedLoadouts", returns(false))
stub(Engine, "SetPlayerDataEx", Engine.SetPlayerData)

stub(Engine, "Exit", function()
	Engine.Exec("quit")
end)

stub(Engine, "TimeUntilPromoExpires", function()
	local expire_time = tonumber(Engine.GetDvarString("inventory_contentPromoExpireTime")) or 0
	return math.max(0, expire_time - Engine.GetTimeUTC())
end)

local extended_data = {}

local function extended_key(...)
	local args = {...}
	for i = 1, #args do
		args[i] = tostring(args[i])
	end

	return table.concat(args, ".")
end

stub(Engine, "SetPlayerDataExtendedEx", function(controller, group, ...)
	local args = {...}
	local value = table.remove(args)
	extended_data[extended_key(controller, group, unpack(args))] = value
end)

stub(Engine, "GetPlayerDataExtendedEx", function(controller, group, classes, slot, ...)
	local value = extended_data[extended_key(controller, group, classes, slot, ...)]
	if value ~= nil then
		return value
	end

	return Engine.GetPlayerData(controller, group, classes, 0, ...)
end)

local supply_drops_table = "mp/supplydrops.csv"

function compat_mp.get_supply_drop_guid(supply_drop_type)
	local name = tostring(supply_drop_type)
	local cod_points = name:find("_cp$") ~= nil
	if cod_points then
		name = name:sub(1, -4)
	end

	return Engine.TableLookup(supply_drops_table, 1, name, cod_points and 4 or 2)
end

function compat_mp.get_supply_drop_price(supply_drop_type)
	local name = tostring(supply_drop_type)
	if name:find("_cp$") then
		return {
			type = InventoryCurrencyType and InventoryCurrencyType.CoDPoints or 4,
			amount = tonumber(Engine.TableLookup(supply_drops_table, 1, name:sub(1, -4), 5)) or 0
		}
	end

	return {
		type = InventoryCurrencyType and InventoryCurrencyType.Credits or 2,
		amount = tonumber(Engine.TableLookup(supply_drops_table, 1, name, 3)) or 0
	}
end

local get_supply_drop_price = Engine.Loot_GetSupplyDropPrice
Engine.Loot_GetSupplyDropPrice = function(supply_drop_type, ...)
	if type(supply_drop_type) == "number" and get_supply_drop_price then
		return get_supply_drop_price(supply_drop_type, ...)
	end

	return compat_mp.get_supply_drop_price(supply_drop_type)
end

local can_open_supply_drop = Engine.Loot_CanOpenSupplyDrop
Engine.Loot_CanOpenSupplyDrop = function(controller, supply_drop_type, ...)
	if type(supply_drop_type) == "number" and can_open_supply_drop then
		return can_open_supply_drop(controller, supply_drop_type, ...)
	end

	return true
end

Engine.Loot_OpenSupplyDrop = function(controller, supply_drop_type, transaction)
	dispatch({
		name = "supply_drop",
		controller = controller,
		supplyDropType = supply_drop_type,
		transaction = transaction,
		success = false,
		duplicateRefund = false,
		items = {},
		currencies = {},
		replacements = {},
		cards = {}
	})
end

Engine.Inventory_PurchaseItem = function(controller, purchase_id, quantity)
	dispatch({
		name = "inventory",
		controller = controller,
		purchaseID = purchase_id,
		quantity = quantity,
		inventoryTaskType = 23,
		inventoryEventType = 3,
		success = true
	})

	return true
end

stub(Engine, "Loot_GetSupplyDropCount", returns(0))
stub(Engine, "Loot_HasUnviewedSupplyDrop", returns(false))
stub(Engine, "Loot_GetUnviewedSupplyDrop", returns(nil))
stub(Engine, "Loot_MarkSupplyDropOpened", noop)
stub(Engine, "Loot_SetWeeklyRewardViewed", noop)
stub(Engine, "Loot_GetWeeklyReward", function()
	return {
		viewed = true,
		items = {}
	}
end)

local item_sets_table = "mp/itemsets.csv"
local item_sets_cache = {}

stub(Engine, "Loot_GetItemSets", function(drop, promo)
	promo = tonumber(promo) or 0
	local key = tostring(drop) .. ":" .. promo
	if item_sets_cache[key] then
		return item_sets_cache[key]
	end

	local sets = {}
	local rows = Engine.TableGetRowCount(item_sets_table)
	for row = 0, rows - 1 do
		local id = tonumber(Engine.TableLookupByRow(item_sets_table, row, 0))
		if id and Engine.TableLookupByRow(item_sets_table, row, 13) == drop
			and (tonumber(Engine.TableLookupByRow(item_sets_table, row, 3)) or 0) == promo then
			local set = {
				setID = id,
				name = "@" .. Engine.TableLookupByRow(item_sets_table, row, 2),
				reward = Engine.TableLookupByRow(item_sets_table, row, 4),
				items = {}
			}

			for col = 5, 12 do
				local item = Engine.TableLookupByRow(item_sets_table, row, col)
				if item ~= nil and item ~= "" then
					table.insert(set.items, item)
				end
			end

			table.insert(sets, set)
		end
	end

	item_sets_cache[key] = sets
	return sets
end)

stub(Depot, "Inventory_GetCurrencyBalance", returns(0))

stub(Store, "GetStoreRegion", returns(0))
stub(Store, "PurchaseOffer", noop)
stub(Store, "IsReady", returns(false))
stub(Store, "FoundPlayerPurchases", returns(true))
stub(Store, "FindPlayerPurchases", noop)
stub(Store, "ClearContentCache", noop)
stub(Store, "FetchContentForCategory", noop)
stub(Store, "GetCategories", function()
	return {}
end)
stub(Store, "GetNumItemsInCategory", returns(0))
stub(Store, "GetCompleteItemInfoByIndex", returns(nil))
stub(Store, "IsItemPurchasedByIndex", returns(false))

stub(Friends, "IsRecentPlayerInvitable", returns(false))
stub(Friends, "IsRecentPlayerJoinable", returns(false))
stub(Friends, "IsLivePartyFriendInvitable", returns(false))
stub(Friends, "IsLivePartyFriendJoinable", returns(false))
stub(Friends, "IsLivePartyFriendMe", returns(false))
stub(Friends, "IsLivePartyLocal", returns(false))
stub(Friends, "InviteAllLiveParty", noop)
stub(Friends, "InviteLivePartyFriend", noop)
stub(Friends, "JoinLivePartyFriend", noop)
stub(Friends, "OpenLivePartyUI", noop)
stub(Friends, "ShowLivePartyFriendGamercard", noop)

stub(Lobby, "GetCurrencyEarnedLastGame", returns(0))
stub(Lobby, "GetPrivateMatchTeam", returns(0))
stub(Lobby, "GetMapCustomField", returns(""))
stub(LobbyMember, "SelectedMember_SetLocalPrivateMatchTeam", noop)

stub(Playlist, "GetPlaylistXpScaleWithParty", function(...)
	return Playlist.GetPlaylistXpScale and Playlist.GetPlaylistXpScale(...) or 1
end)
stub(Playlist, "GetPlaylistDepotCreditScale", returns(1))

stub(Matchmaking, "CanAddSearch", returns(false))
stub(Matchmaking, "AddSearch", noop)
stub(Matchmaking, "IsExplicitDLCPlaylist", returns(true))
stub(Matchmaking, "SetIgnoredDLCMapMask", noop)
stub(Matchmaking, "GetNextIgnoredDLCMapMask", function(mask)
	return mask
end)

stub(Cac, "GetActiveSquadMember", returns(0))
stub(Cac, "GetSquadLoc", returns(0))
stub(Cac, "GetSquadMemberName", returns(""))

if Engine then
	Engine.UsingSplitscreenUpscaling = Engine.UsingSplitscreenUpscaling or function ()
		return false
	end

	Engine.IsNetworkConnected = Engine.IsNetworkConnected or function ()
		return true
	end

	Engine.HasAcceptedInvite = Engine.HasAcceptedInvite or function ()
		return false
	end

	Engine.SetAndEnableCustomClanTag = Engine.SetAndEnableCustomClanTag or function ()

	end

	Engine.CanViewClanTags = Engine.CanViewClanTags or function ()
		return true
	end

	Engine.IsUserUGCRestricted = Engine.IsUserUGCRestricted or function ()
		return false
	end

	Engine.AllowOnline = Engine.AllowOnline or function ()
		return true
	end

	Engine.IsSpecialRegion = Engine.IsSpecialRegion or function ()
		return false
	end

	Engine.HasSnapshot = Engine.HasSnapshot or function ()
		return true
	end

	Engine.ClearCustomClanTag = Engine.ClearCustomClanTag or function ()

	end

	Engine.GetCurrentDayMonthYear = Engine.GetCurrentDayMonthYear or function ()
		return 1, 1, 2100
	end

	Engine.GetCurrentYear = Engine.GetCurrentYear or function ()
		return 2100
	end

	Engine.UserCanAccessFriendsList = Engine.UserCanAccessFriendsList or function ()
		return false
	end

	Engine.AnyoneHasSeasonPass = Engine.AnyoneHasSeasonPass or function ()
		return false
	end

	Engine.IsProfanity = Engine.IsProfanity or function ()
		return false
	end

	Engine.IsChatRestricted = Engine.IsChatRestricted or function ()
		return false
	end

	Engine.UserCanAccessStore = Engine.UserCanAccessStore or function ()
		return false
	end

	Engine.UserIsGuest = Engine.UserIsGuest or function ()
		return false
	end

	Engine.ShowXB3GoldUpsell = Engine.ShowXB3GoldUpsell or function ()
		return false
	end

	Engine.DoWeHaveStats = Engine.DoWeHaveStats or function ()
		return false
	end

	Engine.FormatTimeHoursMinutesSeconds = Engine.FormatTimeHoursMinutesSeconds or function ()
		return "Time here"
	end

	Engine.FormatTimeDaysHoursMinutesSeconds = Engine.FormatTimeDaysHoursMinutesSeconds or function ()
		return "Time here"
	end

	Engine.FormatTimeDaysHoursMinutesSecondsTight = Engine.FormatTimeDaysHoursMinutesSecondsTight or function ()
		return "Time here"
	end

	Engine.FormatTimeDaysHoursMinutesTight = Engine.FormatTimeDaysHoursMinutesTight or function ()
		return "Time here"
	end

	Engine.GetFormattedTime = Engine.GetFormattedTime or function ()
		return "Time here"
	end

	Engine.GetTimeUTC = Engine.GetTimeUTC or function ()
		return 0
	end

	Engine.GetTimeMsecs = Engine.GetTimeMsecs or function ()
		return 0
	end

	Engine.IsPC = Engine.IsPC or function ()
		return false
	end

	Engine.GetControllerForLocalClient = Engine.GetControllerForLocalClient or function ()
		return 0
	end

	Engine.PartyEveryoneHasMap = Engine.PartyEveryoneHasMap or function ()
		return true
	end

	Engine.IsActiveLocalClientPrimary = Engine.IsActiveLocalClientPrimary or function ()
		return true
	end

	Engine.HasSaveDevice = Engine.HasSaveDevice or function ()
		return true
	end

	Engine.SetPlayerCostumeFieldUpdate = Engine.SetPlayerCostumeFieldUpdate or function ()
		return true
	end

	Engine.ToUpperCase = Engine.ToUpperCase or function ( f37_arg0 )
		return "UPPER: " .. f37_arg0
	end

	Engine.Inventory_GetAllItems = Engine.Inventory_GetAllItems or function ()
		return {}
	end

	Engine.GetCombatRecordWeaponStatsData = Engine.GetCombatRecordWeaponStatsData or function ()
		return nil
	end

	Engine.LocalClientProfileCanSave = Engine.LocalClientProfileCanSave or function ()
		return true
	end

	Engine.GetAspectRatio = Engine.GetAspectRatio or function ()
		return 1.78
	end

	Engine.AnyoneHasBadReputation = Engine.AnyoneHasBadReputation or function ()
		return false
	end

	Engine.EmblemIsLayerLinked = Engine.EmblemIsLayerLinked or function ()
		return false
	end

	Engine.SetSelectedLayerColor = Engine.SetSelectedLayerColor or function ()

	end

	Engine.UsingStreamingInstall = Engine.UsingStreamingInstall or function ()
		return false
	end

	Engine.Help = Engine.Help or function ()

	end

	Engine.ForceUpdateArenas = Engine.ForceUpdateArenas or function ()

	end

	Engine.IsMainThread = Engine.IsMainThread or function ()
		return true
	end

	Engine.IsRightToLeftLanguage = Engine.IsRightToLeftLanguage or function ()
		return false
	end

	Engine.ShouldPromptForLanguage = Engine.ShouldPromptForLanguage or function ()
		return false
	end

	Engine.GetSupportedLanguages = Engine.GetSupportedLanguages or function ()
		return {
			id = 0,
			name = Engine.Localize( "@MENU_ENGLISH" )
		}
	end

	Engine.SetLanguage = Engine.SetLanguage or function ()

	end

	Engine.IsLanguageChoiceAllowed = Engine.IsLanguageChoiceAllowed or function ()
		return false
	end

	Engine.ShowSubtitlesOption = Engine.ShowSubtitlesOption or function ()
		return true
	end

	Engine.CheckPlayerData = Engine.CheckPlayerData or function ()
		return false
	end

	Engine.GetLootWeaponBaseName = Engine.GetLootWeaponBaseName or function ( f56_arg0 )
		return f56_arg0
	end

	Engine.GetOverflowLootGuid = Engine.GetOverflowLootGuid or function ()
		return "0x0"
	end

	Engine.SetOverflowLootGuid = Engine.SetOverflowLootGuid or function ()

	end

	Engine.Inventory_GetArmorySlotsUsed = Engine.Inventory_GetArmorySlotsUsed or function ()
		return 0
	end

	Engine.Inventory_GetItemTypeByReference = Engine.Inventory_GetItemTypeByReference or function ()
		return 0
	end

	Engine.IsAnyUserUGCRestricted = Engine.IsAnyUserUGCRestricted or function ()
		return false
	end

	Engine.GetFormattedTag = Engine.GetFormattedTag or function ()
		return ""
	end

	Engine.Inventory_IsItemUsableForPlayer = Engine.Inventory_IsItemUsableForPlayer or function ()
		return false
	end

	Engine.GetRankedPromotionSlot = Engine.GetRankedPromotionSlot or function ()
		return 0
	end

	Engine.LookupStatsTableColumnForGUID = Engine.LookupStatsTableColumnForGUID or function ()
		return ""
	end

	Engine.LookupCostumeTableColumnForGUID = Engine.LookupCostumeTableColumnForGUID or function ()
		return ""
	end

	Engine.LookupCostumeOverrideTableColumnForGUID = Engine.LookupCostumeOverrideTableColumnForGUID or function ()
		return ""
	end

	Engine.GetItemGUIDFromReference = Engine.GetItemGUIDFromReference or function ()
		return 0
	end

	Engine.GetRawItemGUID = Engine.GetRawItemGUID or function ()
		return 0
	end

	Engine.IsDownloadOfAllEmblemsFinished = Engine.IsDownloadOfAllEmblemsFinished or function ()
		return true
	end

	Engine.IsCoopMPMode = Engine.IsCoopMPMode or function ()
		return false
	end

	Engine.PopupErrorOnBlockedClanTag = Engine.PopupErrorOnBlockedClanTag or function ()
		return false
	end

	Engine.IsCurrentLanguageJapanese = Engine.IsCurrentLanguageJapanese or function ()
		return false
	end

	Engine.Inventory_GetItemExpirationType = Engine.Inventory_GetItemExpirationType or function ()
		return 0
	end

	Engine.PlatformXUIDToHexXUID = Engine.PlatformXUIDToHexXUID or function ( f75_arg0 )
		return f75_arg0
	end

	Engine.GetIntroMovieViewed = Engine.GetIntroMovieViewed or function ( f76_arg0, f76_arg1 )
		return false
	end

	Engine.Content_IsEnumerationDone = Engine.Content_IsEnumerationDone or function ()
		return true
	end

	Engine.DLC_CanPurchase = Engine.DLC_CanPurchase or function ()
		return false
	end

	Engine.DLC_CanPurchasePackName = Engine.DLC_CanPurchasePackName or function ()
		return false
	end

	Engine.DLC_GetFirstName = Engine.DLC_GetFirstName or function ()
		return ""
	end

	Engine.DLC_GetPackName = Engine.DLC_GetPackName or function ()
		return nil
	end

	Engine.Inventory_IsItemLootRoll = Engine.Inventory_IsItemLootRoll or function ()
		return false
	end

	Engine.PartyMemberMissingMapPack = Engine.PartyMemberMissingMapPack or function ()
		return false, "", ""
	end

	Engine.GetAnyNewCACItemBreadcrumbState = Engine.GetAnyNewCACItemBreadcrumbState or function ()
		return false
	end

	Engine.GetNewItemCategoryBreadcrumbState = Engine.GetNewItemCategoryBreadcrumbState or function ()
		return false
	end

	Engine.GetNewItemBreadcrumbState = Engine.GetNewItemBreadcrumbState or function ()
		return false
	end

	Engine.SetNewItemBreadcrumbState = Engine.SetNewItemBreadcrumbState or function ()

	end

	Engine.HasActiveEmblem = Engine.HasActiveEmblem or function ()
		return false
	end

	Engine.HasAcceptedEULA = Engine.HasAcceptedEULA or function ()
		return true
	end

	Engine.AcceptEULA = Engine.AcceptEULA or function ()

	end

	Engine.WantsDisplayKillstreakCounter = Engine.WantsDisplayKillstreakCounter or function ()
		return true
	end

	Engine.DisplayKillstreakCounterToggle = Engine.DisplayKillstreakCounterToggle or function ()

	end

	Engine.WantsDisplayMedalSplashes = Engine.WantsDisplayMedalSplashes or function ()
		return true
	end

	Engine.DisplayMedalSplashesToggle = Engine.DisplayMedalSplashesToggle or function ()

	end

	Engine.ShouldDisplayWeaponEmblems = Engine.ShouldDisplayWeaponEmblems or function ()

	end

	Engine.DisplayWeaponEmblemsToggle = Engine.DisplayWeaponEmblemsToggle or function ()

	end

end
if Game then
	if not Engine.IsConsoleGame() then
		local f0_local0 = Game.GetNumPlayersOnTeam
		Game.GetNumPlayersOnTeam = function ( f97_arg0 )
			return f0_local0( f97_arg0 )
		end

	end
	Game.IsKillCamEntityActive = Game.IsKillCamEntityActive or function ()
		return false
	end

	Game.IsSpectatorCameraActive = Game.IsSpectatorCameraActive or function ()
		return false
	end

	Game.StartBlur = Game.StartBlur or function ()

	end

	Game.GetMapDisplayName = Game.GetMapDisplayName or function ()
		return "OINK"
	end

	Game.IsStunned = Game.IsStunned or function ()
		return false
	end

	Game.GetPlayerWeaponPrimaryFireType = Game.GetPlayerWeaponPrimaryFireType or function ()
		return 0
	end

	Game.IsWeaponOverheated = Game.IsWeaponOverheated or function ()
		return false
	end

	Game.GetPlayerFragWeapon = Game.GetPlayerFragWeapon or function ()
		return "none"
	end

	Game.GetPlayerSmokeWeapon = Game.GetPlayerSmokeWeapon or function ()
		return "none"
	end

	Game.GetPlayerWeaponBaseName = Game.GetPlayerWeaponBaseName or function ()

	end

	Game.IsWeaponInAltMode = Game.IsWeaponInAltMode or function ()
		return false
	end

	Game.AnyUnderbarrelWeaponEquipped = Game.AnyUnderbarrelWeaponEquipped or function ()
		return false
	end

	Game.IsRadarEnabled = Game.IsRadarEnabled or function ()
		return false
	end

	Game.GetPerkIndexForName = Game.GetPerkIndexForName or function ()
		return 0
	end

	Game.SpectatingThirdPerson = Game.SpectatingThirdPerson or function ()
		return false
	end

end
if Store then
	Store.RequestContentServerImages = Store.RequestContentServerImages or function ()

	end

	Store.GetCategoryForDLCName = Store.GetCategoryForDLCName or function ()
		return 0
	end

	Store.OneClickPurchase_ShowProductPicker = Store.OneClickPurchase_ShowProductPicker or function ()

	end

	Store.OneClickPurchase_ShowPurchaseDialog = Store.OneClickPurchase_ShowPurchaseDialog or function ()

	end

	Store.GetProductIdsForDlcNames = Store.GetProductIdsForDlcNames or function ()

	end

	Store.IsOneClickPurchaseEnabled = Store.IsOneClickPurchaseEnabled or function ()
		return false
	end

	Store.ShowEmptyStoreDialog = Store.ShowEmptyStoreDialog or function ()

	end

	Store.IsReady = Store.IsReady or function ()
		return false
	end

	Store.GetProductGroupInfo = Store.GetProductGroupInfo or function ()
		return {}
	end

	Store.GetAnyNewStoreItemBreadcrumbState = Store.GetAnyNewStoreItemBreadcrumbState or function ()
		return false
	end

	Store.GetNewStoreItemCategoryBreadcrumbState = Store.GetNewStoreItemCategoryBreadcrumbState or function ()
		return false
	end

	Store.GetNewStoreItemBreadcrumbState = Store.GetNewStoreItemBreadcrumbState or function ()
		return true
	end

	Store.SetNewStoreItemBreadcrumbState = Store.SetNewStoreItemBreadcrumbState or function ()

	end

end
if Friends then
	Friends.HasPartyGameInvite = Friends.HasPartyGameInvite or function ()
		return false
	end

	Friends.AcceptLivePartyInvitation = Friends.AcceptLivePartyInvitation or function ()

	end

	Friends.CanShowFriendGamercard = Friends.CanShowFriendGamercard or function ()
		return true
	end

	Friends.GetEliteClanFriendMemberStatus = Friends.GetEliteClanFriendMemberStatus or function ( f129_arg0, f129_arg1 )
		return ""
	end

	Friends.IsUserInBlockList = Friends.IsUserInBlockList or function ( f130_arg0 )
		return false
	end

	Friends.GetOnlineFriendXUID = Friends.GetOnlineFriendXUID or function ( f131_arg0, f131_arg1 )
		return "0"
	end

	Friends.GetRecentPlayerXUID = Friends.GetRecentPlayerXUID or function ( f132_arg0, f132_arg1 )
		return "0"
	end

	Friends.GetLivePartyFriendXUID = Friends.GetLivePartyFriendXUID or function ( f133_arg0, f133_arg1 )
		return "0"
	end

	Friends.GetEliteClanFriendXUID = Friends.GetEliteClanFriendXUID or function ( f134_arg0, f134_arg1 )
		return "0"
	end

	Friends.GetEliteClanFriendMemberStatus = Friends.GetEliteClanFriendMemberStatus or function ( f135_arg0, f135_arg1 )
		return ""
	end

end
if Squad then
	Squad.PostMatch = Squad.PostMatch or function ()
		return ""
	end

	Squad.GetHostSquadName = Squad.GetHostSquadName or function ()
		return ""
	end

	Squad.UpdateReportPlayerCardCache = Squad.UpdateReportPlayerCardCache or function ()
		return ""
	end

	Squad.GetReportPlayercard = Squad.GetReportPlayercard or function ()

	end

	Squad.GetClanTagForReport = Squad.GetClanTagForReport or function ()
		return ""
	end

	Squad.GetCompareInfoForLobby = Squad.GetCompareInfoForLobby or function ()
		return {}
	end

end
if Leaderboards then
	Leaderboards.GetPlayerValue = Leaderboards.GetPlayerValue or function ( f142_arg0, f142_arg1 )
		return ""
	end

	Leaderboards.GetValue = Leaderboards.GetValue or function ( f143_arg0, f143_arg1, f143_arg2 )
		return ""
	end

	Leaderboards.GetOffset = Leaderboards.GetOffset or function ( f144_arg0 )
		return 0
	end

	Leaderboards.GetCurrentIndex = Leaderboards.GetCurrentIndex or function ( f145_arg0 )
		return 0
	end

	Leaderboards.UpdateLeaderboard = Leaderboards.UpdateLeaderboard or function ( f146_arg0, f146_arg1 )

	end

	Leaderboards.UpdateCurrentIndex = Leaderboards.UpdateCurrentIndex or function ( f147_arg0, f147_arg1 )

	end

	Leaderboards.OnSelect = Leaderboards.OnSelect or function ( f148_arg0 )

	end

end
if Lobby then
	Lobby.ShowMemberInfo = Lobby.ShowMemberInfo or function ( f149_arg0, f149_arg1, f149_arg2 )
		return true
	end

	Lobby.UnsetAllMLGSpectators = Lobby.UnsetAllMLGSpectators or function ()

	end

	Lobby.EnteringLobby = Lobby.EnteringLobby or function ()
		return false
	end

	Lobby.EnteredLobby = Lobby.EnteredLobby or function ()

	end

	Lobby.GetPlayerLimit = Lobby.GetPlayerLimit or function ()
		return -1
	end

	Lobby.SetPlayerLimit = Lobby.SetPlayerLimit or function ()

	end

	Lobby.IsUsingMLGRules = Lobby.IsUsingMLGRules or function ()
		return false
	end

	Lobby.SetUsingMLGRules = Lobby.SetUsingMLGRules or function ( f156_arg0 )

	end

	Lobby.CreateCostumeGuid = Lobby.CreateCostumeGuid or function ()
		return 0
	end

	Lobby.GetIPAddress = Lobby.GetIPAddress or function ()
		return ""
	end

	Lobby.GetGeographicRegion = Lobby.GetGeographicRegion or function ()
		return ""
	end

	Lobby.GetBandwidth = Lobby.GetBandwidth or function ()
		return ""
	end

	Lobby.IsOnWifi = Lobby.IsOnWifi or function ()
		return false
	end

end
if LobbyMember then
	LobbyMember.SelectedMember_SetLocalMemberMLGSpectator = LobbyMember.SelectedMember_SetLocalMemberMLGSpectator or function ()

	end

end
Clan = Clan or {}
Clan.IsEnabled = Clan.IsEnabled or function ()
	return false
end

Clan.IsDownloadingData = Clan.IsDownloadingData or function ()
	return false
end

Clan.GetCurrentClanWarName = Clan.GetCurrentClanWarName or function ()
	return ""
end

Clan.GetClanInfo = Clan.GetClanInfo or function ()
	return {}
end

Clan.GetProposalInfo = Clan.GetProposalInfo or function ()
	return {}
end

Clan.GetClanWarInfo = Clan.GetClanWarInfo or function ()
	return {}
end

Clan.GetIntelData = Clan.GetIntelData or function ()
	return {}
end

Clan.GetRemoteClanIntelData = Clan.GetRemoteClanIntelData or function ()
	return {}
end

Clan.IsEntitlementUnlocked = Clan.IsEntitlementUnlocked or function ()
	return false
end

Clan.GetRemoteName = Clan.GetRemoteName or function ()
	return ""
end

Clan.CanAccessClanManage = Clan.CanAccessClanManage or function ()
	return false
end

Clan.GetTokenCountAsString = Clan.GetTokenCountAsString or function ()
	return "0"
end

Clan.SavePlayerEmblemToClan = Clan.SavePlayerEmblemToClan or function ()

end

MLG = MLG or {}
MLG.HighlightClientNum = MLG.HighlightClientNum or function ()

end

MLG.IsThirdPerson = MLG.IsThirdPerson or function ()

end

CoDAnywhere = CoDAnywhere or {}
CoDAnywhere.HasUCDSaveGame = CoDAnywhere.HasUCDSaveGame or function ()
	return false
end

CoDAnywhere.HasUCDAccount = CoDAnywhere.HasUCDAccount or function ()
	return false
end

CoDAnywhere.ServiceAvailable = CoDAnywhere.ServiceAvailable or function ()
	return true
end

Playlist = Playlist or {}
Playlist.GetPlaylistIdFromNum = Playlist.GetPlaylistIdFromNum or function ()
	return 1
end

Playlist.GetPartyMissingMapPacks = Playlist.GetPartyMissingMapPacks or function ()
	return false, "", ""
end

if Playlist then
	Playlist.GetCanShowItem = Playlist.GetCanShowItem or function ()
		return true
	end

end
MatchRules = MatchRules or {}
MatchRules.SelectSaveDevice = MatchRules.SelectSaveDevice or function ()

end

MatchRules.HasSelectedSaveDevice = MatchRules.HasSaveDevice or function ()
	return true
end

MatchRules.HasMLGRecipeInFastFile = MatchRules.HasMLGRecipeInFastFile or function ()
	return false
end

if Engine.InFrontend() and not LUI.MPDepotBase then
	local success, err = pcall(require, "LUI.mp_menus.depot")
	if not success then
		print("[compat_mp] failed to load depot: " .. tostring(err))
	end

	EnableGlobals()
end
