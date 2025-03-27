#include "Connection.h"
#include "ConnectionManager.h"
#include <iostream>

Connection::Connection(net::io_context& ioc): _ioc(ioc),
                                              m_ws_ptr(std::make_unique<stream<tcp_stream>>(make_strand(ioc))) {
    const boost::uuids::uuid uuid = boost::uuids::random_generator()();
    m_uuid = boost::uuids::to_string(uuid);
    std::cout << "Connection::Connection() uuid is : " << m_uuid << std::endl;
}

Connection::~Connection() {
    try {
        std::cerr << "Connection::~Connection() uuid is : " << m_uuid << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << "Connection::~Connection uuid is : " << m_uuid << "  Error: " << e.what() << std::endl;
    }
}

std::string Connection::GetUUID() {
    return m_uuid;
}

net::ip::tcp::socket& Connection::GetSocket() const {
    return get_lowest_layer(*m_ws_ptr).socket();
}

void Connection::AsyncAccept() {
    auto self = shared_from_this();
    m_ws_ptr->async_accept([self](auto ec) {
            if (!ec) {
                ConnectionManager::GetInstance().AddConnection(self);
                self->Start();
            }
            else {
                std::cerr << "Connection::AsyncAccept() uuid is : " << self->m_uuid << "  Error: " << ec.message() << std::endl;
                ConnectionManager::GetInstance().RemoveConnection(self->m_uuid);
            }
        }
    );
}

void Connection::Start() {
    auto self = shared_from_this();
    m_ws_ptr->async_read(m_recv_buffer, [self](auto ec, auto bytes_transferred) {
            if (!ec) {
                // 读取数据
                self->m_ws_ptr->text(self->m_ws_ptr->got_text());
                std::string recv_data = beast::buffers_to_string(self->m_recv_buffer.data());

                // 清空buffer
                self->m_recv_buffer.consume(self->m_recv_buffer.size());

                // 发回数据
                self->AsyncSend(std::move(recv_data));

                // 继续读取数据
                self->Start();
            }else {
                std::cerr << "Connection::AsyncSend() uuid is : " << self->m_uuid << "  Error: " << ec.message() << std::endl;
                ConnectionManager::GetInstance().RemoveConnection(self->m_uuid);
            }
    });
}

void Connection::AsyncSend(std::string msg) {
    std::unique_lock lock(m_send_mtx);
    const int que_len = m_send_queue.size();
    m_send_queue.push(std::move(msg));
    if (que_len > 0) {
        return;
    }
    std::string send_msg = m_send_queue.front();
    lock.unlock();

    // 发送队列里的数据
    AsyncSendCallBack(std::move(send_msg));
}

void Connection::AsyncSendCallBack(const std::string& msg) {
    auto self = shared_from_this();
    m_ws_ptr->async_write(net::buffer(msg.c_str(), msg.size()), [self](auto ec, auto bytes_transferred) {
        if (!ec) {
            // 发送数据成功
            std::unique_lock lock(self->m_send_mtx);
            self->m_send_queue.pop();
            if (self->m_send_queue.empty()) {
                return;
            }
            std::string send_msg = self->m_send_queue.front();
            lock.unlock();

            // 发送队列里剩余数据
            self->AsyncSendCallBack(std::move(send_msg));
        }else {
            std::cerr << "Connection::AsyncSendCallBack uuid is : " << self->m_uuid << "  Error: " << ec.message() << std::endl;
            ConnectionManager::GetInstance().RemoveConnection(self->m_uuid);
        }
    });
}

