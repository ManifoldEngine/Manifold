workspace "ECSEngine"
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    moduledir = "Modules"
    thirdpartiesdir = "third-parties"

    configurations { "Debug", "Release" }
    platforms { "Win64", "MacOSX" }
    startproject "Sandbox"
    language "C++"
    cppdialect "C++20"
    flags { "MultiProcessorCompile" }
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)
    
    filter "configurations:Debug"
        defines { "ECSE_DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "ECSE_RELEASE" }
        optimize "On"

    filter "platforms:Win64"
        architecture "x64"
        system "windows"

    filter "platforms:MacOS"
        architecture "x64"
        system "macosx"
    
    filter "system:windows"
        defines { "ECSE_WINDOWS" }

    filter "system:macosx"
        defines { "ECSE_MACOSX" }

-- Modules
project "Core"
    kind "SharedLib"
    location (moduledir .. "/%{prj.name}")

    files { moduledir .. "/%{prj.name}/**.h", moduledir .. "/%{prj.name}/**.cpp" }

    links { "ECS" }

    includedirs { moduledir, moduledir .. "/%{prj.name}" }

    defines { "%{prj.name}_EXPORTS" }

project "ECS"
    kind "SharedLib"
    location (moduledir .. "/%{prj.name}")

    files { moduledir .. "/%{prj.name}/**.h", moduledir .. "/%{prj.name}/**.cpp" }

    includedirs { moduledir, moduledir .. "/%{prj.name}" }

    defines { "%{prj.name}_EXPORTS" }

project "Events"
    kind "StaticLib"
    location (moduledir .. "/%{prj.name}")

    files { moduledir .. "/%{prj.name}/**.h", moduledir .. "/%{prj.name}/**.cpp" }

    includedirs { moduledir, moduledir .. "/%{prj.name}" }

project "OpenGL"
    kind "SharedLib"
    location (moduledir .. "/%{prj.name}")

    files { moduledir .. "/%{prj.name}/**.h", moduledir .. "/%{prj.name}/**.cpp" }

    links { "Core", "Events" }

    includedirs { moduledir, moduledir .. "/%{prj.name}" }

    defines { "%{prj.name}_EXPORTS", "GLEW_STATIC" }

    -- glew
    includedirs { thirdpartiesdir .. "/glew-2.2.0/include" }
    libdirs { thirdpartiesdir .. "/glew-2.2.0/lib/Release/x64/" }
    links { "glew32s" }

    -- glfw
    includedirs { thirdpartiesdir .. "/glfw-3.3.8.bin.WIN64/include" }
    libdirs { thirdpartiesdir .. "/glfw-3.3.8.bin.WIN64/lib-vc2022/" }
    links { "glfw3" }

    -- openGL
    links { "OpenGL32" }

-- Executables
project "Sandbox"
    kind "ConsoleApp"
    location "%{prj.name}"

    files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }

    links { "Core", "OpenGL" }

    includedirs { moduledir }

    filter { "platforms:Win64" }
        entrypoint "mainCRTStartup"

project "Tests"
    kind "ConsoleApp"
    location "%{prj.name}"

    files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }
    
    links { "Core", "ECS" }
    
    includedirs { thirdpartiesdir .. "/SimpleTests", moduledir }
