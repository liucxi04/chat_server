#include "chat_server.h"

#include <functional>
#include <string>
#include "json.hpp"

using namespace std::placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : m_server(loop, listenAddr, nameArg), m_loop(loop)
{
    m_server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    m_server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    m_server.setThreadNum(4);
}

void ChatServer::start()
{
    m_server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
    }
}

void ChatServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{

}