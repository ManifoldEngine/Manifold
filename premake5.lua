workspace "ECSEngine"
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    enginedir = "Engine"
    moduledir = enginedir .. "/Modules"
    thirdpartiesdir = enginedir .. "/ThirdParties"

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

-- Modules
project "Core"
    kind "SharedLib"
    location (moduledir .. "/%{prj.name}")

    files { moduledir .. "/%{prj.name}/**.h", moduledir .. "/%{prj.name}/**.cpp" }

    links { "ECS", "Utils" }

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

project "Utils"
    kind "StaticLib"
    location (moduledir .. "/%{prj.name}")

    files { moduledir .. "/%{prj.name}/**.h", moduledir .. "/%{prj.name}/**.cpp" }

    includedirs { moduledir, moduledir .. "/%{prj.name}" }

project "OpenGL"
    kind "SharedLib"
    location (moduledir .. "/%{prj.name}")

    files { moduledir .. "/%{prj.name}/**.h", moduledir .. "/%{prj.name}/**.cpp" }

    links { "Core", "Events", "Utils" }

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

    -- stb (image)
    includedirs { thirdpartiesdir .. "/stb"}
    
    filter("configurations:Debug")
        defines { "ECSE_OPENGL_DEBUG" }

-- Executables
project "Sandbox"
    kind "ConsoleApp"
    location "%{prj.name}"

    files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }

    links { "Core", "OpenGL" }

    includedirs { moduledir, "/%{prj.name}" }

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

    -- stb (image)
    includedirs { thirdpartiesdir .. "/stb"}
project "Tests"
    kind "ConsoleApp"
    location (enginedir .. "/%{prj.name}")

    files { enginedir .. "/%{prj.name}/**.h", enginedir .. "/%{prj.name}/**.cpp" }
    
    links { "Core", "ECS", "OpenGL" }
    
    includedirs { thirdpartiesdir .. "/SimpleTests/include", moduledir }
