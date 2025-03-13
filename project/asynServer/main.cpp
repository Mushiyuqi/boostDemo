#include <iostream>
#include <thread>
#include <csignal>
#include <mutex>
#include "LogicSystem.h"
#include "CServer.h"

bool bstop = false;
std::condition_variable cond_quit;
std::mutex mutex_quit;

// 处理退出信号
void sig_handler(const int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        std::unique_lock<std::mutex> lock(mutex_quit);
        bstop = true;
        // 唤醒主线程
        cond_quit.notify_one();
    }
}

int main() {
    try{
        // 让逻辑系统的线程变成主线程的子线程
        LogicSystem::GetInstance();

        boost::asio::io_context io_context;
        std::thread net_work_thread([&io_context]() {
            CServer server(io_context, 10086);
            // 创建一个线程池
            boost::asio::thread_pool pool(THREAD_NUM);
            for (int i = 0; i < 8; ++i) {
                boost::asio::post(pool, [&io_context]() { io_context.run();});
            }
        });

        // 注册信号
        std::signal(SIGINT, sig_handler);
        std::signal(SIGTERM, sig_handler);

        while (!bstop) {
            // 挂起主线程
            std::unique_lock<std::mutex> lock(mutex_quit);
            cond_quit.wait(lock);
        }

        io_context.stop();
        net_work_thread.join();

    }catch (std::exception &e) {
        std::cerr << "Exception : " << e.what() << std::endl;
    }
    return 0;
}
