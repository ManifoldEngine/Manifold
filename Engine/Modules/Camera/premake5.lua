include "../../../locations.lua"

project "Camera"
    kind "StaticLib"
    location (moduledir .. "/%{prj.name}")
    
    files { 
        moduledir .. "/%{prj.name}/**.h",
        moduledir .. "/%{prj.name}/**.cpp" 
    }

    links { "Core", "ECS" }

    includedirs { moduledir .. "/**" }