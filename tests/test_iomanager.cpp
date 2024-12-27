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
        hzh::IOManager::GetThis()->addEvent(sock, hzh::IOManager::WRITE, [&](){
            LOG_DEBUG("success fd: %d", sock);
        });
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