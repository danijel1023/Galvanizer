cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)

project(GalvanizerProj VERSION 0.0.0)

#Create library
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
add_library(Galvanizer STATIC ${SRC_FILES})

target_compile_definitions(Galvanizer PUBLIC GALVANIZER_STATIC)
target_include_directories(Galvanizer PRIVATE "include")
target_compile_options(Galvanizer PRIVATE -Wall -Werror)

# 1 - Error
# 2 - Warning
# 3 - Info
# 4 - Trace
target_compile_definitions(Galvanizer PRIVATE DEBUG_LOG_LVL=4)

# Remove this set
set(GALVANIZER_BUILD_TESTS TRUE)
if (${GALVANIZER_BUILD_TESTS})
    add_subdirectory("tests")
endif ()
