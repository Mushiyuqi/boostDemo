#include "AsioThreadPool.h"

AsioThreadPool::AsioThreadPool(std::size_t threadNum): m_workPtr(
    new boost::asio::executor_work_guard<boost::asio::io_context::executor_type>(
        boost::asio::make_work_guard(m_ioContext))) {
    for (std::size_t i = 0; i < threadNum; ++i) {
        m_threads.emplace_back([this] {
            m_ioContext.run();
        });
    }
}

AsioThreadPool::~AsioThreadPool() {
    std::cerr << "AsioThreadPool destruct" << std::endl;
    Stop();
}

boost::asio::io_context& AsioThreadPool::GetIOContext() {
    return m_ioContext;
}

void AsioThreadPool::Stop() {
    m_workPtr.reset();

    m_ioContext.stop();

    for (auto& t : m_threads) {
        t.join();
    }
}
