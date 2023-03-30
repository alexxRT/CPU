cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(main)

add_subdirectory(token)

add_executable(main main.cpp)

target_link_libraries(main PUBLIC token)
target_include_directories(main PUBLIC token)


