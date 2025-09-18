local locations = require("locations")

group "Engine"
    project "Core"
        kind "StaticLib"
        location (locations.moduledir .. "/%{prj.name}")

        files { 
            locations.moduledir .. "/%{prj.name}/**.h",
            locations.moduledir .. "/%{prj.name}/**.cpp" 
        }

        includedirs { locations.moduledir .. "/**" }
group ""

group "_TestLibs"
    project "CoreTests"
        kind "SharedLib"
        location (locations.moduledir .. "/Core/Tests")

        files { locations.moduledir .. "/Core/Tests/**.h", locations.moduledir .. "/Core/Tests/**.cpp" }
        
        links { "Core" }
        
        includedirs { locations.moduledir .. "/**" }
group ""