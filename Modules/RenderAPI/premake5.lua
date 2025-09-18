local locations = require("locations")

group "Engine"
    project "RenderAPI"
        kind "StaticLib"
        location (locations.moduledir .. "/%{prj.name}")
        
        files { 
            locations.moduledir .. "/%{prj.name}/**.h",
            locations.moduledir .. "/%{prj.name}/**.cpp" 
        }

        includedirs { locations.moduledir .. "/**" }
group ""
