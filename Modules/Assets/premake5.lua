include "../../../locations.lua"

group "Engine"
    -- reflect-cpp    
    includedirs { thirdpartiesdir .. "/reflect-cpp/include" }
    
    project "Assets"
        kind "StaticLib"
        location (moduledir .. "/%{prj.name}")
        
        files { 
            moduledir .. "/%{prj.name}/**.h",
            moduledir .. "/%{prj.name}/**.cpp" 
        }

        includedirs { moduledir .. "/**" }

        -- reflect-cpp    
        libdirs { thirdpartiesdir .. "/reflect-cpp/build/Release" }
        links { "reflectcpp" }
group ""