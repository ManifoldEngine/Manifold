workspace "ECSEngine"
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    moduledir = "Modules"
    thirdpartiesdir = "third-parties"

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
        includedirs { thirdpartiesdir .. "/SimpleTests" }
    
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

    includedirs { moduledir, moduledir .. "/%{prj.name}" }

    defines { "%{prj.name}_EXPORTS" }

project "OpenGL"
    kind "SharedLib"
    location (moduledir .. "/%{prj.name}")

    files { moduledir .. "/%{prj.name}/**.h", moduledir .. "/%{prj.name}/**.cpp" }

    links { "Core" }

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

project "Tests"
    kind "ConsoleApp"
    location "%{prj.name}"

    filter "configurations:Test"
        files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }
        links { "Core", "ECS" }
        includedirs {  moduledir }