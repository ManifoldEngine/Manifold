local locations = require("locations")

group "AssetImporter"
    project "AssetImporter"
        kind "ConsoleApp"
        location (locations.enginedir .. "/%{prj.name}")
        
        targetdir (locations.enginedir .. "/%{prj.name}/bin/")
        objdir (locations.enginedir .. "/%{prj.name}/bin-int/")

        files { 
            locations.enginedir .. "/%{prj.name}/**.h",
            locations.enginedir .. "/%{prj.name}/**.cpp" 
        }
        
        includedirs { locations.moduledir .. "/**", locations.enginedir .. "/%{prj.name}/" }

        removefiles { locations.enginedir .. "/%{prj.name}/ThirdParties/**" }

        links { "Core", "Resources", "RenderAPI" }

        -- assimp

        local assimpPath = locations.enginedir .. "/%{prj.name}/ThirdParties/assimp"

        includedirs { assimpPath .. "/include" }
        libdirs { assimpPath .. "/lib/Debug" }
        links { "assimp-vc143-mtd" }
group ""
