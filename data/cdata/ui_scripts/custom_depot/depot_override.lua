GetCurrencyBalance = function(currency_type)
    return custom_depot.get_function("get_currency")(currency_type)
end

Inventory_PurchaseItem_orig = Engine.Inventory_PurchaseItem
Engine.Inventory_PurchaseItem = function(controller, item_guid, unk2)
    if not custom_depot.get_function("has_item")(item_guid) then
        custom_depot.get_function("add_item")(item_guid, true)

        local item_value = Engine.TableLookup(LootTable.File, LootTable.Cols.GUID, item_guid, LootTable.Cols.Value)
        custom_depot.get_function("remove_currency")(InventoryCurrencyType.Parts, item_value)

        custom_depot.get_function("save_depot_data")()

        if custom_depot.collection_details_menu then
            custom_depot.collection_details_menu:OnCraftedItem()
        end
    end

    return Inventory_PurchaseItem_orig(controller, item_guid, unk2)
end

GetItemLockState_orig = Engine.GetItemLockState
Engine.GetItemLockState = function(controller, item_guid)
    if custom_depot.get_function("has_item")(item_guid) then
        return "Unlocked", 0, ""
    end

    return GetItemLockState_orig(controller, item_guid)
end

GetItemSet_orig = GetItemSet
GetItemSet = function(item_set_id)
    local item_set = GetItemSet_orig(item_set_id)
    local items_unlocked = 0

    for k, v in pairs(item_set.setItems) do
        if custom_depot.get_function("has_item")(v.guid) and (not v.isOwned or v.lockState == "Unlocked") then
            v.isOwned = true
            v.lockState = "Unlocked"
            items_unlocked = items_unlocked + 1
        end
    end

    if items_unlocked == #item_set.setItems then
        if not item_set.completed then
            item_set.completed = true
        end

        if not custom_depot.get_function("has_item")(item_set.setReward.guid) then
            custom_depot.get_function("add_item")(item_set.setReward.guid, true)
            custom_depot.get_function("save_depot_data")()

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

            if not custom_depot.get_function("has_item")(rewardData.setReward.guid) then
                custom_depot.get_function("add_item")(rewardData.setReward.guid, true)
                custom_depot.get_function("save_depot_data")()
            end
        end

        item_sets.seasons[i].completedSets = seasons_completed_sets
    end

    for k, v in pairs(item_sets.itemToSetMap) do
        local items_unlocked = 0

        for i = 1, #v.setItems do
            if custom_depot.get_function("has_item")(v.setItems[i].guid) and
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

            if not custom_depot.get_function("has_item")(v.setReward.guid) then
                custom_depot.get_function("add_item")(v.setReward.guid, true)
                custom_depot.get_function("save_depot_data")()
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
        if not custom_depot.get_function("has_reward_splash")(reward_data.setReward.guid) then
            LUI.FlowManager.RequestAddMenu(nil, "MPDepotCollectionRewardSplash", true, controller, false, {
                collectionData = reward_data
            })

            if custom_depot.collection_details_menu then
                custom_depot.collection_details_menu:OnCompletedSet()
            end

            custom_depot.get_function("add_reward_splash")(reward_data.setReward.guid, true)
            custom_depot.get_function("save_depot_data")()
        end

        return true
    else
        return false
    end
end

TryShowSeasonCompleted_orig = TryShowSeasonCompleted
TryShowSeasonCompleted = function(controller, reward_data, unk1)
    if reward_data.completed then
        if not custom_depot.get_function("has_reward_splash")(reward_data.setReward.guid) then
            LUI.FlowManager.RequestAddMenu(nil, "MPDepotCollectionRewardSplash", true, controller, false, {
                collectionData = reward_data
            })

            if custom_depot.collection_details_menu then
                custom_depot.collection_details_menu:OnCompletedSet()
            end

            custom_depot.get_function("add_reward_splash")(reward_data.setReward.guid, true)
            custom_depot.get_function("save_depot_data")()
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

MPDepotOpenLootMenu_orig = LUI.MenuBuilder.m_types_build["MPDepotOpenLootMenu"]
MPDepotOpenLootMenu = function(unk1, unk2)
    local open_loot_menu = MPDepotOpenLootMenu_orig(unk1, unk2)

    local supply_drop_orig = open_loot_menu.m_eventHandlers["supply_drop"]
    open_loot_menu:registerEventHandler("supply_drop", function(f48_arg0, f48_arg1)
        f48_arg1.success = true
        f48_arg1.transaction = f48_arg0.supplyDropTransaction
        f48_arg1.duplicateRefund = false
        f48_arg1.items = {}
        f48_arg1.currencies = {}
        f48_arg1.replacements = {}
        f48_arg1.cards = {}

        local supply_drop_price = LUI.MPDepot.GetSupplyDropPrice(f48_arg0.supplyDropType)
        custom_depot.get_function("remove_currency")(supply_drop_price.type, supply_drop_price.amount)
        custom_depot.get_function("save_depot_data")()

        for i = 1, unk2.crateType:find("_basic") and math.random(1, 2) or math.random(2, 3) do
            local items_list = LUI.MPLootDropsBase.GetGenericItemList(x, LUI.MPDepot.LootDropsData[LUI.MPDepot
                .SuppyDropLootStream[unk2.crateType]].lootTableColName)
            local random_item = items_list[math.random(#items_list)]

            while random_item.inventoryItemType ~= Cac.InventoryItemType.Loot do
                random_item = items_list[math.random(#items_list)]
            end

            if random_item then
                f48_arg1.items[i] = random_item.guid
            end
        end

        for i = 1, #f48_arg1.items do
            if not custom_depot.get_function("has_item")(f48_arg1.items[i]) then
                custom_depot.get_function("add_item")(f48_arg1.items[i], true)
            else
                local item_rarity = tonumber(Engine.TableLookup(LootTable.File, LootTable.Cols.GUID, f48_arg1.items[i],
                    LootTable.Cols.Rarity))
                local dismantled_amount = 0

                if item_rarity == ItemRarity.Common then
                    dismantled_amount = math.random(1, 75)
                elseif item_rarity == ItemRarity.Rare then
                    dismantled_amount = math.random(75, 155)
                elseif item_rarity == ItemRarity.Legendary then
                    dismantled_amount = math.random(155, 260)
                elseif item_rarity == ItemRarity.Epic then
                    dismantled_amount = math.random(260, 550)
                end

                table.insert(f48_arg1.replacements, {
                    item_index = i,
                    currency = {
                        amount = dismantled_amount
                    }
                })

                custom_depot.get_function("add_currency")(InventoryCurrencyType.Parts, dismantled_amount)
            end
        end

        custom_depot.get_function("save_depot_data")()
        supply_drop_orig(f48_arg0, f48_arg1)
    end)

    local slow_purchase_transfer_orig = open_loot_menu.m_eventHandlers["slow_purchase_transfer"]
    open_loot_menu:registerEventHandler("slow_purchase_transfer", function(f33_arg0, f33_arg1)
        local f33_local0 = 0
        if f33_arg0.slowPurchaseTimer then
            f33_arg0.slowPurchaseTimer:close()
            f33_arg0.slowPurchaseTimer = nil
        end
        local f33_local1 = CoD.CreateState(-500, 0, 500, 20, CoD.AnchorTypes.Top)
        f33_local1.font = CoD.TextSettings.BodyFont.Font
        f33_local1.verticalAlignment = LUI.VerticalAlignment.Top
        f33_local1.horizontalAlignment = LUI.HorizontalAlignment.Center
        f33_local1.color = Colors.mw1_green
        f33_local1.alpha = 1
        f33_arg0.slowPurchaseText = LUI.UIText.new(f33_local1)
        f33_arg0:addElement(f33_arg0.slowPurchaseText)
        f33_arg0.slowPurchaseText:setText(Engine.Localize("@DEPOT_TRANSFER_IN_PROGRESS_DOT"))
        f33_arg0.slowPurchaseText.textState = 1
        f33_arg0.slowPurchaseText:registerEventHandler("update_slow_purchase_text", f0_local30)
        f33_arg0.slowPurchaseText:addElement(LUI.UITimer.new(1000, "update_slow_purchase_text"))
        local f33_local2 = LUI.MenuBuilder.BuildRegisteredType("progressBar")
        f33_local2:registerAnimationState("default", {
            topAnchor = false,
            bottomAnchor = true,
            leftAnchor = false,
            rightAnchor = false,
            top = 0,
            height = 40
        })
        f33_local2:animateToState("default")
        f33_arg0.slowPurchaseText:addElement(f33_local2)
        f33_local2:animateFill(f33_local0)
        f33_arg0.purchaseTimeoutTimer = LUI.UITimer.new(f33_local0, "abort_purchase_transfer", nil, true)
        f33_arg0:addElement(f33_arg0.purchaseTimeoutTimer)
    end)

    return open_loot_menu
end
LUI.MenuBuilder.m_types_build["MPDepotOpenLootMenu"] = MPDepotOpenLootMenu

AddLootDropTabSelector_orig = LUI.MPDepotBase.AddLootDropTabSelector
LUI.MPDepotBase.AddLootDropTabSelector = function(unk1, unk2)
    if not custom_depot.get_function("has_accepted_mod_eula")() then
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

    if not custom_depot.get_function("has_seen_mod_eula")() then
        LUI.FlowManager.RequestAddMenu(nil, "mod_eula", true, 0, false, {
            acceptCallback = function()
                custom_depot.get_function("set_has_accepted_mod_eula")(true)
                custom_depot.get_function("set_has_seen_mod_eula")(true)
                LUI.FlowManager.RequestLeaveMenu(depot_menu)
            end,
            declineCallback = function()
                custom_depot.get_function("set_has_accepted_mod_eula")(false)
                custom_depot.get_function("set_has_seen_mod_eula")(true)
            end
        })
    end

    return depot_menu
end
LUI.MenuBuilder.m_types_build["MPDepotMenu"] = MPDepotMenu

GetLootDataForRef_orig = LUI.InventoryUtils.GetLootDataForRef
LUI.InventoryUtils.GetLootDataForRef = function(f13_arg0, f13_arg1, f13_arg2, f13_arg3, f13_arg4)
    local loot_data = GetLootDataForRef_orig(f13_arg0, f13_arg1, f13_arg2, f13_arg3, f13_arg4)

    if loot_data and custom_depot.get_function("has_item")(loot_data.guid) then
        loot_data.lockState = "Unlocked"
    end

    return loot_data
end
