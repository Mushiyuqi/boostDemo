#pragma once
#include <memory>
#include <mutex>
#include <iostream>

template <typename T>
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;

    // 所有对象共享一个 _instance
    static std::shared_ptr<T> _instance;

public:
    ~Singleton() {
        std::cerr << "this is singleton destruct" << std::endl;
    }

    /**
     * 调用 GetInstance 返回 同一个 _instance指向的对象
     * @return _instance
     */
    static std::shared_ptr<T> GetInstance() {
        // 只会在第一次调用时初始化
        static std::once_flag s_flag;

        // 只会被调用一次且线程安全
        std::call_once(s_flag, [&]() {
            // 创建对象
            _instance = std::shared_ptr<T>(new T);
        });

        return _instance;
    }

    void PrintAddress() {
        std::cout << _instance.get() << std::endl;
    }
};

// 静态成员变量初始化
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
