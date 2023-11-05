#include "TCPSocket.hpp"
#include "Request.hpp"
#include "Rountings.hpp"
#include <thread>
#include <filesystem>
#include <fstream>
#include <stdlib.h>
#include <cassert>
#include <coroutine>

namespace fs = std::filesystem;
#define BUF_SIZE 2048

class HTTPServer {
private:
    TCPSocket* sock;

    bool endsWith(const std::string& fullString, const std::string& ending) {
        return (fullString.length() >= ending.length()) && (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }

    void setServerHeader(std::string& payload) {
        payload += "Server: CHTTP\r\n";
    }

    void setContentLengthHeader(std::string& payload, int len) {
        char buffer[33];
        snprintf(buffer, sizeof(buffer), "%d", len);
        payload += "Content-Length: " + std::string(buffer) + "\r\n";
    }

    void setContentTypeHeader(std::string& payload, const std::string& file) {
        if (endsWith(file, ".png"))
            payload += "Content-Type: image/png\r\n";
        else if (endsWith(file, ".html"))
            payload += "Content-Type: text/html; charset=utf-8\r\n";
        else
            payload += "Content-Type: text/plain\r\n";
    }

public:
    HTTPServer() {
        sock = new TCPSocket();
    }

    std::string receiveRequest() {
        char* buf = new char[BUF_SIZE];

        int res = sock->receive(buf, BUF_SIZE);
        std::string tmp(buf);
        std::string msg = tmp.substr(0, tmp.find("\r\n\r\n") + 1);
        memset(buf, 0, BUF_SIZE);
        delete[] buf;

        return msg;
    }

    void handleRequest(Request req) {
        if (ROUTES.count(req.getPath()) == 0) {
            send404(req.getMethod() == "GET");
            std::cout << "404 Not found: " << req.getPath() << std::endl;
        } else {
            send200(req.getPath(), req.getMethod() == "GET");
            std::cout << "200 OK: " << req.getPath() << std::endl;
        }
    }

    void acceptAll() {
        while (true) {
            if (sock->acceptClient()) {
                Request req(receiveRequest());
                handleRequest(req);
            }
        }
    }

    void start() {
        std::thread infinite_loop(&HTTPServer::acceptAll, this);
        infinite_loop.join();
    }

    void sendPage(const std::string& setedHeaders, const fs::path& page) {
        std::ifstream fin(page, std::ifstream::binary);
        if (!fin.is_open()) {
            std::cerr << "Failed to open file: " << page << std::endl;
            return;
        }

        int file_size = fs::file_size(page);

        std::string response = setedHeaders;
        response.resize(setedHeaders.size() + file_size, ' ');

        fin.read(&response[setedHeaders.size()], file_size);
        fin.close();

        sock->send(response.c_str(), response.size());
    }

    void send404(bool get_flag = true) {
        std::string response = "HTTP/1.0 404 Not Found\r\n";

        setServerHeader(response);
        setContentLengthHeader(response, fs::file_size(ROUTES["/404"]));
        setContentTypeHeader(response, ROUTES["/404"]);

        response += "\r\n";
        if (get_flag)
            sendPage(response, ROUTES["/404"]);
        else
            sock->send(response.c_str(), response.size());
    }

    void send200(const std::string& route, bool get_flag = true) {
        std::string response = "HTTP/1.0 200 OK\r\n";

        setServerHeader(response);
        setContentLengthHeader(response, fs::file_size(ROUTES[route]));
        setContentTypeHeader(response, ROUTES[route]);

        response += "\r\n";
        if (get_flag)
            sendPage(response, ROUTES[route]);
        else
            sock->send(response.c_str(), response.size());
    }
};
