local locations = require("locations")

group ""
    project "Tests"
        kind "ConsoleApp"
        location (locations.enginedir .. "/%{prj.name}")

        files { locations.enginedir .. "/%{prj.name}/**.h", locations.enginedir .. "/%{prj.name}/**.cpp" }
                
        includedirs { locations.thirdpartiesdir .. "/SimpleTests/include", locations.moduledir }

        defines { "MANI_TESTS_BUILD_OUTPUTDIR=\"" .. locations.outputdir .. "/\"" }
group ""