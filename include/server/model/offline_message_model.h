#ifndef OFFLINE_MESSAGE_MODEL
#define OFFLINE_MESSAGE_MODEL

#include <string>
#include <vector>

class OfflineMsgModel {
public:
    bool insert(int user_id, std::string msg);

    bool remove(int user_id);

    std::vector<std::string> query(int user_id);
};

#endif