#include <iostream>
#include "include/endPoint.h"
#include <queue>
#include <memory>
#include "example.pb.h"  // 包含生成的头文件

int main() {
    Book book;
    book.set_name("CPP programing");
    book.set_pages(100);
    book.set_price(200);

    // 将book序列化成二进制
    std::string bookstr;
    book.SerializeToString(&bookstr);
    std::cout << "serialize str is " << bookstr << std::endl;

    // 将bookstr反序列化到book2中
    Book book2;
    book2.ParseFromString(bookstr);

    std::cout << "book2" << std::endl
    << "name is " << book2.name() << std::endl
    << "price is " << book2.price() << std::endl
    << "pages is " << book2.pages() << std::endl;

    return 0;
}
