#include <iostream>
#include <string_view>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main() {
    const std::string_view DELIMITER {"\r\n\r\n"};

    boost::system::error_code error;
    boost::asio::io_context context;
    tcp::socket socket { context };
    tcp::resolver resolver { context };
    auto endpoints = resolver.resolve("127.0.0.1", "15001");

    boost::asio::connect(socket, endpoints, error);
    if(error) {
        std::cerr << "Something fucked up: " << error.message() << std::endl;
        exit(1);
    };

    // send request
    auto send = boost::asio::write(socket, boost::asio::buffer("Hello!\r\n\r\n"));
    if(send <= 0) {
        std::cerr << "Something fucked up: " << error.message() << std::endl;
        exit(1);
    };          

    // read response
    boost::asio::streambuf response;
    // Read the response headers, which are terminated by a blank line.
    auto bytes = boost::asio::read_until(socket, response, DELIMITER.data(), error);
    
    const auto data { response.data() };
    std::string received {
        boost::asio::buffers_begin(data), 
        boost::asio::buffers_begin(data) + bytes - DELIMITER.size()
    };
    response.consume(bytes);

    std::cout << "Received:" << received;

    return 0;
}