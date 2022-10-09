#include "chat_server.h"
#include "chat_service.h"
#include "json.hpp"

#include <functional>
#include <string>

using namespace std::placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
        : m_server(loop, listenAddr, nameArg), m_loop(loop) {
    m_server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    m_server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    m_server.setThreadNum(4);
}

void ChatServer::start() {
    m_server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &conn) {
    if (!conn->connected()) {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

void ChatServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time) {
    string buf = buffer->retrieveAllAsString();
    json js = json::parse(buf);
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    msgHandler(conn, js, time);
}