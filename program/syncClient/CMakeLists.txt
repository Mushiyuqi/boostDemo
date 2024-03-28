cmake_minimum_required(VERSION 3.28)
project(syncClient)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS system)

# 添加可执行文件
add_executable(syncClient
    main.cpp)

# 添加Boost库的依赖
target_include_directories(syncClient PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(syncClient Boost::boost)