#ifndef GROUP
#define GROUP

#include "group_user.h"

#include <string>
#include <vector>

class Group {
public:
    Group(int id = -1, std::string name = "", std::string desc = "")
            : m_id(id), m_name(name), m_desc(desc) {}

    void setId(int id) { m_id = id; }

    void setName(std::string name) { m_name = name; }

    void setDesc(std::string desc) { m_desc = desc; }

    int getId() { return m_id; }

    std::string getName() { return m_name; }

    std::string getDesc() { return m_desc; }

    std::vector<GroupUser> &getUsers() { return m_users; }

private:
    int m_id;
    std::string m_name;
    std::string m_desc;

    std::vector<GroupUser> m_users;
};

#endif