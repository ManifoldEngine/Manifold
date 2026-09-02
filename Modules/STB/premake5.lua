local locations = require("locations")

group "Engine"
    project "STB"
        kind "StaticLib"
        location (locations.moduledir .. "/%{prj.name}")

        files { 
            locations.moduledir .. "/%{prj.name}/**.h", 
            locations.moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "Resources", "RenderAPI" }

        includedirs { locations.moduledir .. "/**" }
        -- stb
        includedirs { locations.moduledir .. "/%{prj.name}/ThirdParties/stb" }
group ""
