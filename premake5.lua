workspace "ECSEngine"
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    moduledir = "Modules"

    configurations { "Debug", "Test", "Release" }
    startproject "Sandbox"
    architecture "x64"
    language "C++"
    cppdialect "C++20"
    flags { "MultiProcessorCompile" }
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)
    
    filter "configurations:Debug"
        defines { "ECSE_DEBUG" }
        symbols "On"

    filter "configurations:Test"
        defines { "ECSE_DEBUG", "ECSE_TEST" }
        symbols "On"
        includedirs { "third-parties/SimpleTests" }
    
    filter "configurations:Release"
        defines { "ECSE_RELEASE" }
        optimize "On"
    
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

    includedirs { moduledir .. "/%{prj.name}" }

    defines { "%{prj.name}_EXPORTS" }

-- Executables
project "Sandbox"
    kind "ConsoleApp"
    location "%{prj.name}"

    files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }

    links { "Core" }

    includedirs { moduledir }

    defines { "%{prj.name}_EXPORTS" }

project "Tests"
    kind "ConsoleApp"
    location "%{prj.name}"

    filter "configurations:Test"
        files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }
        links { "Core", "ECS" }
        includedirs {  moduledir }