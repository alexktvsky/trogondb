cmake_minimum_required(VERSION 3.25)
string(TIMESTAMP START_TIME "%H:%M:%S")
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

project(trogondb)

add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME}
PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(cmake/platform_defines.cmake)
include(cmake/release_defines.cmake)
include(cmake/find_dependencies.cmake)
include(cmake/create_package.cmake)

target_include_directories(${PROJECT_NAME}
PRIVATE
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_sources(${PROJECT_NAME}
PRIVATE
    src/trogondb/main.cpp
    src/trogondb/server.cpp
    src/trogondb/config_node.cpp
    src/trogondb/config_parser.cpp
    src/trogondb/command_line_parser.cpp
    src/trogondb/store.cpp
    src/trogondb/entry.cpp
    src/trogondb/session.cpp
    src/trogondb/os/process.cpp
    src/trogondb/logging/logger.cpp
    src/trogondb/logging/level.cpp
    src/trogondb/logging/handler.cpp
    src/trogondb/logging/stream_handler.cpp
    src/trogondb/logging/file_handler.cpp
    src/trogondb/logging/rotating_file_handler.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -fno-rtti -Wno-vla)
if(CMAKE_BUILD_TYPE STREQUAL Release)
    target_compile_options(${PROJECT_NAME} PRIVATE -O2)
elseif(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_options(${PROJECT_NAME} PRIVATE -O0 -g)
endif()

target_link_options(${PROJECT_NAME}
PRIVATE
    # -static
    # -static-libstdc++
    # -static-libgcc
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_REQUIRED_LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "bin")

string(TIMESTAMP END_TIME "%H:%M:%S")
message(STATUS "Start: ${START_TIME}")
message(STATUS "End: ${END_TIME}")
