#ifndef CHATPROJECT_REQUESTHANDLER_H
#define CHATPROJECT_REQUESTHANDLER_H

#include "../../shared/definitions.h"

gboolean serverRequest_Registration(void **specialAdditionalServerData);

gboolean serverRequest_Authorization(void **specialAdditionalServerData);

gboolean serverRequest_CreateDialog(void *data[2]);

gboolean serverRequest_SendMessage(void *data[2]);

gboolean serverRequest_SendFriendRequest(void *data[2]);

gboolean serverRequest_RemoveFriend(void *data[2]);

gboolean serverRequest_LeaveDialog(void *data[2]);

gboolean serverRequest_loadMessages(void *data[2]);

gboolean serverRequest_DialogAddUser(void *data[2]);

gboolean serverRequest_FriendIsOnline(void *data[2]);

gboolean serverRequest_FriendIsOffline(void *data[2]);

#endif //CHATPROJECT_REQUESTHANDLER_H