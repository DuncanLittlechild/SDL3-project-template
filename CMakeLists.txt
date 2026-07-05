cmake_minimum_required(VERSION 3.24)
project(low_level_game)

set(CMAKE_CXX_STANDARD 23)
# Toggle to set whether this is a prod build (ON) or debug (OFF)
set(PRODUCTION_BUILD OFF CACHE BOOL "Make this a production build" FORCE)

# Statically links the c and c++ runtime libraries
# this way of doing this only works on g++, MSCV or clang emulating g++ doesn't really work for this
add_link_options(-static)


# Forces SDL3_ttf to use the vendored versions of its libraries found in its./external folder instead of system ones
set(SDLTTF_VENDORED ON CACHE BOOL "Forces SDL3_ttf to use the vendored versions of its libraries" FORCE)
# Forces SDL3_image to use the vendored versions of its libraries found in its./external folder instead of system ones
set(SDLIMAGE_VENDORED ON CACHE BOOL "Forces SDL3_image to use the vendored versions of its libraries" FORCE)

# SDL3_image requires the assembler NASM to be present to properly process AVIF files
# This is quite small, but if it is not available, uncomment the line below.
# set(SDLIMAGE_AVIF OFF CACHE BOOL "Disable support for AVIF image format in SDL3_image" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/SDL3-3.4.12)
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/SDL3_image-3.4.4)
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/SDL3_ttf-3.2.2)

# AS imgui has no cmake, it must be compiled and statically linked manually
add_library(imgui_lib STATIC
       ${CMAKE_SOURCE_DIR}/third_party/imgui-1.92.8/imgui.cpp
       ${CMAKE_SOURCE_DIR}/third_party/imgui-1.92.8/imgui_draw.cpp
       ${CMAKE_SOURCE_DIR}/third_party/imgui-1.92.8/imgui_demo.cpp
       ${CMAKE_SOURCE_DIR}/third_party/imgui-1.92.8/imgui_tables.cpp
       ${CMAKE_SOURCE_DIR}/third_party/imgui-1.92.8/imgui_widgets.cpp
       ${CMAKE_SOURCE_DIR}/third_party/imgui-1.92.8/backends/imgui_impl_sdl3.cpp
       ${CMAKE_SOURCE_DIR}/third_party/imgui-1.92.8/backends/imgui_impl_sdlrenderer3.cpp
        # Add/remove backends to match what you actually use
)
target_include_directories(imgui_lib PUBLIC
       ${CMAKE_SOURCE_DIR}/third_party/imgui-1.92.8
       ${CMAKE_SOURCE_DIR}/third_party/imgui-1.92.8/backends
)
target_link_libraries(imgui_lib PUBLIC SDL3::SDL3)

# --- Your executable ---
add_executable(low_level_game
        core/game_loop.cpp
        core/app_state.cpp
        imgui_utils/imgui_utils.cpp
        imgui_utils/imgui_utils.h
)

target_link_libraries(low_level_game PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        imgui_lib
)