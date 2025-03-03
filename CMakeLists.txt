cmake_minimum_required(VERSION 3.28)
project(boostDemo)

set(CMAKE_CXX_STANDARD 23)

# 添加可执行文件
add_executable(boostDemo main.cpp
        include/endPoint.h
        source/endPoint.cpp
        source/syncReadWrite.cpp
        include/syncReadWrite.h
        include/asynReadWrite.h
        source/asynReadWrite.cpp
        example.pb.cc
        example.pb.h
)

# 添加Boost库的依赖
find_package(Boost REQUIRED COMPONENTS system)
target_include_directories(boostDemo PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(boostDemo Boost::boost)

# 添加Protobuf库的依赖
find_package(absl REQUIRED)  # 查找 Abseil 库
target_link_libraries(boostDemo absl::log_internal_check_op)
find_package(Protobuf REQUIRED)
target_link_libraries(boostDemo ${PROTOBUF_LIBRARIES})
