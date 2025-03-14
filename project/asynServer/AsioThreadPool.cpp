#include "AsioThreadPool.h"

AsioThreadPool::AsioThreadPool(std::size_t threadNum): m_workPtr(new boost::asio::io_service::work(m_ioContext)){
    for(std::size_t i = 0; i < threadNum; ++i) {
        m_threads.emplace_back([this] {
            m_ioContext.run();
        });
    }
}

AsioThreadPool::~AsioThreadPool() {
    std::cerr << "AsioThreadPool destruct" << std::endl;
    Stop();
}

boost::asio::io_service& AsioThreadPool::GetIOContext() {
    return m_ioContext;
}

void AsioThreadPool::Stop() {
    m_workPtr.reset();

    m_ioContext.stop();

    for(auto& t : m_threads) {
        t.join();
    }
}
