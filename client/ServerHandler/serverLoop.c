#include "serverLoop.h"
#include "requestHandler.h"
#include "../login.h"
#include "../friends.h"
#include "clientCommands.h"

gboolean repeatLoadMessageRequest(GList *additionalServerData) {
    int *currentDialogID = g_list_nth_data(additionalServerData, CURRENT_DIALOG_ID);
    GtkListBox *dialogsListBox = g_list_nth_data(additionalServerData, DIALOGS_LIST_BOX);
    GList *dialogsListBoxRows = gtk_container_get_children(GTK_CONTAINER(dialogsListBox));
    GList *temp = dialogsListBoxRows;
    while (temp != NULL) {
        GtkWidget *button = gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(temp->data))));
        Dialog *dialog = g_object_get_data(G_OBJECT(button), "Data");
        if (dialog->ID == *currentDialogID) {
            FullDialogInfo dialogInfo;
            dialogInfo.ID = dialog->ID;
            SOCKET *serverDescriptor = g_list_nth_data(additionalServerData, SERVER_SOCKET);
            clientRequest_LoadMessages(*serverDescriptor, dialogInfo);
            return FALSE;
        }

        temp = temp->next;
    }

    return FALSE;
}

void serverRequestProcess(GList *additionalServerData) {
    SOCKET *serverSocket = g_list_nth_data(additionalServerData, SERVER_SOCKET);
    int requestCount = 1;
    while (TRUE) {
        void *data = malloc(MAX_PACKAGE_SIZE);
        int bytesReceived = recv(*serverSocket, data, MAX_PACKAGE_SIZE, 0);
        if (bytesReceived < 0) {
            g_critical("Server is offline");
            gdk_threads_add_idle(G_SOURCE_FUNC(popupNotification), "Server is offline");

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
                }

                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_Authorization), list);
                break;
            }
            case CREATE_DIALOG: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_CreateDialog), list);

                FullDialogInfo *dialogInfo = data;
                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': Creating a dialog with chatID '%d'", dialogInfo->ID);
                break;
            }
            case SEND_MESSAGE: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_SendMessage), list);

                FullMessageInfo *messageInfo = data;
                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': Receiving a message in dialog with chatID '%d'", messageInfo->chatID);
                break;
            }
            case SEND_FRIEND_REQUEST: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_SendFriendRequest), list);

                FullUserInfo *userInfo = data;
                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': Receiving a friend request from '%s'", userInfo->username);
                break;
            }
            case FRIEND_REQUEST_ACCEPTED: {
                gdk_threads_add_idle(G_SOURCE_FUNC(addFriend), list);

                FullUserInfo *userInfo = data;
                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': Accepted a friend request with username '%s'", userInfo->username);
                break;
            }
            case REMOVE_FRIEND: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_RemoveFriend), list);

                FullUserInfo *userInfo = data;
                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': You was removed from friend-list of '%s'", userInfo->username);
                break;
            }
            case LEAVE_DIALOG: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_LeaveDialog), list);

                FullUserInfo *userInfo = data;
                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': User '%s' leave dialog with chatID '%s'", userInfo->username, userInfo->additionalInfo);
                break;
            }
            case LOAD_MESSAGES: {
                if (bytesReceived != sizeof(MessagesPackage)) {
                    g_warning("Client received corrupted data (MessagesPackage). Trying to repeat request...");
                    g_free(data);
                    gdk_threads_add_idle(G_SOURCE_FUNC(repeatLoadMessageRequest), additionalServerData);
                    break;
                }

                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_loadMessages), list);
                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': Loading messages");
                break;
            }
            case DIALOG_ADD_USER: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_DialogAddUser), list);

                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': Adding user to chat");
                break;
            }
            case FRIEND_IS_ONLINE: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_FriendIsOnline), list);

                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': Friend is online");
                break;
            }
            case FRIEND_IS_OFFLINE: {
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_FriendIsOffline), list);

                g_message("File - 'serverLoop.c', foo - 'serverRequestProcess': Friend disconnected");
                break;
            }
            default: {
                g_warning("File 'serverLoop.c', foo 'serverRequestProcess': Unknown type of request with code '%d' from server\n", *request);
                g_free(data);
            }
        }
    }
}