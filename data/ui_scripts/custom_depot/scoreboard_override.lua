local player_score = 0
local player_old_score = 0

Scoreboard_orig = LUI.MenuBuilder.m_types_build["scoreboard"]
Scoreboard = function(unk1, unk2)
    local scoreboard = Scoreboard_orig(unk1, unk2)

    scoreboard:registerOmnvarHandler("ui_round_end", function(f22_arg0, f22_arg1)
        if GameX.IsRankedMatch() then
            local gamemode = GameX.GetGameMode()
            local player_stats = Game.GetPlayerScoreInfoAtRank(Game.GetPlayerTeam(), Game.GetPlayerScoreRanking())

            if Engine.GetDvarInt(string.format("scr_%s_halftime", gamemode)) == 1 or IsGameTypeRoundBased(gamemode) or
                gamemode == "conf" or gamemode == "war" then
                player_score = player_stats.score
            else
                player_score = player_stats.extrascore0
            end

            local currency_gain = math.floor((player_score - player_old_score) * 10 / 100)

            if currency_gain == 0 then
                return
            end

            custom_depot.get_function("add_currency")(InventoryCurrencyType.Parts, currency_gain)

            player_old_score = player_score
            custom_depot.get_function("save_depot_data")()
        end
    end)

    return scoreboard
end
LUI.MenuBuilder.m_types_build["scoreboard"] = Scoreboard
