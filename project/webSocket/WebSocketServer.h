#pragma once
#include "global.h"
#include "ConnectionManager.h"

class WebSocketServer {
public:
    WebSocketServer(const WebSocketServer&) = delete;
    WebSocketServer& operator=(const WebSocketServer&) = delete;
    explicit WebSocketServer(net::io_context& ioc, const std::string& ip, unsigned short port);
    ~WebSocketServer();
private:
    void StartAccept(); // tcp层接收连接

    net::ip::tcp::acceptor m_acceptor;
    net::io_context& _ioc;

};

