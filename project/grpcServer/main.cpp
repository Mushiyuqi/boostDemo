#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "msg.grpc.pb.h"
#include "msg.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using hello::Greeter;
using hello::HelloReply;
using hello::HelloRequest;

class GreeterServiceImpl final : public Greeter::Service
{
    // Status 是返回的状态
    Status SayHello(ServerContext* context, const HelloRequest* request,
                    HelloReply* reply) override
    {
        const std::string prefix("Mushiyuqi grpc server has received: ");
        reply->set_message(prefix + request->message());
        return Status::OK;
    }
};

// 启动服务
void RunServer()
{
    const std::string server_address("0.0.0.0:50051");
    GreeterServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // 注册服务
    builder.RegisterService(&service);
    const std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main()
{
    RunServer();
    return 0;
}
