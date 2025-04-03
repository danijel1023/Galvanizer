cmake_minimum_required(VERSION 3.22)
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
add_subdirectory("vendor/glad")

#Build stb
add_subdirectory("vendor/stb")

#Build glfw
option(GLFW_BUILD_DOCS OFF FORCE)
option(GLFW_BUILD_TESTS OFF FORCE)
option(GLFW_BUILD_EXAMPLES OFF FORCE)
add_subdirectory("vendor/glfw")


# Link all of the libraries
target_link_libraries(Galvanizer PRIVATE glad glfw stb)


# Remove this set
set(GALVANIZER_BUILD_TESTS TRUE)
if (${GALVANIZER_BUILD_TESTS})
    add_subdirectory("tests")
endif ()
