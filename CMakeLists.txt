cmake_minimum_required(VERSION 3.16)
project(my_project VERSION 1.0 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)
# 1. Define the executable with ALL .cpp files
add_executable(db_scratch
    daemon.cpp
    src/storage_manager/access_methods.cpp
    src/storage_manager/buffer_manager.cpp
    src/storage_manager/disk_operator.cpp
    src/storage_manager/heap_writer.cpp
    src/storage_manager/index_writer.cpp
    src/storage_manager/insert.cpp
    src/query_manager/lexer.cpp
    src/query_manager/parser.cpp
    src/query_manager/planner.cpp
    src/catalog_manager/schema_manager.cpp
    tui/base_elements.cpp
    tui/components.cpp
    tui/input_handler.cpp
    tui/renderer.cpp
)

# 2. Tell CMake where ao find header files
target_include_directories(db_scratch PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/storage_manager/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/query_manager/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/catalog_manager/headers
)

# 3. Features and Options
target_compile_features(db_scratch PRIVATE cxx_std_17)

if (MSVC)
    target_compile_options(db_scratch PRIVATE /W4)
else()
    target_compile_options(db_scratch PRIVATE -Wall -Wextra -Wpedantic)
endif()

