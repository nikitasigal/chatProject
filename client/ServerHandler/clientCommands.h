#ifndef CHATPROJECT_CLIENTCOMMANDS_H
#define CHATPROJECT_CLIENTCOMMANDS_H

#include "../../shared/definitions.h"

void clientRequest_LoadMessages(SOCKET serverSocket, FullDialogInfo dialogInfo);

void clientRequest_CreateDialog(SOCKET serverSocket, FullDialogInfo dialogInfo);

void clientRequest_SendMessage(SOCKET serverSocket, FullMessageInfo messageInfo);

void clientRequest_Registration(SOCKET serverSocket, FullUserInfo userInfo);

void clientRequest_Authorization(SOCKET serverSocket, FullUserInfo userInfo);

void clientRequest_SendFriendRequest(SOCKET serverSocket, FullUserInfo userInfo);

void clientRequest_FriendRequestAccepted(SOCKET serverSocket, FullUserInfo userInfo);

void clientRequest_FriendRequestDeclined(SOCKET serverSocket, FullUserInfo userInfo);

void clientRequest_RemoveFriend(SOCKET serverSocket, FullUserInfo userInfo);

void clientRequest_LeaveDialog(SOCKET serverSocket, FullUserInfo userInfo);

void clientRequest_DialogAddUser(SOCKET serverSocket, FullUserInfo userInfo);

#endif //CHATPROJECT_CLIENTCOMMANDS_H
