#include <iostream>
#include "include/endPoint.h"
#include <queue>
#include <memory>


int main() {
    std::queue<std::shared_ptr<std::string>> q;
    {
        std::string buf("hello world");
        q.emplace(std::make_shared<std::string>(buf.c_str()));
    }
    std::cout<< *q.front() <<std::endl;
    return 0;
}
