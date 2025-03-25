#include <iostream>
#include "global.h"
#include "AsioIOContextPool.h"
#include "HttpServer.h"


int main()
{
    try {
        AsioIOContextPool::GetInstance();

        boost::asio::io_context io_context{};
        boost::asio::signal_set signals{io_context, SIGINT, SIGTERM};
        signals.async_wait([&io_context](auto, auto) {
            io_context.stop();
        });

        HttpServer server{io_context, "127.0.0.1", 10086};
        io_context.run();

    }catch (std::exception& e) {
        std::cerr << "main() Exception: " << e.what() << "\n";
    }
    return 0;
}
