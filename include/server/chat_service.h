#ifndef CHATSERVICE
#define CHATSERVICE

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>

#include "json.hpp"
#include "user_model.h"
#include "offline_message_model.h"
#include "friend_model.h"

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

    // 一对一聊天
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 添加好友
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 处理用户异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

    // 处理服务器异常退出
    void reset();

    MsgHandler getHandler(int type);

private:
    ChatService();

    std::unordered_map<int, MsgHandler> m_handlers;

    std::unordered_map<int, TcpConnectionPtr> m_userConn;

    UserModel m_userModel;

    OfflineMsgModel m_offlineMsgModel;

    FriendModel m_friendModel;
    
    std::mutex m_mutex;
};

#endif
