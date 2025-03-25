#pragma once
#include <iostream>
#include <memory>
#include "global.h"
#include "const.h"

class HttpServer;

class HttpConnection: public std::enable_shared_from_this<HttpConnection> {
public:
    explicit HttpConnection(net::io_context& ioc, HttpServer* server);
    ~HttpConnection();
    void Start();
    tcp::socket& GetSocket();
    std::string& GetUUID();
private:
    HttpServer* _server;
    std::string m_uuid;

    tcp::socket m_socket;
    beast::flat_buffer m_buffer{FLAT_SIZE};
    http::request<http::dynamic_body> m_request;
    http::response<http::dynamic_body> m_response;

    // 定时器 让socket来调度 60s 超时
    net::steady_timer m_deadline{m_socket.get_executor(), std::chrono::seconds(60)};

    // 读请求
    void ReadRequest();

    // 处理请求
    void ProcessRequest();
    // 创建GET回应体
    void CreateGetResponse();
    // 创建POST回应体
    void CreatePostResponse();
    // 发送回应
    void WriteResponse();


    // 超时关闭请求
    void CheckDeadline();
    // 读取文件
    static std::string ReadFile(const std::string& path);
};

