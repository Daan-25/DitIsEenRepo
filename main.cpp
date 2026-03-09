#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

int get_port() {
    const char* port_env = std::getenv("PORT");
    if (!port_env) return 8080;

    try {
        return std::stoi(port_env);
    } catch (...) {
        return 8080;
    }
}

int main() {
    int port = get_port();

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 16) < 0) {
        std::perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "Listening on port " << port << std::endl;

    while (true) {
        sockaddr_in client{};
        socklen_t client_len = sizeof(client);

        int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client), &client_len);
        if (client_fd < 0) continue;

        char buffer[4096];
        std::memset(buffer, 0, sizeof(buffer));
        recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        const std::string body =
            "<!doctype html>"
            "<html><head><meta charset='utf-8'><title>Hello</title></head>"
            "<body><h1>Hello World from C++ on Azure VM! CMAKE!!!</h1></body></html>";

        const std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "Connection: close\r\n\r\n" +
            body;

        send(client_fd, response.c_str(), response.size(), 0);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
