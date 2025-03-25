#pragma once
#include <memory>
#include <map>
#include <mutex>
#include <boost/asio.hpp>
#include "HttpConnection.h"
#include "global.h"

using boost::asio::ip::tcp;
class HttpServer {
public:
    HttpServer(boost::asio::io_context& io_context, std::string ip, short port);
    ~HttpServer();
    void ClearConnection(std::string uuid);

private:
    void HandleAccept(std::shared_ptr<HttpConnection> connection, const boost::system::error_code& error);
    void StartAccept();
    boost::asio::io_context& _io_context;
    tcp::acceptor m_acceptor;
    short m_port;
    std::map<std::string, std::shared_ptr<HttpConnection>> m_connections;
    std::mutex m_mutex;
};
