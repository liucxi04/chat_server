#include "offline_message_model.h"

#include "db.h"

bool OfflineMsgModel::insert(int user_id, std::string msg) {
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values(%d, '%s')", user_id, msg.c_str());

    MySQL mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) {
            return true;
        }
    }

    return false;
}

bool OfflineMsgModel::remove(int user_id) {
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid = %d", user_id);

    MySQL mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) {
            return true;
        }
    }

    return false;
}

std::vector<std::string> OfflineMsgModel::query(int user_id) {
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d", user_id);

    MySQL mysql;
    std::vector<std::string> messages;
    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                messages.emplace_back(row[0]);
            }
            mysql_free_result(res);
        }
    }
    return messages;
}