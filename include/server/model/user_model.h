#ifndef USER_MODEL
#define USER_MODEL

#include "user.h"

class UserModel {
public:
    bool insert(User &user);

    User query(int id);

    bool updateState(const User& user);

    bool resetState();
};

#endif