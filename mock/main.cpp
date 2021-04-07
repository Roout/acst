#include <iostream>
#include <string_view>
#include <charconv>
#include "MockStream.hpp"

int main() {
    std::byte out[1024];
    boost::asio::io_context context;
    
    MockAsyncStream ostream { context.get_executor() };
    ostream.m_buffer = out;
    ostream.m_size = sizeof(out);

    std::string_view sv { "GET / HTTP/1.1\r\n\r\n" };
    boost::asio::async_write(
        ostream,
        boost::asio::buffer(sv), 
        [](const boost::system::error_code& error, size_t bytes) {
            if (!error) {
                std::cout << "Great! " << bytes << " was transfered!\n";
            }
            else {
                std::cerr << "Error occur! " << error.message() << '\n';
            }
        }
    );

    std::string buffer = "Hello!";
    MockAsyncStream istream { context.get_executor() };
    istream.m_buffer = reinterpret_cast<std::byte*>(buffer.data());
    istream.m_size = buffer.size();
    
    boost::asio::streambuf input;
    boost::asio::async_read(
        istream, input, [](const boost::system::error_code& error, size_t bytes) {
            if (!error) {
                std::cout << "Great! " << bytes << " was transfered!\n";
            }
            else {
                std::cerr << "Error occur! " << error.message() << '\n';
            }
        }
    );

    context.run();

    std::string recieved {
        boost::asio::buffers_begin(input.data()),
        boost::asio::buffers_begin(input.data()) + input.size()
    };
    std::cout << "Recieve: " << recieved << std::endl;
    return 0;
}