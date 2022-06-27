local localization = {
    english = {
        LANGUAGE_BUTTON = "Language",
        LANGUAGE_BUTTON_DESC = "Change your game language.",
        POPUP_RESTART_REQUIRED_TITLE = "RESTART REQUIRED",
        POPUP_RESTART_REQUIRED_TEXT = "WARNING: To work properly, you need to have your language's zone files in the zone directory of your game root. You have changed your game language, do you wish to restart?",
        POPUP_NO_ZONE_FOUND_TITLE = "LANGUAGE NOT FOUND",
        POPUP_NO_ZONE_FOUND_TEXT = "We are unable to find the language folder you have selected. To avoid bugs, we will not change anything.",
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
        POPUP_RESTART_REQUIRED_TEXT = "ATTENTION : Pour fonctionner correctement, vous devez avoir les fichiers de zone de votre langue dans le répertoire de zone de la racine de votre jeu. Vous avez changé la langue de votre jeu, souhaitez-vous redémarrer ?",
        POPUP_NO_ZONE_FOUND_TITLE = "LANGUE INTROUVABLE",
        POPUP_NO_ZONE_FOUND_TEXT = "Nous ne parvenons pas à trouver le dossier de langue que vous avez sélectionné. Pour éviter les bugs, nous ne changerons rien.",
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
        POPUP_RESTART_REQUIRED_TEXT = "ATTENZIONE: per funzionare correttamente, devi avere i file di zona della tua lingua nella directory zone della tua root di gioco. Hai cambiato la lingua del gioco, desideri riavviare?",
        POPUP_NO_ZONE_FOUND_TITLE = "LINGUA NON TROVATA",
        POPUP_NO_ZONE_FOUND_TEXT = "Non siamo in grado di trovare la cartella della lingua che hai selezionato. Per evitare bug, non cambieremo nulla.",
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
        POPUP_RESTART_REQUIRED_TEXT = "WARNUNG: Um richtig zu funktionieren, müssen Sie die Zonendateien Ihrer Sprache im Zonenverzeichnis Ihres Spielstammverzeichnisses haben. Du hast deine Spielsprache geändert, möchtest du neu starten?",
        POPUP_NO_ZONE_FOUND_TITLE = "SPRACHE NICHT GEFUNDEN",
        POPUP_NO_ZONE_FOUND_TEXT = "Wir können den von Ihnen ausgewählten Sprachordner nicht finden. Um Fehler zu vermeiden, werden wir nichts ändern.",
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
        POPUP_RESTART_REQUIRED_TEXT = "ADVERTENCIA: para que funcione correctamente, debe tener los archivos de zona de su idioma en el directorio de zona de la raíz del juego. Ha cambiado el idioma de su juego, ¿desea reiniciar?",
        POPUP_NO_ZONE_FOUND_TITLE = "IDIOMA NO ENCONTRADO",
        POPUP_NO_ZONE_FOUND_TEXT = "No podemos encontrar la carpeta de idioma que ha seleccionado. Para evitar errores, no cambiaremos nada.",
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
        -- Permissions denied, but exists.
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
