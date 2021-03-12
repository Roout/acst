#ifndef CONNECTION_HPP__
#define CONNECTION_HPP__

#include <string_view>
#include <memory>

#include <boost/asio.hpp>

#include "../common/Logger.hpp"
#include "../common/SwitchBuffer.hpp"

class Client;

class Connection : 
    public std::enable_shared_from_this<Connection> 
{
public:
    Connection(size_t id
        , std::weak_ptr<Client> client
        , std::shared_ptr<boost::asio::io_context> context
    );

    void Start(const char *address, int port);

    // not thread-safe
    void Write(std::string text);

    void Read();

private:

    void WriteBuffer();

    void OnConnect(const boost::system::error_code& error);

    void OnWrite(const boost::system::error_code& error, size_t bytes);

    void OnRead(const boost::system::error_code& error, size_t bytes);

    void Shutdown();

private:
    const std::string_view DELIMITER { "\r\n\r\n" };

    const size_t m_id { 0 };
    Log m_log;
    std::weak_ptr<Client> m_client;

    std::shared_ptr<boost::asio::io_context> m_context;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::io_context::strand m_strand;
    
    boost::asio::streambuf m_inbox;
    SwitchBuffer m_outbox;
    bool m_isIdle { true };
};


#endif // CONNECTION_HPP__