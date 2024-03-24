#pragma once
#include <boost/asio.hpp>

//向sock中写入数据
extern void write_to_socket(boost::asio::ip::tcp::socket& sock);

//向客户端发送代码
extern int send_data_by_write_some();

//通过send函数让tcp直接将数据发完，每发完别告诉我直到发完为止
extern int send_data_by_send();

//通过全局函数write将数据直接发完 (是阻塞的)
extern int send_data_by_write();


//从socket中读取数据
extern std::string read_from_socket(boost::asio::ip::tcp::socket& sock);

extern int read_data_by_read_some();

//直接读完 (阻塞的)
extern int read_data_by_receive();

//通过全局函数read将数据全部读完 (是阻塞的)
extern int read_data_by_read();