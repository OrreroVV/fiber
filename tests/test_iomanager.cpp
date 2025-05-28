/**
 * @author Orrero
 * @date 2024-12-27
 * @brief 
 * 
 * @copyright Copyright (c) 2024 by Orrero, All Rights Reserved
 */

#include "hzh/utils.h"
#include "hzh/singleton.h"
#include "hzh/iomanager.h"
#include <functional>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>
#include <cstring>
#include <cassert>

std::atomic<int> cnt = { 0 };
int sz = 100;
std::chrono::_V2::steady_clock::time_point start;
std::string SERVER = "10.0.16.159";
int PORT = 13000;

void addEvent(int sock) {

    hzh::Fiber::ptr cur = hzh::Fiber::GetThis();
    LOG_DEBUG("addEvent fiber id: %lld", hzh::Fiber::GetMainFiberId());
    hzh::IOManager::GetThis()->addEvent(sock, hzh::IOManager::WRITE, [=](){
        LOG_INFO("addevent WRITE handler fiber id: %d", hzh::Fiber::GetThis()->getId());
        int err;
        socklen_t len = sizeof(err);
        if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len) < 0) {
            LOG_ERROR("getsockopt failed");
            close(sock);
            return;
        }
        if (err != 0) {
            return;
        }


        std::string httpRequest =
        "GET / HTTP/1.1\r\n"
        "Host: 10.1.2.234:80 \r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n";
    

        write(sock, httpRequest.c_str(), httpRequest.size());
        hzh::IOManager::GetThis()->addEvent(sock, hzh::IOManager::READ, [=](){

            LOG_INFO("addevent READ handler fiber id: %d", hzh::Fiber::GetThis()->getId());
            char buffer[2048] = { };
            int ret = read(sock, buffer, 2048);
            LOG_DEBUG("read ret: %d \n%s", ret, buffer);
            close(sock);
            // cur->swapIn();// fiber 2->3 error  2->0 0->3
            
            // hzh::IOManager::GetThis()->schedule(cur);


            ++cnt;
            if (cnt % 100 == 0) {
                auto end = std::chrono::steady_clock::now();
            
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                std::cout << cnt << " IOManager executed in " << duration.count() << " ms" << std::endl;
            }

        });
    });

    // cur->YieldToHold();
    // std::cout << ++cnt << std::endl;
    // ++cnt;
    // if (cnt % 100 == 0) {
    //     auto end = std::chrono::steady_clock::now();
    
    //     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    //     std::cout << cnt << " IOManager executed in " << duration.count() << " ms" << std::endl;
    // }

    LOG_INFO("read already");
}

void test_() {
    //110.242.68.66
    std::string ip = SERVER;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    if (sock < 0) {
        std::cout << "socket error" << std::endl;
        return;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
        LOG_DEBUG("connect success");
    } else if(errno == EINPROGRESS) {
        hzh::IOManager::GetThis()->schedule(std::bind(addEvent, sock));
    } else {
        std::cout << "connect failed immediately" << std::endl;
        close(sock);
    }
}

#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

void sendHttpRequest(const std::string& host, int port) {
    // 1. 创建 socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    // 2. 获取 IP 地址
    struct hostent* server = gethostbyname(host.c_str());
    if (!server) {
        std::cerr << "No such host: " << host << std::endl;
        close(sock);
        return;
    }

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    std::memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // 3. 连接
    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return;
    }

    // 4. 构造 HTTP GET 请求
    std::string request =
        "GET / HTTP/1.1\r\n"
        "Host: " + host + "\r\n"
        "Connection: close\r\n"
        "\r\n";

    // 5. 发送请求
    ssize_t sent = send(sock, request.c_str(), request.size(), 0);
    if (sent < 0) {
        perror("send");
        close(sock);
        return;
    }

    // 6. 接收响应
    char buffer[4096];
    ssize_t received;
    while ((received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[received] = '\0';
        // std::cout << buffer;
    }

    // 7. 关闭连接
    close(sock);
}

void start_iom() {
    
    hzh::IOManager iom(5, true, "thread");
    for (int i = 0; i < sz; i++) {
        sendHttpRequest(SERVER, PORT);
        // iom.schedule(test_);
    }

    auto end = std::chrono::steady_clock::now();
            
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << cnt << " IOManager executed in " << duration.count() << " ms" << std::endl;

    std::cout << "schedule success" << std::endl;
}

int main() {
    start = std::chrono::steady_clock::now();
    start_iom();
    return 0;
}