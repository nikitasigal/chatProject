#include "serverLoop.h"
#include "requestHandler.h"
#include "../friends.h"

void serverRequestProcess(GList *additionalServerData) {
    SOCKET *serverSocket = g_list_nth_data(additionalServerData, SERVER_SOCKET);
    int i = 1;
    while (TRUE) {
        void *data = malloc(820000);
        int bytesReceived = recv(*serverSocket, data, 820000, 0);
        if (bytesReceived < 0) {
            printf("ERROR: Server is offline\n");
            return;
        }
        printf("Received request number %d\n", i++);

        Request *request = data;
        switch (*request) {
            case REGISTRATION: {
                FullUserInfo *userInfo = g_malloc(sizeof(FullUserInfo));
                FullUserInfo *temp = (FullUserInfo *) data;
                userInfo->request = REGISTRATION;
                userInfo->ID = temp->ID;
                strcpy(userInfo->firstName, temp->firstName);
                strcpy(userInfo->secondName, temp->secondName);
                strcpy(userInfo->username, temp->username);

                GList *list = NULL;
                list = g_list_append(list, userInfo);
                list = g_list_append(list, additionalServerData);
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_Registration), list);

                break;
            }
            case AUTHORIZATION: {
                AuthorizationPackage *startPackage = g_malloc(sizeof(AuthorizationPackage));
                AuthorizationPackage *temp = (AuthorizationPackage *) data;

                startPackage->request = AUTHORIZATION;
                startPackage->requestCount = temp->requestCount;
                startPackage->dialogCount = temp->dialogCount;
                startPackage->friendCount = temp->friendCount;
                startPackage->authorizedUser = temp->authorizedUser;

                for (int j = 0; j < temp->requestCount; ++j)
                    startPackage->requests[j] = temp->requests[j];
                for (int j = 0; j < temp->friendCount; ++j)
                    startPackage->friends[j] = temp->friends[j];
                for (int j = 0; j < temp->dialogCount; ++j)
                    startPackage->dialogList[j] = temp->dialogList[j];


                GList *list = NULL;
                list = g_list_append(list, startPackage);
                list = g_list_append(list, additionalServerData);

                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_Authorization), list);

                break;
            }
            case CREATE_DIALOG: {
                FullDialogInfo *dialogInfo = (FullDialogInfo *) data;
                serverRequest_CreateDialog(*dialogInfo, additionalServerData);
                printf("LOG INFO, file - 'serverLoop.c', foo - 'serverRequestProcess': Creating a dialog with ID '%d'\n", dialogInfo->ID);
                break;
            }
            case SEND_MESSAGE: {
                FullMessageInfo *messageInfo = (FullMessageInfo *) data;
                serverRequest_SendMessage(*messageInfo, additionalServerData);
                printf("LOG INFO, file - 'serverLoop.c', foo - 'serverRequestProcess': Receiving a message in dialog with ID '%d'\n", messageInfo->ID);

                break;
            }
            case SEND_FRIEND_REQUEST: {
                FullUserInfo *userInfo = (FullUserInfo *) data;
                serverRequest_SendFriendRequest(*userInfo, additionalServerData);
                printf("LOG INFO, file - 'serverLoop.c', foo - 'serverRequestProcess': Receiving a friend request from '%s'\n", userInfo->username);

                break;
            }
            case FRIEND_REQUEST_ACCEPTED: {
                FullUserInfo *userInfo = (FullUserInfo *) data;
                addFriend(userInfo, additionalServerData);
                printf("LOG INFO, file - 'serverLoop.c', foo - 'serverRequestProcess': Accepted a friend request with username '%s'\n", userInfo->username);

                break;
            }
            case REMOVE_FRIEND: {
                FullUserInfo *userInfo = (FullUserInfo *) data;
                serverRequest_RemoveFriend(*userInfo, additionalServerData);
                printf("LOG INFO, file - 'serverLoop.c', foo - 'serverRequestProcess': You was removed from friend-list of '%s'\n", userInfo->username);

                break;
            }
            case LEAVE_DIALOG: {
                FullUserInfo *userInfo = (FullUserInfo *) data;
                serverRequest_LeaveDialog(*userInfo, additionalServerData);
                printf("LOG INFO, file - 'serverLoop.c', foo - 'serverRequestProcess': User '%s' leave dialog with ID '%s'\n", userInfo->username, userInfo->additionalInfo);

                break;
            }
            default:
                printf("WARNING, file 'serverLoop.c', foo 'serverRequestProcess': Unknown type of request with code '%d' from server\n", *request);
        }
        free(data);
    }
}