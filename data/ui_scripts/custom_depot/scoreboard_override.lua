-- from roundend.lua, dev comments says that the game["round_end"] array contains indexes for this table
local ending_reasons = {"MP_DRAW", "LUA_MENU_REPORT_DRAW", "MP_ROUND_WIN", "MP_ROUND_LOSS", "LUA_MENU_REPORT_VICTORY",
                        "LUA_MENU_REPORT_DEFEAT", "MP_HALFTIME", "MP_OVERTIME", "MP_ROUNDEND", "MP_INTERMISSION",
                        "MP_SWITCHING_SIDES", "MP_MATCH_BONUS_IS", "MP_MATCH_TIE", "MP_GAME_END", "SPLASHES_BLANK"}

local function starts_with(str, start)
    return str:sub(1, #start) == start
end

local player_old_score = 0

local scoreboard_orig = LUI.MenuBuilder.m_types_build["scoreboard"]
local scoreboard = function(unk1, unk2)
    local scoreboard = scoreboard_orig(unk1, unk2)

    scoreboard:registerOmnvarHandler("ui_round_end", function(f22_arg0, f22_arg1)
        if GameX.IsRankedMatch() then
            local player_score = 0

            local gamemode = GameX.GetGameMode()
            local player_stats = Game.GetPlayerScoreInfoAtRank(Game.GetPlayerTeam(), Game.GetPlayerScoreRanking())

            --[[
            this will do the job for when its needed, aka when round loss/win occurs
            this check may be true more than once cuz this callback happens 3 times, 
                but the player_old_score variable will stop us from adding more currency
            ]] --
            local unlocalized_string = ending_reasons[Game.GetOmnvar("ui_round_end_title")]
            local is_round_based = starts_with(unlocalized_string, "MP_ROUND") or IsGameTypeRoundBased(gamemode)

            if is_round_based or gamemode == "conf" or gamemode == "war" then
                player_score = player_stats.score
            else
                player_score = player_stats.extrascore0
            end

            local currency_gain = math.floor((player_score - player_old_score) * 10 / 100)

            if currency_gain <= 0 then
                return
            end

            custom_depot.get_function("add_currency")(InventoryCurrencyType.Parts, currency_gain)

            if custom_depot.functions["has_accepted_mod_eula"]() then
                custom_depot.get_function("add_currency")(InventoryCurrencyType.Credits, math.random(2, 3))
            end

            player_old_score = player_score
            custom_depot.get_function("save_depot_data")()
        end
    end)

    return scoreboard
end
LUI.MenuBuilder.m_types_build["scoreboard"] = scoreboard
