updatecancelled = false
taskinterval = 100

updater.cancelupdate()

function startupdatecheck(popup, autoclose)
    updatecancelled = false

    local callback = function()
        if (not updater.getupdatecheckstatus()) then
            if (autoclose) then
                LUI.FlowManager.RequestLeaveMenu(popup)
                return
            end

            popup.text:setText("Error: " .. updater.getlasterror())
            return
        end

        if (not updater.isupdateavailable()) then
            if (autoclose) then
                LUI.FlowManager.RequestLeaveMenu(popup)
                return
            end

            popup.text:setText("No updates available")
            return
        end

        LUI.yesnopopup({
            title = "NOTICE",
            text = "An update is available, proceed with installation?",
            callback = function(result)
                if (result) then
                    startupdatedownload(popup, autoclose)
                else
                    LUI.FlowManager.RequestLeaveMenu(popup)
                end
            end
        })
    end

    updater.startupdatecheck()
    createtask({
        done = updater.isupdatecheckdone, 
        cancelled = isupdatecancelled, 
        callback = callback, 
        interval = taskinterval
    })
end

function startupdatedownload(popup, autoclose)
    updater.startupdatedownload()

    local textupdate = nil
    local previousfile = nil
    textupdate = game:oninterval(function()
        local file = updater.getcurrentfile()
        if (file == previousfile) then
            return
        end

        file = previousfile
        popup.text:setText("Downloading file " .. updater.getcurrentfile() .. "...")
    end, 10)

    local callback = function()
        textupdate:clear()

        if (not updater.getupdatedownloadstatus()) then
            if (autoclose) then
                LUI.FlowManager.RequestLeaveMenu(popup)
                return
            end

            popup.text:setText("Error: " .. updater.getlasterror())
            return
        end

        popup.text:setText("Update successful")

        if (updater.isrestartrequired()) then
            LUI.confirmationpopup({
                title = "RESTART REQUIRED",
                text = "Update requires restart",
                buttontext = "RESTART",
                callback = function()
                    updater.relaunch()
                end
            })
        else
            if (LUI.mp_menus) then
                Engine.Exec("lui_restart; lui_open mp_main_menu")
            else
                Engine.Exec("lui_restart")
            end
        end

        if (autoclose) then
            LUI.FlowManager.RequestLeaveMenu(popup)
        end
    end

    createtask({
        done = updater.isupdatedownloaddone,
        cancelled = isupdatecancelled,
        callback = callback,
        interval = taskinterval
    })
end

function updaterpopup(oncancel)
    return LUI.openpopupmenu("generic_waiting_popup_", {
        oncancel = oncancel,
        withcancel = true,
        text = "Checking for updates..."
    })
end

function createtask(data)
    local interval = nil
    interval = game:oninterval(function()
        if (data.cancelled()) then
            interval:clear()
            return
        end

        if (data.done()) then
            interval:clear()
            data.callback()
        end
    end, data.interval)
    return interval
end

function isupdatecancelled()
    return updatecancelled
end

function tryupdate(autoclose)
    updatecancelled = false
    local popup = updaterpopup(function()
        updater.cancelupdate()
        updatecancelled = true
    end)

    startupdatecheck(popup, autoclose)
end

function tryautoupdate()
    if (not updater.autoupdatesenabled()) then
        return
    end

    if (not updater.gethastriedupdate()) then
        game:ontimeout(function()
            updater.sethastriedupdate(true)
            tryupdate(true)
        end, 100)
    end
end

LUI.onmenuopen("mp_main_menu", tryautoupdate)
LUI.onmenuopen("main_lockout", tryautoupdate)