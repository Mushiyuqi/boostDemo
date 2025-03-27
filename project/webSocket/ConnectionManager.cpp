#include "ConnectionManager.h"
#include <iostream>

ConnectionManager& ConnectionManager::GetInstance() {
    static ConnectionManager instance;
    return instance;
}

void ConnectionManager::AddConnection(const std::shared_ptr<Connection>& conn) {
    m_connections.insert(std::make_pair(conn->GetUUID(), conn));
}

void ConnectionManager::RemoveConnection(const std::string& uuid) {
    m_connections.erase(uuid);
}

ConnectionManager::~ConnectionManager() {
    std::cerr << "ConnectionManager::~ConnectionManager()" << std::endl;
    m_connections.clear();
}
