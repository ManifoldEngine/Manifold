local locations = require("locations")

group "Engine"
    project "FMod"
        kind "StaticLib"
        location (locations.moduledir .. "/%{prj.name}")
        
        files { 
            locations.moduledir .. "/%{prj.name}/**.h",
            locations.moduledir .. "/%{prj.name}/**.cpp" 
        }

        includedirs { locations.moduledir .. "/**" }

        links { "Core", "Resources" }

        --fmod
        local ThirdParties = locations.moduledir .. "/%{prj.name}/ThirdParties/"
        includedirs { ThirdParties .. "FMod/api/core/inc" }
        libdirs { ThirdParties .. "FMod/api/core/lib/x64" }
        links { "fmod_vc" }

        postbuildcommands {
            "{COPYFILE} " .. ThirdParties .. "FMod/api/core/lib/x64/fmod.dll " .. locations.bin .. "/" .. locations.outputdir .. "/fmod.dll",
            "echo Build completed."
        }
group ""
