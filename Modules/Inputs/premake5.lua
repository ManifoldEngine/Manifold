include "../../../locations.lua"

group "Engine"
    project "Inputs"
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
    project "InputsTests"
        kind "SharedLib"
        location (moduledir .. "/Inputs/Tests")

        files { moduledir .. "/Inputs/Tests/**.h", moduledir .. "/Inputs/Tests/**.cpp" }
        
        links { "Inputs", "Core" }
        
        includedirs { thirdpartiesdir .. "/SimpleTests/include", moduledir .. "/**" }
group ""