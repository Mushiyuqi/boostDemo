#pragma once
#include <boost/asio.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <queue>
#include <json/json.h>
#include "const.h"
#include "msg.pb.h"
#include "MsgNode.h"

/**
 * shared_ptr 是通过 = 传参 等方式来共享一块区域 这样创建的新的ptr拥有相同的计数 本质还是一个智能指针管理一个区域
 * 不能对以用智能指针管理的区域 再用 make_ptr和一个地址 创建一个智能指针
 * 为了能在类中不使用外部管理本类实体的share_ptr来创建一个在类内部使用的share_ptr 而引入了shared_from_this()
 * 为了能使用 shared_from_this() 需让类继承std::enable_shared_from_this<className>
 * 该函数返回一个指向自己的share_ptr 该ptr与原本(外部)指向本实体的ptr保持同步计数
 */
class CServer;
class CSession: public std::enable_shared_from_this<CSession>{
public:
    CSession(boost::asio::io_context& ioc, CServer* server);

    ~CSession(){
        std::cerr<< "A Session died " << "uuid is " << m_uuid << std::endl;
    }

    boost::asio::ip::tcp::socket& Socket(){
        return m_socket;
    }

    std::string& getUuid(){
        return m_uuid;
    }

    // 发送接口
    void Send(const std::string& msg, short id);
    void Send(const char* msg, int max_len, short id);

    void Start();
private:
    /// 当对端关闭时 服务端还会再调用一次读事件(tcp)

    // 读回调
    void HandleReadHead(const boost::system::error_code& ec, size_t byt_transferred, std::shared_ptr<CSession>& _self_shared);
    void HandleReadMsg(const boost::system::error_code& ec, size_t byt_transferred, std::shared_ptr<CSession>& _self_shared);
    // 写回调
    void HandleWrite(const boost::system::error_code& ec, size_t byt_transferred, std::shared_ptr<CSession> _self_shared);

    // 打印数据
    static void PrintRecvData(const char* data, int length);

    void Close();

private:
    // socket 是否关闭
    bool m_b_close;

    boost::asio::ip::tcp::socket m_socket; // 自己管理的socket
    CServer* _server; // 用于清除存储在 CServer 中的自己
    std::string m_uuid; // 自己的编号

    // 接收消息的结构
    std::shared_ptr<MsgNode> _recv_head_node; // 收到的头部结构
    std::shared_ptr<RecvNode> _recv_msg_node;  // 收到的消息体信息

    // 发送队列
    std::queue<std::shared_ptr<SendNode>> _send_que;

    // 锁
    std::mutex m_send_lock;
};
