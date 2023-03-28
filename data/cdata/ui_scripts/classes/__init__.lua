if game:issingleplayer() or (Engine.InFrontend()) then
    return
end

local getclasscount = Cac.GetCustomClassCount
Cac.GetCustomClassCount = function(...)
    local value = Engine.GetDvarBool("sv_disableCustomClasses")
    if (value) then
        return 0
    end

    return getclasscount(...)
end