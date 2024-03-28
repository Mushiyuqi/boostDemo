#pragma once
#include <memory>
#include <boost/asio.hpp>
#include <iostream>
#include <queue>

const int RECVSIZE = 1024;
class MsgNode{
public:
    //做为发送节点
    MsgNode(const char* msg, size_t total_len):_total_len(total_len), _cur_len(0){
        _msg = new char[total_len];
        std::memcpy(_msg, msg, total_len);
    }
    //作为接收节点
    MsgNode(size_t total_len):_total_len(total_len), _cur_len(0){
        _msg = new char[total_len];
    }

    ~MsgNode(){
        delete[] _msg;
    }

    size_t _total_len;
    size_t _cur_len;
    char* _msg;
};

class Session {
public:
    Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    void Connect(const boost::asio::ip::tcp::endpoint& ep);

    /// 没有考虑到发送数据的一致性的版本
    /// 没有使用队列来管理要发送的数据
    //写数据后的回调函数，写成功后就会调用该函数 判断是否发完 保证数据全部发完
    void WriteCallBackErr(const boost::system::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<MsgNode> msg_node);
    //写数据
    void WriteToSocketErr(const std::string buf);

    /// 队列版本
    //写数据后的回调函数，写成功后就会调用该函数 判断是否发完 保证数据全部发完
    void WriteCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred);
    //写数据
    void WriteToSocket(const std::string buf);

    /// 对于长度较大的数据 不频繁的调用回调函数的版本
    void WriteAllCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void WriteAllToSocket(const std::string buf);
private:
    // 添加发送队列 由于异步发送要确保多次发送出的数据的有序性
    // 发给同一个人的数据要确保有序性
    bool send_pending;
    std::queue<std::shared_ptr<MsgNode>> _send_queue;
    std::shared_ptr<boost::asio::ip::tcp::socket> _socket;

    std::shared_ptr<MsgNode> _send_node;

};

