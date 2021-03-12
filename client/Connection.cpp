#include "Connection.hpp"
#include "Client.hpp"

#include <functional>
#include <algorithm>

#include <boost/format.hpp>

using boost::asio::ip::tcp;

Connection::Connection(size_t id
    , std::weak_ptr<Client> client
    , std::shared_ptr<boost::asio::io_context> context
)
    : m_id { id }
    , m_log { (boost::format("log_%1%.txt") % id).str().c_str() }
    , m_client { client }
    , m_context { context }
    , m_socket { *context }
    , m_strand { *context }
{
}

void Connection::Start(const char *address, int port) {
    boost::asio::ip::tcp::resolver resolver(*m_context);
    const auto endpoints = resolver.resolve(address, std::to_string(port));
    if(endpoints.empty()) {
        this->Shutdown();
    }
    else {
        const auto endpoint { endpoints.cbegin()->endpoint() };
        m_socket.async_connect(
            endpoint, 
            std::bind(&Connection::OnConnect, this->shared_from_this(), std::placeholders::_1)
        );
    }
}

void Connection::Write(std::string text) {
    m_outbox.Enque(std::move(text));
    if(m_isIdle) {
        this->WriteBuffer();
    } 
}

void Connection::WriteBuffer() {
    m_isIdle = false;
    m_outbox.SwapBuffers();
    boost::asio::async_write(
        m_socket,
        m_outbox.GetBufferSequence(),
        boost::asio::bind_executor(
            m_strand,
            std::bind(&Connection::OnWrite, 
                this->shared_from_this(), 
                std::placeholders::_1, 
                std::placeholders::_2
            )
        )
    );
}

void Connection::OnConnect(const boost::system::error_code& error) {
    if(error) {
        m_log.Write(LogType::error, 
            "Client", m_id , "failed to connect with error: ", error.message(), "\n" 
        );
        this->Shutdown();
    } 
    else {
        m_log.Write(LogType::info
            , "Client"
            , m_id
            , "connected successfully. Used local port:"
            , m_socket.local_endpoint()
            , '\n');

        auto text = boost::format("endpoint=%1% with id=%2% joined.%3%") 
            % m_socket.local_endpoint() 
            % m_id 
            % DELIMITER;
        this->Write(text.str());
    }
}

void Connection::OnWrite(const boost::system::error_code& error, size_t bytes) {
    if(error) {
        m_isIdle = true;
        m_log.Write(LogType::error, 
            "Client has error on writting: ", error.message(), '\n'
        );
        this->Shutdown();
    }
    else {
        m_log.Write(LogType::info, 
            "Client", m_id, "just sent: ", bytes, " bytes\n"
        );
        if(m_outbox.GetQueueSize()) {
            this->WriteBuffer();
        } 
        else {
            m_isIdle = true;
        }
    } 
    
}

void Connection::Shutdown() {
    if(auto locked = m_client.lock(); locked) {
        locked->Erase(this);
    }
    boost::system::error_code error;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
    m_socket.close(error);
}
