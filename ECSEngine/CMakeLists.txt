# CMakeList.txt : CMake project for ECSEngine, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (ECSEngine "ECSEngine.cpp" "ECSEngine.h")

target_link_libraries(ECSEngine PRIVATE "ECS")

# TODO: Add tests and install targets if needed.
