#pragma once
#include "Singleton.hpp"
#include <boost/asio.hpp>

class AsioThreadPool : public Singleton<AsioThreadPool> {
    // 使Singleton能够访问私有构造函数
    friend Singleton<AsioThreadPool>;
public:
    ~AsioThreadPool();
    AsioThreadPool(const AsioThreadPool&) = delete;
    AsioThreadPool& operator=(const AsioThreadPool&) = delete;
    boost::asio::io_context& GetIOContext();
    void Stop();

private:
    AsioThreadPool(std::size_t threadNum = std::thread::hardware_concurrency());
    boost::asio::io_context m_ioContext;
    std::unique_ptr<boost::asio::io_context::work> m_workPtr;
    std::vector<std::thread> m_threads;

};
