cmake_minimum_required(VERSION 3.16)
project(AeroGestao)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SQLite3
find_package(SQLite3 REQUIRED)

# Backend executable
add_executable(aerogestao-server
    backend/src/main.cpp
    backend/src/database.cpp
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
