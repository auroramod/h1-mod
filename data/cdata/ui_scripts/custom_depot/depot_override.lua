local get_function = custom_depot.get_function

GetCurrencyBalance = function(currency_type)
    return get_function("get_currency")(currency_type)
end

Depot.Inventory_GetCurrencyBalance = function(controller, currency_type)
    return get_function("get_currency")(currency_type)
end

local bundles_table = "mp/bundles.csv"
local supply_drops_table = "mp/supplydrops.csv"

local function find_row(file, col, value)
    value = tostring(value):lower()

    local rows = Engine.TableGetRowCount(file)
    for row = 0, rows - 1 do
        local cell = Engine.TableLookupByRow(file, row, col)
        if cell and cell:lower() == value then
            return row
        end
    end
end

local function is_supply_drop(guid)
    return find_row(supply_drops_table, 2, guid) ~= nil or find_row(supply_drops_table, 4, guid) ~= nil
end

local function purchase_bundle(row)
    local currency_type = tonumber(Engine.TableLookupByRow(bundles_table, row, 6)) or 0
    local price = tonumber(Engine.TableLookupByRow(bundles_table, row, 7)) or 0
    if currency_type ~= 0 and get_function("get_currency")(currency_type) < price then
        return false
    end

    get_function("remove_currency")(currency_type, price)
    get_function("add_currency")(tonumber(Engine.TableLookupByRow(bundles_table, row, 8)),
        tonumber(Engine.TableLookupByRow(bundles_table, row, 9)))

    local limit_item = Engine.TableLookupByRow(bundles_table, row, 4)
    if limit_item and limit_item ~= "" then
        get_function("add_quantity")(limit_item, 1)
    end

    for col = 10, 16, 2 do
        local guid = Engine.TableLookupByRow(bundles_table, row, col)
        local amount = tonumber(Engine.TableLookupByRow(bundles_table, row, col + 1)) or 1
        if guid and guid ~= "" then
            if is_supply_drop(guid) then
                get_function("add_supply_drops")(guid, amount)
            else
                get_function("add_item")(guid, true)
                get_function("add_quantity")(guid, amount)
            end
        end
    end

    return true
end

local function purchase_loot(row)
    local guid = Engine.TableLookupByRow(LootTable.File, row, LootTable.Cols.GUID)
    if get_function("has_item")(guid) then
        return true
    end

    local value = tonumber(Engine.TableLookupByRow(LootTable.File, row, LootTable.Cols.Value)) or 0
    if get_function("get_currency")(InventoryCurrencyType.Parts) < value then
        return false
    end

    get_function("remove_currency")(InventoryCurrencyType.Parts, value)
    get_function("add_item")(guid, true)

    if custom_depot.collection_details_menu then
        custom_depot.collection_details_menu:OnCraftedItem()
    end

    return true
end

local function purchase_item(purchase_id)
    local row = find_row(bundles_table, 2, purchase_id)
    if row then
        return purchase_bundle(row)
    end

    row = find_row(LootTable.File, LootTable.Cols.Purchase, purchase_id)
    if row then
        return purchase_loot(row)
    end

    get_function("add_item")(purchase_id, true)
    return true
end

Engine.Inventory_PurchaseItem = function(controller, purchase_id, quantity)
    local success = purchase_item(purchase_id)
    get_function("save_depot_data")()

    compat_mp.dispatch({
        name = "inventory",
        controller = controller,
        purchaseID = purchase_id,
        quantity = quantity,
        inventoryTaskType = 23,
        inventoryEventType = 3,
        success = success
    })

    return success
end

local get_item_quantity = Engine.Inventory_GetItemQuantity
Engine.Inventory_GetItemQuantity = function(controller, guid, ...)
    local quantity = get_item_quantity and get_item_quantity(controller, guid, ...) or 0
    quantity = math.max(quantity, get_function("get_quantity")(guid))

    if get_function("has_item")(guid) then
        quantity = math.max(quantity, 1)
    end

    return quantity
end

local consume_item = Engine.Inventory_ConsumeItem
Engine.Inventory_ConsumeItem = function(controller, guid, amount, ...)
    get_function("add_quantity")(guid, -(tonumber(amount) or 1))
    get_function("save_depot_data")()

    if consume_item then
        return consume_item(controller, guid, amount, ...)
    end
end

Engine.Loot_GetSupplyDropCount = function(controller, supply_drop_type)
    local guid = compat_mp.get_supply_drop_guid(supply_drop_type)
    if not guid or guid == "" then
        return 0
    end

    return get_function("get_supply_drop_count")(guid)
end

GetItemLockState_orig = Engine.GetItemLockState
Engine.GetItemLockState = function(controller, item_guid)
    if get_function("has_item")(item_guid) then
        return "Unlocked", 0, ""
    end

    return GetItemLockState_orig(controller, item_guid)
end

GetItemSet_orig = GetItemSet
GetItemSet = function(item_set_id)
    local item_set = GetItemSet_orig(item_set_id)
    local items_unlocked = 0

    for k, v in pairs(item_set.setItems) do
        if get_function("has_item")(v.guid) and (not v.isOwned or v.lockState == "Unlocked") then
            v.isOwned = true
            v.lockState = "Unlocked"
            items_unlocked = items_unlocked + 1
        end
    end

    if items_unlocked == #item_set.setItems then
        if not item_set.completed then
            item_set.completed = true
        end

        if not get_function("has_item")(item_set.setReward.guid) then
            get_function("add_item")(item_set.setReward.guid, true)
            get_function("save_depot_data")()

            if custom_depot.collection_details_menu then
                custom_depot.collection_details_menu:OnCompletedSet()
            end
        end
    end

    item_set.numOwned = items_unlocked
    return item_set
end

GetItemSets_orig = GetItemSets
GetItemSets = function()
    local item_sets = GetItemSets_orig()
    local completed_sets = 0

    for i = 1, #item_sets.seasons do
        local seasons_completed_sets = 0
        local sets = item_sets.seasons[i].sets
        local rewardData = item_sets.seasons[i].rewardData

        for i = 1, #sets do
            if sets[i].completed then
                completed_sets = completed_sets + 1
                seasons_completed_sets = seasons_completed_sets + 1
            end
        end

        if item_sets.seasons[i].completedSets == #sets then
            rewardData.setReward.isOwned = true
            rewardData.setReward.lockState = "Unlocked"
            rewardData.completed = true

            if not get_function("has_item")(rewardData.setReward.guid) then
                get_function("add_item")(rewardData.setReward.guid, true)
                get_function("save_depot_data")()
            end
        end

        item_sets.seasons[i].completedSets = seasons_completed_sets
    end

    for k, v in pairs(item_sets.itemToSetMap) do
        local items_unlocked = 0

        for i = 1, #v.setItems do
            if get_function("has_item")(v.setItems[i].guid) and
                (not v.setItems[i].isOwned or v.setItems[i].lockState == "Unlocked") then
                v.setItems[i].isOwned = true
                v.setItems[i].lockState = "Unlocked"
                items_unlocked = items_unlocked + 1
            end
        end

        if items_unlocked == #v.setItems then
            if not v.completed then
                v.completed = true
                completed_sets = completed_sets + 1
            end

            if not get_function("has_item")(v.setReward.guid) then
                get_function("add_item")(v.setReward.guid, true)
                get_function("save_depot_data")()
            end
        end

        v.numOwned = items_unlocked
    end

    item_sets.completedSets = completed_sets
    return item_sets
end

IsContentPromoUnlocked_orig = IsContentPromoUnlocked
IsContentPromoUnlocked = function()
    return true
end

TryShowCollectionCompleted_orig = TryShowCollectionCompleted
TryShowCollectionCompleted = function(controller, reward_data, unk1)
    if reward_data.completed then
        if not get_function("has_reward_splash")(reward_data.setReward.guid) then
            LUI.FlowManager.RequestAddMenu(nil, "MPDepotCollectionRewardSplash", true, controller, false, {
                collectionData = reward_data
            })

            if custom_depot.collection_details_menu then
                custom_depot.collection_details_menu:OnCompletedSet()
            end

            get_function("add_reward_splash")(reward_data.setReward.guid, true)
            get_function("save_depot_data")()
        end

        return true
    else
        return false
    end
end

TryShowSeasonCompleted_orig = TryShowSeasonCompleted
TryShowSeasonCompleted = function(controller, reward_data, unk1)
    if reward_data.completed then
        if not get_function("has_reward_splash")(reward_data.setReward.guid) then
            LUI.FlowManager.RequestAddMenu(nil, "MPDepotCollectionRewardSplash", true, controller, false, {
                collectionData = reward_data
            })

            if custom_depot.collection_details_menu then
                custom_depot.collection_details_menu:OnCompletedSet()
            end

            get_function("add_reward_splash")(reward_data.setReward.guid, true)
            get_function("save_depot_data")()
        end

        return true
    else
        return false
    end
end

MPDepotCollectionDetailsMenu_orig = LUI.MenuBuilder.m_types_build["MPDepotCollectionDetailsMenu"]
MPDepotCollectionDetailsMenu = function(unk1, unk2)
    custom_depot.collection_details_menu = MPDepotCollectionDetailsMenu_orig(unk1, unk2)
    return custom_depot.collection_details_menu
end
LUI.MenuBuilder.m_types_build["MPDepotCollectionDetailsMenu"] = MPDepotCollectionDetailsMenu

local loot_lists = {}

local function get_loot_list(stream)
    if not stream then
        return {}
    end

    if loot_lists[stream] then
        return loot_lists[stream]
    end

    local list = {}
    local drop_data = LUI.MPDepot.LootDropsData[stream]
    if drop_data then
        local items = LUI.MPLootDropsBase.GetGenericItemList(nil, drop_data.lootTableColName)
        for i = 1, #items do
            if not Cac.InventoryItemType or items[i].inventoryItemType == Cac.InventoryItemType.Loot then
                table.insert(list, items[i])
            end
        end

        if #list == 0 then
            list = items
        end
    end

    loot_lists[stream] = list
    return list
end

local function get_duplicate_parts(guid)
    local rarity = tonumber(Engine.TableLookup(LootTable.File, LootTable.Cols.GUID, guid, LootTable.Cols.Rarity))
    if rarity == ItemRarity.Common then
        return math.random(1, 75)
    elseif rarity == ItemRarity.Rare then
        return math.random(75, 155)
    elseif rarity == ItemRarity.Legendary then
        return math.random(155, 260)
    elseif rarity == ItemRarity.Epic then
        return math.random(260, 550)
    end

    return 0
end

local function pay_for_supply_drop(supply_drop_type)
    local guid = compat_mp.get_supply_drop_guid(supply_drop_type)
    if guid and guid ~= "" and get_function("get_supply_drop_count")(guid) > 0 then
        get_function("add_supply_drops")(guid, -1)
        return true
    end

    local price = compat_mp.get_supply_drop_price(supply_drop_type)
    if get_function("get_currency")(price.type) < price.amount then
        return false
    end

    get_function("remove_currency")(price.type, price.amount)
    return true
end

local function open_supply_drop(menu, event)
    local supply_drop_type = tostring(event.supplyDropType or menu.supplyDropType)
    local base_type = supply_drop_type:gsub("_cp$", "")
    local stream = LUI.MPDepot.SuppyDropLootStream[base_type] or LUI.MPDepot.SuppyDropLootStream[menu.crateType]
    local items_list = get_loot_list(stream)

    if #items_list == 0 or not pay_for_supply_drop(supply_drop_type) then
        return false
    end

    local count = base_type:find("_basic") and math.random(1, 2) or math.random(2, 3)
    for i = 1, count do
        event.items[i] = items_list[math.random(#items_list)].guid
    end

    for i = 1, #event.items do
        if not get_function("has_item")(event.items[i]) then
            get_function("add_item")(event.items[i], true)
        else
            local amount = get_duplicate_parts(event.items[i])
            table.insert(event.replacements, {
                item_index = i,
                currency = {
                    amount = amount
                }
            })

            get_function("add_currency")(InventoryCurrencyType.Parts, amount)
        end
    end

    get_function("save_depot_data")()
    return true
end

MPDepotOpenLootMenu_orig = LUI.MenuBuilder.m_types_build["MPDepotOpenLootMenu"]
MPDepotOpenLootMenu = function(unk1, unk2)
    local open_loot_menu = MPDepotOpenLootMenu_orig(unk1, unk2)

    local supply_drop_orig = open_loot_menu.m_eventHandlers["supply_drop"]
    open_loot_menu:registerEventHandler("supply_drop", function(menu, event)
        if event.transaction ~= nil and event.transaction == menu.supplyDropTransaction and not event.success then
            event.duplicateRefund = false
            event.items = {}
            event.currencies = {}
            event.replacements = {}
            event.cards = {}
            event.success = open_supply_drop(menu, event)
        end

        supply_drop_orig(menu, event)
    end)

    return open_loot_menu
end
LUI.MenuBuilder.m_types_build["MPDepotOpenLootMenu"] = MPDepotOpenLootMenu

AddLootDropTabSelector_orig = LUI.MPDepotBase.AddLootDropTabSelector
LUI.MPDepotBase.AddLootDropTabSelector = function(unk1, unk2)
    if not get_function("has_accepted_mod_eula")() then
        local item_sets = GetItemSets()

        unk1:AddButtonWithInfo("depot_collections", "@DEPOT_COLLECTIONS", "MPDepotCollectionsMenu", nil, nil,
            Engine.Localize("@MPUI_X_SLASH_Y", item_sets.completedSets, item_sets.numSets))

        unk1:AddButtonWithInfo("depot_armory", "@DEPOT_ARMORY", "MPDepotArmoryMenu")
        return
    end

    AddLootDropTabSelector_orig(unk1, unk2)
end

MPDepotMenu_orig = LUI.MenuBuilder.m_types_build["MPDepotMenu"]
MPDepotMenu = function(unk1, unk2)
    local depot_menu = MPDepotMenu_orig(unk1, unk2)

    if not get_function("has_seen_mod_eula")() then
        LUI.FlowManager.RequestAddMenu(nil, "mod_eula", true, 0, false, {
            acceptCallback = function()
                get_function("set_has_accepted_mod_eula")(true)
                get_function("set_has_seen_mod_eula")(true)
                LUI.FlowManager.RequestLeaveMenu(depot_menu)
            end,
            declineCallback = function()
                get_function("set_has_accepted_mod_eula")(false)
                get_function("set_has_seen_mod_eula")(true)
            end
        })
    end

    return depot_menu
end
LUI.MenuBuilder.m_types_build["MPDepotMenu"] = MPDepotMenu

GetLootDataForRef_orig = LUI.InventoryUtils.GetLootDataForRef
LUI.InventoryUtils.GetLootDataForRef = function(f13_arg0, f13_arg1, f13_arg2, f13_arg3, f13_arg4)
    local loot_data = GetLootDataForRef_orig(f13_arg0, f13_arg1, f13_arg2, f13_arg3, f13_arg4)

    if loot_data and get_function("has_item")(loot_data.guid) then
        loot_data.lockState = "Unlocked"
    end

    return loot_data
end
