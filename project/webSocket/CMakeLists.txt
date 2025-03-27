cmake_minimum_required(VERSION 3.28)
project(webSocket)

set(CMAKE_CXX_STANDARD 20)

add_executable(webSocket main.cpp
        global.h
        Connection.h
        Connection.cpp
        ConnectionManager.cpp
        ConnectionManager.h
        WebSocketServer.cpp
        WebSocketServer.h)

# 添加Boost库的依赖
find_package(Boost REQUIRED COMPONENTS system filesystem thread)
target_include_directories(webSocket PRIVATE ${Boost_INCLUDE_DIRS})
target_link_directories(webSocket PRIVATE Boost::system Boost::filesystem Boost::thread Boost::beast)

# 添加Protobuf库的依赖
find_package(absl REQUIRED)  # 查找 Abseil 库
target_link_libraries(webSocket absl::log_internal_check_op)
find_package(Protobuf REQUIRED)
target_link_libraries(webSocket ${PROTOBUF_LIBRARIES})

# 添加JSONCPP库的依赖
target_link_libraries(webSocket jsoncpp.so)