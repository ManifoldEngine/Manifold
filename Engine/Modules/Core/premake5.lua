include "../../../locations.lua"

group "Engine"
    project "Core"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")

        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "ECS", "ECSEUtils" }

        includedirs { moduledir .. "/**" }
group ""

group "_TestLibs"
    project "CoreTests"
        kind "SharedLib"
        location (moduledir .. "/Tests")

        files { moduledir .. "/Core/Tests/**.h", moduledir .. "/Core/Tests/**.cpp" }
        
        links { "Core" }
        
        includedirs { thirdpartiesdir .. "/SimpleTests/include", moduledir .. "/**" }
group ""