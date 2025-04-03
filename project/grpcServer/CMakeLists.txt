cmake_minimum_required(VERSION 3.28)
project(grpcServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(grpcServer
        main.cpp
        msg.grpc.pb.h
        msg.grpc.pb.cc
        msg.pb.h
        msg.pb.cc
)

# 添加Boost库的依赖
find_package(Boost REQUIRED COMPONENTS system filesystem thread)
target_include_directories(grpcServer PRIVATE ${Boost_INCLUDE_DIRS})
target_link_directories(grpcServer PRIVATE
        Boost::system
        Boost::filesystem
        Boost::thread
        Boost::beast)

# 添加Abseil库的依赖
find_package(absl REQUIRED)
target_link_libraries(grpcServer PRIVATE
        absl::log_internal_check_op
        absl::cord
        absl::strings
        absl::base
        absl::synchronization
)

# 添加Protobuf库的依赖
find_package(Protobuf REQUIRED)
target_link_libraries(grpcServer PRIVATE
        protobuf::libprotobuf
        protobuf::libprotoc
        protobuf::libprotobuf-lite
)

# 添加grpc库的依赖
target_link_libraries(grpcServer PRIVATE
        libgpr.so
        libgrpc.so
        libgrpc++.so
        libgrpc++_reflection.so
        libaddress_sorting.so
)

# 添加JSONCPP库的依赖
target_link_libraries(grpcServer PRIVATE jsoncpp.so)