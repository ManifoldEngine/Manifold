local mani = require("Scripts/LuaScripts/mani")
local fs = require("Scripts/LuaScripts/windowsFilesystem")


function displayHelp()
    print(string.format(
[[Creates a blank project in the parent folder.
Usage:
    [ProjectName] : name of the project - generates project files and builds third parties
    --no-template : skip project files generation - only build third parties
    --no-thirdparties : skip third parties - only generate project files

To display this 
    -h or --help
]], arg[0], arg[0], arg[0], arg[0], arg[0]))

end

local projectName = "Project"
local projectPath = ".."
local templatePath = "ProjectTemplate"

local enginDirToken = "{{ENGINE_DIR}}"
local projectNameToken = "{{PROJECT_NAME}}"

local engineDir = fs.getCurrentDirectoryName()

local shouldCreateTemplateFiles = true
local shouldBuildThirdParties = true

-- get project name
if (#arg < 1) then
    displayHelp()
    return
else
    if (mani.contains(arg, "-h") or mani.contains(arg, "--help")) then
        displayHelp()
        return
    end

    for i, value in ipairs(arg) do
        if (value == "--no-template") then
            shouldCreateTemplateFiles = false
        elseif (value == "--no-thirdparties") then
            shouldBuildThirdParties = false
        else
            projectName = value
        end
    end
end

if shouldCreateTemplateFiles then
    print("Creating project files at []" .. projectPath .. "/" .. projectName .. "]")
    -- copy template project files over the project location
    function copyAndReplaceTokens (directoryPath, destinationPath) 
        for entry in fs.ls(directoryPath) do
            if entry ~= ".." then
                local sourceEntry = entry
                local destEntry = entry
                
                -- overwrite tokens
                if destEntry == projectNameToken then
                    destEntry = projectName
                end
    
                if destEntry == enginDirToken then
                    destEntry = engineDir
                end
    
                local entryPath = directoryPath .. "\\" .. sourceEntry
                local entryDestPath = destinationPath .. "\\" .. destEntry
    
                if fs.isDirectory(entryPath) then
                    -- recurse through directories
                    fs.createDirectory(entryDestPath)
                    copyAndReplaceTokens(entryPath, entryDestPath)
                else
                    -- otherwise copy and replace tokens
                    print("copying [" .. entryPath .. "] to [" .. entryDestPath .. "]")
                    fs.copyFile(entryPath, entryDestPath)
                    fs.replaceTokenInFile(entryDestPath, enginDirToken, engineDir)
                    fs.replaceTokenInFile(entryDestPath, projectNameToken, projectName)
                end
            end
        end
    end

    copyAndReplaceTokens(templatePath, projectPath)
end

-- build third parties
if shouldBuildThirdParties then
    local cmake = "ThirdParties\\cmake\\bin\\cmake.exe"
    local MSBuild = "\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe\""
    
    print("Building third parties...")
    
    -- assimp
    local assimpDir = "ThirdParties\\assimp\\"
    print("building assimp")
    os.execute(cmake .. " " .. assimpDir .. "CMakeLists.txt -B " .. assimpDir)
    os.execute(cmake .. " --build " .. assimpDir)
    
    print("======================================")
    
    -- gl3w
    local gl3wDir = "ThirdParties\\gl3w\\"
    print("building gl3w")
    os.execute("py " .. gl3wDir .. "gl3w_gen.py --root=" .. gl3wDir)
    
    print("======================================")
    
    -- glfw
    local glfwDir = "ThirdParties\\glfw\\"
    print("building glfw")
    os.execute(cmake .. " -S " .. glfwDir .. " -B " .. glfwDir .. " -D BUILD_SHARED_LIBS=OFF")
    os.execute(MSBuild .. " " .. glfwDir .. "ALL_BUILD.vcxproj")
    
    print("======================================")
    
    -- reflect-cpp
    local rflDir = "ThirdParties\\reflect-cpp"
    print("building reflect-cpp")
    os.execute(cmake .. " -S " .. rflDir .. " -B " .. rflDir .. "\\build -DCMAKE_BUILD=Release")
    -- os.execute(cmake .. " --build " .. rflDir .. "\\build -j 4") -- gcc, clang
    os.execute(cmake .. " --build " .. rflDir .. "\\build --config Release -j 4") -- MSVC
end
