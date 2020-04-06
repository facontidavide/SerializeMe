cmake_minimum_required(VERSION 2.8)

project(SerializeMe)

add_definitions(-std=c++11)
include_directories(include)

add_executable(${PROJECT_NAME}
    example.cpp
    include/SerializeMe/deserialize.hpp
    )
