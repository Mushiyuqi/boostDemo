#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/write.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;
namespace this_coro = boost::asio::this_coro;


awaitable<void> echo(tcp::socket socket) {
    try {
        char data[46]; // Json处理后 hello world 的tlv数据的长度
        for (;;) {
            // 读取数据
            co_await async_read(socket, boost::asio::buffer(data, 46));

            // 协程等待异步函数完成
            co_await async_write(socket, boost::asio::buffer(data, 46));
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception : " << e.what() << "\n";
    }
}

awaitable<void> listener() {
    // 获取协程的调度器
    const auto executor = co_await this_coro::executor;
    tcp::acceptor acceptor{executor, {tcp::v4(), 10086}};
    for (;;) {
        /**
         * co_await 将异步函数变成可等待的
         * co_await 使执行到此时阻塞的等待接收
         * 并释放资源, 不阻塞主线程
         * 直到接收完成, 继续执行
         *
         * 即当前函数等待在这里, 其他协程继续执行
         */
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));

        // 启动一个协程
        co_spawn(executor, echo(std::move(socket)), detached);
    }
}


int main() {
    try {
        boost::asio::io_context io_context{};
        boost::asio::signal_set signals{io_context, SIGINT, SIGTERM};
        signals.async_wait([&io_context](auto, auto) { io_context.stop(); });

        // 启动协程
        // detached 使协程独立运行
        co_spawn(io_context, listener(), detached);
        co_spawn(io_context, listener(), detached);

        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception : " << e.what() << "\n";
    }
    return 0;
}
