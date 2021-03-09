#ifndef SERVER_HPP__
#define SERVER_HPP__

#include <iostream>
#include <vector>
#include <memory>

#include <boost/asio.hpp>
#include "Session.hpp"

class Server {
public:
    Server(std::shared_ptr<boost::asio::io_context> context, int port) 
        : m_context { context }
        , m_acceptor{ *context, tcp::endpoint(tcp::v4(), port) }
    {
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
                std::cerr << "Server accepted connection on endpoint: " << m_socket->remote_endpoint(error) << std::endl; 
                m_sessions.push_back(std::make_shared<Session>(m_context, std::move(*m_socket)));

                // wait for the new connections again
                this->Start();
            }
            else {
                std::cerr << "Watafuck!\n";
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

    std::vector<std::shared_ptr<Session>> m_sessions;
};

#endif // SERVER_HPP__