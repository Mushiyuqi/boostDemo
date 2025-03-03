cmake_minimum_required(VERSION 3.28)
project(asynServer)

set(CMAKE_CXX_STANDARD 17)

add_executable(asynServer main.cpp
        CSession.h
        CSession.cpp
        CServer.h
        CServer.cpp
        const.h
        msg.pb.h
        msg.pb.cc)

# 添加Boost库的依赖
find_package(Boost REQUIRED COMPONENTS system filesystem thread)
target_include_directories(asynServer PRIVATE ${Boost_INCLUDE_DIRS})
target_link_directories(asynServer PRIVATE Boost::system Boost::filesystem Boost::thread)

# 添加Protobuf库的依赖
find_package(absl REQUIRED)  # 查找 Abseil 库
target_link_libraries(asynServer absl::log_internal_check_op)
find_package(Protobuf REQUIRED)
target_link_libraries(asynServer ${PROTOBUF_LIBRARIES})

# 添加JSONCPP库的依赖
target_link_libraries(asynServer jsoncpp.so)