cmake_minimum_required(VERSION 3.28)
project(asynServer)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system filesystem thread)


add_executable(asynServer main.cpp
        CSession.h
        CSession.cpp
        CServer.h
        CServer.cpp
        MsgNode.h
        MsgNode.cpp)

target_include_directories(asynServer PRIVATE ${Boost_INCLUDE_DIRS})
target_link_directories(asynServer PRIVATE Boost::system Boost::filesystem Boost::thread)
