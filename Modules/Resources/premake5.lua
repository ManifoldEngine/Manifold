include "../../../locations.lua"

group "Engine"
    project "Resources"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        includedirs { moduledir .. "/**" }
group ""