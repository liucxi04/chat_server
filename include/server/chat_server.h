#ifndef CHATSERVER
#define CHATSERVER

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;

class ChatServer {
public:
    // 初始化聊天服务器
    ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg);

    // 启动服务
    void start();

private:
    void onConnection(const TcpConnectionPtr &conn);

    void onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time);

private:
    TcpServer m_server;
    EventLoop *m_loop;
};

#endif