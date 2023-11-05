#include "HTTPServer.hpp"

int main() {
    HTTPServer server;
    std::cout << "Starting server at ip: 127.0.0.1:80" << std::endl;
    server.start();
    return 0;
}
