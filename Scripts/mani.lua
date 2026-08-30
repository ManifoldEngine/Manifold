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

mani.findProject = function(projectName)
    local currentProject = premake.api.scope.project

    -- go up to the workspace project
    while currentProject.name ~= "root" do
        if currentProject["projects"] ~= nil then
            break
        end
        currentProject = currentProject.parent
    end

    return currentProject.projects[projectName]
end

mani.forwardLinks = function(projectName)
    -- path to project's links
    -- premake.api.scope.project.parent.projects[projectName].current.links
    local project = mani.findProject(projectName)
    return links { table.unpack(project.current.links) }
end


return mani