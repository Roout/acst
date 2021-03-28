#ifndef CLIENT_HPP__
#define CLIENT_HPP__

#include <memory>
#include <iostream>
#include <mutex>

#include <boost/asio.hpp>

class Connection;

class Client : 
    public std::enable_shared_from_this<Client> {
public:

    Client(std::shared_ptr<boost::asio::io_context> context)
        : m_context { context }
    {
        std::cout << "Client is starting...\n";
    }

    ~Client() {
        std::cout << "Client ends...\n";
    }

    void Connect(const char *address, int port);

    void Erase(Connection * connection);

private:
    std::shared_ptr<boost::asio::io_context> m_context { nullptr };

    std::vector<std::shared_ptr<Connection>> m_connections;

    std::mutex m_mutex;

    size_t id { 0 };
};


#endif // CLIENT_HPP__