add_library(ECS SHARED "ECS.cpp" "ECS.h" "Entity.h"  "EntityRegistry.h" "EntityRegistry.cpp")

target_link_libraries(ECS PRIVATE "OSUtils")

target_include_directories(ECS PUBLIC ${PROJECT_SOURCE_DIR})