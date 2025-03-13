#include "CServer.h"
#include "CSession.h"
#include "AsioIOServicePool.h"
#include <memory>

CServer::CServer(boost::asio::io_context& ioc, short port):
    _ioc{ioc}, m_acceptor(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
    std::cout << "Server start. port is " << port << std::endl;
    StartAccept();
}

void CServer::ClearSession(std::string uuid) {
    _sessions.erase(uuid);
}

void CServer::StartAccept() {
    /**
     *  直接用智能指针的话会把new_session提前删除
     *  Session* new_session = new Session(_ioc);
     *  使用智能指针要确保数据不会被销毁(进入每一个异步回调函数时都引用计数加1)
     *
     *  使用服务池里的 io_context 来创建 session
     */

    auto& io_context = AsioIOServicePool::GetInstance()->GetIOContext();
    std::shared_ptr<CSession> new_session = std::make_shared<CSession>(io_context, this);
    m_acceptor.async_accept(new_session->Socket(),
                            std::bind(&CServer::HandleAccept, this, new_session, std::placeholders::_1));
}

void CServer::HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code& ec) {
    if (!ec) {
        new_session->Start();
        _sessions.insert({new_session->getUuid(), new_session});
    }
    else {
        std::cout << "accept error " << std::endl;
    }
    StartAccept();
}
