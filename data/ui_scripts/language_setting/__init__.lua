local localization = {
    english = {
        LANGUAGE_BUTTON = "Language",
        LANGUAGE_BUTTON_DESC = "Change your game language.",
        POPUP_RESTART_REQUIRED_TITLE = "RESTART REQUIRED",
        POPUP_RESTART_REQUIRED_TEXT = "WARNING: To work properly, you need to have your language into zone directory of your game root. You have changed your game language, wish you restart?",
        POPUP_NO_ZONE_FOUND_TITLE = "NO LANGUAGE ZONE FOLDER FOUND",
        POPUP_NO_ZONE_FOUND_TEXT = "We are unable to find the language folder you have selected, to avoid bugs, we will not change anything.",
        LANG_ENGLISH = "English",
        LANG_FRENCH = "French",
        LANG_ITALIAN = "Italian",
        LANG_GERMAN = "German",
        LANG_SPANISH = "Spanish"
    },
    french = {
        LANGUAGE_BUTTON = "Langue",
        LANGUAGE_BUTTON_DESC = "Changez la langue de votre jeu.",
        POPUP_RESTART_REQUIRED_TITLE = "REDÉMARRAGE REQUIS",
        POPUP_RESTART_REQUIRED_TEXT = "ATTENTION : Pour fonctionner correctement, vous devez avoir votre langue dans le répertoire zone de la racine de votre jeu. Vous avez changé la langue de votre jeu, souhaitez-vous redémarrer ?",
        POPUP_NO_ZONE_FOUND_TITLE = "NO LANGUAGE ZONE FOLDER FOUND",
        POPUP_NO_ZONE_FOUND_TEXT = "We are unable to find the language folder you have selected, to avoid bugs, we will not change anything.",
        LANG_ENGLISH = "Anglais",
        LANG_FRENCH = "Français",
        LANG_ITALIAN = "Italien",
        LANG_GERMAN = "Allemand",
        LANG_SPANISH = "Espagnol"
    },
    italian = {
        LANGUAGE_BUTTON = "Lingua",
        LANGUAGE_BUTTON_DESC = "Cambia la tua lingua di gioco.",
        POPUP_RESTART_REQUIRED_TITLE = "RIPARTENZA RICHIESTA",
        POPUP_RESTART_REQUIRED_TEXT = "ATTENZIONE: per funzionare correttamente, devi avere la tua lingua nella directory zone della tua root di gioco. Hai cambiato la lingua del gioco, desideri riavviare?",
        POPUP_NO_ZONE_FOUND_TITLE = "NO LANGUAGE ZONE FOLDER FOUND",
        POPUP_NO_ZONE_FOUND_TEXT = "We are unable to find the language folder you have selected, to avoid bugs, we will not change anything.",
        LANG_ENGLISH = "Inglese",
        LANG_FRENCH = "Francese",
        LANG_ITALIAN = "Italiano",
        LANG_GERMAN = "Tedesco",
        LANG_SPANISH = "Spagnolo"
    },
    german = {
        LANGUAGE_BUTTON = "Sprache",
        LANGUAGE_BUTTON_DESC = "Ändern Sie Ihre Spielsprache.",
        POPUP_RESTART_REQUIRED_TITLE = "NEUSTART ERFORDERLICH",
        POPUP_RESTART_REQUIRED_TEXT = "WARNUNG: Um richtig zu funktionieren, müssen Sie Ihre Sprache im Zonenverzeichnis Ihres Spielstammverzeichnisses haben. Du hast deine Spielsprache geändert, möchtest du neu starten?",
        POPUP_NO_ZONE_FOUND_TITLE = "NO LANGUAGE ZONE FOLDER FOUND",
        POPUP_NO_ZONE_FOUND_TEXT = "We are unable to find the language folder you have selected, to avoid bugs, we will not change anything.",
        LANG_ENGLISH = "Englisch",
        LANG_FRENCH = "Französisch",
        LANG_ITALIAN = "Italienisch",
        LANG_GERMAN = "Deutsch",
        LANG_SPANISH = "Spanisch"
    },
    spanish = {
        LANGUAGE_BUTTON = "Idioma",
        LANGUAGE_BUTTON_DESC = "Cambia el idioma de tu juego.",
        POPUP_RESTART_REQUIRED_TITLE = "REINICIO REQUERIDO",
        POPUP_RESTART_REQUIRED_TEXT = "ADVERTENCIA: para que funcione correctamente, debe tener su idioma en el directorio de zone de la raíz del juego. Has cambiado el idioma de tu juego, ¿deseas reiniciar?",
        POPUP_NO_ZONE_FOUND_TITLE = "NO LANGUAGE ZONE FOLDER FOUND",
        POPUP_NO_ZONE_FOUND_TEXT = "We are unable to find the language folder you have selected, to avoid bugs, we will not change anything.",
        LANG_ENGLISH = "Inglés",
        LANG_FRENCH = "Francés",
        LANG_ITALIAN = "Italiano",
        LANG_GERMAN = "Alemán",
        LANG_SPANISH = "Español"
    }
}
local language = "english"

function get_actual_language()
    local file_path = "h1-mod/language.txt"
    local file = io.open(file_path, "r")

    if file == nil then
        return
    end

    local data = file:read("*a")
    language = data
end

function set_language(value)
    local file_path = "h1-mod/language.txt"
    local file = io.open(file_path, "w")
    file:write(value)
    file:close()
end

function does_zone_folder_exists(language)
    local isok, errstr, errcode = os.rename("zone/" .. language .. "/", "zone/" .. language .. "/")

    if isok == nil then
        if errcode == 13 then
            return true
        end

        return false
    end

    return true
end

get_actual_language()
Engine.SetDvarFromString("game_language", language)
LUI.addmenuoptionbutton("pc_controls", {
    index = 6,
    dvar = "game_language",
    text = localization[language].LANGUAGE_BUTTON,
    description = localization[language].LANGUAGE_BUTTON_DESC,
    options = {{
        text = localization[language].LANG_ENGLISH,
        value = "english"
    }, {
        text = localization[language].LANG_FRENCH,
        value = "french"
    }, {
        text = localization[language].LANG_ITALIAN,
        value = "italian"
    }, {
        text = localization[language].LANG_GERMAN,
        value = "german"
    }, {
        text = localization[language].LANG_SPANISH,
        value = "spanish"
    }},
    callback = function(value)
        LUI.yesnopopup({
            title = localization[value].POPUP_RESTART_REQUIRED_TITLE,
            text = localization[value].POPUP_RESTART_REQUIRED_TEXT,
            callback = function(result)
                if (result) then
                    if not does_zone_folder_exists(value) then
                        LUI.confirmationpopup({
                            title = localization[value].POPUP_NO_ZONE_FOUND_TITLE,
                            text = localization[value].POPUP_NO_ZONE_FOUND_TEXT,
                            buttontext = "OK",
                            callback = function()
                                LUI.FlowManager.RequestLeaveMenu(popup)
                            end
                        })

                        return
                    end

                    set_language(value)
                    updater.relaunch()
                else
                    LUI.FlowManager.RequestLeaveMenu(popup)
                end
            end
        })
    end
})
