#include "AsioIOContextPool.h"

AsioIOContextPool::AsioIOContextPool(std::size_t size): m_ioContexts(size), m_works(size), m_nextIOContext(0) {
    for (std::size_t i = 0; i < size; ++i) {
        m_works[i] = std::make_unique<Work>(boost::asio::make_work_guard(m_ioContexts[i]));
    }

    // 遍历多个iocontext 创建多个线程 每个线程都启动iocontext
    for (std::size_t i = 0; i < size; ++i) {
        m_threads.emplace_back([this, i]() {
            m_ioContexts[i].run();
        });
    }
}

AsioIOContextPool::~AsioIOContextPool() {
    std::cerr << "AsioIOContextPool destruct " << std::endl;
    Stop();
}

boost::asio::io_context& AsioIOContextPool::GetIOContext() {
    boost::asio::io_context& context = m_ioContexts[m_nextIOContext++];
    if(m_nextIOContext == m_ioContexts.size())
        m_nextIOContext = 0;

    return context;
}

void AsioIOContextPool::Stop() {
    for (auto& work : m_works) {
        // 释放智能指针
        // 调用 Work 析构函数, iocontext 恢复默认行为
        work.reset();
    }

    for (auto& io_context : m_ioContexts) {
        // 停止 iocontext
        io_context.stop();
    }

    for (auto& thread : m_threads) {
        thread.join();
    }
}