include "../../../locations.lua"
group "Engine"
    project "Camera"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "ECS" }

        includedirs { moduledir .. "/**" }
group ""