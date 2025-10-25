local locations = require("locations")

local m = {}

m.setup = function(workspaceName, projectName)
    -- setup workspace
    workspace (workspaceName)

    -- build configs
    configurations { "Debug", "Release", "Distribution" }
        platforms { "Win64" }
        startproject (projectName)
        language "C++"
        cppdialect "C++23"
        flags { "MultiProcessorCompile" }
        targetdir ("bin/" .. locations.outputdir)
        objdir ("bin-int/" .. locations.outputdir)

        -- ManiMaths
        includedirs { locations.thirdpartiesdir .. "/ManiMaths/include" }
        -- ManiTests
        includedirs { locations.thirdpartiesdir .. "/ManiTests/include" }
        -- ManiZ
        includedirs { locations.thirdpartiesdir .. "/ManiZ/include"}

        -- use opengl coordinate system
        defines { "MANI_COORDINATE_ZMINUSFORWARD_YUP" }

        defines { "MANI_PROJECT_NAME=\"" .. projectName .. "\"" }

        filter "configurations:Debug"
            defines { "MANI_DEBUG", "MANI_CONSOLE_APP" }
            defines { "MANI_PROJECTROOT_PATH=\"".. _MAIN_SCRIPT_DIR .. "\"" }
            symbols "On"
        
        filter "configurations:Release"
            defines { "MANI_RELEASE" }
            defines { "MANI_PROJECTROOT_PATH=\"".. _MAIN_SCRIPT_DIR .. "\"" }
            optimize "On"

        filter "configurations:Distribution"
            defines { "MANI_DISTRIBUTION" }
            optimize "On"

        filter "platforms:Win64"
            architecture "x64"
            system "windows"

        -- filter "platforms:WebGL"
        --     defines { "MANI_WEBGL" }
        --     linkoptions { "-sUSE_GLFW=3", "-sMAX_WEBGL_VERSION=2" }
        --     system "windows"
            
        filter "system:windows"
            defines { "MANI_WINDOWS" }

        filter "kind:ConsoleApp"
            defines { "MANI_CONSOLE_APP" }
end

m.includeEngine = function()
    group "Engine"
        include "Engine"
    group ""
end

return m