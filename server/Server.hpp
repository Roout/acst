#ifndef SERVER_HPP__
#define SERVER_HPP__

#include <iostream>
#include <vector>
#include <memory>

#include <boost/asio.hpp>
#include "Session.hpp"
#include "../common/Logger.hpp"

class Server {
public:
    Server(std::shared_ptr<boost::asio::io_context> context, int port) 
        : m_context { context }
        , m_acceptor{ *context, tcp::endpoint(tcp::v4(), port) }
        , m_log { "server.txt" }
    {
        std::cout << "Start server...\n";
    }

    ~Server() {
        std::cout << "Close server...\n";
    }
    
    void Start() {
        m_socket.emplace(*m_context);
        m_acceptor.set_option(
            // To avoid "bind: Address already in use"
            tcp::acceptor::reuse_address(true)
        );
        m_acceptor.async_accept(*m_socket, [this](const boost::system::error_code& code) {
            if (!code) {
                boost::system::error_code error; 
                m_log.Write(LogType::info,
                    "Server accepted connection on endpoint:", m_socket->remote_endpoint(error), '\n'
                ); 
                std::make_shared<Session>(m_sessionId++, m_context, std::move(*m_socket))->ReadRequest();
                // wait for the new connections again
                this->Start();
            }
            else {
                m_log.Write(LogType::error,
                    "Failed to accept connection with error:", code.message(), '\n'
                ); 
            }
        });
    }

    /**
     * Shutdown the server closing all connections beforehand 
     */
    void Shutdown() {
        boost::system::error_code ec;
        m_acceptor.close(ec);
    }
    
private:

    std::shared_ptr<boost::asio::io_context> m_context;
    tcp::acceptor m_acceptor;
    std::optional<tcp::socket> m_socket;

    Log m_log;

    size_t m_sessionId { 0 };
};

#endif // SERVER_HPP__