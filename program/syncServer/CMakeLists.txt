cmake_minimum_required(VERSION 3.28)
project(syncServer)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS system)

# 添加可执行文件
add_executable(syncServer
    main.cpp)

# 添加Boost库的依赖
target_include_directories(syncServer PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(syncServer Boost::boost)