add_library(ECS SHARED "ECS.cpp" "ECS.h" "Entity.h"  "EntityRegistry.h" "EntityRegistry.cpp" "RegistryView.h"   "Tests/EntityRegistryTests.cpp")

target_include_directories(ECS PUBLIC ${PROJECT_SOURCE_DIR})

target_compile_definitions(ECS PUBLIC ECS_EXPORTS)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory("Tests")
endif()