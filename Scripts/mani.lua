local mani = {
    workspace = require("workspace"),
}

mani.printObj = function(obj)
    for k, v in pairs(obj) do
        print(k, v)
    end
end

mani.contains = function(object, inValue)
    for index, value in ipairs(object) do
        if value == inValue then
            return true
        end
    end
    return false
end

return mani