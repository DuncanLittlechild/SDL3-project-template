cmake_minimum_required(VERSION 3.24)
project(low_level_game)
set(CMAKE_CXX_STANDARD 23)
# Toggle to set whether this is a prod build (ON) or debug (OFF)
set(PRODUCTION_BUILD OFF CACHE BOOL "Make this a production build" FORCE)

# Statically links the c and c++ runtime libraries
# this way of doing this only works on g++, MSCV or clang emulating MSVC doesn't really work for this
add_link_options(-static)

# -- Set Library options --
# Forces SDL3_ttf to use the vendored versions of its libraries found in its./external folder instead of system ones
# Note that this only works if you remembered to clone the external dependencies, in this case the vendored libraries
set(SDLTTF_VENDORED ON CACHE BOOL "Forces SDL3_ttf to use the vendored versions of its libraries" FORCE)
# Disables the Harfbuzz external dependency. This is by far the largest dependency, and only needed for non-latin fonts
# with unusual glyph formation (eg, arabic, where letters look different depending on where they are in the word) or
# right to left font rendering.
set(SDLTTF_HARFBUZZ OFF CACHE BOOL "Disables the Harfbuzz external dependency" FORCE)


# Forces SDL3_image to use the vendored versions of its libraries found in its./external folder instead of system ones
set(SDLIMAGE_VENDORED ON CACHE BOOL "Forces SDL3_image to use the vendored versions of its libraries" FORCE)
# The following lines disable the largest external dependencies of SDL_image, while at the same time disabling it's
# ability to process the relevat image types. What iamge types are disabled is obvious, so comment out if necessary.
set(SDLIMAGE_JXL OFF CACHE BOOL "Disable support for JPEG-XL image format in SDL3_image" FORCE)
set(SDLIMAGE_TIF OFF CACHE BOOL "Disable support for TIF image format in SDL3_image" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "Disable support for WEBP image format in SDL3_image" FORCE)
set(SDLIMAGE_SVG OFF CACHE BOOL "Disable support for SVG image format in SDL3_image" FORCE)
# THis option also disables the dependency on the small assembler NASM
# This dependency is only for initial compilation, and NASM isn't hard to get, but its here regardless.
set(SDLIMAGE_AVIF OFF CACHE BOOL "Disable support for AVIF image format in SDL3_image" FORCE)

# -- Add Libraries --
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL3-3.4.12)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL3_image-3.4.4)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL3_ttf-3.2.2)

# AS imgui has no cmake, it must be compiled and statically linked manually
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui-1.92.8")
add_library(imgui_lib STATIC
       ${IMGUI_DIR}/imgui.cpp
       ${IMGUI_DIR}/imgui_draw.cpp
       ${IMGUI_DIR}/imgui_demo.cpp
       ${IMGUI_DIR}/imgui_tables.cpp
       ${IMGUI_DIR}/imgui_widgets.cpp
       ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
       ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp
        # Add/remove backends to match what you actually use
)
target_include_directories(imgui_lib PUBLIC
       ${IMGUI_DIR}
       ${IMGUI_DIR}/backends
)
target_link_libraries(imgui_lib PUBLIC SDL3::SDL3)

# Adds stb as a header-only library
add_library(stb_header INTERFACE)
target_include_directories(stb_header INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb/")

# --- Your executable ---
#Bundles togetehr all cpp files in src recursively, so they can be added as an executable all as one
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(${CMAKE_PROJECT_NAME}
    "${MY_SOURCES}"
)

# -- Include headers --
#Path for include headers
# This is the only place where stb is mentioned, because it is a header-only library
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# -- Preprocessor directives creation --
if (PRODUCTION_BUILD)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC RESOURCES_PATH="./resources/")
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC PRODUCTION_BUILD=1)
else()
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC PRODUCTION_BUILD=0)
endif()
# Creates the RESOURCES_PATH preprocessor macro which directs all file loading operations to the resources file


# -- Final linking step --
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        imgui_lib
        stb_header
)