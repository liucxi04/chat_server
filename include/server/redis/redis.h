#ifndef REDIS
#define REDIS

#include <hiredis/hiredis.h>
#include <thread>
#include <functional>

/*
redis作为集群服务器通信的基于发布-订阅消息队列时，会遇到两个难搞的bug问题，参考我的博客详细描述：
https://blog.csdn.net/QIANGWEIYUAN/article/details/97895611
*/
class Redis {
public:
    Redis();

    ~Redis();

    // 连接redis服务器 
    bool connect();

    // 向redis指定的通道channel发布消息
    bool publish(int channel, std::string message);

    // 向redis指定的通道subscribe订阅消息
    bool subscribe(int channel);

    // 向redis指定的通道unsubscribe取消订阅消息
    bool unsubscribe(int channel);

    // 在独立线程中接收订阅通道中的消息
    void observerChannelMessage();

    // 初始化向业务层上报通道消息的回调对象
    void initNotifyHandler(std::function<void(int, std::string)> fn);

private:
    // hiredis同步上下文对象，负责publish消息
    redisContext *m_publish_context;

    // hiredis同步上下文对象，负责subscribe消息
    redisContext *m_subscribe_context;

    // 回调操作，收到订阅的消息，给service层上报
    std::function<void(int, std::string)> m_notify_message_handler;
};

#endif
