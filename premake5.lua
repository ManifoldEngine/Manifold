include "locations.lua"

workspace "ECSEngine"

configurations { "Debug", "Release", "Distribution" }
    platforms { "Win64", "MacOSX" }
    startproject "Sandbox"
    language "C++"
    cppdialect "C++20"
    flags { "MultiProcessorCompile" }
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)

    --glm
    includedirs { thirdpartiesdir .. "/glm" }
    --json
    includedirs { thirdpartiesdir .. "/json/include"}
      
    filter "configurations:Debug"
        includedirs { thirdpartiesdir .. "/SimpleTests/include" }
        defines { "ECSE_DEBUG" }
        defines { "ECSE_PROJECTROOT_PATH=\"".. _MAIN_SCRIPT_DIR .. "\"" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "ECSE_RELEASE" }
        defines { "ECSE_PROJECTROOT_PATH=\"".. _MAIN_SCRIPT_DIR .. "\"" }
        optimize "On"

    filter "configurations:Distribution"
        defines { "ECSE_DISTRIBUTION" }
        optimize "On"

    filter "platforms:MacOS"
        architecture "x64"
        system "macosx"

    filter "platforms:Win64"
        architecture "x64"
        system "windows"
    
    filter "system:macosx"
        defines { "ECSE_MACOSX" }
            
    filter "system:windows"
        defines { "ECSE_WINDOWS" }

group "Engine"
    include "Engine"

group ""
-- Executables
project "Sandbox"
    kind "ConsoleApp"
    location "%{prj.name}"

    files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }

    links { "Core", "OpenGL", "ECS", "Camera", "FloatingCamera", "Assets", "RenderAPI" }

    includedirs { moduledir .. "/**", "/%{prj.name}" }

    -- glew
    includedirs { thirdpartiesdir .. "/glew-2.2.0/include" }
    libdirs { thirdpartiesdir .. "/glew-2.2.0/lib/Release/x64/" }
    links { "glew32s" }

    ---- glfw
    includedirs { thirdpartiesdir .. "/glfw-3.3.8.bin.WIN64/include" }
    libdirs { thirdpartiesdir .. "/glfw-3.3.8.bin.WIN64/lib-vc2022/" }
    links { "glfw3" }

    -- openGL
    links { "OpenGL32" }

    -- stb (image)
    includedirs { thirdpartiesdir .. "/stb"}

