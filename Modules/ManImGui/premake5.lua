include "../../../locations.lua"

group "Engine"
    project "ManImGui"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        ignoredefaultlibraries { "MSVCRTD" }
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "Inputs", "OpenGL" }

        includedirs { moduledir .. "/**" }

        -- ImGui
        imguiPath = moduledir .. "/%{prj.name}/ThirdParties/ImGui"

        removefiles { imguiPath .. "/**" }
        
        files {
            imguiPath .. "/*.h",
            imguiPath .. "/*.cpp",
            imguiPath .. "/backends/imgui_impl_glfw.h",
            imguiPath .. "/backends/imgui_impl_glfw.cpp",
            imguiPath .. "/backends/imgui_impl_opengl3.h",
            imguiPath .. "/backends/imgui_impl_opengl3.cpp",
            imguiPath .. "/misc/debuggers/imgui.natvis",
            imguiPath .. "/misc/cpp/imgui_stdlib.*"
        }

        includedirs { imguiPath }

        -- openGL
        links { "OpenGL32" }
group ""