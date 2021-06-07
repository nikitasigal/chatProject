#ifndef TESTGTK_CLIENTCOMMAND_H
#define TESTGTK_CLIENTCOMMAND_H

#include "definitions.h"

void clientRequest_LoadMessages(SOCKET serverSocket, FullDialogInfo dialogInfo);

void clientRequest_CreateDialog(SOCKET serverSocket, FullDialogInfo dialogInfo);

void clientRequest_SendMessage(SOCKET serverSocket, FullMessageInfo messageInfo);

void clientRequest_Registration(SOCKET serverSocket, FullUserInfo userInfo);

void clientRequest_Authorization(SOCKET serverSocket, FullUserInfo userInfo);

void clientRequest_SendFriendRequest(SOCKET serverSocket, FullUserInfo userInfo);

void clientRequest_FriendRequestAccepted(SOCKET serverSocket, FullUserInfo userInfo);

void clientRequest_FriendRequestDeclined(SOCKET serverSocket, FullUserInfo userInfo);

void clientRequest_RemoveFriend(SOCKET serverSocket, FullUserInfo userInfo);

void serverRequestProcess(GList *additionalServerData);

void serverRequest_CreateDialog(FullDialogInfo dialogInfo, GList *additionalInfo);

void serverRequest_SendMessage(FullMessageInfo messageInfo, GList *additionalInfo);

gboolean serverRequest_Registration(GList *specialAdditionalServerData);

gboolean serverRequest_Authorization(GList *specialAdditionalServerData);

#endif //TESTGTK_CLIENTCOMMAND_H
