# pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <string>
#include <json/json.h>

// 重载命名空间
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp; //tcp是一个类

namespace m_program_state {
    inline std::size_t request_count() {
        // 单例的count
        static std::size_t count = 0;
        return ++count;
    }

    inline std::time_t now() {
        return std::time(nullptr);
    }
}

