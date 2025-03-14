#pragma once
#include "Singleton.hpp"
#include <boost/asio.hpp>
#include <vector>

class AsioIOContextPool: public Singleton<AsioIOContextPool>{
    // 使Singleton能够访问私有构造函数
    friend Singleton<AsioIOContextPool>;
public:
    // 设置别名
    using IOContext = boost::asio::io_context;
    using Work = boost::asio::io_context::work;
    using WorkPtr = std::unique_ptr<Work>;
    ~AsioIOContextPool();
    AsioIOContextPool(const AsioIOContextPool&) = delete;
    AsioIOContextPool& operator=(const AsioIOContextPool&) = delete;
    // 使用round-robin算法，选择一个io_context
    IOContext& GetIOContext();
    void Stop();
private:
    // hardware_concurrency() 获取机器的CPU逻辑核心数
    AsioIOContextPool(std::size_t size = std::thread::hardware_concurrency());
    std::vector<IOContext> m_ioContexts;
    std::vector<WorkPtr> m_works;
    std::vector<std::thread> m_threads;
    std::size_t m_nextIOContext;

};


