include "../../../locations.lua"

group "Engine"
    project "Events"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        includedirs { moduledir .. "/**" }
group ""

group "_TestLibs"
    project "EventsTests"
        kind "SharedLib"
        location (moduledir .. "/Events/Tests")

        files { moduledir .. "/Events/Tests/**.h", moduledir .. "/Events/Tests/**.cpp" }
        
        links { "Events" }
        
        includedirs { thirdpartiesdir .. "/SimpleTests/include", moduledir .. "/**" }
group ""