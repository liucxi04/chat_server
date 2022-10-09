#include "chat_service.h"
#include "public.h"

#include <muduo/base/Logging.h>
#include <string>
#include <vector>

using namespace muduo;
using namespace std::placeholders;

ChatService *ChatService::instance() {
    static ChatService service;
    return &service;
}

ChatService::ChatService() {
    // 基本业务
    m_handlers[LOGIN_MSG] = std::bind(&ChatService::login, this, _1, _2, _3);
    m_handlers[LOGOUT_MSG] = std::bind(&ChatService::logout, this, _1, _2, _3);
    m_handlers[REG_MSG] = std::bind(&ChatService::reg, this, _1, _2, _3);
    m_handlers[ONE_CHAT_MSG] = std::bind(&ChatService::oneChat, this, _1, _2, _3);
    m_handlers[ADD_FRIEND_MSG] = std::bind(&ChatService::addFriend, this, _1, _2, _3);
    // 群组业务
    m_handlers[CREATE_GROUP_MSG] = std::bind(&ChatService::createGroup, this, _1, _2, _3);
    m_handlers[ADD_GROUP_MSG] = std::bind(&ChatService::addGroup, this, _1, _2, _3);
    m_handlers[GROUP_CHAT_MSG] = std::bind(&ChatService::groupChat, this, _1, _2, _3);

    if (m_redis.connect()) {
        m_redis.initNotifyHandler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
}

MsgHandler ChatService::getHandler(int type) {
    auto it = m_handlers.find(type);
    if (it == m_handlers.end()) {
        // 返回一个默认操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time) {
            LOG_ERROR << "msg type: " << type << "can not find handler!";
        };
    } else {
        return it->second;
    }
}

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int id = js["id"].get<int>();
    std::string password = js["password"];

    User user = m_userModel.query(id);
    if (user.getId() == id && user.getPassword() == password) {
        if (user.getState() == "online") {
            json response;
            response["msgid"] = MsgType::LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "this account is using, input another!";
            conn->send(response.dump());
        } else {
            // 记录用户连接信息，需要保证互斥
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_userConn[id] = conn;
            }

            // redis 订阅消息
            m_redis.subscribe(id);

            // 修改用户登录状态
            user.setState("online");
            m_userModel.updateState(user);

            json response;
            response["msgid"] = MsgType::LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            // 处理离线信息
            std::vector<std::string> message = m_offlineMsgModel.query(id);
            if (!message.empty()) {
                response["offlinemessage"] = message;
                m_offlineMsgModel.remove(id);
            }

            // 返回好友列表
            std::vector<User> friends = m_friendModel.query(id);
            if (!friends.empty()) {
                // 处理离线信息
                std::vector<std::string> tmp;
                for (auto &u: friends) {
                    json js2;
                    js2["id"] = u.getId();
                    js2["name"] = u.getName();
                    js2["state"] = u.getState();
                    tmp.push_back(js2.dump());
                }
                response["friends"] = tmp;
            }

            // 返回群组信息
            std::vector<Group> groups = m_groupModel.queryGroups(id);
            if (!groups.empty()) {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                std::vector<string> g;
                for (Group &group: groups) {
                    json group_json;
                    group_json["id"] = group.getId();
                    group_json["groupname"] = group.getName();
                    group_json["groupdesc"] = group.getDesc();
                    std::vector<string> u;
                    for (GroupUser &gp: group.getUsers()) {
                        json j;
                        j["id"] = gp.getId();
                        j["name"] = gp.getName();
                        j["state"] = gp.getState();
                        j["role"] = gp.getRole();
                        u.push_back(j.dump());
                    }
                    group_json["users"] = u;
                    g.push_back(group_json.dump());
                }

                response["groups"] = g;
            }

            conn->send(response.dump());
        }
    } else {
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "account id or password error!";
        conn->send(response.dump());
    }
}

void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    std::string name = js["name"];
    std::string password = js["password"];
    User user(-1, name, password);

    bool state = m_userModel.insert(user);
    if (state) {
        json response;
        response["msgid"] = MsgType::REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    } else {
        json response;
        response["msgid"] = MsgType::REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void ChatService::logout(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int user_id = js["id"].get<int>();

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_userConn.find(user_id);
        if (it != m_userConn.end()) {
            m_userConn.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    m_redis.unsubscribe(user_id);

    // 更新用户的状态信息
    User user(user_id, "", "", "offline");
    m_userModel.updateState(user);
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn) {
    User user;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto it = m_userConn.begin(); it != m_userConn.end(); ++it) {
            if (it->second == conn) {
                user.setId(it->first);
                m_userConn.erase(it);
                break;
            }
        }
    }

    m_redis.unsubscribe(user.getId());

    user.setState("offline");
    m_userModel.updateState(user);
}

void ChatService::reset() {
    m_userModel.resetState();
}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int to = js["toid"].get<int>();

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_userConn.find(to);
        if (it != m_userConn.end()) {
            // 在线，转发消息
            it->second->send(js.dump());
            return;
        }
    }

    User user = m_userModel.query(to);
    if (user.getState() == "online") {
        m_redis.publish(to, js.dump());
        return;
    }

    // 不在线，存储离线消息
    m_offlineMsgModel.insert(to, js.dump());
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int user_id = js["id"].get<int>();
    int friend_id = js["friendid"].get<int>();

    m_friendModel.insert(user_id, friend_id);
}

// 创建群组
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = js["id"].get<int>();
    std::string name = js["groupname"];
    std::string desc = js["groupdesc"];

    Group group(-1, name, desc);
    if (m_groupModel.createGroup(group)) {
        m_groupModel.addGroup(userid, group.getId(), "creater");
    }
}

// 加入群组
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int user_id = js["id"].get<int>();
    int group_id = js["groupid"].get<int>();
    m_groupModel.addGroup(user_id, group_id, "normal");
}

// 群聊天
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int user_id = js["id"].get<int>();
    int group_id = js["groupid"].get<int>();
    std::vector<int> ids = m_groupModel.queryGroupUsers(user_id, group_id);

    std::lock_guard<std::mutex> lock(m_mutex);
    for (int id: ids) {
        auto it = m_userConn.find(id);
        if (it != m_userConn.end()) {
            it->second->send(js.dump());
        } else {
            // 查询 toid 是否在线
            User user = m_userModel.query(id);
            if (user.getState() == "online") {
                m_redis.publish(id, js.dump());
            } else {
                // 存储离线群消息
                m_offlineMsgModel.insert(id, js.dump());
            }
        }
    }
}

// 从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int userid, const string& msg) {
    std::lock_guard <std::mutex> lock(m_mutex);
    auto it = m_userConn.find(userid);
    if (it != m_userConn.end()) {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    m_offlineMsgModel.insert(userid, msg);
}
//
// {"msgid":1, "id":13, "password":"123456"}   登录消息
// {"msgid":3, "name":"liucxi", "password":"123456"}   注册消息
// {"msgid":5, "id":15, "name":"li si", "to":13, "msg":"hello"}    一对一聊天

// {"msgid":6, "id":15, "friendid":13}    添加好友