local locations = require("locations")

group "Engine"
    project "Resources"
        kind "StaticLib"
        location (locations.moduledir .. "/%{prj.name}")
        
        files { 
            locations.moduledir .. "/%{prj.name}/**.h",
            locations.moduledir .. "/%{prj.name}/**.cpp" 
        }

        includedirs { locations.moduledir .. "/**" }
group ""

group "_TestLibs"
    project "ResourcesTests"
        kind "SharedLib"
        location (locations.moduledir .. "/Resources/Tests/")

        files { locations.moduledir .. "/Resources/Tests/**.h", locations.moduledir .. "/Resources/Tests/**.cpp" }

        links { "Core", "Resources" }

        includedirs { locations.moduledir .. "/**" }
group""