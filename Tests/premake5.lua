include "../../../locations.lua"

group ""
    project "Tests"
        kind "ConsoleApp"
        location (enginedir .. "/%{prj.name}")

        files { enginedir .. "/%{prj.name}/**.h", enginedir .. "/%{prj.name}/**.cpp" }
        
        links { "Core", "ECS", "OpenGL" }
        
        includedirs { thirdpartiesdir .. "/SimpleTests/include", moduledir }

        defines { "MANI_TESTS_BUILD_OUTPUTDIR=\"" .. outputdir .. "/\"" }
group ""