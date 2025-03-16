#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <json/json.h>
#include "msg.pb.h"
#include "MsgNode.h"

int main() {
    try {
        //创建上下文服务 endpoint socket error_code
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::endpoint remote_ep{boost::asio::ip::make_address("127.0.0.1"), 10086};
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
                std::this_thread::sleep_for(std::chrono::milliseconds(200));

                // 序列化
                Json::Value root;
                root["id"] = 1001;
                root["data"] = "hello world";
                std::string request = root.toStyledString();

                // 转为网络字节序
                SendNode send_node(request.c_str(), request.length(), static_cast<short>(root["id"].asInt()));

                boost::asio::write(sock, boost::asio::buffer(send_node.m_data, send_node.m_total_len));
            }
        });

        //接收线程
        std::thread recv_thread([&sock]() {
            for (;;) {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                std::cout << "begin to receive..." << std::endl;

                // 转为本地字节序
                char reply_head[HEAD_TOTAL_LEN];
                size_t reply_length = boost::asio::read(sock, boost::asio::buffer(reply_head, HEAD_TOTAL_LEN));

                short id = 0;
                memcpy(&id, reply_head, HEAD_ID_LENGTH);
                id = boost::asio::detail::socket_ops::network_to_host_short(id);

                short msglen = 0;
                memcpy(&msglen, reply_head + HEAD_ID_LENGTH, HEAD_DATA_LEN);
                msglen = boost::asio::detail::socket_ops::network_to_host_short(msglen);

                // 反序列化
                RecvNode recv_node(msglen, id);
                size_t msg_length = boost::asio::read(sock, boost::asio::buffer(recv_node.m_data, recv_node.m_total_len));
                Json::Value root;
                Json::Reader reader;
                reader.parse(recv_node.m_data, recv_node.m_data + recv_node.m_total_len, root);

                std::cout << "msg id is  : " << root["id"].asInt() << std::endl;
                std::cout << "msg data is: " << root["data"].asString() << std::endl;
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
