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

group "_TestLibs"
        project "CameraTests"
            kind "SharedLib"
            location (locations.moduledir .. "/Camera/Tests")

            files { locations.moduledir .. "/Camera/Tests/**.h", locations.moduledir .. "/Camera/Tests/**.cpp" }
            
            links { "Core", "Camera" }
            
            includedirs { locations.moduledir .. "/**" }
group ""