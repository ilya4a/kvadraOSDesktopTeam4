cmake_minimum_required(VERSION 4.1)
project(kvadraOSDesktopTeam4)

set(CMAKE_CXX_STANDARD 20)

add_executable(kvadraOSDesktopTeam4 main.cpp
        src/Scanner.cpp
        include/Scanner.h
        src/SearchOptions.cpp
        include/SearchOptions.h)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
