#ifndef CHATPROJECT_SQLITE_H
#define CHATPROJECT_SQLITE_H

#include <sqlite3.h>
#include "../shared/definitions.h"

#define QUERY_SIZE 200

void sqlRegister(sqlite3 *conn, FullUserInfo *user);

void sqlAuthorize(sqlite3 *conn, FullUserInfo *user, AuthorizationPackage *package);

void sqlCreateDialog(sqlite3 *conn, FullDialogInfo *dialog);

void sqlSendMessage(sqlite3 *conn, FullMessageInfo *message, int sendbackList[30], int *sendbackCount);

void sqlSendFriendRequest(sqlite3 *conn, FullUserInfo *user, int *friendID);

void sqlAcceptFriendRequest(sqlite3 *conn, FullUserInfo *user, FullUserInfo *sender);

void sqlDeclineFriendRequest(sqlite3 *conn, FullUserInfo *user);

void sqlRemoveFriend(sqlite3 *conn, FullUserInfo *user, int *friendID);

void sqlLeaveDialog(sqlite3 *conn, FullUserInfo *user);

void sqlLoadMessages(sqlite3 *conn, FullDialogInfo *dialog, MessagesPackage *package);

#endif //CHATPROJECT_SQLITE_H
