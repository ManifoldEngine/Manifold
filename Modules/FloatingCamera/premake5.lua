include "../../../locations.lua"

group "Engine"
    project "FloatingCamera"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "Camera", "Inputs" }

        includedirs { moduledir .. "/**" }
group ""
