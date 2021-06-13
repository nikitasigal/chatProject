#include <string.h>
#include "sqlite.h"

void sqlRegister(sqlite3 *conn, FullUserInfo *user) {
    int result;
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    //Query 1 - Try to insert new user
    sprintf(query, "INSERT INTO users (username, password, first_name, second_name) \n"
                   "VALUES ('%s','%s','%s','%s')", user->username, user->password, user->firstName, user->lastName);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        printf("<LOG> sqlRegister(): User '%s' already exists\n", user->username);
        user->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    //Query 2 - Return chatID of this user
    sprintf(query, "SELECT id\n"
                   "FROM users\n"
                   "WHERE username = '%s'", user->username);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    result = sqlite3_step(stmt);
    if (result != SQLITE_ROW) {
        printf("<ERROR> sqlRegister(): User '%s' not found\n", user->username);
        user->ID = -1;
        return;
    }
    user->ID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}

void getChatMembers(sqlite3 *conn, FullUserInfo memberList[30], short *memberCount, int chatID) {
    int result;
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    //Select all userList that are in this chat
    sprintf(query, "SELECT users.id, username, first_name, second_name\n"
                   "FROM users\n"
                   "JOIN chats_to_users ctu on users.id = ctu.user_id\n"
                   "JOIN chats c on c.id = ctu.chat_id\n"
                   "WHERE c.id = %d", chatID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    *memberCount = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        memberList[*memberCount].ID = sqlite3_column_int(stmt, 0);
        strcpy(memberList[*memberCount].username, (const char *) sqlite3_column_text(stmt, 1));
        strcpy(memberList[*memberCount].password, "***");
        strcpy(memberList[*memberCount].firstName, (const char *) sqlite3_column_text(stmt, 2));
        strcpy(memberList[*memberCount].lastName, (const char *) sqlite3_column_text(stmt, 3));
        (*memberCount)++;
    }
    sqlite3_finalize(stmt);
}

void sqlAuthorize(sqlite3 *conn, FullUserInfo *user, AuthorizationPackage *auPackage) {
    int result;
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    //Query 1 - Check user credentials
    sprintf(query, "SELECT id, username, password, first_name, second_name\n"
                   "FROM users\n"
                   "WHERE username = '%s'", user->username);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    result = sqlite3_step(stmt);
    if (result != SQLITE_ROW) {
        printf("<LOG> sqlAuthorize(): User '%s' not found\n", user->username);
        auPackage->authorizedUser.ID = -1;
        return;
    }
    if (strcmp((const char *) sqlite3_column_text(stmt, 2), user->password)) {
        printf("<LOG> sqlAuthorize(): Received password for user '%s' is incorrect\n", user->username);
        auPackage->authorizedUser.ID = -2;
        return;
    }

    auPackage->authorizedUser.ID = sqlite3_column_int(stmt, 0);
    strcpy(auPackage->authorizedUser.username, (const char*) sqlite3_column_text(stmt, 1));
    strcpy(auPackage->authorizedUser.password, "***");
    strcpy(auPackage->authorizedUser.firstName, (const char*) sqlite3_column_text(stmt, 3));
    strcpy(auPackage->authorizedUser.lastName, (const char*) sqlite3_column_text(stmt, 4));
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);



    //Query 2 - extract dialogs
    sprintf(query, "SELECT chats.id, chats.name, chats.is_group\n"
                   "FROM chats\n"
                   "JOIN chats_to_users ctu on chats.id = ctu.chat_id\n"
                   "JOIN users u on ctu.user_id = u.id\n"
                   "WHERE u.id = %d", auPackage->authorizedUser.ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    auPackage->dialogCount = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auPackage->dialogList[auPackage->dialogCount].ID = sqlite3_column_int(stmt, 0);
        strcpy(auPackage->dialogList[auPackage->dialogCount].name, (const char *) sqlite3_column_text(stmt, 1));
        auPackage->dialogList[auPackage->dialogCount].isGroup = sqlite3_column_int(stmt, 2);

        getChatMembers(conn,
                       auPackage->dialogList[auPackage->dialogCount].userList,
                       &(auPackage->dialogList[auPackage->dialogCount].usersCount),
                       auPackage->dialogList[auPackage->dialogCount].ID);

        auPackage->dialogCount++;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);



    //Query 3 - extract friends
    sprintf(query, "SELECT id, username, first_name, second_name\n"
                   "FROM users\n"
                   "JOIN friend_list fl on users.id = fl.user_id\n"
                   "WHERE fl.friend_id = %d", auPackage->authorizedUser.ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    auPackage->friendCount = 0;
    while(sqlite3_step(stmt)==SQLITE_ROW){
        auPackage->friends[auPackage->friendCount].ID = sqlite3_column_int(stmt, 0);
        strcpy(auPackage->friends[auPackage->friendCount].username, (const char*) sqlite3_column_text(stmt, 1));
        strcpy(auPackage->friends[auPackage->friendCount].password, "***");
        strcpy(auPackage->friends[auPackage->friendCount].firstName, (const char*) sqlite3_column_text(stmt, 2));
        strcpy(auPackage->friends[auPackage->friendCount].lastName, (const char*) sqlite3_column_text(stmt, 3));
        auPackage->friendCount++;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);



    //Query 4 - extract friend requests
    sprintf(query, "SELECT id, username, first_name, second_name\n"
                   "FROM users\n"
                   "JOIN friend_requests fr on users.id = fr.user_id\n"
                   "WHERE fr.requested_id = %d", auPackage->authorizedUser.ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    auPackage->requestCount = 0;
    while(sqlite3_step(stmt)==SQLITE_ROW){
        auPackage->requests[auPackage->requestCount].ID = sqlite3_column_int(stmt, 0);
        strcpy(auPackage->requests[auPackage->requestCount].username, (const char*) sqlite3_column_text(stmt, 1));
        strcpy(auPackage->requests[auPackage->requestCount].password, "***");
        strcpy(auPackage->requests[auPackage->requestCount].firstName, (const char*) sqlite3_column_text(stmt, 2));
        strcpy(auPackage->requests[auPackage->requestCount].lastName, (const char*) sqlite3_column_text(stmt, 3));
        auPackage->requestCount++;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}


