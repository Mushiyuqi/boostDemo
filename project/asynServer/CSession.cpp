#include "CSession.h"
#include "CServer.h"
#include "MsgNode.h"

void CSession::Start() {
    memset(m_data.data(), 0, max_length);
    m_socket.async_read_some(boost::asio::buffer(m_data.data(), max_length), std::bind(&CSession::HandleRead, this, std::placeholders::_1, std::placeholders::_2, shared_from_this()));
}

void CSession::Send(char *msg, int max_length) {
    std::lock_guard<std::mutex> lock(m_send_lock);// lock_guard 构造时加锁，析构时解锁
    bool pending = false;// 队列中是否有数据正在发送

    if(_send_que.size() > 0){
        pending = true;
    }

    _send_que.push(std::make_shared<MsgNode>(msg, max_length));

    if(pending)
        return;

    boost::asio::async_write(m_socket, boost::asio::buffer(msg, max_length), std::bind(&CSession::HandleWrite, this, std::placeholders::_1, std::placeholders::_2, shared_from_this()));
}

void CSession::HandleRead(const boost::system::error_code& ec, size_t byt_transferred, std::shared_ptr<CSession> _self_shared) {
    if(!ec){
        std::cout<< "server receive data is " << m_data.data() << std::endl;
        Send(m_data.data(), byt_transferred);
        memset(m_data.data(), 0, max_length);
        m_socket.async_read_some(boost::asio::buffer(m_data.data(), max_length),
                                 std::bind(&CSession::HandleRead, this, std::placeholders::_1, std::placeholders::_2, _self_shared));

    }else{
        std::cerr<< "read error " << ec.value() << " "<< ec.message() <<std::endl;
        /// map移除uuid时会判断map中是否会有这个uuid
        _server->clearSession(m_uuid);
    }
}

void CSession::HandleWrite(const boost::system::error_code &ec, size_t byt_transferred, std::shared_ptr<CSession> _self_shared) {
    if(!ec){
        // 先加锁
        std::lock_guard<std::mutex> lock(m_send_lock);
        _send_que.pop();
        if(_send_que.empty())
            return;
        auto& msg_node = _send_que.front();
        boost::asio::async_write(m_socket, boost::asio::buffer(msg_node->m_data, msg_node->m_max_len), std::bind(&CSession::HandleWrite, this, std::placeholders::_1, std::placeholders::_2, _self_shared));
    }else{
        std::cerr<< "handle write failed, error code is " << ec.value() << ", message is "<< ec.message() <<std::endl;
        _server->clearSession(m_uuid);
    }
}
