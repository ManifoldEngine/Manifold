local locations = require("locations")
group "Engine"
    project "UI"
        kind "StaticLib"
        location (locations.moduledir .. "/%{prj.name}")
        
        files { 
            locations.moduledir .. "/%{prj.name}/**.h",
            locations.moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "RenderAPI", "Camera", "Resources" }

        includedirs { locations.moduledir .. "/**" }
group ""
