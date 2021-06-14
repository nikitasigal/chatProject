#ifndef CHATPROJECT_SQLITE_H
#define CHATPROJECT_SQLITE_H

#include <sqlite3.h>
#include "../shared/definitions.h"

#define QUERY_SIZE 1500

void sqlGetFriendsList(sqlite3 *conn, int *friendList, short *friendCount, FullUserInfo *user);

void sqlRegister(sqlite3 *conn, FullUserInfo *user);

void sqlAuthorize(sqlite3 *conn, FullUserInfo *user, AuthorizationPackage *package);

void sqlCreateDialog(sqlite3 *conn, FullDialogInfo *dialog);

void sqlSendMessage(sqlite3 *conn, FullMessageInfo *message, FullDialogInfo *dialog);

void sqlSendFriendRequest(sqlite3 *conn, FullUserInfo *sender, int *recipientID);

void sqlAcceptFriendRequest(sqlite3 *conn, FullUserInfo *recipient, FullUserInfo *sender);

void sqlDeclineFriendRequest(sqlite3 *conn, FullUserInfo *recipient);

void sqlRemoveFriend(sqlite3 *conn, FullUserInfo *user, int *friendID);

void sqlLeaveDialog(sqlite3 *conn, FullUserInfo *leaveRequest, FullDialogInfo *dialog);

void sqlJoinDialog(sqlite3 *conn, FullUserInfo *joinRequest, FullDialogInfo *dialog);

void sqlLoadMessages(sqlite3 *conn, FullDialogInfo *dialog, MessagesPackage *package);

#endif //CHATPROJECT_SQLITE_H