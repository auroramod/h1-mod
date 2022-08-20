Engine.GetLuiRoot():registerEventHandler("mod_download_init", function(element, event)
    local popup = LUI.openpopupmenu("generic_waiting_popup_", {
        oncancel = function()
            download.abort()
            LUI.FlowManager.RequestLeaveMenu(popup)
        end,
        withcancel = true,
        text = "Downloading " .. event.request.name .. "..."
    })

    Engine.GetLuiRoot():registerEventHandler("mod_download_done", function()
        LUI.FlowManager.RequestLeaveMenu(popup)
    end)
end)
