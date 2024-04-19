local fs = require("Scripts/LuaScripts/filesystem")

function displayHelp()
    print(string.format(
[[Creates a bland project in the parent folder.
Usage:
    %s [ProjectName]
To display this 
    %s -h
Or
    %s --help
]], arg[0], arg[0], arg[0], arg[0]))

end

local projectName = "Project"
local projectPath = ".."
local templatePath = "ProjectTemplate"

local engineDir = fs.getCurrentDirectoryName()

-- get project name
if (#arg < 1) then
    displayHelp()
    return
else
    if (arg[1] == "-h" or arg[1] == "--help") then
        displayHelp()
        return
    end

    projectName = arg[1]
end

print("Initializing project " .. projectPath .. "/" .. projectName)

print("copying project template to " .. projectPath)
for filename in fs.ls(templatePath) do
    local sourcePath = templatePath .. "\\" .. filename
    local destPath = projectPath .. "\\" .. filename
    print("copying and filling in project tokens [" .. sourcePath .. "] to [" .. destPath .. "]")
    fs.copyFile(sourcePath, destPath)
    fs.replaceTokenInFile(destPath, "{{ENGINE_DIR}}", engineDir)
    fs.replaceTokenInFile(destPath, "{{PROJECT_NAME}}", projectName)
end

print("Creating project directories")
fs.createDirectory(projectPath .. "\\" .. projectName)
fs.createDirectory(projectPath .. "\\" .. projectName .. "\\" .. "Assets")
fs.createDirectory(projectPath .. "\\" .. projectName .. "\\" .. "Sources")