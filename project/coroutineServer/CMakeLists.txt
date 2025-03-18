cmake_minimum_required(VERSION 3.28)
project(corountineServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(corountineServer main.cpp
                )

# 添加Boost库的依赖
find_package(Boost REQUIRED COMPONENTS system filesystem thread)
target_include_directories(corountineServer PRIVATE ${Boost_INCLUDE_DIRS})
target_link_directories(corountineServer PRIVATE Boost::system Boost::filesystem Boost::thread)

# 添加Protobuf库的依赖
find_package(absl REQUIRED)  # 查找 Abseil 库
target_link_libraries(corountineServer absl::log_internal_check_op)
find_package(Protobuf REQUIRED)
target_link_libraries(corountineServer ${PROTOBUF_LIBRARIES})

# 添加JSONCPP库的依赖
target_link_libraries(corountineServer jsoncpp.so)