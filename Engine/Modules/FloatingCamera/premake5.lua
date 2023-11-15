include "../../../locations.lua"

group "Engine"
    project "FloatingCamera"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "Camera", "Inputs" }

        includedirs { moduledir .. "/**" }
group ""

group "_TestLibs"
    project "FloatingCameraTests"
        kind "SharedLib"
        location (moduledir .. "/FloatingCamera/Tests")

        files { moduledir .. "/FloatingCamera/Tests/**.h", moduledir .. "/FloatingCamera/Tests/**.cpp" }
        
        links { "FloatingCamera" }
        
        includedirs { thirdpartiesdir .. "/SimpleTests/include", moduledir .. "/**" }
group ""