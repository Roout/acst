#include "Connection.hpp"
#include "Client.hpp"

int main() {
    const char *ADDRESS = "127.0.0.1";
    const int PORT = 15001;

    auto context = std::make_shared<boost::asio::io_context>();
    std::shared_ptr<Client> client = std::make_shared<Client>(context);

    // add some work
    client->Connect(ADDRESS, PORT);
    // client->Connect(ADDRESS, PORT);
    // client->Connect(ADDRESS, PORT);
    // client->Connect(ADDRESS, PORT);

    std::vector<std::thread> threads;
    for(size_t i = 0; i < 4; i++) {
        threads.emplace_back([context](){
            for(;;) {
                try {
                    context->run();
                    break;
                }
                catch(...) {
                    // TODO: handle exceptions
                }
            }
        });
    }

    for(auto&&t: threads) {
        t.join();
    }

    return 0;
}