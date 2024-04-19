include "../../../locations.lua"

group "Engine"
    project "Collision"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core" }

        includedirs { moduledir .. "/**" }
group ""

group "_TestLibs"
    project "CollisionTests"
        kind "SharedLib"
        location (moduledir .. "/Collision/Tests")

        files { moduledir .. "/Collision/Tests/**.h", moduledir .. "/Collision/Tests/**.cpp" }
        
        links { "Collision", "Core" }
        
        includedirs { thirdpartiesdir .. "/SimpleTests/include", moduledir .. "/**" }
group ""