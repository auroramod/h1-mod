game:addlocalizedstring("MOD_EULA_1",
    "SOMETHING TO WRITE HERE, HELLO WORLD, I GUESS I WILL NEED SOMEONE TO WRITE THIS TEXT BECAUSE MY ENGLISH IS SHITTY AS FUCK.")
game:addlocalizedstring("MOD_EULA_2",
    "YEAH, WE HAVE NOW THIS \"EULA\" TO ACCEPT FOR SOME COUNTRIES, AND H1-MOD STAFF IS NOT RESPONSIBLE, OK? BYE NOW.")

local mod_eula = function(unk1, unk2)
    return LUI.EULABase.new(CoD.CreateState(0, 0, 0, 0, CoD.AnchorTypes.All), {
        textStrings = LUI.EULABase.CreateTextStrings("@MOD_EULA_", 2),
        declineCallback = function(unk3)
            unk2.declineCallback(unk3)
        end,
        acceptCallback = function(unk4)
            unk2.acceptCallback(unk4)
        end
    })
end

LUI.MenuBuilder.registerPopupType("mod_eula", mod_eula)
