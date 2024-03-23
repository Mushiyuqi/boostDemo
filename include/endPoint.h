#pragma once

extern int create_tcp_socket();//创建tcp套接字

extern int client_end_point();//客户端 生成端点
extern int connect_to_end();//创建 客户端 连接操作

extern int server_end_point();//服务端 生成端点
extern int create_acceptor_socket();//创建 服务器端 套接字 并绑定
extern int accept_new_connection();//服务端 接受客户端连接

extern int dns_connect_to_end();//解析域名实现客户端到服务端的连接

//buffer
extern void use_const_buffer();//模拟原始的方式创建buffers_sequence
//用高效的方式创建buffers_sequence
extern void use_buffer_str();
extern void use_buffer_array();

//streambuf 将输入输出流与streambuf进行绑定，实现流式输入输出
extern void use_stream_buffer();