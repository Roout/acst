#include <iostream>
#include <string_view>
#include "MockStream.hpp"

int main() {
    std::byte out[1024];
    boost::asio::io_context context;
    
    MockAsyncWriteStream ostream { context.get_executor(), out, sizeof(out) };

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
    MockAsyncReadStream istream { 
        context.get_executor(), 
        boost::asio::const_buffer(buffer.data(), buffer.size()) 
    };
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