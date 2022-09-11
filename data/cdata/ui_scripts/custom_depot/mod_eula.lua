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
