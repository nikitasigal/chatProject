#ifndef CHATPROJECT_REQUESTHANDLER_H
#define CHATPROJECT_REQUESTHANDLER_H

#include "../../shared/definitions.h"

gboolean serverRequest_Registration(GList *specialAdditionalServerData);

gboolean serverRequest_Authorization(GList *specialAdditionalServerData);

void serverRequest_CreateDialog(FullDialogInfo dialogInfo, GList *additionalInfo);

void serverRequest_SendMessage(FullMessageInfo messageInfo, GList *additionalInfo);

void serverRequest_SendFriendRequest(FullUserInfo userInfo, GList *additionalInfo);

void serverRequest_RemoveFriend(FullUserInfo userInfo, GList *additionalInfo);

void serverRequest_LeaveDialog(FullUserInfo userInfo, GList *additionalInfo);

void serverRequest_FriendIsOnline(FullUserInfo userInfo, GList *additionalInfo);

void serverRequest_FriendDisconnect(FullUserInfo userInfo, GList *additionalInfo);

#endif //CHATPROJECT_REQUESTHANDLER_H
