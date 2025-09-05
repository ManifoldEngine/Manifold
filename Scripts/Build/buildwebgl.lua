-- buildwebgl.lua
include "../../../locations.lua"

premake.modules.buildwebgl = {}

local m = premake.modules.buildwebgl

local THIRD_PARTIES_PATH = "Engine\\ThirdParties" 
local EMCC_PATH = THIRD_PARTIES_PATH .. "\\emscripten\\upstream\\emscripten\\"
local PREMAKE_PATH = THIRD_PARTIES_PATH .. "\\premake\\premake5.exe"

local binOutput = "bin\\Debug-windows-"
local binIntOutput = "bin-int\\Debug-windows-\\WebGL\\Debug"

-- windows specific function to recursively get files with extension
local function getfilesWithExtension(directory, extension)
    local filesWithExtension = {}
    -- get directory content recursively
    local targetDirector = directory
    if extension ~= nil then
        targetDirector = targetDirector:gsub("/", "\\") .. "\\*" .. extension
    end

    local handle = io.popen("dir /s /b " .. targetDirector)
    -- Read the entire output
    local result = handle:read("*all")
    handle:close()

    -- Iterate over filenames in the result
    for filename in result:gmatch("[^\r\n]+") do
        table.insert(filesWithExtension, filename)
    end

    return filesWithExtension
end

-- Function to recursively copy a directory and its subdirectories
local function copyDirectory(source, destination)
    -- Create destination directory if it doesn't exist
    os.execute("mkdir \"" .. destination .. "\"")

    -- Iterate over files and subdirectories in the source directory
    for file in io.popen('dir "' .. source .. '" /b /a-d'):lines() do
        -- Construct full paths
        local sourcePath = source .. "\\" .. file
        local destinationPath = destination .. "\\" .. file

        -- Copy file
        os.execute("copy \"" .. sourcePath .. "\" \"" .. destinationPath .. "\"")
    end

    -- Recursively copy subdirectories
    for directory in io.popen('dir "' .. source .. '" /b /ad'):lines() do
        if directory ~= "." and directory ~= ".." then
            copyDirectory(source .. "\\" .. directory, destination .. "\\" .. directory)
        end
    end
end

newaction {
    trigger = "buildwebgl",
    description = "Builds a wasm executable",

    onStart = function()
        print("Starting EMCC Build")

        os.execute(THIRD_PARTIES_PATH .. "\\emscripten\\emsdk.bat")
        os.execute(THIRD_PARTIES_PATH .. "\\emscripten\\emsdk_env.bat")
    end,    

    onWorkspace = function(wks)
        -- produce makefiles
        local gmakeCommand = PREMAKE_PATH .. " gmake"
        print(gmakeCommand)
        os.execute(gmakeCommand)
    end,

    onProject = function(prj)
        -- we only build executables
        if prj.kind ~= "ConsoleApp" then
            return
        end

        if prj.name ~= "Sandbox" then
            return
        end

        print("Building for " .. prj.name)
        -- build
        local makeCommand = EMCC_PATH .. "emmake make config=debug_webgl " .. prj.name
        print(makeCommand)
        os.execute(makeCommand)

        -- get all object files
        local files = getfilesWithExtension(binIntOutput .. "/", ".o")
        local filesString = table.concat(files, " ")

        local buildCommand = EMCC_PATH .. "em++ -O3 -sUSE_GLFW=3 -sMAX_WEBGL_VERSION=2 " .. filesString .. " -sASYNCIFY -o " .. binOutput .. "/" .. prj.name .. ".html --preload-file Engine/Assets/ --preload-file " .. prj.name .. "/Assets/"
        print(buildCommand)
        os.execute(buildCommand)
    end,

    onEnd = function()
        print("WASM Build completed.")
    end
}

return m