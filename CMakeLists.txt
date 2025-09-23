cmake_minimum_required(VERSION 3.30)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
cmake_policy(SET CMP0077 NEW)

option(GALVANIZER_BUILD_TESTS "Build Galvanizer test applications" ON)
set(CMAKE_CXX_STANDARD 20)

project(GalvanizerProj VERSION 0.0.0)


# Generate Shaders on project configuration
execute_process(
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src/shaders/"
        COMMAND python "GenDefShader.py"
)

# Generate Shaders if out of date on project build
set(ShadersDir "${CMAKE_SOURCE_DIR}/src/shaders")
add_custom_command(
        WORKING_DIRECTORY "${ShadersDir}"
        COMMAND python "GenDefShader.py"

        DEPENDS "${ShadersDir}/Default.frag" "${ShadersDir}/Default.vert" "${ShadersDir}/GenDefShader.py"
        OUTPUT "${ShadersDir}/Default.inl"
)

# Create library
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
add_library(Galvanizer STATIC ${SRC_FILES} "${ShadersDir}/Default.inl")

target_include_directories(Galvanizer PRIVATE "include" "vendor/glfw/include" "vendor/stb/include")
target_compile_definitions(Galvanizer PUBLIC GALVANIZER_STATIC)
target_compile_definitions(Galvanizer PRIVATE DEBUG_LOG_LVL=4)  # Error(1), Warning(2), Info(3), Trace(4)

target_compile_options(Galvanizer PRIVATE -Wall -Werror)


# Build glad
add_subdirectory("vendor/glad" SYSTEM)

#Build stb
add_subdirectory("vendor/stb" SYSTEM)

#Build glfw
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
add_subdirectory("vendor/glfw" SYSTEM)

#Build msdf-atlas-gen
set(MSDF_ATLAS_BUILD_STANDALONE OFF)
set(MSDF_ATLAS_USE_VCPKG OFF)
set(MSDF_ATLAS_USE_SKIA OFF)
add_subdirectory("vendor/msdf-atlas-gen" SYSTEM)

# Link all of the libraries
target_link_libraries(Galvanizer PRIVATE glad glfw stb msdf-atlas-gen)

if (GALVANIZER_BUILD_TESTS)
    message("Galvanizer tests included!")
    add_subdirectory("tests")
endif ()
