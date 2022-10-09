#ifndef USER
#define USER

#include <string>
#include <utility>

class User {
public:
    explicit User(int id = -1, std::string name = "", std::string password = "", std::string state = "offline")
            : m_id(id), m_name(std::move(name)), m_password(std::move(password)), m_state(std::move(state)) {}

    void setId(int id) { m_id = id; }

    void setName(std::string name) { m_name = std::move(name); }

    void setPassword(std::string password) { m_password = std::move(password); }

    void setState(std::string state) { m_state = std::move(state); }

    int getId() const { return m_id; }

    std::string getName() const { return m_name; }

    std::string getPassword() const { return m_password; }

    std::string getState() const { return m_state; }

private:
    int m_id;
    std::string m_name;
    std::string m_password;
    std::string m_state;
};

#endif