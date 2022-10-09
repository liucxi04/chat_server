#ifndef CHAT_SERVICE
#define CHAT_SERVICE

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>

#include "json.hpp"
#include "user_model.h"
#include "offline_message_model.h"
#include "friend_model.h"
#include "group_model.h"
#include "redis.h"

using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;

typedef std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)> MsgHandler;

class ChatService {
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

    // 创建群组
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 加入群组
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 群聊天
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 注销
    void logout(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 处理用户异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

    // 处理服务器异常退出
    void reset();

    MsgHandler getHandler(int type);

    void handleRedisSubscribeMessage(int, const string&);

private:
    ChatService();

    std::unordered_map<int, MsgHandler> m_handlers;

    std::unordered_map<int, TcpConnectionPtr> m_userConn;

    UserModel m_userModel;

    OfflineMsgModel m_offlineMsgModel;

    FriendModel m_friendModel;

    GroupModel m_groupModel;

    std::mutex m_mutex;

    Redis m_redis;
};

#endif
