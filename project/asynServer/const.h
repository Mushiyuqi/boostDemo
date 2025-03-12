# pragma once
// 整个包的长度是 HEAD_TOTAL_LEN + MAX_LENGTH
#define MAX_LENGTH 1024 * 2 // 数据的最大长度

#define HEAD_TOTAL_LEN 4    // 头部总长度
#define HEAD_ID_LENGTH 2    // 头部ID的长度
#define HEAD_DATA_LEN 2     // 头部数据长度

#define MAX_RECVQUE 10000 // 接收队列的最大长度
#define MAX_SENDQUE 1000 //发送队列的最大长度

enum MSG_IDS {
    MSG_HELLO_WORlD = 1001
};