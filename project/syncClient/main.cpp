#include <iostream>
#include <boost/asio.hpp>

const int MAX_LENGTH = 1024;

int main(){
    try{
        //创建上下文服务 endpoint socket error_code
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::endpoint remote_ep{boost::asio::ip::address::from_string("10.252.178.72"), 10086};
        boost::asio::ip::tcp::socket sock{ioc, remote_ep.protocol()};
        boost::system::error_code ec{boost::asio::error::host_not_found};

        //建立连接
        sock.connect(remote_ep, ec);
        if(ec){
            std::cerr<< "connect failed, code is " << ec.value() << " error message is " << ec.message() <<std::endl;
            return -1;
        }

        /*//获取并发送的数据
        std::cout<< "Enter Message: ";
        std::array<char, MAX_LENGTH> request{};
        std::cin.getline(request.data(), MAX_LENGTH);
        std::size_t request_length = strlen(request.data());
        int res_write = boost::asio::write(sock, boost::asio::buffer(request, request_length));

        if(res_write <= 0){
            std::cerr<< "write to socket failed!" <<std::endl;
            return res_write;
        }*/

        //for(;;){
            //获取并发送的数据
            std::cout<< "Enter Message: ";
            std::array<char, MAX_LENGTH> request{};
            std::cin.getline(request.data(), MAX_LENGTH);
            std::size_t request_length = strlen(request.data());
            int res_write = boost::asio::write(sock, boost::asio::buffer(request, request_length));

            if(res_write <= 0){
                std::cerr<< "write to socket failed!" <<std::endl;
                return res_write;
            }

            //接收数据
            std::array<char, MAX_LENGTH> reply{};
            //此处不考虑 我们不知道服务器要发多少数据过来
            std::size_t reply_length = boost::asio::read(sock, boost::asio::buffer(reply, request_length));
            std::cout<<"Reply is: ";
            std::cout.write(reply.data(), reply_length);
            std::cout<<std::endl;
        //}

    }catch(boost::system::system_error& e) {
        std::cout<<"Error: " << e.what() << std::endl;
        return e.code().value();
    }
    return 0;
}
