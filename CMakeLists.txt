cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)

project(GalvanizerProj VERSION 0.0.0)

#Create library
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
add_library(Galvanizer STATIC ${SRC_FILES})

target_compile_definitions(Galvanizer PUBLIC GALVANIZER_STATIC)
target_include_directories(Galvanizer PRIVATE "include" "vendor/glfw/include")
target_compile_options(Galvanizer PRIVATE -Wall -Werror)

# 1 - Error
# 2 - Warning
# 3 - Info
# 4 - Trace
target_compile_definitions(Galvanizer PRIVATE DEBUG_LOG_LVL=4)


#Build glad & glfw libraries
add_subdirectory("vendor/glad")

option(GLFW_BUILD_DOCS OFF FORCE)
option(GLFW_BUILD_TESTS OFF FORCE)
option(GLFW_BUILD_EXAMPLES OFF FORCE)
add_subdirectory("vendor/glfw")


#Link all of the libraries
target_link_libraries(Galvanizer PRIVATE glad glfw)


# Remove this set
set(GALVANIZER_BUILD_TESTS TRUE)
if (${GALVANIZER_BUILD_TESTS})
    add_subdirectory("tests")
endif ()
