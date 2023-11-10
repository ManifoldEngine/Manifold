include "../../../locations.lua"

group "Engine"
    project "ECSEUtils"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        includedirs { moduledir .. "/**" }
group ""