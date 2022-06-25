if (not Engine.InFrontend()) then
	return
end

updatecancelled = false
updater.cancelupdate()

function startupdatecheck(popup, autoclose)
	Engine.GetLuiRoot():registerEventHandler("update_check_done", function(element, event)
		if (updatecancelled) then
			return
		end

		if (not updater.getupdatecheckstatus()) then
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
	end)

	updater.startupdatecheck()
end

function startupdatedownload(popup, autoclose)
	local textupdate = nil
	local previousfile = nil
	local timer = LUI.UITimer.new(10, "update_file")

	popup:addElement(timer)
	popup:registerEventHandler("update_file", function()
		local file = updater.getcurrentfile()
		if (file == previousfile) then
			return
		end

		file = previousfile
		popup.text:setText("Downloading file " .. updater.getcurrentfile() .. "...")
	end)

	Engine.GetLuiRoot():registerEventHandler("update_done", function(element, event)
		timer:close()

		if (updatecancelled) then
			return
		end

		if (not updater.getupdatedownloadstatus()) then
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
			Engine.Exec("lui_restart")
		end

		if (autoclose) then
			LUI.FlowManager.RequestLeaveMenu(popup)
		end
	end)

	updater.startupdatedownload()
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
		local timer = LUI.UITimer.new(100, "tryupdate")
		Engine.GetLuiRoot():addElement(timer)
		Engine.GetLuiRoot():registerEventHandler("tryupdate", function()
			timer:close()
			updater.sethastriedupdate(true)
			tryupdate(true)
		end)
	end
end

LUI.tryupdating = tryupdate
LUI.onmenuopen("main_lockout", tryautoupdate)
LUI.onmenuopen("mp_main_menu", tryautoupdate)
