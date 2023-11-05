#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

class TCPSocket
{
private:
    int server_fd, new_socket;
    struct sockaddr_in address;

public:
    TCPSocket()
    {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        new_socket = 0;
        int opt = 1;
        if (server_fd < 0)
        {
            printf("Error while creating socket\n");
        }
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        port(80);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 5) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
    }

    ~TCPSocket()
    {
        ::close(server_fd);
        ::close(new_socket);
    }

    void closeClient()
    {
        ::close(new_socket);
    }

    bool acceptClient()
    {
        socklen_t slen = sizeof(address);
        return (new_socket = accept(server_fd, (struct sockaddr *)&address, &slen)) != -1;
    }

    void port(int port)
    {
        address.sin_port = htons(port);
    }

    int receive(char *buf, int count)
    {
        return recv(new_socket, buf, count, 0);
    }

    int send(const char *buf, int count)
    {
        return ::send(new_socket, buf, count, 0);
    }
};