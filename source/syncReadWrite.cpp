#include "../include/syncReadWrite.h"

void write_to_socket(boost::asio::ip::tcp::socket& sock){
    std::string buf{"Hello World!"};
    std::size_t total_bytes_written{0};
    //循环发送
    //write_some 返回每次写入的字节数
    while(total_bytes_written != buf.length()){
        //write_some 不是一次将数据全发完 可能只是发送了一部分
        //total_bytes_written 记录以发送了多少数据
        //需要一个篮子（ConstBufferSequence）用buffer（）快速将数据放入篮子
        //buf.c_str() + total_bytes_written 给buf加一个字符串的偏移 剩下没发完的长度为 buf.length() - total_bytes_written
        total_bytes_written +=sock.write_some(
                boost::asio::buffer(buf.c_str() + total_bytes_written,
                                    buf.length() - total_bytes_written));

    }
}