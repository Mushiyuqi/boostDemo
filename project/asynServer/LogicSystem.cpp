#include "LogicSystem.h"

LogicSystem::LogicSystem(): m_b_stop(false) {
    RegisterCallBacks();
    m_worker_thread = std::thread(&LogicSystem::DealMsg, this);
}

LogicSystem::~LogicSystem() {
    std::cerr << "LogicSystem destruct " << std::endl;

    // 设置结束标志
    m_b_stop = true;
    // 唤醒工作线程
    m_consume.notify_one();
    // 等待工作线程退出
    m_worker_thread.join();
}

void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> msg) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_msg_que.push(msg);

    if(m_msg_que.size() == 1) {
        // 唤醒工作线程
        m_consume.notify_one();
    }
}

void LogicSystem::DealMsg() {
    for (;;) {
        std::unique_lock<std::mutex> lock(m_mutex);

        // 判断队列为空则用条件变量等待
        // 没有资源就挂起
        while (m_msg_que.empty() && !m_b_stop) {
            // 释放资源并解锁
            m_consume.wait(lock);
        }

        // 判断如果为关闭状态, 取出逻辑队列所有数据及时处理并退出循环
        // 1. 停服 有数据
        // 2. 停服 无数据
        if (m_b_stop) {
            while (!m_msg_que.empty()) {
                auto msg_node = m_msg_que.front();
                std::cout << "LogicSystem: recv msg id is : " << msg_node->_recvnode->m_msg_id << std::endl;
                auto call_back_iter = m_fun_callback.find(msg_node->_recvnode->m_msg_id);
                if (call_back_iter == m_fun_callback.end()) {
                    // 没找到对应的回调函数 剔除首元素
                    m_msg_que.pop();
                    continue;
                }
                // 处理数据
                call_back_iter->second(msg_node->_session, msg_node->_recvnode->m_msg_id,
                                       std::string(msg_node->_recvnode->m_data, msg_node->_recvnode->m_total_len));
                m_msg_que.pop();
            }

            break;
        }

        // 如果没有停服, 并且队列有数据
        // 3. 不停服 有数据
        auto msg_node = m_msg_que.front();
        std::cout << "LogicSystem : recv msg id is : " << msg_node->_recvnode->m_msg_id << std::endl;

        auto call_back_iter = m_fun_callback.find(msg_node->_recvnode->m_msg_id);
        if (call_back_iter == m_fun_callback.end()) {
            // 没找到对应的回调函数 剔除首元素
            m_msg_que.pop();
            continue;
        }
        // 处理数据
        call_back_iter->second(msg_node->_session, msg_node->_recvnode->m_msg_id,
                               std::string(msg_node->_recvnode->m_data, msg_node->_recvnode->m_total_len));
        m_msg_que.pop();
    }
}

void LogicSystem::RegisterCallBacks() {
    // 注册回调函数 [...]
    m_fun_callback[MSG_HELLO_WORlD] = std::bind(&LogicSystem::HelloWorldCallBack, this, std::placeholders::_1,
                                                std::placeholders::_2, std::placeholders::_3);
}

void LogicSystem::HelloWorldCallBack(std::shared_ptr<CSession> session, const short& msg_id,
                                     const std::string& msg_data) {
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);
    std::cout << "receive msg id   : " << root["id"].asInt() << std::endl;
    std::cout << "receive msg data : " << root["data"].asString() << std::endl;
    root["data"] = "server has receive msg, msg data is : " + root["data"].asString();

    // 序列化发回
    session->Send(root.toStyledString(), msg_id);
}

