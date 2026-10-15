cmake_minimum_required(VERSION 3.21)
project(RayguiTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_path(RAYGUI_INCLUDE_DIRS NAMES raygui.h REQUIRED)

add_executable(raygui_test main.cpp)

target_include_directories(raygui_test PRIVATE
    "${RAYGUI_INCLUDE_DIRS}"
)

target_include_directories(raygui_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-linux/include"
)


target_link_libraries(raygui_test PRIVATE raylib glfw)