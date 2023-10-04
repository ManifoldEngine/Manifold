workspace "ECSEngine"
    configurations { "Debug", "Release" }
    startproject "Sandbox"
    architecture "x64"
    language "C++"
    cppdialect "C++20"
    flags { "MultiProcessorCompile" }
    
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
    
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Sandbox"
    kind "ConsoleApp"
    location "%{prj.name}"
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)

    files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }

    links { "ECS" }

    includedirs { "ECSEngine", "ECS" }

    defines { "%{prj.name}_EXPORTS" }

project "ECSEngine"
    kind "SharedLib"
    location "%{prj.name}"
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)

    files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }

    includedirs { "ECS/" }

    defines { "%{prj.name}_EXPORTS" }

project "ECS"
    kind "SharedLib"
    location "%{prj.name}"
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)

    files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }

    defines { "%{prj.name}_EXPORTS" }

group "Engine"
    project "ECSEngine"
    project "ECS"

group "Sandbox"
    project "Sandbox"
