include "../../../locations.lua"

group "Engine"
    project "OpenGL"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")

        files { 
            moduledir .. "/%{prj.name}/**.h", 
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "Events", "ECSEUtils", "Camera" }

        includedirs { moduledir .. "/**" }

        defines { "GLEW_STATIC" }

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
group ""

group "_TestLibs"
    project "OpenGLTests"
        kind "SharedLib"
        location (moduledir .. "/OpenGL/Tests")

        files { moduledir .. "/OpenGL/Tests/**.h", moduledir .. "/OpenGL/Tests/**.cpp" }
        
        links { "OpenGL" }
        
        includedirs { thirdpartiesdir .. "/SimpleTests/include", moduledir .. "/**" }
group ""