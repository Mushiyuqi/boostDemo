#pragma once
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <json/json.h>
#include "const.h"

class LogicSystem;

class MsgNode {
    friend class CSession;

public:
    /**
     *  创建一个node节点 长度为指定长度+1 末尾为'\0'
     * @param max_len node的长度 HEAD_LENGTH + Data length
     */
    explicit MsgNode(const short max_len): m_cur_len(0), m_total_len(max_len) {
        m_data = new char[m_total_len + 1]();
        m_data[m_total_len] = '\0'; // 给m_data添加结束符
    }

    /**
     * 释放底层资源
     */
    ~MsgNode() {
        delete[] m_data;
    }

    /**
     * 重置 node节点
     */
    void Clear() {
        ::memset(m_data, 0, m_total_len);
        m_cur_len = 0;
    }

    short m_cur_len;
    short m_total_len;
    char* m_data;
};

class RecvNode : public MsgNode {
    friend class LogicSystem;
public:
    RecvNode(short max_len, short msg_id);
private:
    short m_msg_id;
};

class SendNode : public MsgNode {
    friend class LogicSystem;
public:
    SendNode(const char* msg, short max_len, short msg_id);
private:
    short m_msg_id;
};