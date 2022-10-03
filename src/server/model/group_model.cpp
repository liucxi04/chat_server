#include "group_model.h"
#include "db.h"

// 创建群组
bool GroupModel::createGroup(Group &group)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname, groupdesc) values('%s', '%s')",
            group.getName().c_str(), group.getDesc().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }

    return false;
}

// 加入群组
bool GroupModel::addGroup(int userid, int groupid, std::string role)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values(%d, %d, '%s')", groupid, userid, role.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }

    return false;
}

// 查询用户所在群组
std::vector<Group> GroupModel::queryGroups(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.groupname, a.groupdesc from allgroup a inner join \
        groupuser b on a.id = b.groupid where b.userid = %d",
            userid);

    MySQL mysql;
    std::vector<Group> groups;

    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group(atoi(row[0]), row[1], row[2]);
                groups.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    for (Group &group : groups)
    {
        sprintf(sql, "select a.id, a.name, a.state, b.grouprole from user a inner join \
        groupuser b on a.id = b.userid where b.groupid = %d",
                group.getId());

        if (mysql.connect())
        {
            MYSQL_RES *res = mysql.query(sql);
            if (res != nullptr)
            {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(res)) != nullptr)
                {
                    GroupUser user;
                    user.setId(atoi(row[0]));
                    user.setName(row[1]);
                    user.setState(row[2]);
                    user.setRole(row[3]);
                    group.getUsers().push_back(user);
                }
                mysql_free_result(res);
            }
        }
    }

    return groups;
}

// 查询群组用户列表，用于发送群消息
std::vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d", groupid, userid);

    MySQL mysql;
    std::vector<int> ids;

    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                ids.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }

    return ids;
}