#include <iostream>
#include <boost/asio.hpp>
#include <memory>

const int MAX_LENGTH = 1024 * 1024;
const int HEAD_LENGTH = sizeof (size_t);

int main(){
    try{
        //创建上下文服务 endpoint socket error_code
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::endpoint remote_ep{boost::asio::ip::address::from_string("127.0.0.1"), 10086};
        boost::asio::ip::tcp::socket sock{ioc, remote_ep.protocol()};
        boost::system::error_code ec{boost::asio::error::host_not_found};

        //建立连接
        sock.connect(remote_ep, ec);

        if(ec){
            std::cerr<< "connect failed, code is " << ec.value() << " error message is " << ec.message() <<std::endl;
            return -1;
        }

        for(;;){
            //获取并发送的数据
            std::cout<< "Enter Message: ";
            std::shared_ptr<std::array<char, MAX_LENGTH>> request = std::make_shared<std::array<char, MAX_LENGTH>>();
            std::cin.getline(request->data(), MAX_LENGTH);

            std::size_t request_length = strlen(request->data());
            std::cout<< "send data length :" << request_length << std::endl;

            std::shared_ptr<std::array<char, MAX_LENGTH + HEAD_LENGTH>> send_data = std::make_shared<std::array<char, MAX_LENGTH + HEAD_LENGTH>>();
            memcpy(send_data->data(), &request_length, HEAD_LENGTH);
            memcpy(send_data->data() + HEAD_LENGTH, request->data(), request_length);

            //计算时间
            auto start = std::chrono::high_resolution_clock::now();

            int res_write = boost::asio::write(sock, boost::asio::buffer(send_data->data(), request_length + HEAD_LENGTH));
            if(res_write <= 0){
                std::cerr<< "write to socket failed!" <<std::endl;
                return res_write;
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            //std::cout << "Elapsed time: " << duration << " microseconds" << std::endl;

            //接收数据
            std::array<char, HEAD_LENGTH> reply_head{};
            std::size_t reply_length = boost::asio::read(sock, boost::asio::buffer(reply_head.data(), HEAD_LENGTH));
            std::size_t msg_len = 0;
            memcpy(&msg_len, reply_head.data(), HEAD_LENGTH);

            std::shared_ptr<std::array<char, MAX_LENGTH>> msg = std::make_shared< std::array<char, MAX_LENGTH>>();
            size_t msg_length = boost::asio::read(sock, boost::asio::buffer(msg->data(), msg_len));

            std::cout<<"Reply is     : ";
            std::cout.write(msg->data(), msg_len) << std::endl;
            std::cout<<"Reply len is : " << msg_len << std::endl;
        }

    }catch(boost::system::system_error& e) {
        std::cout<<"Error: " << e.what() << std::endl;
        return e.code().value();
    }
    return 0;
}
