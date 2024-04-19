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

        removefiles { enginedir .. "/%{prj.name}/ThirdParties/**" }

        includedirs { moduledir .. "/**", enginedir .. "/%{prj.name}" }

        links { "Core", "Assets", "RenderAPI", "Scene" }

        -- assimp
        includedirs { enginedir .. "/%{prj.name}/ThirdParties/assimp/include" }
        libdirs { enginedir .. "/%{prj.name}/ThirdParties/assimp/lib/Debug" }
        links { "assimp-vc143-mtd" }

        postbuildcommands {
            "{COPYFILE} ThirdParties/assimp/bin/Debug/assimp-vc143-mtd.dll bin/assimp-vc143-mtd.dll"
        }
group ""
