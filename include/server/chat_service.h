#ifndef CHATSERVICE
#define CHATSERVICE

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include "json.hpp"

using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;

typedef std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)> MsgHandler;

class ChatService
{
public:
    static ChatService *instance();
    // 登录
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 注册
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);

private:
    ChatService();

    std::unordered_map<int, MsgHandler> m_handlers;
};

#endif
