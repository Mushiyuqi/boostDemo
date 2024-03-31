#include "CSession.h"
#include "CServer.h"

void CSession::Start() {
    memset(m_data.data(), 0, max_length);
    m_socket.async_read_some(boost::asio::buffer(m_data.data(), max_length), std::bind(&CSession::handle_read, this, std::placeholders::_1, std::placeholders::_2, shared_from_this()));
}

void CSession::handle_read(const boost::system::error_code& ec, size_t byt_transferred, std::shared_ptr<CSession> _self_shared) {
    if(!ec){
        std::cout<< "server receive data is " << m_data.data() << std::endl;

        boost::asio::async_write(m_socket, boost::asio::buffer(m_data.data(), max_length), std::bind(&CSession::handle_write, this, std::placeholders::_1, std::placeholders::_2, _self_shared));
        memset(m_data.data(), 0, max_length);
        m_socket.async_read_some(boost::asio::buffer(m_data.data(), max_length), std::bind(&CSession::handle_read, this, std::placeholders::_1, std::placeholders::_2, _self_shared));

    }else{
        std::cout<< "read error " << ec.value() << " "<< ec.message() <<std::endl;
        /// map移除uuid时会判断map中是否会有这个uuid
        _server->clearSession(m_uuid);
    }
}

void CSession::handle_write(const boost::system::error_code &ec, size_t byt_transferred, std::shared_ptr<CSession> _self_shared) {
    if(!ec){
        memset(m_data.data(), 0, max_length);
        m_socket.async_read_some(boost::asio::buffer(m_data.data(), max_length), std::bind(&CSession::handle_read, this, std::placeholders::_1, std::placeholders::_2, _self_shared));
    }else{
        std::cout<< "write error " << ec.value() << " "<< ec.message() <<std::endl;
        _server->clearSession(m_uuid);
    }
}

