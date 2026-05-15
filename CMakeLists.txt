cmake_minimum_required(VERSION 3.16)
project(AeroGestao C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# SQLite3
find_package(SQLite3 REQUIRED)

# Backend executable
add_executable(aerogestao-server
    backend/src/main.c
    backend/src/database.c
)

target_link_libraries(aerogestao-server
    SQLite::SQLite3
)

# Windows specific
if(WIN32)
    target_link_libraries(aerogestao-server ws2_32)
endif()

# Copy frontend files to build directory
add_custom_target(copy_frontend ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/frontend
    ${CMAKE_BINARY_DIR}/frontend
)

add_dependencies(aerogestao-server copy_frontend)
