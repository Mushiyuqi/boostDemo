#pragma once
#include <boost/asio.hpp>

//向sock中写入数据
extern void write_to_socket(boost::asio::ip::tcp::socket& sock);

