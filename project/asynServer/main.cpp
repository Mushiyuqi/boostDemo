#include <iostream>
#include "CServer.h"
#include <thread>

boost::asio::io_context ioc;
CServer s(ioc, 10086);

void net(){
    try{
        /// 开启事件循环
        ioc.run();
    }catch (std::exception& e){
        std::cerr << "Exception" << e.what() << std::endl;
        return;
    }
}

int main() {
    std::shared_ptr<std::thread> t1 = std::make_shared<std::thread>(net);
    t1->join();

    // std::shared_ptr<std::thread> t2 = std::make_shared<std::thread>(net);
    // t2->join();
    return 0;
}
