#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>
#include <cstring>
#include <fcntl.h>

constexpr int MAX_EVENTS = 10;
constexpr const char* SOCKET_PATH = "main-collect-1";

int set_non_block(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int test_unix_epoll() {
    int listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("socket");
        return -1;
    }

    unlink(SOCKET_PATH); // 确保旧 socket 不在

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return -1;
    }

    if (listen(listen_fd, 5) == -1) {
        perror("listen");
        return -1;
    }

    set_non_block(listen_fd);

    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        return -1;
    }

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev) == -1) {
        perror("epoll_ctl");
        return -1;
    }

    epoll_event events[MAX_EVENTS];

    std::cout << "🚀 Server started, listening on UNIX socket: " << SOCKET_PATH << std::endl;

    while (true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;

            if (fd == listen_fd) {
                while (true) {
                    int client_fd = accept(listen_fd, nullptr, nullptr);
                    if (client_fd == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        perror("accept");
                        continue;
                    }

                    set_non_block(client_fd);
                    epoll_event client_ev{};
                    client_ev.events = EPOLLIN | EPOLLET;  // 边缘触发
                    client_ev.data.fd = client_fd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &client_ev);
                    std::cout << "📥 Accepted new client: FD " << client_fd << std::endl;
                }
            } else {
                char buf[256] = {};
                int ret = read(fd, buf, sizeof(buf) - 1);
                if (ret > 0) {
                    std::cout << "📨 From client FD " << fd << ": " << buf << std::endl;
                } else if (ret == 0) {
                    std::cout << "❌ Client FD " << fd << " disconnected" << std::endl;
                    close(fd);
                } else {
                    perror("read");
                    close(fd);
                }
            }
        }
    }

    close(epfd);
    close(listen_fd);
    unlink(SOCKET_PATH);
    return 0;
}

int main() {
    return test_unix_epoll();
}
