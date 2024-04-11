include "../../../locations.lua"

group "Engine"
    project "PhysX"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        links { "Core", "RenderAPI" }

        includedirs { moduledir .. "/**" }

        -- physX
        defines { "_DEBUG" }
        includedirs { thirdpartiesdir .. "/PhysX/physx/include"}
        libdirs { thirdpartiesdir .. "/PhysX/physx/bin/win.x86_64.vc143.mt/debug/"}
        links {
            "PhysX_64",
            "PhysXCooking_64",
            "PhysXCommon_64",
            "PhysXExtensions_static_64",
            "PhysXPvdSDK_static_64",
            "PhysXFoundation_64"
        }
        
        filter "platforms:webgl"
            removefiles { 
                moduledir .. "/%{prj.name}/**.h",
                moduledir .. "/%{prj.name}/**.cpp" 
            }
group ""

group "_TestLibs"
    project "PhysXTests"
        kind "SharedLib"
        location (moduledir .. "/PhysX/Tests")

        files { moduledir .. "/PhysX/Tests/**.h", moduledir .. "/PhysX/Tests/**.cpp" }
        
        links { "PhysX" }
        
        includedirs { thirdpartiesdir .. "/SimpleTests/include", moduledir .. "/**" }

        filter "platforms:webgl"
            removefiles { moduledir .. "/PhysX/Tests/**.h", moduledir .. "/PhysX/Tests/**.cpp" }
group ""
