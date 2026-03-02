cmake_minimum_required(VERSION 3.16)
project(my_project VERSION 1.0 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)
# 1. Define the executable with ALL .cpp files
add_executable(db_scratch
    daemon.cpp
    storage_manager/buffer_manager.cpp
    storage_manager/disk_operator.cpp
    storage_manager/access_methods.cpp
    storage_manager/heap_writer.cpp
    storage_manager/index_writer.cpp
    storage_manager/insert.cpp
)

# 2. Tell CMake where to find header files
target_include_directories(db_scratch PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/storage_manager/headers
)

# 3. Features and Options
target_compile_features(db_scratch PRIVATE cxx_std_17)

if (MSVC)
    target_compile_options(db_scratch PRIVATE /W4)
else()
    target_compile_options(db_scratch PRIVATE -Wall -Wextra -Wpedantic)
endif()

