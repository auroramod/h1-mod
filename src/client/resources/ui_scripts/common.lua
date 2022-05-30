local menucallbacks = {}
local originalmenus = {}
local stack = {}

function getchildren(element)
	local children = {}
	local first = element:getFirstChild()

	while (first) do
		table.insert(children, first)
		first = first:getNextSibling()
	end

	return children
end

function printchildtree(element, indent, last)
	indent = indent or ""

	print(indent .. "+- " .. element.id .. " " .. (element:getText() or ""))
	indent =  indent .. (last and "   " or "|  ")

	local children = getchildren(element)
	for i = 1, #children do
		printchildtree(children[i], indent, i == #children)
	end
end

LUI.MenuBuilder.m_types_build["generic_waiting_popup_"] = function (menu, event)
	local oncancel = stack.oncancel
	local popup = LUI.MenuBuilder.BuildRegisteredType("waiting_popup", {
		message_text = stack.text,
		isLiveWithCancel = true,
		cancel_func = function(...)
			local args = {...}
			oncancel()
			LUI.FlowManager.RequestLeaveMenu(args[1])
		end
	})

	popup.text = popup:getLastChild():getPreviousSibling():getPreviousSibling()

	stack = {
		ret = popup
	}

	return popup
end

LUI.MenuBuilder.m_types_build["generic_yes_no_popup_"] = function()
	local callback = stack.callback
	local popup = LUI.MenuBuilder.BuildRegisteredType("generic_yesno_popup", {
		popup_title = stack.title,
		message_text = stack.text,
		yes_action = function()
			callback(true)
		end,
		no_action = function()
			callback(false)
		end
	})

	stack = {
		ret = popup
	}

	return popup
end

LUI.MenuBuilder.m_types_build["generic_confirmation_popup_"] = function()
	local popup = LUI.MenuBuilder.BuildRegisteredType( "generic_confirmation_popup", {
		cancel_will_close = false,
		popup_title = stack.title,
		message_text = stack.text,
		button_text = stack.buttontext,
		confirmation_action = stack.callback
	})

	stack = {
		ret = popup
	}

	return stack.ret
end

LUI.onmenuopen = function(name, callback)
	if (not LUI.MenuBuilder.m_types_build[name]) then
		return
	end

	if (not menucallbacks[name]) then
		menucallbacks[name] = {}
	end

	table.insert(menucallbacks[name], callback)

	if (not originalmenus[name]) then
		originalmenus[name] = LUI.MenuBuilder.m_types_build[name]
		LUI.MenuBuilder.m_types_build[name] = function(...)
			local args = {...}
			local menu = originalmenus[name](unpack(args))

			for k, v in next, menucallbacks[name] do
				v(menu, unpack(args))
			end

			return menu
		end
	end
end

local addoptionstextinfo = LUI.Options.AddOptionTextInfo
LUI.Options.AddOptionTextInfo = function(menu)
	local result = addoptionstextinfo(menu)
	menu.optionTextInfo = result
	return result
end

LUI.addmenubutton = function(name, data)
	LUI.onmenuopen(name, function(menu)
		if (not menu.list) then
			return
		end

		local button = menu:AddButton(data.text, data.callback, nil, true, nil, {
			desc_text = data.description
		})

		local buttonlist = menu:getChildById(menu.type .. "_list")

		if (data.id) then
			button.id = data.id
		end

		if (data.index) then
			buttonlist:removeElement(button)
			buttonlist:insertElement(button, data.index)
		end

		local hintbox = menu.optionTextInfo
		menu:removeElement(hintbox)

		LUI.Options.InitScrollingList(menu.list, nil)
		menu.optionTextInfo = LUI.Options.AddOptionTextInfo(menu)
	end)
end

LUI.openmenu = function(menu, args)
	stack = args
	LUI.FlowManager.RequestAddMenu(nil, menu)
	return stack.ret
end

LUI.openpopupmenu = function(menu, args)
	stack = args
	LUI.FlowManager.RequestPopupMenu(nil, menu)
	return stack.ret
end

LUI.yesnopopup = function(data)
	for k, v in next, data do
		stack[k] = v
	end
	LUI.FlowManager.RequestPopupMenu(nil, "generic_yes_no_popup_")
	return stack.ret
end

LUI.confirmationpopup = function(data)
	for k, v in next, data do
		stack[k] = v
	end
	LUI.FlowManager.RequestPopupMenu(nil, "generic_confirmation_popup_")
	return stack.ret
end