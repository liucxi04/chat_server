#ifndef GROUPMODEL
#define GROUPMODEL

#include "group.h"
#include <vector>
#include <string>

class GroupModel {
public:
    // 创建群组
    bool createGroup(Group &group);

    // 加入群组
    bool addGroup(int userid, int groupid, std::string role);

    // 查询用户所在群组
    std::vector<Group> queryGroups(int userid);

    // 查询群组用户列表，用于发送群消息
    std::vector<int> queryGroupUsers(int userid, int groupid);

};

#endif