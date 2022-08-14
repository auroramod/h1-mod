if game:issingleplayer() then
    return
end

-- from mpdepotbase.lua, global definition isn't working
InventoryCurrencyType = {
    LaunchCredits = 1,
    Credits = 2,
    Parts = 3,
    CoDPoints = 4,
    Bonus = 5,
    Max = 6
}

ItemRarity = {
    Common = 0,
    Rare = 1,
    Legendary = 2,
    Epic = 3
}

custom_depot = {
    collection_details_menu = nil,
    data = {
        currencies = {
            launchCredits = 0, -- LaunchCredits
            credits = 0, -- Credits
            parts = 0, -- Parts
            codPoints = 0, -- CoDPoints
            bonus = 0 -- Bonus
        },
        items = {},
        reward_splashes = {},
        has_accepted_mod_eula = false,
        has_seen_mod_eula = false
    },
    directory_path = "players2/user",
    file_name = "depot.json",
    file_path = nil,
    functions = {}
}

custom_depot.file_path = string.format("%s/%s", custom_depot.directory_path, custom_depot.file_name)

custom_depot.get_function = function(function_name)
    if not function_name or not custom_depot.functions[function_name] then
        return nil
    end

    return custom_depot.functions[function_name]
end

custom_depot.functions["save_depot_data"] = function()
    io.writefile(custom_depot.file_path, json.encode(custom_depot.data), false)
end

custom_depot.functions["load_depot_data"] = function()
    if not io.directoryexists(custom_depot.directory_path) then
        io.createdirectory(custom_depot.directory_path)
    end

    if not io.fileexists(custom_depot.file_path) then
        custom_depot.get_function("save_depot_data")()
    end

    custom_depot.data = json.decode(io.readfile(custom_depot.file_path))
end

local function convert_currency_to_string(type)
    if type == InventoryCurrencyType.LaunchCredits then
        return "launchCredits"
    elseif type == InventoryCurrencyType.Credits then
        return "credits"
    elseif type == InventoryCurrencyType.Parts then
        return "parts"
    elseif type == InventoryCurrencyType.CoDPoints then
        return "codPoints"
    elseif type == InventoryCurrencyType.Bonus then
        return "bonus"
    end
end

custom_depot.functions["add_currency"] = function(currency_type, amount)
    local type = convert_currency_to_string(currency_type)
    custom_depot.data.currencies[type] = custom_depot.data.currencies[type] + amount
end

custom_depot.functions["remove_currency"] = function(currency_type, amount)
    local type = convert_currency_to_string(currency_type)
    custom_depot.data.currencies[type] = custom_depot.data.currencies[type] - amount
end

custom_depot.functions["get_currency"] = function(currency_type)
    local type = convert_currency_to_string(currency_type)

    if not currency_type or not custom_depot.data.currencies[type] then
        return nil
    end

    return custom_depot.data.currencies[type]
end

custom_depot.functions["add_item"] = function(item, value)
    custom_depot.data.items[item] = value
end

custom_depot.functions["has_item"] = function(item)
    return custom_depot.data.items[item] ~= nil
end

custom_depot.functions["add_reward_splash"] = function(item, value)
    custom_depot.data.reward_splashes[item] = value
end

custom_depot.functions["has_reward_splash"] = function(item)
    return custom_depot.data.reward_splashes[item] ~= nil
end

custom_depot.functions["has_accepted_mod_eula"] = function()
    return custom_depot.data.has_accepted_mod_eula
end

custom_depot.functions["set_has_accepted_mod_eula"] = function(value)
    custom_depot.data.has_accepted_mod_eula = value
    custom_depot.get_function("save_depot_data")()
end

custom_depot.functions["has_seen_mod_eula"] = function()
    return custom_depot.data.has_seen_mod_eula
end

custom_depot.functions["set_has_seen_mod_eula"] = function(value)
    custom_depot.data.has_seen_mod_eula = value
    custom_depot.get_function("save_depot_data")()
end

custom_depot.get_function("load_depot_data")()

if Engine.InFrontend() then
    require("mod_eula")
    require("depot_override")
end

if not Engine.InFrontend() then
    require("scoreboard_override")
end
