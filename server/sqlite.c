#include <string.h>
#include <gtk/gtk.h>
#include "sqlite.h"

void getChatMembers(sqlite3 *conn, FullUserInfo memberList[MAX_NUMBER_OF_USERS], short *memberCount, int chatID) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    //Select all users that are in this chat
    sprintf(query, "SELECT users.id, username, first_name, second_name\n"
                   "FROM users\n"
                   "JOIN chats_to_users ctu on users.id == ctu.user_id\n"
                   "JOIN chats c on c.id == ctu.chat_id\n"
                   "WHERE c.id == '%d'", chatID);
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

void sqlRegister(sqlite3 *conn, FullUserInfo *user) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    //Query 1 - Try to insert new user
    sprintf(query, "INSERT INTO users (username, password, first_name, second_name)"
                   "VALUES ('%s','%s','%s','%s')", user->username, user->password, user->firstName, user->lastName);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_message("sqlRegister(): User '%s' already exists", user->username);
        user->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    //Query 2 - Return ID of this user
    sprintf(query, "SELECT id\n"
                   "FROM users\n"
                   "WHERE username == '%s'", user->username);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_critical("sqlRegister(): User '%s' not found", user->username);
        user->ID = -1;
        return;
    }
    user->ID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}

void sqlAuthorize(sqlite3 *conn, FullUserInfo *user, AuthorizationPackage *package) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    //Query 1 - Check user credentials
    sprintf(query, "SELECT id, username, password, first_name, second_name\n"
                   "FROM users\n"
                   "WHERE username == '%s'", user->username);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_message("sqlAuthorize(): User '%s' not found", user->username);
        package->authorizedUser.ID = -1;
        return;
    }
    if (strcmp((const char *) sqlite3_column_text(stmt, 2), user->password)) {
        g_message("sqlAuthorize(): Received password for user '%s' is incorrect", user->username);
        package->authorizedUser.ID = -2;
        return;
    }

    package->authorizedUser.ID = sqlite3_column_int(stmt, 0);
    strcpy(package->authorizedUser.username, (const char *) sqlite3_column_text(stmt, 1));
    strcpy(package->authorizedUser.password, "***");
    strcpy(package->authorizedUser.firstName, (const char *) sqlite3_column_text(stmt, 3));
    strcpy(package->authorizedUser.lastName, (const char *) sqlite3_column_text(stmt, 4));
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    //Query 2 - extract dialogs
    sprintf(query, "SELECT chats.id, name, is_group\n"
                   "FROM chats\n"
                   "JOIN chats_to_users ctu on chats.id == ctu.chat_id\n"
                   "JOIN users u on ctu.user_id == u.id\n"
                   "WHERE u.id == '%d'", package->authorizedUser.ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    package->dialogCount = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        package->dialogList[package->dialogCount].ID = sqlite3_column_int(stmt, 0);
        strcpy(package->dialogList[package->dialogCount].name, (const char *) sqlite3_column_text(stmt, 1));
        package->dialogList[package->dialogCount].isGroup = (char) sqlite3_column_int(stmt, 2);

        getChatMembers(conn,
                       package->dialogList[package->dialogCount].userList,
                       &(package->dialogList[package->dialogCount].userCount),
                       package->dialogList[package->dialogCount].ID);

        package->dialogCount++;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);



    //Query 3 - extract friends
    sprintf(query, "SELECT id, username, first_name, second_name\n"
                   "FROM users\n"
                   "JOIN friend_list fl on users.id == fl.user_id\n"
                   "WHERE fl.friend_id == '%d'", package->authorizedUser.ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    package->friendCount = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        package->friends[package->friendCount].ID = sqlite3_column_int(stmt, 0);
        strcpy(package->friends[package->friendCount].username, (const char *) sqlite3_column_text(stmt, 1));
        strcpy(package->friends[package->friendCount].password, "***");
        strcpy(package->friends[package->friendCount].firstName, (const char *) sqlite3_column_text(stmt, 2));
        strcpy(package->friends[package->friendCount].lastName, (const char *) sqlite3_column_text(stmt, 3));
        package->friendCount++;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);



    //Query 4 - extract friend requests
    sprintf(query, "SELECT id, username, first_name, second_name\n"
                   "FROM users\n"
                   "JOIN friend_requests fr on users.id == fr.user_id\n"
                   "WHERE fr.requested_id == '%d'", package->authorizedUser.ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    package->requestCount = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        package->requests[package->requestCount].ID = sqlite3_column_int(stmt, 0);
        strcpy(package->requests[package->requestCount].username, (const char *) sqlite3_column_text(stmt, 1));
        strcpy(package->requests[package->requestCount].password, "***");
        strcpy(package->requests[package->requestCount].firstName, (const char *) sqlite3_column_text(stmt, 2));
        strcpy(package->requests[package->requestCount].lastName, (const char *) sqlite3_column_text(stmt, 3));
        package->requestCount++;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}

void sqlCreateDialog(sqlite3 *conn, FullDialogInfo *dialog) {
    int result;
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    //Query 1 - Create new entry in table 'chats'
    sprintf(query, "INSERT INTO chats (name, is_group)\n"
                   "VALUES ('%s', '%d')", dialog->name, dialog->isGroup);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        g_critical("sqlCreateDialog(): Dialog '%s' could not be created", dialog->name);
        dialog->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);


    //Query 2 - Get new dialog's chatID
    sprintf(query, "SELECT id\n"
                   "FROM chats\n"
                   "ORDER BY id DESC\n"
                   "LIMIT 1");
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    result = sqlite3_step(stmt);
    if (result != SQLITE_ROW) {
        g_critical("sqlCreateDialog(): Table 'chats' is empty");
        dialog->ID = -1;
        return;
    }
    dialog->ID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);


    //Query 3 - Create entries in table 'chats_to_users'
    for (int i = 0; i < dialog->userCount; ++i) {
        //Query 1 - Create new entry in table 'chats'
        sprintf(query, "INSERT INTO chats_to_users\n"
                       "VALUES ('%d', '%d')", dialog->ID, dialog->userList[i].ID);
        sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
        result = sqlite3_step(stmt);
        if (result != SQLITE_DONE) {
            g_critical("sqlCreateDialog(): User '%s' could not be added to dialog '%s'",
                       dialog->userList[i].username, dialog->name);
            dialog->ID = -1;
            return;
        }
        sqlite3_finalize(stmt);
        memset(query, 0, QUERY_SIZE);
    }
}

void sqlSendMessage(sqlite3 *conn, FullMessageInfo *message, FullDialogInfo *dialog) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;
    char *timestamp = g_date_time_format(g_date_time_new_now_local(), "%H:%M:%S, %d %b %Y, %a");

    //Query 1 - Find sender's chatID in 'userList' table
    sprintf(query, "SELECT id\n"
                   "FROM users\n"
                   "WHERE username == '%s'", message->username);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_critical("sqlSendMessage(): User '%s' does not exist", message->username);
        message->chatID = -1;
        return;
    }
    int senderID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    //Query 2 - Create new entry in 'messages' table
    strcpy(message->timestamp, timestamp);
    sprintf(query, "INSERT INTO messages (user_id, chat_id, time, text)\n"
                   "VALUES ('%d', '%d', '%s', '%s')", senderID, message->chatID, message->timestamp, message->text);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_critical("sqlSendMessage(): Insertion failed");
        message->chatID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    //Query 3 - Get ID's of chat members
    getChatMembers(conn, dialog->userList, &(dialog->userCount), message->chatID);
}

void sqlSendFriendRequest(sqlite3 *conn, FullUserInfo *user, int *friendID) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    // Query 1 - Get requested user's ID
    sprintf(query, "SELECT id\n"
                   "FROM users\n"
                   "WHERE username == '%s'", user->additionalInfo);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_warning("sqlSendFriendRequest(): User '%s' not found", user->additionalInfo);
        user->ID = -2;
        return;
    }
    *friendID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 2 - Check user existence
    sprintf(query, "SELECT *\n"
                   "FROM friend_requests\n"
                   "WHERE (user_id == '%d' AND requested_id == '%d')\n"
                   "   OR (user_id == '%d' AND requested_id == '%d')",
            user->ID, *friendID, *friendID, user->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_message("sqlSendFriendRequest(): Request between '%d' and '%d' already exists", user->ID, *friendID);
        user->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 3 - Check friendship existence
    sprintf(query, "SELECT *\n"
                   "FROM friend_list\n"
                   "WHERE (user_id == '%d' AND friend_id == '%d')\n"
                   "   OR (user_id == '%d' AND friend_id == '%d')",
            user->ID, *friendID, *friendID, user->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_message("sqlSendFriendRequest(): Friendship between '%d' and '%d' already exists", user->ID, *friendID);
        user->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 4 - Insert user into 'friend_requests' table
    sprintf(query, "INSERT INTO friend_requests\n"
                   "VALUES ('%d', '%d')", user->ID, *friendID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_message("sqlSendFriendRequest(): Request between '%d' and '%d' failed to insert", user->ID, *friendID);
        user->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}

void sqlAcceptFriendRequest(sqlite3 *conn, FullUserInfo *user, FullUserInfo *sender) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    // Query 1 - Get information about sender
    sprintf(query, "SELECT id, username, first_name, second_name\n"
                   "FROM users\n"
                   "WHERE username == '%s'", user->additionalInfo);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_warning("sqlAcceptFriendRequest(): User '%s' not found", user->additionalInfo);
        user->ID = -1;
        return;
    }
    sender->ID = sqlite3_column_int(stmt, 0);
    strcpy(sender->username, (const char *) sqlite3_column_text(stmt, 1));
    strcpy(sender->password, "***");
    strcpy(sender->firstName, (const char *) sqlite3_column_text(stmt, 2));
    strcpy(sender->lastName, (const char *) sqlite3_column_text(stmt, 3));
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 2 - Delete user entry from 'friend_requests' table
    sprintf(query, "DELETE FROM friend_requests\n"
                   "WHERE user_id == '%d' AND requested_id == '%d'", sender->ID, user->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_warning("sqlAcceptFriendRequest(): Request between '%s'(%d) and '%s'(%d) could not be deleted",
                  user->username, user->ID,
                  sender->username, sender->ID);
        user->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 3 - Add new entries to 'friends' table
    sprintf(query, "INSERT INTO friend_list\n"
                   "VALUES ('%d', '%d'),\n"
                   "       ('%d', '%d');", sender->ID, user->ID, user->ID, sender->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_warning("sqlAcceptFriendRequest(): Friendship between '%s'(%d) and '%s'(%d) could not be inserted",
                  user->username, user->ID,
                  sender->username, sender->ID);
        user->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}

void sqlDeclineFriendRequest(sqlite3 *conn, FullUserInfo *user) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    // Query 1 - Get sender's ID
    sprintf(query, "SELECT id, username\n"
                   "FROM users\n"
                   "WHERE username == '%s'", user->additionalInfo);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_warning("sqlAcceptFriendRequest(): User '%s' not found", user->additionalInfo);
        user->ID = -2;
        return;
    }
    FullUserInfo sender;
    sender.ID = sqlite3_column_int(stmt, 0);
    strcpy(sender.username, (const char *) sqlite3_column_text(stmt, 1));
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 2 - Delete user entry from 'friend_requests' table
    sprintf(query, "DELETE FROM friend_requests\n"
                   "WHERE user_id == '%d' AND requested_id == '%d'", sender.ID, user->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_warning("sqlAcceptFriendRequest(): Request between '%s'(%d) and '%s'(%d) could not be deleted",
                  user->username, user->ID,
                  sender.username, sender.ID);
        user->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}

void sqlRemoveFriend(sqlite3 *conn, FullUserInfo *user, int *friendID) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    // Query 1 - Get sender's ID
    sprintf(query, "SELECT id\n"
                   "FROM users\n"
                   "WHERE username == '%s'", user->additionalInfo);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_warning("sqlRemoveFriend: User '%s' not found", user->additionalInfo);
        user->ID = -2;
        return;
    }
    *friendID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 2 - Delete entries from 'friend_list' table
    sprintf(query, "                                                                                DELETE\n"
                   "FROM friend_list\n"
                   "WHERE (user_id == '%d' AND friend_id == '%d')\n"
                   "   OR (user_id == '%d' AND friend_id == '%d')", *friendID,
            user->ID, user->ID, *friendID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_warning("sqlRemoveFriend(): Friendship between '%d' and '%d' could not be deleted", user->ID,
                  *friendID);
        user->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}

void sqlLeaveDialog(sqlite3 *conn, FullUserInfo *leaveRequest, FullDialogInfo *dialog) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    // Query 1 - Leave dialog
    sprintf(query, "DELETE\n"
                   "FROM chats_to_users\n"
                   "WHERE chat_id == '%s'\n"
                   "  AND user_id == '%d'", leaveRequest->additionalInfo, leaveRequest->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_warning("sqlLeaveDialog(): User '%s' could not leave dialog '%d'", leaveRequest->username, leaveRequest->ID);
        leaveRequest->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 2 - Get ID's of chat members
    getChatMembers(conn, dialog->userList, &(dialog->userCount), leaveRequest->ID);
}

void sqlJoinDialog(sqlite3 *conn, FullUserInfo *addRequest, FullDialogInfo *dialog) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    // Query 1 - Get sender's ID
    sprintf(query, "SELECT id\n"
                   "FROM users\n"
                   "WHERE username == '%s'", addRequest->username);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_warning("sqlJoinDialog(): User '%s' not found", addRequest->username);
        addRequest->ID = -1;
        return;
    }
    int addedID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 2 - Get information about a dialog
    dialog->ID = addRequest->ID;
    addRequest->ID = addedID;   // Now addRequest->ID contains ID of added user
    sprintf(query, "SELECT name, is_group\n"
                   "FROM chats\n"
                   "WHERE id == '%d'", dialog->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_warning("sqlJoinDialog(): Dialog '%d' not found", dialog->ID);
        addRequest->ID = -1;
        return;
    }
    strcpy(dialog->name, (const char *) sqlite3_column_text(stmt, 0));
    dialog->isSupposedToOpen = (char) sqlite3_column_int(stmt, 1);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
    getChatMembers(conn, dialog->userList, &(dialog->userCount), dialog->ID);

    // Query 3 - Add addRequest to dialog
    sprintf(query, "INSERT INTO chats_to_users\n"
                   "VALUES ('%d', '%d')", dialog->ID, addedID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_warning("sqlJoinDialog(): Insertion failed");
        addRequest->ID = -1;
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}

void sqlLoadMessages(sqlite3 *conn, FullDialogInfo *dialog, MessagesPackage *package) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    sprintf(query, "SELECT username, first_name, second_name, time, text\n"
                   "FROM messages\n"
                   "JOIN users u on u.id == messages.user_id\n"
                   "WHERE chat_id == '%d'\n"
                   "ORDER BY messages.id ASC\n"
                   "LIMIT 500", dialog->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    package->messagesCount = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        package->messagesList[package->messagesCount].chatID = dialog->ID;
        strcpy(package->messagesList[package->messagesCount].username, (const char *) sqlite3_column_text(stmt, 0));
        strcpy(package->messagesList[package->messagesCount].firstName, (const char *) sqlite3_column_text(stmt, 1));
        strcpy(package->messagesList[package->messagesCount].lastName, (const char *) sqlite3_column_text(stmt, 2));
        strcpy(package->messagesList[package->messagesCount].timestamp, (const char *) sqlite3_column_text(stmt, 3));
        strcpy(package->messagesList[package->messagesCount].text, (const char *) sqlite3_column_text(stmt, 4));
        package->messagesCount++;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}