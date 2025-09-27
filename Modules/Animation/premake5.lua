local locations = require("locations")
group "Engine"
    project "Animation"
        kind "StaticLib"
        location (locations.moduledir .. "/%{prj.name}")
        
        files { 
            locations.moduledir .. "/%{prj.name}/**.h",
            locations.moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "RenderAPI", "Camera", "Resources" }

        includedirs { locations.moduledir .. "/**" }
group ""

group "_TestLibs"
        project "AnimationTests"
            kind "SharedLib"
            location (locations.moduledir .. "/Animation/Tests")

            files { locations.moduledir .. "/Animation/Tests/**.h", locations.moduledir .. "/Animation/Tests/**.cpp" }
            
            links { "Core", "Animation" }
            
            includedirs { locations.moduledir .. "/**" }
group ""