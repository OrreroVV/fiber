#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <string>
#include <thread>
#include <chrono>

void test_unix() {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return;
    }

    sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;

    std::string name = "main-collect-1";
    std::strncpy(addr.sun_path, name.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(fd);
        return;
    }

    std::cout << "✅ Connected to server. Sending messages every 2 seconds...\n";

    while (true) {
        std::string data = "hello world";
        int ret = send(fd, data.c_str(), data.size(), 0);
        if (ret == -1) {
            perror("send");
            break;
        }
        std::cout << "📤 Sent: " << data << " (" << ret << " bytes)" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    close(fd);
}

int main() {
    test_unix();
    return 0;
}
