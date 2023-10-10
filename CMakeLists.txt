cmake_minimum_required(VERSION 2.8)

project(SerializeMe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(${PROJECT_NAME}
    example.cpp
    include/SerializeMe/SerializeMe.hpp
    )
