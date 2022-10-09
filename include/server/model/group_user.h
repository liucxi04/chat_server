#ifndef GROUP_USER
#define GROUP_USER

#include "user.h"

#include <utility>

class GroupUser : public User {
public:
    void setRole(std::string role) { m_role = std::move(role); }

    std::string getRole() const { return m_role; }

private:
    std::string m_role;
};

#endif