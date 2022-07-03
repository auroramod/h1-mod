game:addlocalizedstring("CUSTOM_DEPOT_EULA_1", "Dear User,")
game:addlocalizedstring("CUSTOM_DEPOT_EULA_2",
    "By using this feature, you acknowledge that you are over 18 years old, and that any sort of chance games / gambling are allowed in your country (even if they do not involve real money).")
game:addlocalizedstring("CUSTOM_DEPOT_EULA_3",
    "The H1-Mod team is not responsible if you break the law within your country, and the sole responsibility will be upon you to respect the same.")
game:addlocalizedstring("CUSTOM_DEPOT_EULA_4",
    "The H1-Mod team will never include real money transactions within the modified systems. The only way to get currency, should you wish to, is by playing the game.")
game:addlocalizedstring("CUSTOM_DEPOT_EULA_5", "Best Regards,")
game:addlocalizedstring("CUSTOM_DEPOT_EULA_6", "The H1-Mod Team.")

local mod_eula = function(unk1, unk2)
    return LUI.EULABase.new(CoD.CreateState(0, 0, 0, 0, CoD.AnchorTypes.All), {
        textStrings = LUI.EULABase.CreateTextStrings("@CUSTOM_DEPOT_EULA_", 6),
        declineCallback = function(unk3)
            unk2.declineCallback(unk3)
        end,
        acceptCallback = function(unk4)
            unk2.acceptCallback(unk4)
        end
    })
end

LUI.MenuBuilder.registerPopupType("mod_eula", mod_eula)
