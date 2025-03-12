#include "CSession.h"
#include "CServer.h"
#include <iomanip>
#include <thread>
#include <utility>
#include "LogicSystem.h"

void CSession::PrintRecvData(const char* data, const int length) {
    std::stringstream ss;
    std::string result = "0x";
    for (int i = 0; i < length; i++) {
        std::string hexstr;
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << std::endl;
        ss >> hexstr;
        result += hexstr;
    }
    std::cout << "receive raw data is : " << data << std::endl;;
}

CSession::CSession(boost::asio::io_context& ioc, CServer* server): m_socket(ioc), m_b_close(false), _server(server) {
    boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
    m_uuid = boost::uuids::to_string(a_uuid);
    std::cout << "A Session be created " << "uuid is " << m_uuid << std::endl;

    _recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

void CSession::Send(const std::string& msg, const short id) {
    std::lock_guard<std::mutex> lock(m_send_lock); // lock_guard 构造时加锁，析构时解锁
    int send_que_size = _send_que.size();
    if (send_que_size > MAX_SENDQUE) {
        std::cerr << "session: " << m_uuid << " send que fulled, size is " << MAX_SENDQUE << std::endl;
        return;
    }

    _send_que.push(std::make_shared<SendNode>(msg.c_str(), msg.size(), id));
    if (send_que_size > 0) {
        return;
    }

    boost::asio::async_write(m_socket, boost::asio::buffer(_send_que.front()->m_data, _send_que.front()->m_total_len),
                             std::bind(&CSession::HandleWrite, this, std::placeholders::_1, std::placeholders::_2,
                                       shared_from_this()));
}

void CSession::Send(const char* msg, const int max_len, const short id) {
    std::lock_guard<std::mutex> lock(m_send_lock); // lock_guard 构造时加锁，析构时解锁
    int send_que_size = _send_que.size();
    if (send_que_size > MAX_SENDQUE) {
        std::cerr << "session: " << m_uuid << " send que fulled, size is " << MAX_SENDQUE << std::endl;
        return;
    }

    _send_que.push(std::make_shared<SendNode>(msg, max_len, id));
    if (send_que_size > 0) {
        return;
    }

    boost::asio::async_write(m_socket, boost::asio::buffer(_send_que.front()->m_data, _send_que.front()->m_total_len),
                             std::bind(&CSession::HandleWrite, this, std::placeholders::_1, std::placeholders::_2,
                                       shared_from_this()));
}

void CSession::Close() {
    m_socket.close();
    m_b_close = true;
}

void CSession::Start() {
    _recv_head_node->Clear();
    m_socket.async_read_some(boost::asio::buffer(_recv_head_node->m_data, HEAD_TOTAL_LEN),
                             std::bind(&CSession::HandleReadHead, this, std::placeholders::_1, std::placeholders::_2,
                                       shared_from_this()));
}

void CSession::HandleReadHead(const boost::system::error_code& ec, size_t byt_transferred,
                              std::shared_ptr<CSession>& _self_shared) {
    if (!ec) {
        if (byt_transferred < HEAD_TOTAL_LEN) {
            std::cout << "session: " << m_uuid << " recv head error, size is " << byt_transferred << std::endl;
            Close();
            _server->clearSession(m_uuid);
            return;
        }

        // 解析ID
        short id = 0;
        memcpy(&id, _recv_head_node->m_data, HEAD_ID_LENGTH);
        // 网络字节序转换成本地字节序
        id = boost::asio::detail::socket_ops::network_to_host_short(id);

        if (id > MAX_RECVQUE) {
            std::cerr << "invalid id is : " << id << std::endl;
            Close();
            _server->clearSession(m_uuid);
            return;
        }

        // 解析数据长度
        short data_len = 0;
        memcpy(&data_len, _recv_head_node->m_data + HEAD_ID_LENGTH, HEAD_DATA_LEN);
        // 网络字节序转换成本地字节序
        data_len = boost::asio::detail::socket_ops::network_to_host_short(data_len);

        if (data_len > MAX_LENGTH) {
            std::cerr << "invalid data length is : " << data_len << std::endl;
            Close();
            _server->clearSession(m_uuid);
            return;
        }

        std::cout << "## \033[32mreceive a head\033[0m, id is : " << id;
        std::cout << ", data len is : " << data_len;
        std::cout << " ##" << std::endl;

        // 读取消息体
        _recv_msg_node = std::make_shared<RecvNode>(data_len, id);
        boost::asio::async_read(m_socket, boost::asio::buffer(_recv_msg_node->m_data, _recv_msg_node->m_total_len),
                                std::bind(&CSession::HandleReadMsg, this, std::placeholders::_1, std::placeholders::_2,
                                          _self_shared));
    }
    else {
        std::cerr << "handle read failed, error is " << ec.what() << std::endl;
        Close();
        _server->clearSession(m_uuid);
    }
}

void CSession::HandleReadMsg(const boost::system::error_code& ec, size_t byt_transferred,
                             std::shared_ptr<CSession>& _self_shared) {
    if (!ec) {
        // 原始数据
        // PrintRecvData(_recv_msg_node->m_data, byt_transferred);

        // 将数据放入逻辑队列
        LogicSystem::GetInstance()->PostMsgToQue(std::make_shared<LogicNode>(shared_from_this(), _recv_msg_node));

        //再次接收头部数据
        Start();
    }
    else {
        std::cerr << "handle read msg failed,  error is " << ec.what() << std::endl;
        Close();
        _server->clearSession(m_uuid);
    }
}

void CSession::HandleWrite(const boost::system::error_code& ec, size_t byt_transferred,
                           std::shared_ptr<CSession> _self_shared) {
    if (!ec) {
        // 先加锁
        std::lock_guard<std::mutex> lock(m_send_lock);

        _send_que.pop();
        if (!_send_que.empty()) {
            auto& msg_node = _send_que.front();
            boost::asio::async_write(m_socket, boost::asio::buffer(msg_node->m_data, msg_node->m_total_len),
                                     std::bind(&CSession::HandleWrite, this, std::placeholders::_1,
                                               std::placeholders::_2,
                                               _self_shared));
        }
    }
    else {
        std::cerr << "handle write failed, error code is " << ec.value() << ", message is " << ec.message() <<
            std::endl;
        Close();
        _server->clearSession(m_uuid);
    }
}

LogicNode::LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recvnode):
    _session(std::move(session)), _recvnode(std::move(recvnode)) {
}
