local locations = require("locations")

group "Engine"
    project "Inputs"
        kind "StaticLib"
        location (locations.moduledir .. "/%{prj.name}")
        
        files { 
            locations.moduledir .. "/%{prj.name}/**.h",
            locations.moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core" }

        includedirs { locations.moduledir .. "/**" }
group ""

group "_TestLibs"
    project "InputsTests"
        kind "SharedLib"
        location (locations.moduledir .. "/Inputs/Tests")

        files { locations.moduledir .. "/Inputs/Tests/**.h", locations.moduledir .. "/Inputs/Tests/**.cpp" }
        
        links { "Inputs", "Core" }
        
        includedirs { locations.moduledir .. "/**" }
group ""