local locations = require("locations")

group "Engine"
    project "Events"
        kind "StaticLib"
        location (locations.moduledir .. "/%{prj.name}")
        
        files { 
            locations.moduledir .. "/%{prj.name}/**.h",
            locations.moduledir .. "/%{prj.name}/**.cpp" 
        }

        includedirs { locations.moduledir .. "/**" }
group ""

group "_TestLibs"
    project "EventsTests"
        kind "SharedLib"
        location (locations.moduledir .. "/Events/Tests")

        files { locations.moduledir .. "/Events/Tests/**.h", locations.moduledir .. "/Events/Tests/**.cpp" }
        
        links { "Events" }
        
        includedirs { locations.moduledir .. "/**" }
group ""