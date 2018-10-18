cmake_minimum_required(VERSION 2.8)

project(DeserializeMe)

add_definitions(-std=c++11)
include_directories(include)

add_executable(${PROJECT_NAME}
    main.cpp
    include/DeserializeMe/deserialize.hpp
    )
