include "locations.lua"
include "Engine/BuildScripts/buildwebgl.lua"

workspace "Mani"

configurations { "Debug", "Release", "Distribution" }
    platforms { "Win64", "MacOSX", "WebGL" }
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
    --physx
    includedirs { thirdpartiesdir .. "/PhysX/physx/include"}

    filter "configurations:Debug"
        includedirs { thirdpartiesdir .. "/SimpleTests/include" }
        defines { "MANI_DEBUG" }
        defines { "MANI_PROJECTROOT_PATH=\"".. _MAIN_SCRIPT_DIR .. "\"" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "MANI_RELEASE" }
        defines { "MANI_PROJECTROOT_PATH=\"".. _MAIN_SCRIPT_DIR .. "\"" }
        optimize "On"

    filter "configurations:Distribution"
        defines { "MANI_DISTRIBUTION" }
        optimize "On"

    filter "platforms:MacOS"
        architecture "x64"
        system "macosx"

    filter "platforms:Win64"
        architecture "x64"
        system "windows"

    filter "platforms:WebGL"
        defines { "MANI_WEBGL" }
        linkoptions { "-sUSE_GLFW=3", "-sMAX_WEBGL_VERSION=2" }
        -- rtti ("On")
        system "windows"
    
    filter "system:macosx"
        defines { "MANI_MACOSX" }
            
    filter "system:windows"
        defines { "MANI_WINDOWS" }

group "Engine"
    include "Engine"

group ""
-- Executables
project "Sandbox"
    kind "ConsoleApp"
    location "%{prj.name}"

    files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }

    links { "Core", "OpenGL", "ECS", "Camera", "FloatingCamera", "Assets", "RenderAPI", "PhysX", "Inputs" }

    includedirs { moduledir .. "/**", "/%{prj.name}/Sources/" }

