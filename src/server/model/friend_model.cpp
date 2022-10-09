#include "friend_model.h"
#include "db.h"

bool FriendModel::insert(int user_id, int friend_id) {
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, %d)", user_id, friend_id);

    MySQL mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) {
            return true;
        }
    }

    return false;
}

std::vector<User> FriendModel::query(int user_id) {
    char sql[1024] = {0};
    sprintf(sql, "select id, name, state from user inner join friend on friend.friendid = user.id where userid = %d",
            user_id);

    MySQL mysql;
    std::vector<User> friends;
    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                User user(atoi(row[0]), row[1], "", row[2]);
                friends.push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return friends;
}