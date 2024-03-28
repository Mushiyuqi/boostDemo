//
// Created by root on 24-3-26.
//
#include "../include/asynReadWrite.h"

Session::Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket): _socket{socket}, send_pending{false}{}

void Session::Connect(const boost::asio::ip::tcp::endpoint &ep) {
    _socket->connect(ep);
}

void Session::WriteCallBackErr(const boost::system::error_code &ec, std::size_t bytes_transferred,
                               std::shared_ptr<MsgNode> msg_node) {
    /// 如果 async_write_some 没有发完就继续去发送
    if(bytes_transferred + msg_node->_cur_len < msg_node->_total_len){
        /// 更新目前发了多少
        _send_node->_cur_len += bytes_transferred;
        /// 将剩下的发完
        this->_socket->async_write_some(boost::asio::buffer(_send_node->_msg + _send_node->_cur_len,
                                    _send_node->_total_len - _send_node->_cur_len),
                                        std::bind(&Session::WriteCallBackErr, this, std::placeholders::_1, std::placeholders::_2, _send_node));
    }
}
void Session::WriteToSocketErr(const std::string buf) {
    _send_node = std::make_shared<MsgNode>(buf.c_str(), buf.length());
    this->_socket->async_write_some(boost::asio::buffer(_send_node->_msg, _send_node->_total_len),
                                    ///绑定一个回调函数，写成功后就会调用该函数
                                    ///第一个参数是  error_code 第二个参数是 size_t
                                    ///详情请看 async_write_some 源码
                                    std::bind(&Session::WriteCallBackErr, this, std::placeholders::_1, std::placeholders::_2, _send_node)
                                    );

}


void Session::WriteCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred){
    if(ec.value() != 0){
        std::cout << "Error, code is" << ec.value() << " . Message is "<< ec.message() << std::endl;
        return;
    }

    auto& send_data = _send_queue.front();
    send_data->_cur_len += bytes_transferred;
    if(send_data->_total_len > send_data->_cur_len){
        /// 继续发送数据
        this->_socket->async_write_some(boost::asio::buffer(send_data->_msg + send_data->_cur_len, send_data->_total_len - send_data->_cur_len),
                                        std::bind(&Session::WriteCallBack, this, std::placeholders::_1, std::placeholders::_2));
        return;
    }

    /// 拿出队列里第下一个数据进行发送
    _send_queue.pop();
    if(_send_queue.empty())
        return;
    else{
        send_data = _send_queue.front();
        /// 将数据发送出去
        this->_socket->async_write_some(boost::asio::buffer(send_data->_msg, send_data->_total_len),
                                        std::bind(&Session::WriteCallBack, this, std::placeholders::_1, std::placeholders::_2));
        return;
    }
    return;
}

//todo 关于buf生命周期的验证
void Session::WriteToSocket(const std::string buf){
    //将要发送的数据放入队列
    _send_queue.emplace(std::make_shared<MsgNode>(buf.c_str(), buf.length()));

    /// 判断是否有未发完的数据 有为发完的就直接返回 因为已经将数据放入队列了
    if(send_pending){
        return;
    }

    /// 发送数据
    this->_socket->async_write_some(boost::asio::buffer(buf),
                                    std::bind(&Session::WriteCallBack, this, std::placeholders::_1, std::placeholders::_2));
    send_pending = true;
}


void Session::WriteAllCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred){

}

void Session::WriteAllToSocket(const std::string buf) {

}