#ifndef CHATSERVICE
#define CHATSERVICE

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>

#include "json.hpp"
#include "user_model.h"

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

    MsgHandler getHandler(int type);

    // 处理用户异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

private:
    ChatService();

    std::unordered_map<int, MsgHandler> m_handlers;

    std::unordered_map<int, TcpConnectionPtr> m_userConn;

    UserModel m_userModel;

    std::mutex m_mutex;
};

#endif
