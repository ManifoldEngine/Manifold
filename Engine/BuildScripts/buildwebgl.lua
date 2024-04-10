-- buildwebgl.lua

premake.modules.buildwebgl = {}

local m = premake.modules.buildwebgl

local THIRD_PARTIES_PATH = "Engine\\ThirdParties" 
local EMCC_PATH = THIRD_PARTIES_PATH .. "\\emscripten\\upstream\\emscripten\\"
local PREMAKE_PATH = THIRD_PARTIES_PATH .. "\\premake\\premake5.exe"

local binOutput = "bin/webgl"
local binIntOutput = "bin-int/webgl"

-- windows specific function
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

        print("Building for " .. prj.name)
            
        -- build
        os.execute(EMCC_PATH .. "emmake --help")

        local makeCommand = EMCC_PATH .. "emmake make " .. prj.name
        print(makeCommand)
        os.execute(makeCommand)

        -- get all object files
        local files = getfilesWithExtension(binIntOutput .. "/", ".o")
        for i, filename in ipairs(files) do
            files[i] = binIntOutput .. "/" .. filename
        end

        local filesString = table.concat(files, " ")
        local buildCommand = EMCC_PATH .. "em++ " .. filesString .. " -o " .. binOutput .. "/" .. prj.name .. ".html"
        print(buildCommand)
        os.execute(buildCommand)
    end,

    onEnd = function()
        print("WASM Build completed.")
    end
}

return m