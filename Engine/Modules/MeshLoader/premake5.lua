include "../../../locations.lua"

group "Engine"
    project "MeshLoader"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        includedirs { moduledir .. "/**" }

        links { "Core" }

        -- assimp
        includedirs { thirdpartiesdir .. "/assimp/include" }
        libdirs { thirdpartiesdir .. "/assimp/lib/Debug" }
        links { "assimp-vc143-mtd" }
group ""

group "_TestLibs"
    project "MeshLoaderTests"
        kind "SharedLib"
        location (moduledir .. "/MeshLoader/Tests")

        files { moduledir .. "/MeshLoader/Tests/**.h", moduledir .. "/MeshLoader/Tests/**.cpp" }
        
        links { "MeshLoader" }
        
        includedirs { thirdpartiesdir .. "/SimpleTests/include", moduledir .. "/**" }
group ""