cmake_minimum_required(VERSION 3.28)
project(httpClient)

set(CMAKE_CXX_STANDARD 20)

# 添加可执行文件
add_executable(httpClient
        main.cpp)

# 添加Boost库的依赖
find_package(Boost REQUIRED COMPONENTS system filesystem thread)
target_include_directories(httpClient PRIVATE ${Boost_INCLUDE_DIRS})
target_link_directories(httpClient PRIVATE Boost::system Boost::filesystem Boost::thread Boost::beast)

# 添加Protobuf库的依赖
find_package(absl REQUIRED)  # 查找 Abseil 库
target_link_libraries(httpClient absl::log_internal_check_op)
find_package(Protobuf REQUIRED)
target_link_libraries(httpClient ${PROTOBUF_LIBRARIES})

# 添加JSONCPP库的依赖
target_link_libraries(httpClient jsoncpp.so)