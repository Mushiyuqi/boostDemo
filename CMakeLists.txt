cmake_minimum_required(VERSION 3.28)
project(boostDemo)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS system)

# 添加可执行文件
add_executable(boostDemo main.cpp
        include/endPoint.h
        source/endPoint.cpp
        source/syncReadWrite.cpp
        include/syncReadWrite.h
        include/asynReadWrite.h
        source/asynReadWrite.cpp)

# 添加Boost库的依赖
target_include_directories(boostDemo PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(boostDemo Boost::boost)
