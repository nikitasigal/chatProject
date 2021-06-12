#ifndef CHATPROJECT_SQLITE_H
#define CHATPROJECT_SQLITE_H

#include <sqlite3.h>
#include "../shared/definitions.h"

#define QUERY_SIZE 200

void sqlRegister(sqlite3 *conn, FullUserInfo *user);

void sqlAuthorize(sqlite3 *conn, FullUserInfo *user, AuthorizationPackage *auPackage);

void sqlCreateDialog(sqlite3 *conn, FullDialogInfo *dialogInfo);

void sqlSendMessage(sqlite3 *conn, FullMessageInfo *message, int sendbackList[30], int *sendbackCount);

#endif //CHATPROJECT_SQLITE_H
