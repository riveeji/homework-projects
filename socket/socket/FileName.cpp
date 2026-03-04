#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <io.h>
#pragma comment(lib, "ws2_32.lib") // 链接 Winsock 库
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

#define PORT 8080
#define BUFFER_SIZE 1024
#define HTML_DIR "./www" // 资源根目录

void platform_init() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
        exit(EXIT_FAILURE);
    }
#endif
}

void platform_cleanup() {
#ifdef _WIN32
    WSACleanup();
#endif
}

void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE] = { 0 };

    // 接收请求
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    std::cout << "Received Request:\n" << buffer << std::endl;

    // 解析请求
    std::istringstream request_stream(buffer);
    std::string method, path, version;
    request_stream >> method >> path >> version;

    // 默认返回 index.html
    if (path == "/") path = "/index.html";

    std::string file_path = std::string(HTML_DIR) + path;
    std::ifstream file(file_path, std::ios::binary);
    std::ostringstream response;

    if (file) {
        std::string body((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << body.size() << "\r\n";
        response << "Connection: close\r\n\r\n";
        response << body;
    }
    else {
        std::string body = "<h1>404 Not Found</h1>";
        response << "HTTP/1.1 404 Not Found\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << body.size() << "\r\n";
        response << "Connection: close\r\n\r\n";
        response << body;
    }

    // 发送响应
    send(client_socket, response.str().c_str(), response.str().size(), 0);

#ifdef _WIN32
    closesocket(client_socket);
#else
    close(client_socket);
#endif
}

int main() {
    platform_init();

    SOCKET server_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // 创建Socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 绑定端口
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 开始监听
    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server running on http://127.0.0.1:" << PORT << std::endl;

    while (true) {
        SOCKET client_socket;
        if ((client_socket = accept(server_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }
        std::cout << "New connection accepted." << std::endl;

        // 处理客户端请求
        handle_client(client_socket);
    }

#ifdef _WIN32
    closesocket(server_socket);
#else
    close(server_socket);
#endif

    platform_cleanup();
    return 0;
}
