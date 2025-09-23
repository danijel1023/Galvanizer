cmake_minimum_required(VERSION 3.30)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
add_executable(SimpleWindow ${SRC_FILES})


# SimpleWindow
target_include_directories(SimpleWindow PRIVATE "include" "../../vendor/msdf-atlas-gen")

# Galvanizer
target_link_libraries(SimpleWindow PRIVATE Galvanizer msdf-atlas-gen)
target_include_directories(SimpleWindow PRIVATE "../../include")
