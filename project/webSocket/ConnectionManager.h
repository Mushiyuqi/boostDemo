#pragma once
#include "Connection.h"
#include "boost/unordered_map.hpp"

class ConnectionManager {
public:
    static ConnectionManager& GetInstance();
    void AddConnection(const std::shared_ptr<Connection>& conn);
    void RemoveConnection(const std::string& uuid);
    ~ConnectionManager();

    ConnectionManager(const ConnectionManager&) = delete;
    ConnectionManager& operator=(const ConnectionManager&) = delete;

private:
    ConnectionManager() = default;
    boost::unordered_map<std::string, std::shared_ptr<Connection>> m_connections;
};
