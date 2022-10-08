#ifndef OFFLINEMESSAGEMODEL
#define OFFLINEMESSAGEMODEL

#include <string>
#include <vector>

class OfflineMsgModel {
public:
    bool insert(int userid, std::string msg);

    bool remove(int userid);

    std::vector<std::string> query(int userid);
};

#endif