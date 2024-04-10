-- buildwebgl.lua
include "../../locations.lua"

premake.modules.buildwebgl = {}

local m = premake.modules.buildwebgl

local EMCC_PATH = "C:/Repository/emsdk/upstream/emscripten/"
local PREMAKE_PATH = thirdpartiesdir .. "/premake/premake5.exe"

local workspaceLocation = "toInit" 

local binOutput = "bin/" .. outputdir
local binIntOutput = "bin-int/" .. outputdir

-- windows function
local function getfilesWithExtension(directory, extension)
    local filesWithExtension = {}
    -- get directory content
    local handle = io.popen("dir /b " .. directory:gsub("/", "\\") .. "\\*" .. extension)
    -- Read the entire output
    local result = handle:read("*all")
    handle:close()

    -- Iterate over filenames in the result
    for filename in result:gmatch("[^\r\n]+") do
        table.insert(filesWithExtension, filename)
    end

    return filesWithExtension
end


newaction {
    trigger = "buildwebgl",
    description = "Builds a wasm executable",

    onStart = function()
        print("Starting WASM Build")
    end,    

    onWorkspace = function(wks)
        workspaceLocation = wks.location
    end,

    onProject = function(prj)
        -- we only build executables
        if prj.kind ~= "ConsoleApp" then
            return
        end
        projectName = prj.name

        print("Building for " .. projectName)
        
        -- produce makefiles
        local gmakeComment = "premake5.exe gmake"
        print(gmakeComment)
        os.execute(gmakeComment)
        
        -- build
        local makeCommand = EMCC_PATH .. "emmake make " .. projectName
        print(makeCommand)
        os.execute(makeCommand)

        -- get all object files
        local files = getfilesWithExtension(workspaceLocation .. "/bin-int", ".o")
        for i, filename in ipairs(files) do
            files[i] = "bin-int/" .. filename
        end

        local filesString = table.concat(files, " ")
        local buildCommand = EMCC_PATH .. "em++ " .. filesString .. " -o " .. binOutput .. "/" .. projectName .. ".html"
        print(buildCommand)
        os.execute(buildCommand)
    end,

    onEnd = function()
        print("WASM Build completed.")
    end
}

return m