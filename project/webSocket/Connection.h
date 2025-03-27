#pragma once
#include "global.h"
#include <memory>
#include <queue>

class Connection: public std::enable_shared_from_this<Connection>{
public:
    explicit Connection(net::io_context& ioc);
    ~Connection();
    std::string GetUUID();
    net::ip::tcp::socket& GetSocket() const;
    void AsyncAccept(); // WebSocket Accept
    void Start();
    void AsyncSend(std::string msg);
private:
    void AsyncSendHandle(std::shared_ptr<Connection> self, beast::error_code ec, std::size_t bytes_transferred);
    void AsyncSendCallBack(const std::string&);

    std::unique_ptr<stream<tcp_stream>> m_ws_ptr;
    std::string m_uuid;
    net::io_context& _ioc;
    flat_buffer m_recv_buffer;
    std::queue<std::string> m_send_queue;
    std::mutex m_send_mtx;

};

