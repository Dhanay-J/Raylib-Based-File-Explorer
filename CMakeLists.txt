cmake_minimum_required(VERSION 3.12)
project(MyRaylibProject CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Define paths to your local raylib dependency
set(RAYLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/raylib/include")
set(RAYLIB_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/raylib/lib")
set(ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/assets")

# 2. Add your executable
add_executable(raygui_test main.cpp ${ASSETS}/roboto_font.h progressbar.cpp progressbar.hpp)

# 3. Tell CMake where to look for raylib headers
target_include_directories(raygui_test PRIVATE ${RAYLIB_INCLUDE_DIR})

# 4. Tell CMake where to find the static/shared library files
target_link_directories(raygui_test PRIVATE ${RAYLIB_LIB_DIR})

# 5. Link raylib and its required Linux system dependencies
target_link_libraries(raygui_test PRIVATE 
    raylib 
    GL 
    m 
    pthread 
    dl 
    rt 
    X11

)
