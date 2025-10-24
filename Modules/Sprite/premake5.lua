local locations = require("locations")
group "Engine"
    project "Sprite"
        kind "StaticLib"
        location (locations.moduledir .. "/%{prj.name}")
        
        files { 
            locations.moduledir .. "/%{prj.name}/**.h",
            locations.moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "RenderAPI", "Resources" }

        includedirs { locations.moduledir .. "/**" }
group ""

group "_TestLibs"
        project "SpriteTests"
            kind "SharedLib"
            location (locations.moduledir .. "/Sprite/Tests")

            files { locations.moduledir .. "/Sprite/Tests/**.h", locations.moduledir .. "/Sprite/Tests/**.cpp" }
            
            links { "Core", "Sprite" }

            -- we're linking opengl just so we can test the full flow, and as such we need any texture loader implementation
            links { "OpenGL" }
            
            includedirs { locations.moduledir .. "/**" }
group ""