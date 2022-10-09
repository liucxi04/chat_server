#ifndef FRIEND_MODEL
#define FRIEND_MODEL

#include "user.h"
#include <vector>

class FriendModel {
public:
    bool insert(int user_id, int friend_id);

    std::vector<User> query(int user_id);
};

#endif