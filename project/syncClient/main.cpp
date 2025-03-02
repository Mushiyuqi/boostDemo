#include <iostream>
#include <boost/asio.hpp>
#include <memory>

const int MAX_LENGTH = 1024 * 2;
const int HEAD_LENGTH = 2;

int main() {
    try {
        //创建上下文服务 endpoint socket error_code
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::endpoint remote_ep{boost::asio::ip::address::from_string("127.0.0.1"), 10086};
        boost::asio::ip::tcp::socket sock{ioc, remote_ep.protocol()};
        boost::system::error_code ec{boost::asio::error::host_not_found};

        //建立连接
        sock.connect(remote_ep, ec);

        if (ec) {
            std::cerr << "connect failed, code is " << ec.value() << " error message is " << ec.message() << std::endl;
            return -1;
        }

        // 发送线程
        std::thread send_thread([&sock]() {
            for (;;) {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                // std::cout << "begin to send..." << std::endl;
                const char* request = "hello world!";
                short request_len = strlen(request);
                char send_data[MAX_LENGTH] = {0};
                //转为网络字节序
                auto request_host_length = boost::asio::detail::socket_ops::host_to_network_short(request_len);
                memcpy(send_data, &request_host_length, HEAD_LENGTH);
                memcpy(send_data + HEAD_LENGTH, request, request_len);
                boost::asio::write(sock, boost::asio::buffer(send_data, request_len + HEAD_LENGTH));
            }
        });

        //接收线程
        std::thread recv_thread([&sock]() {
            for (;;) {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                std::cout << "begin to receive..." << std::endl;
                char reply_head[HEAD_LENGTH];
                size_t reply_length = boost::asio::read(sock, boost::asio::buffer(reply_head, HEAD_LENGTH));
                short msglen = 0;
                memcpy(&msglen, reply_head, HEAD_LENGTH);
                //转为本地字节序
                msglen = boost::asio::detail::socket_ops::network_to_host_short(msglen);
                char msg[MAX_LENGTH] = {0};
                size_t msg_length = boost::asio::read(sock, boost::asio::buffer(msg, msglen));

                std::cout << "replay is: ";
                std::cout.write(msg, msglen) << std::endl;
                std::cout << "replay len is: " << msglen << std::endl;
                std::cout << std::endl;
            }
        });

        send_thread.join();
        recv_thread.join();
    }
    catch (boost::system::system_error& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return e.code().value();
    }
    return 0;
}
