#include <iostream>
#include "CServer.h"
#include <thread>

void net(){
    try{
        boost::asio::io_context ioc;
        CServer s(ioc, 10086);
        /// 开启事件循环
        ioc.run();
    }catch (std::exception& e){
        std::cerr << "Exception" << e.what() << std::endl;
        return;
    }
}

int main() {
    std::shared_ptr<std::thread> t = std::make_shared<std::thread>(net);
    t->join();
    return 0;
}
