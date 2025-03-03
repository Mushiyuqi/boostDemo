#include "CSession.h"
#include "CServer.h"
#include <iomanip>
#include <thread>

void CSession::PrintRecvData(const char* data, const int length) {
    std::stringstream ss;
    std::string result = "0x";
    for (int i = 0; i < length; i++) {
        std::string hexstr;
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << std::endl;
        ss >> hexstr;
        result += hexstr;
    }
    std::cout << "receive raw data is : " << result << std::endl;;
}

void CSession::Start() {
    memset(m_data.data(), 0, MAX_LENGTH);
    m_socket.async_read_some(boost::asio::buffer(m_data.data(), MAX_LENGTH),
                             std::bind(&CSession::HandleRead, this, std::placeholders::_1, std::placeholders::_2,
                                       shared_from_this()));
}

void CSession::Send(const std::string& msg) {
    std::lock_guard<std::mutex> lock(m_send_lock); // lock_guard 构造时加锁，析构时解锁
    int send_que_size = _send_que.size();
    if (send_que_size > MAX_SENDQUE) {
        std::cout << "session: " << m_uuid << " send que fulled, size is " << MAX_SENDQUE << std::endl;
        return;
    }

    _send_que.push(std::make_shared<MsgNode>(msg.c_str(), msg.size()));
    if (send_que_size > 0) {
        return;
    }

    boost::asio::async_write(m_socket, boost::asio::buffer(_send_que.front()->m_data, _send_que.front()->m_total_len),
                             std::bind(&CSession::HandleWrite, this, std::placeholders::_1, std::placeholders::_2,
                                       shared_from_this()));
}

void CSession::HandleRead(const boost::system::error_code& ec, size_t byt_transferred,
                          std::shared_ptr<CSession>& _self_shared) {
    if (!ec) {
        //debug
        // PrintRecvData(m_data.data(), byt_transferred);
        // const std::chrono::milliseconds dura(2000);
        // std::this_thread::sleep_for(dura);


        // m_data(原始数据) 已经处理的数据
        size_t copy_len = 0;

        /**
         * 我将 _recv_head_node 创建在了构造函数中
         * 给 Send 传递  msg 和 len 他就用 MsgNode 的构造函数 自动创建一个完整的包
         */

        while (byt_transferred > 0) {
            if (!_b_head_parse) {
                //消息的头部没有处理
                if (byt_transferred + _recv_head_node->m_cur_len < HEAD_LENGTH) {
                    // 发送过来的数据和以记录的头部数据之和小于标准头部的长度
                    // 将已发送的数据(发送过来的数据是头部)放入_recv_head_node中
                    memcpy(_recv_head_node->m_data + _recv_head_node->m_cur_len, m_data.data() + copy_len,
                           byt_transferred);
                    _recv_head_node->m_cur_len += byt_transferred;
                    ::memset(m_data.data(), 0, MAX_LENGTH);
                    m_socket.async_read_some(boost::asio::buffer(m_data.data(), MAX_LENGTH),
                                             std::bind(&CSession::HandleRead, this, std::placeholders::_1,
                                                       std::placeholders::_2, _self_shared));
                    return;
                }
                // 收到的消息比头部多
                size_t head_remain = HEAD_LENGTH - _recv_head_node->m_cur_len; // 头部剩余未复制的长度
                memcpy(_recv_head_node->m_data + _recv_head_node->m_cur_len, m_data.data() + copy_len, head_remain);
                // 更新以处理的data长度 和 剩余未处理的长度
                copy_len += head_remain;
                byt_transferred -= head_remain;
                // 获取头部数据
                short data_len = 0;
                memcpy(&data_len, _recv_head_node->m_data, HEAD_LENGTH);

                // 网络字节序转换成本地字节序
                data_len = boost::asio::detail::socket_ops::network_to_host_short(data_len);
                std::cout << "data_len is : " << data_len << std::endl;

                // 头部长度非法
                if (data_len > MAX_LENGTH) {
                    std::cerr << "invalid data length is : " << data_len << std::endl;
                    _server->clearSession(m_uuid);
                    return;
                }
                _recv_msg_node = std::make_shared<MsgNode>(data_len);

                // 消息的长度小于头部规定的长度，说明数据未收全，则先将部分消息放到接收节点里
                if (byt_transferred < data_len) {
                    memcpy(_recv_msg_node->m_data + _recv_msg_node->m_cur_len, m_data.data() + copy_len,
                           byt_transferred);
                    _recv_msg_node->m_cur_len += byt_transferred;
                    ::memset(m_data.data(), 0, MAX_LENGTH);

                    // 头部已经处理
                    _b_head_parse = true;
                    m_socket.async_read_some(boost::asio::buffer(m_data.data(), MAX_LENGTH),
                                             std::bind(&CSession::HandleRead, this, std::placeholders::_1,
                                                       std::placeholders::_2, _self_shared));

                    return;
                }

                // 消息的长度大于头部规定的长度，有两条消息，分别处理
                memcpy(_recv_msg_node->m_data + _recv_msg_node->m_cur_len, m_data.data() + copy_len, data_len);
                _recv_msg_node->m_cur_len += data_len;
                copy_len += data_len;
                byt_transferred -= data_len;
                _recv_msg_node->m_data[_recv_msg_node->m_total_len] = '\0';

                // 反序列化
                Json::Value msg_data;
                Json::Reader reader;
                reader.parse(_recv_msg_node->m_data, _recv_msg_node->m_data + _recv_msg_node->m_total_len, msg_data);
                std::cout << "msg id is  : " << msg_data["id"].asInt() << std::endl;
                std::cout << "msg data is: " << msg_data["data"].asString() << std::endl;

                // 序列化发送回数据
                msg_data["data"] = "server has received msg, msg data is : " + msg_data["data"].asString();
                std::string return_str = msg_data.toStyledString();
                Send(return_str);

                // 继续轮询剩余未处理的数据
                _b_head_parse = false;
                _recv_head_node->Clear();
                // 恰好收完
                if (byt_transferred <= 0) {
                    ::memset(m_data.data(), 0, MAX_LENGTH);
                    m_socket.async_read_some(boost::asio::buffer(m_data, MAX_LENGTH),
                                             std::bind(&CSession::HandleRead, this, std::placeholders::_1,
                                                       std::placeholders::_2, _self_shared));
                    return;
                }
                continue;
            }
            // 消息头部已经解析完了 解析剩余数据

            size_t remain_msg = _recv_msg_node->m_total_len - _recv_msg_node->m_cur_len; // 还需要的数据长度
            // 收到的数据任然不足剩余未处理的
            if (byt_transferred < remain_msg) {
                memcpy(_recv_msg_node->m_data + _recv_msg_node->m_cur_len, m_data.data() + copy_len, byt_transferred);
                _recv_msg_node->m_cur_len += byt_transferred;
                ::memset(m_data.data(), 0, MAX_LENGTH);
                m_socket.async_read_some(boost::asio::buffer(m_data.data(), MAX_LENGTH),
                                         std::bind(&CSession::HandleRead, this, std::placeholders::_1,
                                                   std::placeholders::_2, _self_shared));
                return;
            }

            memcpy(_recv_msg_node->m_data + _recv_msg_node->m_cur_len, m_data.data() + copy_len, remain_msg);
            _recv_msg_node->m_cur_len += remain_msg;
            byt_transferred -= remain_msg;
            copy_len += remain_msg;
            _recv_msg_node->m_data[_recv_msg_node->m_total_len] = '\0';

            // 反序列化
            Json::Value msg_data;
            Json::Reader reader;
            reader.parse(_recv_msg_node->m_data, _recv_msg_node->m_data + _recv_msg_node->m_total_len, msg_data);
            std::cout << "msg id is  : " << msg_data["id"].asInt() << std::endl;
            std::cout << "msg data is: " << msg_data["data"].asString() << std::endl;

            // 序列化发送回数据
            msg_data["data"] = "server has received msg, msg data is : " + msg_data["data"].asString();
            std::string return_str = msg_data.toStyledString();
            Send(return_str);

            // 继续轮询剩余未处理的数据
            _b_head_parse = false;
            _recv_head_node->Clear();
            // 如果 byt_transferred 恰好等于 remain_msg 调用回调函数
            if (byt_transferred <= 0) {
                ::memset(m_data.data(), 0, MAX_LENGTH);
                m_socket.async_read_some(boost::asio::buffer(m_data.data(), MAX_LENGTH),
                                         std::bind(&CSession::HandleRead, this, std::placeholders::_1,
                                                   std::placeholders::_2, _self_shared));
                return;
            }
            continue;
        }
    }
    else {
        std::cerr << "handle read error, error code is " << ec.value() << ", message is " << ec.message() << std::endl;

        /// map移除uuid时会判断map中是否会有这个uuid
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
        _server->clearSession(m_uuid);
    }
}
