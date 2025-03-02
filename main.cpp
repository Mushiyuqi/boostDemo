#include <iostream>
#include "include/endPoint.h"
#include <queue>
#include <memory>

/**
 *  判断是否为大端序
 * 低字节放到高位，大端序
 * 高字节放到低位，小端序
 *
 * @return 大端 1, 小端 0
 */
bool is_big_endian() {
    int num = 1;
    if(*(char *) &num == 1)
        // 系统为小端序
        return false;
    else
        // 系统为大端序
        return true;
}

int main() {
    std::cout << "is big endian: " << is_big_endian() << std::endl;
    return 0;
}
