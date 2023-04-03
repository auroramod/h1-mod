if game:issingleplayer() or (not Engine.InFrontend()) then
    return
end

local cols = {
    name = 0,
    class = 1,
}

local csv = "mp/customWeapons.csv"
local rows = Engine.TableGetRowCount(csv)
for i = 0, rows do
    local weap = Engine.TableLookupByRow(csv, i, cols.name)
    local class = Engine.TableLookupByRow(csv, i, cols.class)
    if (type(Cac.Weapons.Primary[class]) == "table") then
        table.insert(Cac.Weapons.Primary[class], {
            weap,
            0
        })
    elseif (type(Cac.Weapons.Secondary[class]) == "table") then
        table.insert(Cac.Weapons.Secondary[class], {
            weap,
            0
        })
    end
end
