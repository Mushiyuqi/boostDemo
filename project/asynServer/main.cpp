#include <iostream>
#include <thread>
#include <csignal>
#include <mutex>
#include "LogicSystem.h"
#include "AsioIOContextPool.h"
#include "AsioThreadPool.h"
#include "CServer.h"


int main() {
    try{
        // 让逻辑系统的线程变成主线程的子线程
        LogicSystem::GetInstance();

        // AsioIOContextPool::GetInstance();    初始化服务池
        // AsioThreadPool::GetInstance();       初始化线程池
        SERVICE_POOL_MODEL::GetInstance();

        // 监听程序终止信号
        // 处理服务器连接请求
        boost::asio::io_context io_context;
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&io_context](auto, auto) {
            io_context.stop();
        });

        CServer server(io_context, 10086);
        io_context.run();

    }catch (std::exception &e) {
        std::cerr << "Exception : " << e.what() << std::endl;
    }
    return 0;
}
