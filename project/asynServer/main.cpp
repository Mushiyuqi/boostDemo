#include <iostream>
#include <thread>
#include <csignal>
#include <mutex>
#include "LogicSystem.h"
#include "CServer.h"


int main() {
    try{
        // 让逻辑系统的线程变成主线程的子线程
        LogicSystem::GetInstance();

        boost::asio::io_context io_context;
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&io_context](auto, auto) {
            io_context.stop();
        });

        CServer server(io_context, 10086);
        // 创建一个线程池
        boost::asio::thread_pool pool(MAX_THREAD_NUM);
        for (int i = 0; i < THREAD_NUM; ++i) {
            boost::asio::post(pool, [&io_context]() { io_context.run();});
        }

        // 等待线程池结束
        pool.join();

    }catch (std::exception &e) {
        std::cerr << "Exception : " << e.what() << std::endl;
    }
    return 0;
}
