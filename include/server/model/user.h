#ifndef USER
#define USER

#include <string>

class User {
public:
    User(int id = -1, std::string name = "", std::string password = "", std::string state = "offline")
            : m_id(id), m_name(name), m_password(password), m_state(state) {}

    void setId(int id) { m_id = id; }

    void setName(std::string name) { m_name = name; }

    void setPassword(std::string password) { m_password = password; }

    void setState(std::string state) { m_state = state; }

    int getId() { return m_id; }

    std::string getName() { return m_name; }

    std::string getPassword() { return m_password; }

    std::string getState() { return m_state; }

private:
    int m_id;
    std::string m_name;
    std::string m_password;
    std::string m_state;
};

#endif