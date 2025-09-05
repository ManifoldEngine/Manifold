include "../../../locations.lua"

group "Engine"
    project "OpenGL"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")

        files { 
            moduledir .. "/%{prj.name}/**.h", 
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "Camera", "Inputs" }

        includedirs { moduledir .. "/**" }

        -- gl3w
        includedirs { moduledir .. "/%{prj.name}/ThirdParties/gl3w/include" }
        files {
            moduledir .. "/%{prj.name}/ThirdParties/gl3w/include/**.h",
            moduledir .. "/%{prj.name}/ThirdParties/gl3w/src/gl3w.c"
        }

        -- glfw
        includedirs { moduledir .. "/%{prj.name}/ThirdParties/glfw/include" }
        libdirs { moduledir .. "/%{prj.name}/ThirdParties/glfw/lib-vc2022" }
        links { "glfw3" }

        -- openGL
        links { "OpenGL32" }
    
        -- stb (image)
        includedirs { moduledir .. "/%{prj.name}/ThirdParties/stb" }

        -- webgl
        filter("platforms:WebGL")
            includedirs { thirdpartiesdir .. "emscripten/upstream/emscripten/cache/sysroot/include" }
            removeincludedirs { thirdpartiesdir .. "/glew-2.2.0/include" }
            removelibdirs { thirdpartiesdir .. "/glew-2.2.0/lib/Release/x64/" }
            removeincludedirs { thirdpartiesdir .. "/glfw-3.3.8.bin.WIN64/include" }
            removelibdirs { thirdpartiesdir .. "/glfw-3.3.8.bin.WIN64/lib-vc2022/" }
group ""
