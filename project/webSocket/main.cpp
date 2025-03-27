#include <iostream>
#include <memory>
#include "global.h"
#include <queue>
#include <mutex>
#include "WebSocketServer.h"

int main()
{
    try {
        net::io_context ioc;
        boost::asio::signal_set signals{ioc, SIGINT, SIGTERM};
        signals.async_wait([&ioc](auto, auto) {
            ioc.stop();
        });

        WebSocketServer server{ioc, "127.0.0.1", 10086};
        ioc.run();

    }
    catch (std::exception& e) {
        std::cerr << "main() Exception Error : " << e.what() << std::endl;
    }
    return 0;
}
