cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
add_executable(SimpleWindow ${SRC_FILES})


# SimpleWindow
target_include_directories(SimpleWindow PRIVATE "include")

# Galvanizer
target_link_libraries(SimpleWindow PRIVATE Galvanizer)
target_include_directories(SimpleWindow PRIVATE "../../include")
