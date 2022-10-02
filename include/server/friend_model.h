#ifndef FRIENDMODEL
#define FRIENDMODEL

#include "user.h"
#include <vector>

class FriendModel
{
public:
    bool insert(int id, int friendid);

    std::vector<User> query(int id);
};
#endif