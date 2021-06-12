#include <string.h>
#include <gtk/gtk.h>
#include "sqlite.h"

void sqlRegister(sqlite3 *conn, FullUserInfo *user) {
    int result;
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    //Query 1 - Try to insert new user
    sprintf(query, "INSERT INTO users (username, password, first_name, second_name) \n"
                   "VALUES ('%s','%s','%s','%s')", user->username, user->password, user->firstName, user->secondName);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        printf("<LOG> sqlRegister(): User '%s' already exists\n", user->username);
        user->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    //Query 2 - Return ID of this user
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

    //Select all users that are in this chat
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
        strcpy(memberList[*memberCount].secondName, (const char *) sqlite3_column_text(stmt, 3));
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
    strcpy(auPackage->authorizedUser.username, (const char *) sqlite3_column_text(stmt, 1));
    strcpy(auPackage->authorizedUser.password, "***");
    strcpy(auPackage->authorizedUser.firstName, (const char *) sqlite3_column_text(stmt, 3));
    strcpy(auPackage->authorizedUser.secondName, (const char *) sqlite3_column_text(stmt, 4));
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
        strcpy(auPackage->dialogList[auPackage->dialogCount].dialogName, (const char *) sqlite3_column_text(stmt, 1));
        auPackage->dialogList[auPackage->dialogCount].isGroup = sqlite3_column_int(stmt, 2);

        getChatMembers(conn,
                       auPackage->dialogList[auPackage->dialogCount].users,
                       &(auPackage->dialogList[auPackage->dialogCount].usersNumber),
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
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auPackage->friends[auPackage->friendCount].ID = sqlite3_column_int(stmt, 0);
        strcpy(auPackage->friends[auPackage->friendCount].username, (const char *) sqlite3_column_text(stmt, 1));
        strcpy(auPackage->friends[auPackage->friendCount].password, "***");
        strcpy(auPackage->friends[auPackage->friendCount].firstName, (const char *) sqlite3_column_text(stmt, 2));
        strcpy(auPackage->friends[auPackage->friendCount].secondName, (const char *) sqlite3_column_text(stmt, 3));
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
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auPackage->requests[auPackage->requestCount].ID = sqlite3_column_int(stmt, 0);
        strcpy(auPackage->requests[auPackage->requestCount].username, (const char *) sqlite3_column_text(stmt, 1));
        strcpy(auPackage->requests[auPackage->requestCount].password, "***");
        strcpy(auPackage->requests[auPackage->requestCount].firstName, (const char *) sqlite3_column_text(stmt, 2));
        strcpy(auPackage->requests[auPackage->requestCount].secondName, (const char *) sqlite3_column_text(stmt, 3));
        auPackage->requestCount++;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}

void sqlCreateDialog(sqlite3 *conn, FullDialogInfo *dialogInfo) {
    int result;
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;


    //Query 1 - Create new entry in table 'chats'
    sprintf(query, "INSERT INTO chats (name, is_group) VALUES ('%s', %d)", dialogInfo->dialogName, dialogInfo->isGroup);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        printf("<ERROR> sqlCreateDialog(): Dialog '%s' could not be created\n", dialogInfo->dialogName);
        dialogInfo->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);


    //Query 2 - Get new dialog's ID
    sprintf(query, "SELECT id\n"
                   "FROM chats\n"
                   "ORDER BY id DESC\n"
                   "LIMIT 1");
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    result = sqlite3_step(stmt);
    if (result != SQLITE_ROW) {
        printf("<ERROR> sqlCreateDialog(): Table 'chats' is empty\n");
        dialogInfo->ID = -1;
        return;
    }
    dialogInfo->ID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);


    //Query 3 - Create entries in table 'chats_to_users'
    for (int i = 0; i < dialogInfo->usersNumber; ++i) {
        //Query 1 - Create new entry in table 'chats'
        sprintf(query, "INSERT INTO chats_to_users\n"
                       "VALUES (%d, %d)", dialogInfo->ID, dialogInfo->users[i].ID);
        sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
        result = sqlite3_step(stmt);
        if (result != SQLITE_DONE) {
            printf("<ERROR> sqlCreateDialog(): User '%s' could not be added to dialog '%s'\n",
                   dialogInfo->users[i].username, dialogInfo->dialogName);
            dialogInfo->ID = -1;
            return;
        }
        sqlite3_finalize(stmt);
        memset(query, 0, QUERY_SIZE);
    }
}

void sqlSendMessage(sqlite3 *conn, FullMessageInfo *message, int sendbackList[30], int *sendbackCount) {
    int result;
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;
    char *timestamp = g_date_time_format(g_date_time_new_now_local(), "%H:%M:%S, %d %b %Y, %a");


    //Query 1 - Find sender's ID in 'users' table
    int senderID;
    sprintf(query, "SELECT id\n"
                   "FROM users\n"
                   "WHERE username = '%s'", message->login);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    result = sqlite3_step(stmt);
    if (result != SQLITE_ROW) {
        printf("<ERROR> sqlSendMessage(): User '%s' does not exist\n", message->login);
        message->ID = -1;
        return;
    }
    senderID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);



    //Query 2 - Create new entry in 'messages' table
    strcpy(message->date, timestamp);
    sprintf(query, "INSERT INTO messages (user_id, chat_id, time, text)\n"
                   "VALUES (%d, %d, '%s', '%s')", senderID, message->ID, message->date, message->text);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        printf("<ERROR> sqlSendMessage(): Message insertion failed\n");
        message->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);



    //Query 3 - extract sendback ID's
    sprintf(query, "SELECT user_id\n"
                   "FROM chats_to_users\n"
                   "WHERE chat_id = %d", message->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    *sendbackCount = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
        sendbackList[(*sendbackCount)++] = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}


