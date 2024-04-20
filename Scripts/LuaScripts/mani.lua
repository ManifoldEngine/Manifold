local m = {}

m.contains = function(table, inValue)
    for index, value in ipairs(table) do
        if value == inValue then
            return true
        end
    end
    return false
end

return m