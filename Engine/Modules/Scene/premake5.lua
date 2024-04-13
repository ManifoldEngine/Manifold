include "../../../locations.lua"

group "Engine"
    project "Scene"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        includedirs { moduledir .. "/**" }
group ""

group "_TestLibs"
    project "SceneTests"
        kind "SharedLib"
        location (moduledir .. "/Scene/Tests")

        files { moduledir .. "/Scene/Tests/**.h", moduledir .. "/Scene/Tests/**.cpp" }
        
        links { "Scene" }
        
        includedirs { thirdpartiesdir .. "/SimpleTests/include", moduledir .. "/**" }
group ""