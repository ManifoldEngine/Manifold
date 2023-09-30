add_library(ECS "ECS.cpp" "ECS.h" "Entity.h"  "EntityRegistry.h" "EntityRegistry.cpp")

target_include_directories(ECS PUBLIC ${PROJECT_SOURCE_DIR})