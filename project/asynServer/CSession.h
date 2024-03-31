#pragma once
#include <boost/asio.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

class CServer;
/// shared_ptr 是通过 = 传参 等方式来共享一块区域 这样创建的新的ptr拥有相同的计数 本质还是一个智能指针管理一个区域
/// 不能对以用智能指针管理的区域 再用 make_ptr和一个地址 创建一个智能指针
/// 为了能在类中不使用外部管理本类实体的share_ptr来创建一个在类内部使用的share_ptr 而引入了shared_from_this()
/// 为了能使用 shared_from_this() 需让类继承std::enable_shared_from_this<className>
/// 该函数返回一个指向自己的share_ptr 该ptr与原本(外部)指向本实体的ptr保持同步计数
class CSession: public std::enable_shared_from_this<CSession>{
public:
    CSession(boost::asio::io_context& ioc, CServer* server): m_socket(ioc), _server(server){
        boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
        m_uuid = boost::uuids::to_string(a_uuid);
        std::cout<< "A Session be created " << "uuid is " << m_uuid << std::endl;
    }

    ~CSession(){
        std::cout<< "A Session died " << "uuid is " << m_uuid << std::endl;
    }

    boost::asio::ip::tcp::socket& Socket(){
        return m_socket;
    }
    std::string& getUuid(){
        return m_uuid;
    }

    void Start();
private:
    /// 当对端关闭时 服务端还会再调用一次读事件(tcp)

    void handle_read(const boost::system::error_code& ec, size_t byt_transferred, std::shared_ptr<CSession> _self_shared);
    void handle_write(const boost::system::error_code& ec, size_t byt_transferred, std::shared_ptr<CSession> _self_shared);

private:
    enum {max_length = 1024};

    boost::asio::ip::tcp::socket m_socket;
    std::array<char, max_length> m_data;
    CServer* _server;
    std::string m_uuid;
};
