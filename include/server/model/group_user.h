#ifndef GROUPUSER
#define GROUPUSER

#include "user.h"

class GroupUser : public User
{
public:
    void setRole(std::string role) { m_role = role; }

    std::string getRole() { return m_role; }

private:
    std::string m_role;
};

#endif