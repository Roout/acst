#ifndef SESSION_HPP__
#define SESSION_HPP__

#include <memory>
#include <optional>
#include <functional>
#include <string_view>

#include <boost/asio.hpp>

#include "../common/SwitchBuffer.hpp"

using boost::asio::ip::tcp;

class Session 
    : public std::enable_shared_from_this<Session> 
{
public:

    Session(
        std::shared_ptr<boost::asio::io_context> context
        , tcp::socket && socket
    )
        : m_context { context }
        , m_socket { std::move(socket) }
        , m_strand { *context }
    {
    }

    ~Session() {
        this->Close();
    }

    void ReadRequest() {
        auto executor = boost::asio::bind_executor(
            m_strand, 
            std::bind(&Session::ReadHandler, 
                this->shared_from_this(), 
                std::placeholders::_1, 
                std::placeholders::_2
            )
        );
        boost::asio::async_read_until(m_socket, m_inbox, DELIMITER.data(), executor);
    }

    void WriteResponse(std::string text) {
        boost::asio::post(m_strand, [text = std::move(text), self = shared_from_this()]() mutable {
            self->m_outbox.Enque(std::move(text));
            if (self->m_idle) {
                self->Write();
            }
        });
    }

private:

    void ReadHandler(const boost::system::error_code& error, size_t bytes) {
        if(!error) {
            const auto data { m_inbox.data() };
            std::string received {
                boost::asio::buffers_begin(data), 
                boost::asio::buffers_begin(data) + bytes - DELIMITER.size()
            };
            m_inbox.consume(bytes);
            
            std::cerr << "Received: " << received << std::endl;

            this->WriteResponse(std::string("You said: ") + std::move(received) + std::string(DELIMITER));
            // continue to read incoming data
            this->ReadRequest();
        } 
    }

    void Write() {
        // add all text that is queued for write operation to active buffer
        m_outbox.SwapBuffers();
        // initiate write operation
        m_idle = false;
        boost::asio::async_write(
            m_socket,
            m_outbox.GetBufferSequence(),
            boost::asio::bind_executor(
                m_strand,
                std::bind(&Session::WriteHandler, 
                    this->shared_from_this(), 
                    std::placeholders::_1, 
                    std::placeholders::_2
                )
            )
        );
    }

    void WriteHandler(const boost::system::error_code& error, size_t bytes) {
        if(!error) {
            if(m_outbox.GetQueueSize()) {
                // we need to Write other data
                this->Write();
            } 
            else {
                m_idle = true;
            }
        }
        else {
            this->Close();
        }
    }

    void Close() {
        // TODO: check error
        boost::system::error_code error;
        m_socket.shutdown(tcp::socket::shutdown_both, error);
        m_socket.close(error);
    }

private:
    std::shared_ptr<boost::asio::io_context> m_context;

    tcp::socket m_socket;

    boost::asio::io_context::strand m_strand;

    /**
     * A buffer used for incoming information.
     */
    boost::asio::streambuf m_inbox;

    SwitchBuffer m_outbox;

    bool m_idle { true };

    static constexpr std::string_view DELIMITER { "\r\n\r\n" };

};

#endif // SESSION_HPP__