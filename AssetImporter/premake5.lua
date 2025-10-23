local locations = require("locations")

group "AssetImporter"
    project "AssetImporter"
        kind "ConsoleApp"
        location (locations.enginedir .. "/%{prj.name}")
        
        targetdir (locations.enginedir .. "/%{prj.name}/bin/")
        objdir (locations.enginedir .. "/%{prj.name}/bin-int/")

        files { 
            locations.enginedir .. "/%{prj.name}/Sources/**.h",
            locations.enginedir .. "/%{prj.name}/Sources/**.cpp" 
        }
        
        includedirs { locations.moduledir .. "/**", locations.enginedir .. "/%{prj.name}/Sources" }

        links { "Core", "Resources", "RenderAPI", "Sprite" }

        -- assimp
        local assimpPath = (locations.enginedir .. "/%{prj.name}/ThirdParties/assimp")

        includedirs { assimpPath .. "/include" }
        libdirs { assimpPath .. "/lib/Debug" }
        links { "assimp-vc143-mtd" }

        postbuildcommands {
            "{COPYFILE} " .. locations.enginedir .. "/%{prj.name}/bin/%{prj.name}.exe " .. locations.enginedir .. "/%{prj.name}/Build/%{prj.name}.exe",
            "echo Build completed."
        }
group ""
