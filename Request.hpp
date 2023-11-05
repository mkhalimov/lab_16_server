#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <iostream>

std::vector<std::string> split(std::string s, std::string delimiter = " ") {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

class Request {
private:
    std::string path, method, version;
    std::map<std::string, std::string> headers;

    std::pair<std::string, std::string> parseHeader(std::string header) {
        return std::make_pair(header.substr(0, header.find(':')),
                              header.substr(header.find(':') + 2, header.size() - header.find(':') - 1));
    }

public:
    Request() = default;

    Request(std::string payload) {
        std::stringstream ss(payload);
        std::string line;
        std::getline(ss, line);
        std::vector<std::string> vec = split(line);
        method = vec[0], path = vec[1], version = vec[2];
        if (method != "GET" && method != "HEAD") {
            std::cerr << "Invalid method: " << method << std::endl;
            exit(EXIT_FAILURE);
        }
        if (version.substr(0, version.find('/')) != "HTTP") {
            std::cerr << "Invalid version: " << version << std::endl;
            exit(EXIT_FAILURE);
        }
        version = version.substr(0, version.find('/'));

        while (std::getline(ss, line)) {
            std::pair<std::string, std::string> headerParsed = this->parseHeader(line);
            headers[headerParsed.first] = headerParsed.second;
        }
    }

    std::string getHeader(std::string key) {
        return headers[key];
    }

    std::string getMethod() {
        return method;
    }

    std::string getPath() {
        return path;
    }

    std::vector<std::string> getAllHeaders() {
        std::vector<std::string> keys;
        for (const auto& header : headers) {
            keys.push_back(header.first);
        }
        return keys;
    }
};
