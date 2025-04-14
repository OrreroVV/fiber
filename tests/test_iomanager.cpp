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

void test_() {
    //110.242.68.66
    std::string ip = "110.242.68.66";
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
        LOG_DEBUG("connect success");
    } else if(errno == EINPROGRESS) {
        hzh::Fiber::ptr fiber(new hzh::Fiber([sock](){
            hzh::IOManager::GetThis()->addEvent(sock, hzh::IOManager::WRITE, [=](){
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
                write(sock, "hello", 5);
                char buffer[1024] = { };
                int ret = read(sock, buffer, 1024);
                hzh::IOManager::GetThis()->addEvent(sock, hzh::IOManager::READ, [=](){
                    LOG_DEBUG("read ret: %d, %s", ret, buffer);
                });
                hzh::Fiber::GetThis()->swapOut();
            });
            hzh::Fiber::GetThis()->swapOut();
        }));
        hzh::IOManager::GetThis()->schedule(fiber);
    }
}

void start_iom() {
    hzh::IOManager iom(3);
    iom.schedule(test_);
}

int main() {
    start_iom();
    return 0;
}