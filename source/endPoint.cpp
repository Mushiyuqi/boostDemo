//
// Created by root on 24-3-19.
//

#include "../include/endPoint.h"
#include <iostream>
#include <boost/asio.hpp>

int client_end_point(){
    std::string raw_ip_address = "127.0.0.1"; //ip地址
    unsigned short port_num = 3333; //端口
    boost::system::error_code ec; //错误码

    //转换为ip地址
    boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(raw_ip_address, ec);
    //转换错误后的处理
    if(ec.value() != 0){
        std::cout<< "Error parsing the IP address. Error code = "<< ec.value()<< ". Message: "<< ec.message()<< std::endl;
        return ec.value();
    }

    //生成端点
    boost::asio::ip::tcp::endpoint ep(ip_address, port_num);
    return 0;
}

int server_end_point(){
    //选定服务器的端口
    unsigned short port_num = 3333;

    //任何一个地址都可以来和我通信
    boost::asio::ip::address ip_address = boost::asio::ip::address_v6::any();

    //生成端点
    boost::asio::ip::tcp::endpoint ep(ip_address, port_num);
    return 0;
}

int create_tcp_socket(){
    //创建io_context对象 asio的核心
    boost::asio::io_context ioc;
    //创建一个协议
    boost::asio::ip::tcp protocol = boost::asio::ip::tcp::v4();
    //生成socket
    boost::asio::ip::tcp::socket sock(ioc, protocol);
    boost::system::error_code ec;

    sock.open(protocol, ec);
    if(ec.value()!= 0){
        std::cout<< "Error opening the socket. Error code = "<< ec.value()<< ". Message: "<< ec.message()<< std::endl;
        return ec.value();
    }
    return 0;
}

int create_acceptor_socket(){
    //创建io_context对象 asio的核心
    boost::asio::io_context ioc;
    //生成一个服务端的socket服务 就可以用acceptor来接受新的连接了
    //接受ipv4的并且发往我3333端口的信息
    boost::asio::ip::tcp::acceptor acceptor(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 3333));
    return 0;
}

int connect_to_end(){
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;
    try{
        //设置要连接的服务器的端点
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(raw_ip_address), port_num);

        boost::asio::io_context ios;
        boost::asio::ip::tcp::socket sock(ios, ep.protocol());//ep.protocol() 通过ep来转换协议

        //与服务器的端点进行连接
        sock.connect(ep);
    }catch (boost::system::system_error &e){
        std::cout<< "Error code = "<< e.code()<< ". Message: "<< e.what()<< std::endl;
        return e.code().value();
    }
    return 0;
}

int dns_connect_to_end(){
    std::string host = "llfc.club";
    std::string port = "3333";
    boost::asio::io_context ios;
    //创建dns的解析器
    boost::asio::ip::tcp::resolver::query resolver_query(host, port, boost::asio::ip::tcp::resolver::query::numeric_service);
    boost::asio::ip::tcp::resolver resolver(ios); //用于解析
    try{
        auto it = resolver.resolve(resolver_query);//resolver 去解析查询服务返回一个迭代器
        boost::asio::ip::tcp::socket sock(ios);
        sock.connect(*it);
    }catch (boost::system::system_error &e){
        std::cout<< "Error code = "<< e.code()<< ". Message: "<< e.what()<< std::endl;
        return e.code().value();
    }
    return 0;
}

int accept_new_connection(){
    //监听队列的大小
    const int BACKLOG_SIZE = 30;
    unsigned short port_num = 3333;
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::any(), port_num);
    boost::asio::io_context ios;
    try{
        //生成一个接收器
        boost::asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
        //acceptor.bind(ep);//绑定端口
        acceptor.listen(BACKLOG_SIZE);
        for(;;){
            //创建一个socket
            boost::asio::ip::tcp::socket sock(ios);
            //将建立的连接交给socket处理
            acceptor.accept(sock);
            std::cout<< "new connection"<< std::endl;
        }
    }catch (boost::system::system_error &e){
        std::cout<< "Error code = "<< e.code()<< ". Message: "<< e.what()<< std::endl;
        return e.code().value();
    }

}

void use_const_buffer(){
    std::string buf = "施佳欣";
    boost::asio::const_buffer asio_buf(buf.data(), buf.size());
    std::vector<boost::asio::const_buffer> buffers_sequence;
    buffers_sequence.emplace_back(asio_buf);
    //asio.send(buffers_sequence) //此处为伪代码
}

void use_buffer_str(){
    boost::asio::const_buffers_1 output_buffer = boost::asio::buffer("施佳欣");
}

void use_buffer_array(){
    const size_t BUF_SIZE_BYTE = 1024;
    std::unique_ptr<char[]> buf(new char[BUF_SIZE_BYTE]);
    //static_cast<void*>(buf.get())强化类型转换函数，将返回的char*转换为void*
    auto input_buf = boost::asio::buffer(static_cast<void*>(buf.get()), BUF_SIZE_BYTE);

}

void use_stream_buffer(){
    boost::asio::streambuf buf;

    std::ostream output(&buf);
    output << "Message1\nMessage2";

    std::istream input(&buf);
    std::string message1;
    std::getline(input, message1);
}


