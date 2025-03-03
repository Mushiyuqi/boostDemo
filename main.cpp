#include <iostream>
#include <string>
#include "include/endPoint.h"
#include <queue>
#include <memory>
#include "example.pb.h"  // 包含生成的头文件
#include <json/json.h>

int main() {

    Json::Value root;
    root["id"] = 1001;
    root["data"] = "hello world";
    std::string request = root.toStyledString();
    std::cout << "request is : " << request << std::endl;

    Json::Value root2;
    Json::Reader reader;
    reader.parse(request, root2);
    std::cout << "msg id is   : " << root2["id"].asInt() << std::endl;
    std::cout << "msg data is : " << root2["data"].asString() << std::endl;

    return 0;
}
