cmake_minimum_required(VERSION 3.21)
project(RayguiTest CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Locate dependencies via vcpkg automated targets
find_package(raylib CONFIG REQUIRED)
find_package(raygui CONFIG REQUIRED)

# 2. Define executable
add_executable(raygui_test main.cpp)

# 3. Link targets (vcpkg automatically resolves libraries/include directories)
target_link_libraries(raygui_test PRIVATE raylib raygui)
