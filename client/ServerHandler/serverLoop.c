#include "serverLoop.h"
#include "requestHandler.h"
#include "../login.h"
#include "../friends.h"

void serverRequestProcess(GList *additionalServerData) {
    SOCKET *serverSocket = g_list_nth_data(additionalServerData, SERVER_SOCKET);
    int requestCount = 1;
    int tempI = 0;
    while (TRUE) {
        void *data = malloc(MAX_PACKAGE_SIZE);
        int bytesReceived = recv(*serverSocket, data, MAX_PACKAGE_SIZE, 0);
        tempI++;
        if (bytesReceived < 0) {
            g_critical("Server is offline");
            return;
        }
        g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': Received request number %d", requestCount++);

        void *list[2] = {data, additionalServerData};
        Request *request = data;
        switch (*request) {
            case REGISTRATION: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_Registration), list);
                break;
            }
            case AUTHORIZATION: {
                if (bytesReceived != sizeof(AuthorizationPackage)) {
                    g_warning("Client received corrupted data (AuthorizationPackage). Trying to repeat request...");
                    g_free(data);
                    authorizationButtonClicked(NULL, additionalServerData);
                    break;
                } /*else {
                    if (tempI < 10) {
                        authorizationButtonClicked(NULL, additionalServerData);
                        break;
                    }
                }*/

                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_Authorization), list);
                break;
            }
            case CREATE_DIALOG: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_CreateDialog), list);

                FullDialogInfo *dialogInfo = data;
                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': Creating a dialog with chatID '%d'\n", dialogInfo->ID);
                break;
            }
            case SEND_MESSAGE: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_SendMessage), list);

                FullMessageInfo *messageInfo = data;
                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': Receiving a message in dialog with chatID '%d'\n", messageInfo->chatID);
                break;
            }
            case SEND_FRIEND_REQUEST: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_SendFriendRequest), list);

                FullUserInfo *userInfo = data;
                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': Receiving a friend request from '%s'\n", userInfo->username);
                break;
            }
            case FRIEND_REQUEST_ACCEPTED: {
                gdk_threads_add_idle(G_SOURCE_FUNC(addFriend), list);

                FullUserInfo *userInfo = data;
                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': Accepted a friend request with username '%s'\n", userInfo->username);
                break;
            }
            case REMOVE_FRIEND: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_RemoveFriend), list);

                FullUserInfo *userInfo = data;
                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': You was removed from friend-list of '%s'\n", userInfo->username);
                break;
            }
            case LEAVE_DIALOG: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_LeaveDialog), list);

                FullUserInfo *userInfo = data;
                printf("File - 'serverLoop.c', foo - 'serverRequestProcess': User '%s' leave dialog with chatID '%s'\n", userInfo->username, userInfo->additionalInfo);
                break;
            }
            default: {
                g_warning("File 'serverLoop.c', foo 'serverRequestProcess': Unknown type of request with code '%d' from server\n", *request);
                g_free(data);
            }
        }
    }
}