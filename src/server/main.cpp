#include "chat_server.h"
#include "chat_service.h"
#include <signal.h>

// 处理服务器 CTRL + C 结束运行
void resetHandler(int) {
    ChatService::instance()->reset();
    exit(0);
}

int main() {
    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}