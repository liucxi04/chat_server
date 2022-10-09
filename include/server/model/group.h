#ifndef GROUP
#define GROUP

#include "group_user.h"

#include <string>
#include <utility>
#include <vector>

class Group {
public:
    explicit Group(int id = -1, std::string name = "", std::string desc = "")
            : m_id(id), m_name(std::move(name)), m_desc(std::move(desc)) {}

    void setId(int id) { m_id = id; }

    void setName(std::string name) { m_name = std::move(name); }

    void setDesc(std::string desc) { m_desc = std::move(desc); }

    int getId() const { return m_id; }

    std::string getName() const { return m_name; }

    std::string getDesc() const { return m_desc; }

    std::vector<GroupUser> &getUsers() { return m_users; }

private:
    int m_id;
    std::string m_name;
    std::string m_desc;

    std::vector<GroupUser> m_users;
};

#endif