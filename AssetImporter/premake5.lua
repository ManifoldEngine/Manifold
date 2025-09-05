include "../../../locations.lua"

group "AssetImporter"
    project "AssetImporter"
        kind "ConsoleApp"
        location (enginedir .. "/%{prj.name}")
        
        targetdir (enginedir .. "/%{prj.name}/bin/")
        objdir (enginedir .. "/%{prj.name}/bin-int/")

        files { 
            enginedir .. "/%{prj.name}/**.h",
            enginedir .. "/%{prj.name}/**.cpp" 
        }
        
        includedirs { moduledir .. "/**", enginedir .. "/%{prj.name}/" }

        removefiles { enginedir .. "/%{prj.name}/ThirdParties/**" }

        links { "Core", "Resources", "RenderAPI" }

        -- assimp

        assimpPath = enginedir .. "/%{prj.name}/ThirdParties/assimp"

        includedirs { assimpPath .. "/include" }
        libdirs { assimpPath .. "/lib/Debug" }
        links { "assimp-vc143-mtd" }

        postbuildcommands {
            "{COPYFILE} " .. assimpPath .. "/bin/Debug/assimp-vc143-mtd.dll bin/assimp-vc143-mtd.dll"
        }
group ""
