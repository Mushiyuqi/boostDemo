#include <iostream>
#include <set>
#include <thread>
#include <memory>
#include <exception>
#include <array>
#include <boost/asio.hpp>

const int MAX_LENGTH = 1024;
typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
std::set<std::shared_ptr<std::thread>> thread_set;

//会话
void session(socket_ptr sock){
    try{
        for(;;){
            //读取数据
            std::array<char, MAX_LENGTH> data{};
            boost::system::error_code ec;
            std::size_t length = sock->read_some(boost::asio::buffer(data.data(), MAX_LENGTH), ec);
            if(ec == boost::asio::error::eof){
                //对端关闭的错误
                std::cerr<< "connection closed by peer" <<std::endl;
                break;
            }else if(ec){
                //其他错误
                throw boost::system::system_error(ec);
            }
            std::cout<< "receive from " << sock->remote_endpoint().address().to_string() << " " << sock->remote_endpoint().port() <<std::endl;
            std::cout<< "receive message: ";
            std::cout.write(data.data(), length);
            std::cout<<std::endl;

            //回传数据
            boost::asio::write(*sock, boost::asio::buffer(data, MAX_LENGTH));
        }
    }catch (std::exception& e){
        std::cerr<< "Exception in thread: " << e.what() << '\n' << std::endl;
    }
}

//接收客户端的连接
void server(boost::asio::io_context& ioc, unsigned short port){
    //接收客户端连接 acceptor要绑定一个本机地址和端口
    boost::asio::ip::tcp::acceptor acp{ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)};
    for(;;){
        //接收请求并用单独的线程去处理
        socket_ptr sock{std::make_shared<boost::asio::ip::tcp::socket>(ioc)};
        acp.accept(*sock);
        auto t = std::make_shared<std::thread>(session, sock);
        thread_set.emplace(std::move(t));
    }

}

int main(){
    try{
        boost::asio::io_context ioc;
        server(ioc, 12315);
        for(auto& t : thread_set){
            t->join();
        }
    }catch (std::exception& e){
        std::cerr<< "Exception in thread: " << e.what() << '\n' << std::endl;
    }
    return 0;
}
