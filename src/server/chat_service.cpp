#include "chat_service.h"
#include "public.h"

#include <muduo/base/Logging.h>
#include <string>

using namespace muduo;
using namespace std::placeholders;

ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    m_handlers[LOGIN_MSG] = std::bind(&ChatService::login, this, _1, _2, _3);
    m_handlers[REG_MSG] = std::bind(&ChatService::reg, this, _1, _2, _3);
}

MsgHandler ChatService::getHandler(int type)
{
    auto it = m_handlers.find(type);
    if (it == m_handlers.end())
    {
        // 返回一个默认操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
        {
            LOG_ERROR << "msg type: " << type << "can not find handler!";
        };
    }
    else
    {
        return it->second;
    }
}
// 登录
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"];
    std::string password = js["password"];

    User user = m_userModel.query(id);
    if (user.getId() == id && user.getPassword() == password)
    {
        if (user.getState() == "online")
        {
            json response;
            response["msgid"] = MsgType::LOGIN_MSG_ACK;
            response["errno"] = 2;
            conn->send(response.dump());
        }
        else
        {
            // 记录用户连接信息，需要保证互斥
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_userConn[id] = conn;
            }

            // 修改用户登录状态
            user.setState("online");
            m_userModel.updateState(user);

            json response;
            response["msgid"] = MsgType::LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            conn->send(response.dump());
        }
    }
    else
    {
        json response;
        response["msgid"] = MsgType::LOGIN_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}
// 注册
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    std::string name = js["name"];
    std::string password = js["password"];
    User user(-1, name, password);

    bool state = m_userModel.insert(user);
    if (state)
    {
        json response;
        response["msgid"] = MsgType::REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else
    {
        json response;
        response["msgid"] = MsgType::REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto it = m_userConn.begin(); it != m_userConn.end(); ++it)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                m_userConn.erase(it);
                break;
            }
        }
    }
    user.setState("offline");
    m_userModel.updateState(user);
}