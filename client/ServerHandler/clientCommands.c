#include "clientCommands.h"

void clientRequest_LoadMessages(SOCKET serverSocket, FullDialogInfo dialogInfo) {
    dialogInfo.request = LOAD_MESSAGES;
    int bytes = send(serverSocket, (void *) &dialogInfo, sizeof(FullDialogInfo), 0);
    if (bytes < 0)
        g_critical("File - 'clientCommands.c', foo - 'clientRequest_LoadMessages': Can't send request for loading messages. Sent 0 bytes of information");
}

void clientRequest_CreateDialog(SOCKET serverSocket, FullDialogInfo dialogInfo) {
    dialogInfo.request = CREATE_DIALOG;
    int bytes = send(serverSocket, (void *) &dialogInfo, sizeof(FullDialogInfo), 0);
    if (bytes < 0)
        g_critical("File - 'clientCommands.c', foo - 'clientRequest_SendMessage': Can't create a dialog. Sent 0 bytes of information");
}

void clientRequest_SendMessage(SOCKET serverSocket, FullMessageInfo messageInfo) {
    messageInfo.request = SEND_MESSAGE;
    int bytes = send(serverSocket, (void *) &messageInfo, sizeof(FullMessageInfo), 0);
    if (bytes < 0)
        g_critical("File - 'clientCommands.c', foo - 'clientRequest_SendMessage': Can't get a message. Sent 0 bytes of information");
}

void clientRequest_Registration(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = REGISTRATION;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullUserInfo), 0);
    if (bytes < 0)
        g_critical("File - 'clientCommands.c', foo - 'clientRequest_Registration': Registration failed. Sent 0 bytes of information");
}

void clientRequest_Authorization(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = AUTHORIZATION;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullUserInfo), 0);
    if (bytes < 0)
        g_critical("ERROR, file - 'clientCommands.c', foo - 'clientRequest_Authorization': Authorization failed. Sent 0 bytes of information");
}

void clientRequest_SendFriendRequest(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = SEND_FRIEND_REQUEST;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullUserInfo), 0);
    if (bytes < 0)
        g_critical("File - 'clientCommands.c', foo - 'clientRequest_SendFriendRequest': Can't send friend request. Sent 0 bytes of information");
}

void clientRequest_FriendRequestAccepted(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = FRIEND_REQUEST_ACCEPTED;
    int k = sizeof(FullUserInfo);
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullUserInfo), 0);
    if (bytes < 0)
        g_critical("File - 'clientCommands.c', foo - 'clientRequest_FriendRequestAccepted': Can't accept a friend request. Sent 0 bytes of information");
}

void clientRequest_FriendRequestDeclined(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = FRIEND_REQUEST_DECLINED;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullUserInfo), 0);
    if (bytes < 0)
        g_critical("File - 'clientCommands.c', foo - 'clientRequest_FriendRequestDeclined': Can't decline a friend request. Sent 0 bytes of information");
}

void clientRequest_RemoveFriend(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = REMOVE_FRIEND;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullUserInfo), 0);
    if (bytes < 0)
        g_critical("File - 'clientCommands.c', foo - 'clientRequest_RemoveFriend': Can't remove friend. Sent 0 bytes of information");
}

void clientRequest_LeaveDialog(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = LEAVE_DIALOG;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullUserInfo), 0);
    if (bytes < 0)
        g_critical("File - 'clientCommands.c', foo - 'clientRequest_LeaveDialog': Can't leave dialog. Sent 0 bytes of information");
}

void clientRequest_DialogAddUser(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = DIALOG_ADD_USER;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullUserInfo), 0);
    if (bytes < 0)
        g_critical("File - 'clientCommands.c', foo - 'clientRequest_LeaveDialog': Can't add user to chat. Sent 0 bytes of information");
}