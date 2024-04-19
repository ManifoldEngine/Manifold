include "../../../locations.lua"

group "Engine"
    project "OpenGL"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")

        files { 
            moduledir .. "/%{prj.name}/**.h", 
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "Events", "MANIUtils", "Camera", "Inputs" }

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
           defines { "MANI_OPENGL_DEBUG" }
           
        -- webgl
        filter("platforms:WebGL")
            includedirs { thirdpartiesdir .. "emscripten/upstream/emscripten/cache/sysroot/include" }
            removeincludedirs { thirdpartiesdir .. "/glew-2.2.0/include" }
            removelibdirs { thirdpartiesdir .. "/glew-2.2.0/lib/Release/x64/" }
            removeincludedirs { thirdpartiesdir .. "/glfw-3.3.8.bin.WIN64/include" }
            removelibdirs { thirdpartiesdir .. "/glfw-3.3.8.bin.WIN64/lib-vc2022/" }
group ""

group "_TestLibs"
    project "OpenGLTests"
        kind "SharedLib"
        location (moduledir .. "/OpenGL/Tests")

        files { moduledir .. "/OpenGL/Tests/**.h", moduledir .. "/OpenGL/Tests/**.cpp" }
        
        links { "OpenGL" }
        
        includedirs { thirdpartiesdir .. "/SimpleTests/include", moduledir .. "/**" }
group ""