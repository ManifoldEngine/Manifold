add_library(OSUtils STATIC "OSUtils.cpp" "OSUtils.h")

target_include_directories(OSUtils PUBLIC ${PROJECT_SOURCE_DIR})

set_property(TARGET OSUtils PROPERTY CXX_STANDARD 20)