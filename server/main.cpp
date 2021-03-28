#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <boost/asio.hpp>

#include "Server.hpp"

int main() {
    auto context = std::make_shared<boost::asio::io_context>();
    const int port { 15'001 };

    Server server { context, port };
    server.Start();

    std::vector<std::thread> ts;
    for (int i = 0; i < 4; i++) {
        ts.emplace_back([context]() {
            for (;;) {
                try {
                    context->run();
                    break; // run() exited normally
                }
                catch (...) {
                    // Deal with exception as appropriate.
                }
            }
        });
    }
    
    for (auto& t: ts) t.join();

    return 0;
}