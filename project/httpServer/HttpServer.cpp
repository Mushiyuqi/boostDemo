#include "HttpServer.h"
#include "AsioIOContextPool.h"
#include <iostream>

HttpServer::HttpServer(boost::asio::io_context& io_context, std::string ip, short port): _io_context(io_context),
                                                                               m_acceptor(_io_context,
                                                                                   tcp::endpoint(
                                                                                       net::ip::make_address(ip),
                                                                                       port)),
                                                                               m_port(port) {
    std::cout << "HttpServer::HttpServer() server is running" << std::endl
        << "ip is   : " << m_acceptor.local_endpoint().address() << std::endl
        << "port is : " << m_acceptor.local_endpoint().port() << std::endl;

    StartAccept();
}

HttpServer::~HttpServer() {
    m_acceptor.close();
    std::cerr << "HttpServer::~HttpServer()" << std::endl;
}

void HttpServer::ClearConnection(std::string uuid) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connections.erase(uuid);
}

void HttpServer::HandleAccept(std::shared_ptr<HttpConnection> connection, const boost::system::error_code& error) {
    if (!error) {
        connection->Start();
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections[connection->GetUUID()] = connection;
    }
    else {
        std::cerr << "HttpServer::HandleAccept error: " << error.what() << std::endl;
    }
    // 继续接收连接请求
    StartAccept();
}

void HttpServer::StartAccept() {
    // 获取一个io_context
    auto& io_context = AsioIOContextPool::GetInstance().GetIOContext();
    auto connection = std::make_shared<HttpConnection>(io_context, this);
    m_acceptor.async_accept(connection->GetSocket(),
                            std::bind(&HttpServer::HandleAccept, this, connection, std::placeholders::_1));
}

