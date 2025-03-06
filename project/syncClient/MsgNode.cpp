#include "MsgNode.h"

RecvNode::RecvNode(short max_len, short msg_id): MsgNode(max_len), m_msg_id(msg_id){}

SendNode::SendNode(const char* msg, short max_len, short msg_id): MsgNode(HEAD_TOTAL_LEN + max_len), m_msg_id(msg_id) {
    // 先发送id, 转为网络字节序
    unsigned short msg_id_host = boost::asio::detail::socket_ops::host_to_network_short(msg_id);
    memcpy(m_data, &msg_id_host, HEAD_ID_LENGTH);
    // 转为网络字节序
    unsigned short msg_len_host = boost::asio::detail::socket_ops::host_to_network_short(max_len);
    memcpy(m_data + HEAD_ID_LENGTH, &msg_len_host, HEAD_DATA_LEN);
    memcpy(m_data + HEAD_ID_LENGTH + HEAD_DATA_LEN, msg, max_len);
}

