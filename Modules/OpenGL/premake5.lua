local locations = require("locations")

group "Engine"
    project "OpenGL"
        kind "StaticLib"
        location (locations.moduledir .. "/%{prj.name}")

        files { 
            locations.moduledir .. "/%{prj.name}/**.h", 
            locations.moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "Camera", "Inputs", "STB" }

        includedirs { locations.moduledir .. "/**" }

        -- gl3w
        includedirs { locations.moduledir .. "/%{prj.name}/ThirdParties/gl3w/include" }
        files {
            locations.moduledir .. "/%{prj.name}/ThirdParties/gl3w/include/**.h",
            locations.moduledir .. "/%{prj.name}/ThirdParties/gl3w/src/gl3w.c"
        }

        -- glfw
        includedirs { locations.moduledir .. "/%{prj.name}/ThirdParties/glfw/include" }
        libdirs { locations.moduledir .. "/%{prj.name}/ThirdParties/glfw/lib-vc2022" }
        links { "glfw3" }

        -- openGL
        links { "OpenGL32" }

        -- webgl
        filter("platforms:WebGL")
            includedirs { locations.thirdpartiesdir .. "emscripten/upstream/emscripten/cache/sysroot/include" }
            removeincludedirs { locations.thirdpartiesdir .. "/glew-2.2.0/include" }
            removelibdirs { locations.thirdpartiesdir .. "/glew-2.2.0/lib/Release/x64/" }
            removeincludedirs { locations.thirdpartiesdir .. "/glfw-3.3.8.bin.WIN64/include" }
            removelibdirs { locations.thirdpartiesdir .. "/glfw-3.3.8.bin.WIN64/lib-vc2022/" }
group ""
