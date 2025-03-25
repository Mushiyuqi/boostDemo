#include "HttpConnection.h"
#include <fstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include "HttpServer.h"

HttpConnection::HttpConnection(net::io_context& ioc, HttpServer* server): m_socket(ioc), _server(server) {
    const boost::uuids::uuid uuid = boost::uuids::random_generator()();
    m_uuid = boost::uuids::to_string(uuid);
    std::cout << "HttpConnection::HttpConnection() uuid is : " << m_uuid << std::endl;
}

HttpConnection::~HttpConnection() {
    try {
        std::cerr << "HttpConnection::~HttpConnection() uuid is : " << m_uuid << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << "HttpConnection::~HttpConnection() uuid is : " << m_uuid << "  Error: " << e.what() << std::endl;
    }
}

void HttpConnection::Start() {
    ReadRequest();
    // 超时检测
    CheckDeadline();
}

tcp::socket& HttpConnection::GetSocket() {
    return m_socket;
}

std::string& HttpConnection::GetUUID() {
    return m_uuid;
}

void HttpConnection::ReadRequest() {
    auto self = shared_from_this();
    http::async_read(m_socket, m_buffer, m_request, [self](auto ec, auto size) {
        // 忽略未使用的变量
        boost::ignore_unused(size);
        if (!ec) {
            // 处理请求
            self->ProcessRequest();
        }else {
            std::cerr << "HttpConnection::ReadRequest() uuid is : " << self->m_uuid << "  Error: " << ec.message() << std::endl;
            self->m_socket.close();
            self->_server->ClearConnection(self->m_uuid);
        }
    });
}

void HttpConnection::ProcessRequest() {
    // 设置回应版本 (等于请求版本)
    m_response.version(m_request.version());
    // 设置连接方式 长连接 true 短连接 false
    m_response.keep_alive(false);

    // 处理请求
    switch (m_request.method()) {
    case http::verb::get: // 处理Get请求
        m_response.result(http::status::ok);
        m_response.set(http::field::server, "Beast/1.87.0");
        // 创建回应体
        CreateGetResponse();
        break;
    case http::verb::post: // 处理Post请求
        m_response.result(http::status::ok);
        m_response.set(http::field::server, "Beast/1.87.0");
        CreatePostResponse();
        break;
    default: // 出错
        // 设置返回状态 如: bad_request 400; not_found 404
        m_response.result(http::status::bad_request);
        // 编辑 key value  如: Content-Type: application/javascript; charset=utf-8
        m_response.set(http::field::content_type, "text/html");
        // 设置body
        beast::ostream(m_response.body()) << "Invalid request-method '"
        << std::string(m_request.method_string()) <<"'";
        break;
    }

    // 发送回应
    WriteResponse();
}

void HttpConnection::CreateGetResponse() {
    // 判断请求路径
    if (m_request.target() == "/count") {
        m_response.set(http::field::content_type, "text/html");
        beast::ostream(m_response.body()) << ReadFile(
            "/run/media/root/linuxData/study/boostDemo/project/httpServer/root/count.html");
    }
    else if (m_request.target() == "/time") {
        m_response.set(http::field::content_type, "text/html");
        beast::ostream(m_response.body()) << ReadFile(
            "/run/media/root/linuxData/study/boostDemo/project/httpServer/root/time.html");
    }else {
        // 错误路由
        m_response.result(http::status::not_found);
        m_response.set(http::field::content_type, "text/html");
        beast::ostream(m_response.body()) << "404 Not Found";
    }
}

void HttpConnection::CreatePostResponse() {
    if(m_request.target() == "/email") {
        // 获取post请求的body
        auto& body = m_request.body();
        const auto body_str = beast::buffers_to_string(body.data());
        std::cout << "HttpConnection::CreatePostResponse() receive body is:\n" <<  body_str << std::endl;
        m_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        if(!reader.parse(body_str, src_root)) {
            std::cout << "HttpConnection::CreatePostResponse() Failed to parse Json data"  << std::endl;
            root["error"] = 1001;
            std::string json_str = root.toStyledString();
            beast::ostream(m_response.body()) << json_str;
            return;
        }

        auto email = src_root["email"].asString();
        std::cout << "HttpConnection::CreatePostResponse() receive email is:\n" <<  email << std::endl;
        root["error"] = 0;
        root["email"] = email;
        root["msg"] = "receive email post success";
        std::string json_str = root.toStyledString();
        beast::ostream(m_response.body()) << json_str;

    }else {
        // 错误路由
        m_response.result(http::status::not_found);
        m_response.set(http::field::content_type, "text/html");
        beast::ostream(m_response.body()) << "404 Not Found";
    }
}

void HttpConnection::WriteResponse() {
    auto self = shared_from_this();
    // 设置响应长度
    m_response.content_length(m_response.body().size());
    http::async_write(m_socket, m_response, [self](auto ec, auto size) {
        // 忽略未使用的变量
        boost::ignore_unused(size);
        if (!ec) {
            // 短连接 关闭服务器发送端
            self->m_socket.shutdown(tcp::socket::shutdown_send);
            // 关闭定时器
            self->m_deadline.cancel();
        }else {
            std::cerr << "HttpConnection::WriteResponse() uuid is : " << self->m_uuid << "  Error: " << ec.message() << std::endl;
            self->m_socket.close();
            self->_server->ClearConnection(self->m_uuid);
        }
    });

}

void HttpConnection::CheckDeadline() {
    // 防止被回收
    auto self = shared_from_this();

    m_deadline.async_wait([self](const boost::system::error_code& ec) {
        if (!ec) {
            // 超时
            self->m_socket.close();
        }
        self->_server->ClearConnection(self->m_uuid);
    });
}

std::string HttpConnection::ReadFile(const std::string& path) {
    // 读取数据
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf(); // 将文件内容读入 stringstream
    return std::move(buffer.str());
}
