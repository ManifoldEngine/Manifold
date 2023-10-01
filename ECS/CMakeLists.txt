add_library(ECS SHARED "ECS.cpp" "ECS.h" "Entity.h"  "EntityRegistry.h" "EntityRegistry.cpp" "RegistryView.h"  )

target_include_directories(ECS PUBLIC ${PROJECT_SOURCE_DIR})

target_compile_definitions(ECS PUBLIC ECS_EXPORTS)