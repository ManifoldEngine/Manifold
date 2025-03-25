include "../../../locations.lua"

group "Engine"
    project "ManImGui"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "Events", "Inputs", "OpenGL" }

        includedirs { moduledir .. "/**" }

        -- ImGui
        files {
            thirdpartiesdir .. "/ImGui/*.h",
            thirdpartiesdir .. "/ImGui/*.cpp",
            thirdpartiesdir .. "/ImGui/backends/imgui_impl_glfw.h",
            thirdpartiesdir .. "/ImGui/backends/imgui_impl_glfw.cpp",
            thirdpartiesdir .. "/ImGui/backends/imgui_impl_opengl3.h",
            thirdpartiesdir .. "/ImGui/backends/imgui_impl_opengl3.cpp",
            thirdpartiesdir .. "/ImGui/misc/debuggers/imgui.natvis",
            thirdpartiesdir .. "/ImGui/misc/cpp/imgui_stdlib.*"
        }

        includedirs { thirdpartiesdir .. "/ImGui" }

        -- gl3w
        includedirs { thirdpartiesdir .. "/gl3w/include" }
        files {
            thirdpartiesdir .. "/gl3w/include/**.h",
            thirdpartiesdir .. "/gl3w/src/gl3w.c"
        }

        -- glfw
        includedirs { thirdpartiesdir .. "/glfw/include" }
        libdirs { thirdpartiesdir .. "/glfw/src/Debug" }
        links { "glfw3" }

        -- openGL
        links { "OpenGL32" }
group ""