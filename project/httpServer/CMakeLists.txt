cmake_minimum_required(VERSION 3.28)
project(httpServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(httpServer main.cpp
        HttpConnection.h
        HttpConnection.cpp
        global.h
        const.h
        HttpServer.h
        HttpServer.cpp
        AsioIOContextPool.h
        AsioIOContextPool.cpp)

# 添加Boost库的依赖
find_package(Boost REQUIRED COMPONENTS system filesystem thread)
target_include_directories(httpServer PRIVATE ${Boost_INCLUDE_DIRS})
target_link_directories(httpServer PRIVATE Boost::system Boost::filesystem Boost::thread Boost::beast)

# 添加Protobuf库的依赖
find_package(absl REQUIRED)  # 查找 Abseil 库
target_link_libraries(httpServer absl::log_internal_check_op)
find_package(Protobuf REQUIRED)
target_link_libraries(httpServer ${PROTOBUF_LIBRARIES})

# 添加JSONCPP库的依赖
target_link_libraries(httpServer jsoncpp.so)
