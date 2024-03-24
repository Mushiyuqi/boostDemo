#include "../include/syncReadWrite.h"
#include <iostream>

void write_to_socket(boost::asio::ip::tcp::socket& sock){
    std::string buf{"Hello World!"};
    std::size_t total_bytes_written{0};
    //循环发送
    //write_some 返回每次写入的字节数
    while(total_bytes_written != buf.length()){
        //write_some 不是一次将数据全发完 可能只是发送了一部分
        //total_bytes_written 记录以发送了多少数据
        //需要一个篮子（ConstBufferSequence）用buffer（）快速将数据放入篮子
        //buf.c_str() + total_bytes_written 给buf加一个字符串的偏移 剩下没发完的长度为 buf.length() - total_bytes_written
        total_bytes_written +=sock.write_some(
                boost::asio::buffer(buf.c_str() + total_bytes_written,
                                    buf.length() - total_bytes_written));

    }
}

int send_data_by_write_some(){
    std::string raw_ip_address = "192.168.0.1";
    unsigned short port_num = 8888;
    try{
        //生成端点 创建上下文 创建socket
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(raw_ip_address), port_num);
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket sock(ioc, ep.protocol());

        sock.connect(ep);
        write_to_socket(sock); //这是自己写的函数

    }catch(boost::system::system_error& e){
        std::cout<< "Error occured! Error code = "<<e.code()
                  <<". Message: "<<e.what();
        return e.code().value();
    }
    return 0;
}

int send_data_by_send(){
    std::string raw_ip_address = "192.168.0.1";
    unsigned short port_num = 8888;
    try{
        //生成端点 创建上下文 创建socket
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(raw_ip_address), port_num);
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket sock(ioc, ep.protocol());

        sock.connect(ep);
        //tcp给我发完，没发完就一直发 阻塞在那 直到全发完为止
        std::string buf = "Hello World!";
        int send_length = sock.send(boost::asio::buffer(buf.c_str(), buf.length()));
        //send_length < 0 系统级错误
        //send_length = 0 对端关闭了
        //send_length > 0 只能是等以buf.length()
        if(send_length <= 0){
            return -1;
        }

    }catch(boost::system::system_error& e){
        std::cout<< "Error occured! Error code = "<<e.code()
                 <<". Message: "<<e.what();
        return e.code().value();
    }
    return 0;
}

int send_data_by_write(){
    std::string raw_ip_address = "192.168.0.1";
    unsigned short port_num = 8888;
    try{
        //生成端点 创建上下文 创建socket
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(raw_ip_address), port_num);
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket sock(ioc, ep.protocol());

        sock.connect(ep);
        //tcp给我发完，没发完就一直发 阻塞在那 直到全发完为止
        std::string buf = "Hello World!";
        int send_length = boost::asio::write(sock, boost::asio::buffer(buf.c_str(), buf.length()));
        //send_length < 0 系统级错误
        //send_length = 0 对端关闭了
        //send_length > 0 只能是等以buf.length()
        if(send_length <= 0){
            return -1;
        }

    }catch(boost::system::system_error& e){
        std::cout<< "Error occured! Error code = "<<e.code()
                 <<". Message: "<<e.what();
        return e.code().value();
    }
    return 0;
}

std::string read_from_socket(boost::asio::ip::tcp::socket& sock){
    const unsigned char MESSAGE_SIZE = 7;
    char buf[MESSAGE_SIZE];
    std::size_t total_bytes_read = 0;
    while(total_bytes_read != MESSAGE_SIZE){
        total_bytes_read += sock.read_some(
                boost::asio::buffer(buf + total_bytes_read,
                                    MESSAGE_SIZE - total_bytes_read));
    }
    return std::string{buf, total_bytes_read};
}

int read_data_by_read_some(){
    try{
        //创建端点 和socket
        boost::asio::ip::tcp::endpoint ep{boost::asio::ip::address::from_string("127.0.0.1"), 8889};
        boost::asio::io_context ios;
        boost::asio::ip::tcp::socket sock{ios, ep.protocol()};

        sock.connect(ep);
        read_from_socket(sock);
    }catch (boost::system::system_error& e){
        std::cout<< "Error occured! Error code = "<<e.code()
                 <<". Message: "<<e.what();
        return e.code().value();
    }
    return 0;
}

int read_data_by_receive(){
    try{
        //创建端点 和socket
        boost::asio::ip::tcp::endpoint ep{boost::asio::ip::address::from_string("127.0.0.1"), 8889};
        boost::asio::io_context ios;
        boost::asio::ip::tcp::socket sock{ios, ep.protocol()};
        sock.connect(ep);

        const unsigned char BUFF_SIZE = 7;
        char buffer_receive[BUFF_SIZE];
        int receive_length = sock.receive(boost::asio::buffer(buffer_receive, BUFF_SIZE));
        //receive_length < 0 系统级错误
        //receive_length = 0 对端关闭了
        //receive_length > 0 读到的数据长度
        if(receive_length <= 0){
            return -1;
        }

    }catch (boost::system::system_error& e){
        std::cout<< "Error occured! Error code = "<<e.code()
                 <<". Message: "<<e.what();
        return e.code().value();
    }
    return 0;
}

int read_data_by_read(){
    try{
        //创建端点 和socket
        boost::asio::ip::tcp::endpoint ep{boost::asio::ip::address::from_string("127.0.0.1"), 8889};
        boost::asio::io_context ios;
        boost::asio::ip::tcp::socket sock{ios, ep.protocol()};
        sock.connect(ep);

        const unsigned char BUFF_SIZE = 7;
        char buffer_receive[BUFF_SIZE];
        int receive_length = boost::asio::read(sock, boost::asio::buffer(buffer_receive, BUFF_SIZE));
        //receive_length < 0 系统级错误
        //receive_length = 0 对端关闭了
        //receive_length > 0 读到的数据长度
        if(receive_length <= 0){
            return -1;
        }

    }catch (boost::system::system_error& e){
        std::cout<< "Error occured! Error code = "<<e.code()
                 <<". Message: "<<e.what();
        return e.code().value();
    }
    return 0;
}