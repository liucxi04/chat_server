#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

#include <iostream>
#include <string>
#include <functional>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

class ChatServer
{
public:
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const string &nameArg)
        : m_server(loop, listenAddr, nameArg), m_loop(loop)
    {
        m_server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

        m_server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

        m_server.setThreadNum(4);
    }

    void start()
    {
        m_server.start();
    }

private:
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " on line " << endl;
        }
        else
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << "off line " << endl;
            conn->shutdown();
        }
    }

    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buffer,
                   Timestamp time)
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data: " << buf << ", time: " << time.toString();
        conn->send(buf);
    }

private:
    TcpServer m_server;
    EventLoop *m_loop;
};

int main()
{
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}

// g++ -o server server.cpp -lmuduo_net -lmuduo_base -lpthread