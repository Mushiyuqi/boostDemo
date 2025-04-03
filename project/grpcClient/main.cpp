#include <iostream>
#include <string>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "msg.grpc.pb.h"
#include "msg.pb.h"

using grpc::ClientContext;
using grpc::Channel;
using grpc::Status;
using hello::HelloReply;
using hello::HelloRequest;
using hello::Greeter;

class FCClient {
public:
    FCClient(std::shared_ptr<Channel> channel)
        : m_stub(Greeter::NewStub(channel)) {}

    std::string SayHello(const std::string& user) {
        // 客户端上下文
        ClientContext context;
        // 请求和响应
        HelloRequest request;
        HelloReply reply;
        // 设置消息
        request.set_message(user);
        // 阻塞发送消息
        const Status status = m_stub->SayHello(&context, request, &reply);

        if (status.ok()) {
            return reply.message();
        } else {
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<Greeter::Stub> m_stub;

};

int main() {
    FCClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
    std::cout << client.SayHello("hello world!") << std::endl;
    return 0;
}
