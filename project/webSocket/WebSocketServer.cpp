#include "WebSocketServer.h"
#include <iostream>


WebSocketServer::WebSocketServer(net::io_context& ioc, const std::string& ip, const unsigned short port):
m_acceptor(ioc, net::ip::tcp::endpoint(net::ip::make_address(ip), port)), _ioc{ioc} {
    std::cout << "WebSocketServer::WebSocketServer() server is running" << std::endl
            << "ip is   : " << m_acceptor.local_endpoint().address() << std::endl
            << "port is : " << m_acceptor.local_endpoint().port() << std::endl;

    StartAccept();
}

void WebSocketServer::StartAccept() {
    auto conn = std::make_shared<Connection>(_ioc);
    m_acceptor.async_accept(conn->GetSocket(), [this, conn](auto ec) {
        if (!ec) {
            // 将协议升级为websocket
            conn->AsyncAccept();
        } else {
            std::cout << "WebSocketServer::StartAccept() error: " << ec.message() << std::endl;
        }
        StartAccept();
    });
}

WebSocketServer::~WebSocketServer() {
    m_acceptor.close();
    std::cerr << "WebSocketServer::~WebSocketServer()" << std::endl;
}
