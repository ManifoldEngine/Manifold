include "../../../locations.lua"

group "Engine"
    project "ECS"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        includedirs { moduledir .. "/**" }
group ""

group "_TestLibs"
    project "ECSTests"
        kind "SharedLib"
        location (moduledir .. "/ECS/Tests")

        files { moduledir .. "/ECS/Tests/**.h", moduledir .. "/ECS/Tests/**.cpp" }
        
        links { "ECS" }
        
        includedirs { thirdpartiesdir .. "/SimpleTests/include", moduledir .. "/**" }
group ""