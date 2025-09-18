local locations = require("locations")

group "Engine"
    project "FloatingCamera"
        kind "StaticLib"
        location (locations.moduledir .. "/%{prj.name}")
        
        files { 
            locations.moduledir .. "/%{prj.name}/**.h",
            locations.moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "Camera", "Inputs" }

        includedirs { locations.moduledir .. "/**" }
group ""
