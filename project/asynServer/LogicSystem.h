#pragma once
#include <queue>
#include <thread>
#include <map>
#include <json/json.h>
#include <functional>
#include "Singleton.hpp"
#include "CSession.h"
#include "const.h"

typedef std::function<void(std::shared_ptr<CSession>, const short& msg_id, const std::string& msg_data)> FunCallBack;

class LogicSystem:public Singleton<LogicSystem>{
    // GetInstance() 里会new 一个LogicSystem 需要开放权限
    friend class Singleton<LogicSystem>;
public:
    // 智能指针需要释放LogicSystem
    ~LogicSystem();
    // 将数据投递到队列中
    void PostMsgToQue(std::shared_ptr<LogicNode> msg);
private:
    LogicSystem();
    // 注册回调函数
    void RegisterCallBacks();
    // 回调函数 [...]
    void HelloWorldCallBack(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
    // 由工作线程处理消息
    void DealMsg();

    // 接收的消息队列 网络线程和逻辑线程都会访问 需要加锁
    std::queue<std::shared_ptr<LogicNode>> m_msg_que;
    std::mutex m_mutex;
    std::condition_variable m_consume;
    std::thread m_worker_thread;
    bool m_b_stop;
    std::map<short, FunCallBack> m_fun_callback;

};

