cmake_minimum_required(VERSION 3.28)
project(syncClient)

set(CMAKE_CXX_STANDARD 23)

# 添加可执行文件
add_executable(syncClient
        main.cpp
        msg.pb.cc
        msg.pb.h)

# 添加Boost库的依赖
find_package(Boost REQUIRED COMPONENTS system filesystem thread)
target_include_directories(syncClient PRIVATE ${Boost_INCLUDE_DIRS})
target_link_directories(syncClient PRIVATE Boost::system Boost::filesystem Boost::thread)

# 添加Protobuf库的依赖
find_package(absl REQUIRED)  # 查找 Abseil 库
target_link_libraries(syncClient absl::log_internal_check_op)
find_package(Protobuf REQUIRED)
target_link_libraries(syncClient ${PROTOBUF_LIBRARIES})

# 添加JSONCPP库的依赖
target_link_libraries(syncClient jsoncpp.so)