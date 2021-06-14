#include <string.h>
#include <gtk/gtk.h>
#include "sqlite.h"

void sqlGetChatMembers(sqlite3 *conn, FullUserInfo *memberList, short *memberCount, int chatID) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    //Select all users that are in this chat
    sprintf(query, "SELECT u.id, username, first_name, second_name\n"
                   "FROM users u\n"
                   "JOIN chats_to_users ctu on u.id = ctu.user_id\n"
                   "JOIN chats c on c.id = ctu.chat_id\n"
                   "WHERE c.id = '%d'", chatID);
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

void sqlGetFriendsList(sqlite3 *conn, int *friendList, short *friendCount, FullUserInfo *user) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    // Query 1 - Get user's credentials
    sprintf(query, "SELECT username, first_name, second_name\n"
                   "FROM users\n"
                   "WHERE id = '%d'", user->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_message("sqlGetFriendsList(): User '%d' not found", user->ID);
        user->ID = -1;
        sqlite3_finalize(stmt);
        return;
    }
    strcpy(user->username, (const char *) sqlite3_column_text(stmt, 0));
    strcpy(user->firstName, (const char *) sqlite3_column_text(stmt, 1));
    strcpy(user->lastName, (const char *) sqlite3_column_text(stmt, 2));
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 2 - Get user's friend list
    sprintf(query, "SELECT u.id\n"
                   "FROM users u\n"
                   "JOIN friend_list fl on u.id = fl.user_id\n"
                   "WHERE fl.friend_id = '%d'", user->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    *friendCount = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        friendList[(*friendCount)++] = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
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
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    //Query 2 - Return ID of this user
    sprintf(query, "SELECT id\n"
                   "FROM users\n"
                   "WHERE username = '%s'", user->username);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_critical("sqlRegister(): User '%s' not found", user->username);
        user->ID = -1;
        sqlite3_finalize(stmt);
        return;
    }
    user->ID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}

void sqlAuthorize(sqlite3 *conn, FullUserInfo *user, AuthorizationPackage *package) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    // Query 1 - Check user credentials
    sprintf(query, "SELECT id, password, first_name, second_name\n"
                   "FROM users\n"
                   "WHERE username = '%s'", user->username);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_message("sqlAuthorize(): User '%s' not found", user->username);
        package->authorizedUser.ID = -1;
        sqlite3_finalize(stmt);
        return;
    }
    if (strcmp((const char *) sqlite3_column_text(stmt, 1), user->password) != 0) {
        g_message("sqlAuthorize(): Received password for user '%s' is incorrect", user->username);
        package->authorizedUser.ID = -2;
        sqlite3_finalize(stmt);
        return;
    }
    package->authorizedUser.ID = sqlite3_column_int(stmt, 0);
    strcpy(package->authorizedUser.username, user->username);
    strcpy(package->authorizedUser.password, "***");
    strcpy(package->authorizedUser.firstName, (const char *) sqlite3_column_text(stmt, 2));
    strcpy(package->authorizedUser.lastName, (const char *) sqlite3_column_text(stmt, 3));
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    //Query 2 - extract dialogs
    sprintf(query, "SELECT c.id, name, is_group\n"
                   "FROM chats c\n"
                   "JOIN chats_to_users ctu on c.id = ctu.chat_id\n"
                   "JOIN users u on ctu.user_id = u.id\n"
                   "WHERE u.id = '%d'", package->authorizedUser.ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    package->dialogCount = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        package->dialogList[package->dialogCount].ID = sqlite3_column_int(stmt, 0);
        strcpy(package->dialogList[package->dialogCount].name, (const char *) sqlite3_column_text(stmt, 1));
        package->dialogList[package->dialogCount].isGroup = (char) sqlite3_column_int(stmt, 2);

        sqlGetChatMembers(conn,
                          package->dialogList[package->dialogCount].userList,
                          &(package->dialogList[package->dialogCount].userCount),
                          package->dialogList[package->dialogCount].ID);

        package->dialogCount++;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    //Query 3 - extract friendList
    sprintf(query, "SELECT id, username, first_name, second_name\n"
                   "FROM users u\n"
                   "JOIN friend_list fl on u.id == fl.user_id\n"
                   "WHERE fl.friend_id == '%d'", package->authorizedUser.ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    package->friendCount = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        package->friendList[package->friendCount].ID = sqlite3_column_int(stmt, 0);
        strcpy(package->friendList[package->friendCount].username, (const char *) sqlite3_column_text(stmt, 1));
        strcpy(package->friendList[package->friendCount].password, "***");
        strcpy(package->friendList[package->friendCount].firstName, (const char *) sqlite3_column_text(stmt, 2));
        strcpy(package->friendList[package->friendCount].lastName, (const char *) sqlite3_column_text(stmt, 3));
        package->friendCount++;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    //Query 4 - extract friend friendRequestList
    sprintf(query, "SELECT id, username, first_name, second_name\n"
                   "FROM users u\n"
                   "JOIN friend_requests fr on u.id == fr.user_id\n"
                   "WHERE fr.requested_id == '%d'", package->authorizedUser.ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    package->friendRequestCount = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        package->friendRequestList[package->friendRequestCount].ID = sqlite3_column_int(stmt, 0);
        strcpy(package->friendRequestList[package->friendRequestCount].username,
               (const char *) sqlite3_column_text(stmt, 1));
        strcpy(package->friendRequestList[package->friendRequestCount].password, "***");
        strcpy(package->friendRequestList[package->friendRequestCount].firstName,
               (const char *) sqlite3_column_text(stmt, 2));
        strcpy(package->friendRequestList[package->friendRequestCount].lastName,
               (const char *) sqlite3_column_text(stmt, 3));
        package->friendRequestCount++;
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
        sqlite3_finalize(stmt);
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
        sqlite3_finalize(stmt);
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
            sqlite3_finalize(stmt);
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
                   "WHERE username = '%s'", message->username);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_critical("sqlSendMessage(): User '%s' does not exist", message->username);
        message->chatID = -1;
        sqlite3_finalize(stmt);
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
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    //Query 3 - Get ID's of chat members
    sqlGetChatMembers(conn, dialog->userList, &(dialog->userCount), message->chatID);
}

void sqlSendFriendRequest(sqlite3 *conn, FullUserInfo *sender, int *recipientID) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    // Query 1 - Get requested sender's ID
    sprintf(query, "SELECT id\n"
                   "FROM users\n"
                   "WHERE username = '%s'", sender->additionalInfo);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_warning("sqlSendFriendRequest(): User '%s' not found", sender->additionalInfo);
        sender->ID = -2;
        sqlite3_finalize(stmt);
        return;
    }
    *recipientID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 2 - Check sender existence
    sprintf(query, "SELECT *\n"
                   "FROM friend_requests\n"
                   "WHERE (user_id = '%d' AND requested_id = '%d')\n"
                   "   OR (user_id = '%d' AND requested_id = '%d')",
            sender->ID, *recipientID, *recipientID, sender->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_message("sqlSendFriendRequest(): Request between '%d' and '%d' already exists", sender->ID, *recipientID);
        sender->ID = -1;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 3 - Check friendship existence
    sprintf(query, "SELECT *\n"
                   "FROM friend_list\n"
                   "WHERE (user_id = '%d' AND friend_id = '%d')\n"
                   "   OR (user_id = '%d' AND friend_id = '%d')",
            sender->ID, *recipientID, *recipientID, sender->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_message("sqlSendFriendRequest(): Friendship between '%d' and '%d' already exists", sender->ID, *recipientID);
        sender->ID = -1;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 4 - Insert sender into 'friend_requests' table
    sprintf(query, "INSERT INTO friend_requests\n"
                   "VALUES ('%d', '%d')", sender->ID, *recipientID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_message("sqlSendFriendRequest(): Request between '%d' and '%d' failed to insert", sender->ID, *recipientID);
        sender->ID = -1;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}

void sqlAcceptFriendRequest(sqlite3 *conn, FullUserInfo *recipient, FullUserInfo *sender) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    // Query 1 - Get information about sender
    sprintf(query, "SELECT id, username, first_name, second_name\n"
                   "FROM users\n"
                   "WHERE username = '%s'", recipient->additionalInfo);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_warning("sqlAcceptFriendRequest(): User '%s' not found", recipient->additionalInfo);
        recipient->ID = -1;
        sqlite3_finalize(stmt);
        return;
    }
    sender->ID = sqlite3_column_int(stmt, 0);
    strcpy(sender->username, (const char *) sqlite3_column_text(stmt, 1));
    strcpy(sender->password, "***");
    strcpy(sender->firstName, (const char *) sqlite3_column_text(stmt, 2));
    strcpy(sender->lastName, (const char *) sqlite3_column_text(stmt, 3));
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 2 - Delete recipient entry from 'friend_requests' table
    sprintf(query, "DELETE FROM friend_requests\n"
                   "WHERE user_id = '%d' AND requested_id = '%d'", sender->ID, recipient->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_warning("sqlAcceptFriendRequest(): Request from '%s'(%d) to '%s'(%d) could not be deleted",
                  sender->username, sender->ID,
                  recipient->username, recipient->ID);
        recipient->ID = -1;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 3 - Add new entries to 'friendList' table
    sprintf(query, "INSERT INTO friend_list\n"
                   "VALUES ('%d', '%d'),\n"
                   "       ('%d', '%d');", sender->ID, recipient->ID, recipient->ID, sender->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_warning("sqlAcceptFriendRequest(): Friendship between '%s'(%d) and '%s'(%d) could not be inserted",
                  recipient->username, recipient->ID,
                  sender->username, sender->ID);
        recipient->ID = -1;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}

void sqlDeclineFriendRequest(sqlite3 *conn, FullUserInfo *recipient) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    // Query 1 - Get sender's ID
    sprintf(query, "SELECT id, username\n"
                   "FROM users\n"
                   "WHERE username = '%s'", recipient->additionalInfo);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_warning("sqlAcceptFriendRequest(): User '%s' not found", recipient->additionalInfo);
        recipient->ID = -2;
        sqlite3_finalize(stmt);
        return;
    }
    FullUserInfo sender;
    sender.ID = sqlite3_column_int(stmt, 0);
    strcpy(sender.username, (const char *) sqlite3_column_text(stmt, 1));
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 2 - Delete recipient entry from 'friend_requests' table
    sprintf(query, "DELETE FROM friend_requests\n"
                   "WHERE user_id = '%d' AND requested_id == '%d'", sender.ID, recipient->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_warning("sqlAcceptFriendRequest(): Request between '%s'(%d) and '%s'(%d) could not be deleted",
                  recipient->username, recipient->ID,
                  sender.username, sender.ID);
        recipient->ID = -1;
        sqlite3_finalize(stmt);
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
                   "WHERE username = '%s'", user->additionalInfo);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_warning("sqlRemoveFriend: User '%s' not found", user->additionalInfo);
        user->ID = -2;
        sqlite3_finalize(stmt);
        return;
    }
    *friendID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 2 - Delete entries from 'friend_list' table
    sprintf(query, "DELETE\n"
                   "FROM friend_list\n"
                   "WHERE (user_id = '%d' AND friend_id = '%d')\n"
                   "   OR (user_id = '%d' AND friend_id = '%d')", *friendID,
            user->ID, user->ID, *friendID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_warning("sqlRemoveFriend(): Friendship between '%d' and '%d' could not be deleted", user->ID,
                  *friendID);
        user->ID = -1;
        sqlite3_finalize(stmt);
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
                   "WHERE chat_id = '%s'\n"
                   "  AND user_id = '%d'", leaveRequest->additionalInfo, leaveRequest->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_warning("sqlLeaveDialog(): User '%s' could not leave dialog '%d'", leaveRequest->username, leaveRequest->ID);
        leaveRequest->ID = -1;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 2 - Get ID's of chat members
    int chatID = strtol(leaveRequest->additionalInfo, NULL, 10);
    sqlGetChatMembers(conn, dialog->userList, &(dialog->userCount), chatID);
}

void sqlJoinDialog(sqlite3 *conn, FullUserInfo *joinRequest, FullDialogInfo *dialog) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    // Query 1 - Get sender's ID
    sprintf(query, "SELECT id\n"
                   "FROM users\n"
                   "WHERE username = '%s'", joinRequest->username);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_warning("sqlJoinDialog(): User '%s' not found", joinRequest->username);
        joinRequest->ID = -1;
        sqlite3_finalize(stmt);
        return;
    }
    int addedID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);

    // Query 2 - Get information about a dialog
    dialog->ID = joinRequest->ID;
    joinRequest->ID = addedID;   // Now joinRequest->ID contains ID of added user
    sprintf(query, "SELECT name, is_group\n"
                   "FROM chats\n"
                   "WHERE id = '%d'", dialog->ID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        g_warning("sqlJoinDialog(): Dialog '%d' not found", dialog->ID);
        joinRequest->ID = -1;
        sqlite3_finalize(stmt);
        return;
    }
    strcpy(dialog->name, (const char *) sqlite3_column_text(stmt, 0));
    dialog->isSupposedToOpen = (char) sqlite3_column_int(stmt, 1);
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
    sqlGetChatMembers(conn, dialog->userList, &(dialog->userCount), dialog->ID);

    // Query 3 - Add joinRequest to dialog
    sprintf(query, "INSERT INTO chats_to_users\n"
                   "VALUES ('%d', '%d')", dialog->ID, addedID);
    sqlite3_prepare_v2(conn, query, (int) strlen(query), &stmt, NULL);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_warning("sqlJoinDialog(): Insertion failed");
        joinRequest->ID = -1;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    memset(query, 0, QUERY_SIZE);
}

void sqlLoadMessages(sqlite3 *conn, FullDialogInfo *dialog, MessagesPackage *package) {
    char query[QUERY_SIZE] = {0};
    sqlite3_stmt *stmt;

    sprintf(query, "SELECT username, first_name, second_name, time, text\n"
                   "FROM messages m\n"
                   "JOIN users u on u.id == m.user_id\n"
                   "WHERE chat_id == '%d'\n"
                   "ORDER BY m.id\n"
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