cmake_minimum_required(VERSION 3.16)
project(AeroGestao)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
    Core
    Gui
    Widgets
    Sql
    REQUIRED
)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/clientpage.h
    src/clientpage.cpp
    src/adminpage.h
    src/adminpage.cpp
    src/loginpage.h
    src/loginpage.cpp
)

set(PROJECT_RESOURCES
    resources.qrc
)

add_executable(AeroGestao ${PROJECT_SOURCES} ${PROJECT_RESOURCES})

target_link_libraries(AeroGestao
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
)
