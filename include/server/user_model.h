#ifndef USERMODEL
#define USERMODEL

#include "user.h"

class UserModel
{
public:
    bool insert(User &user);

    User query(int id);

    bool updateState(User user);

    bool resetState();
};

#endif