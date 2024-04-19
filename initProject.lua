local fs = require("Scripts/LuaScripts/windowsFilesystem")

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

local enginDirToken = "{{ENGINE_DIR}}"
local projectNameToken = "{{PROJECT_NAME}}"

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
function copyAndReplaceTokens (directoryPath, destinationPath) 
    for entry in fs.ls(directoryPath) do
        if entry ~= ".." then
            local sourceEntry = entry
            local destEntry = entry
            
            if destEntry == projectNameToken then
                destEntry = projectName
            end

            if destEntry == enginDirToken then
                destEntry = engineDir
            end

            local entryPath = directoryPath .. "\\" .. sourceEntry
            local entryDestPath = destinationPath .. "\\" .. destEntry

            if fs.isDirectory(entryPath) then
                fs.createDirectory(entryDestPath)
                copyAndReplaceTokens(entryPath, entryDestPath)
            else
                print("copying [" .. entryPath .. "] to [" .. entryDestPath .. "]")
                fs.copyFile(entryPath, entryDestPath)
                fs.replaceTokenInFile(entryDestPath, enginDirToken, engineDir)
                fs.replaceTokenInFile(entryDestPath, projectNameToken, projectName)
            end
        end
    end
end

copyAndReplaceTokens(templatePath, projectPath)
