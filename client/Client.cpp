#include <algorithm>

#include "Client.hpp"
#include "Connection.hpp"

void Client::Connect(const char *address, int port) {
    std::lock_guard<std::mutex> lock { m_mutex };
    auto ptr = std::make_shared<Connection>(id++, this->weak_from_this(), m_context);
    ptr->Start(address, port);
    m_connections.push_back(ptr);
}

void Client::Erase(Connection * connection) {
    std::lock_guard<std::mutex> lock { m_mutex };
    auto it = std::find_if(m_connections.begin()
        , m_connections.end()
        , [connection](const auto& element) {
            return element.get() == connection;
        }
    );
    if(it != m_connections.end()) {
        std::swap(*it, m_connections.back());
        m_connections.pop_back();
    }
}