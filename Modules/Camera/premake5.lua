local locations = require("locations")
group "Engine"
    project "Camera"
        kind "StaticLib"
        location (locations.moduledir .. "/%{prj.name}")
        
        files { 
            locations.moduledir .. "/%{prj.name}/**.h",
            locations.moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core" }

        includedirs { locations.moduledir .. "/**" }
group ""