Engine.GetLuiRoot():registerEventHandler("mod_download_start", function(element, event)
    local popup = LUI.openpopupmenu("generic_waiting_popup_", {
        oncancel = function()
            download.abort()
        end,
        withcancel = true,
        text = "Downloading files..."
    })

    local file = ""

    popup:registerEventHandler("mod_download_set_file", function(element, event)
        file = event.request.name
        popup.text:setText(string.format("Downloading %s...", file))
    end)

    popup:registerEventHandler("mod_download_progress", function(element, event)
        popup.text:setText(string.format("Downloading %s (%i%%)...", file, math.floor(event.fraction * 100)))
    end)

    popup:registerEventHandler("mod_download_done", function()
        LUI.FlowManager.RequestLeaveMenu(popup)
    end)
end)
