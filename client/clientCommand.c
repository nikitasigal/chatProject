#include "clientCommand.h"
#include "chat.h"
#include "messages.h"
#include "friends.h"
#include "login.h"
#include <windows.h>

void clientRequest_LoadMessages(SOCKET serverSocket, FullDialogInfo dialogInfo) {
    dialogInfo.request = LOAD_MESSAGES;
    int bytes = send(serverSocket, (void *) &dialogInfo, sizeof(FullDialogInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_LoadMessages': Can't send request for loading messages. Sent 0 bytes of information\n");
}

void clientRequest_CreateDialog(SOCKET serverSocket, FullDialogInfo dialogInfo) {
    dialogInfo.request = CREATE_DIALOG;
    int bytes = send(serverSocket, (void *) &dialogInfo, sizeof(FullDialogInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_SendMessage': Can't create a dialog. Sent 0 bytes of information\n");
}

void clientRequest_SendMessage(SOCKET serverSocket, FullMessageInfo messageInfo) {
    messageInfo.request = SEND_MESSAGE;
    int bytes = send(serverSocket, (void *) &messageInfo, sizeof(FullMessageInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_SendMessage': Can't get a message. Sent 0 bytes of information\n");
}

void clientRequest_Registration(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = REGISTRATION;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullMessageInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_Registration': Registration failed. Sent 0 bytes of information\n");
}

void clientRequest_Authorization(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = AUTHORIZATION;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullMessageInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_Authorization': Authorization failed. Sent 0 bytes of information\n");
}

void clientRequest_SendFriendRequest(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = SEND_FRIEND_REQUEST;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullUserInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_SendFriendRequest': Can't send friend request. Sent 0 bytes of information\n");
}

void clientRequest_FriendRequestAccepted(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = FRIEND_REQUEST_ACCEPTED;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullUserInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_FriendRequestAccepted': Can't accept a friend request. Sent 0 bytes of information\n");
}

void clientRequest_FriendRequestDeclined(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = FRIEND_REQUEST_DECLINED;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullUserInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_FriendRequestDeclined': Can't decline a friend request. Sent 0 bytes of information\n");
}

void clientRequest_RemoveFriend(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = REMOVE_FRIEND;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullUserInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_RemoveFriend': Can't remove friend. Sent 0 bytes of information\n");
}

void clientRequest_LeaveDialog(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = LEAVE_DIALOG;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullUserInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_LeaveDialog': Can't leave dialog. Sent 0 bytes of information\n");
}

void serverRequest_CreateDialog(FullDialogInfo dialogInfo, GList *additionalInfo) {
    // Распакуем нужную дополнительную информацию
    GtkListBox *dialogsListBox = g_list_nth_data(additionalInfo, DIALOGS_LIST_BOX);
    GtkMenu *dialogMenu = g_list_nth_data(additionalInfo, DIALOG_MENU);

    // Создадим новый диалог с информацией: ID, name, ссылка на историю чата, ссылка на виджет чата
    Dialog *newDialog = g_malloc(sizeof(Dialog));
    newDialog->ID = dialogInfo.ID;
    newDialog->userList = GTK_LIST_BOX(gtk_list_box_new());
    newDialog->msgList = GTK_LIST_BOX(gtk_list_box_new());
    newDialog->isOpened = FALSE;
    newDialog->isGroup = dialogInfo.isGroup;
    strcpy(newDialog->name, dialogInfo.dialogName);

    gtk_list_box_set_selection_mode(newDialog->msgList, GTK_SELECTION_MULTIPLE);
    GtkWidget *msgListLabelNoMessages = gtk_label_new("Нет сообщений. Будь первым, напиши какую-нибудь чушь!");
    gtk_list_box_set_placeholder(newDialog->msgList, msgListLabelNoMessages);
    gtk_widget_show(msgListLabelNoMessages);
    gtk_list_box_set_selection_mode(newDialog->userList, GTK_SELECTION_NONE);
    g_object_ref(newDialog->msgList);
    g_object_ref(newDialog->userList);

    // Обновим userList
    for (int i = 0; i < dialogInfo.usersNumber; ++i)
        gtk_list_box_insert(newDialog->userList, gtk_label_new(dialogInfo.users[i].username), -1);

    // Создаём окно с кнопкой беседы и ссылку на чат
    GtkWidget *dialogButton = gtk_button_new_with_label(dialogInfo.dialogName);
    GtkWidget *dialogEventBox = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(dialogEventBox), dialogButton);
    g_object_set_data(G_OBJECT(dialogButton), "Data", newDialog);

    // Добавим созданный диалог в список диалогов
    gtk_list_box_insert(dialogsListBox, dialogEventBox, -1);

    GList *dialogList = g_list_nth(additionalInfo, DIALOGS_LIST);
    dialogList->data = g_list_append(dialogList->data, newDialog);

    // Данные для processDialogMenu
    GList *dialogMenuData = NULL;
    dialogMenuData = g_list_append(dialogMenuData, dialogMenu);
    dialogMenuData = g_list_append(dialogMenuData, additionalInfo);
    g_signal_connect(dialogButton, "clicked", (GCallback) openDialog, additionalInfo);
    g_signal_connect(dialogEventBox, "button-press-event", (GCallback) processDialogMenu, dialogMenuData);
    g_signal_connect(newDialog->msgList, "size-allocate", (GCallback) sizeAllocate, g_list_nth_data(additionalInfo, DIALOG_IS_JUST_OPENED));

    gtk_widget_show_all(dialogEventBox);

    // Надо ли нам его сейчас открыть?
    if (dialogInfo.isSupposeToOpen) {
        GList *tempList = NULL;
        tempList = g_list_append(tempList, newDialog);
        tempList = g_list_append(tempList, additionalInfo);
        openDialog(NULL, tempList);
    }
}

void serverRequest_SendMessage(FullMessageInfo messageInfo, GList *additionalInfo) {
    // Распакуем данные
    GList *dialogsList = g_list_nth_data(additionalInfo, DIALOGS_LIST);
    int *currentDialogID = g_list_nth_data(additionalInfo, CURRENT_DIALOG_ID);

    // Ищем диалог
    GList *temp = dialogsList;
    Dialog *currentDialog = NULL;
    while (temp != NULL) {
        currentDialog = temp->data;
        if (messageInfo.ID == currentDialog->ID)
            break;

        temp = temp->next;
    }

    // Диалог не найден
    if (currentDialog == NULL) {
        printf("WARNING, file - 'clientCommand.c', foo - 'serverRequest_SendMessage': Dialog with ID '%d' was not found\n", messageInfo.ID);
        return;
    }

    // Если диалог ещё не был открыт, то не будем загружать сообщение
    if (!currentDialog->isOpened) {
        printf("LOG INFO: received message, but dialog wasn't opened\n");
        return;
    }

    // Вставим новое сообщение
    // Creating main message box
    GtkWidget *msgMainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_widget_set_margin_top(msgMainBox, 8);
    gtk_widget_set_margin_bottom(msgMainBox, 8);
    gtk_widget_set_margin_start(msgMainBox, 10);
    gtk_widget_set_margin_end(msgMainBox, 10);

    // Creating event box that catch clicking
    GtkWidget *eventBox = gtk_event_box_new();

    // Name and date message box
    GtkWidget *msgNameAndDateBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    // Sender name
    GString *fullName = g_string_new("");
    g_string_printf(fullName, "%s %s (%s)", messageInfo.firstName, messageInfo.lastName, messageInfo.login);
    GtkWidget *msgNameLabel = gtk_label_new(fullName->str);
    gtk_widget_set_halign(msgNameLabel, GTK_ALIGN_START);
    g_string_free(fullName, TRUE);

    // Message date
    GtkWidget *msgDateLabel = gtk_label_new(messageInfo.date);
    gtk_widget_set_halign(msgDateLabel, GTK_ALIGN_END);

    // Message text
    GtkWidget *msgTextLabel = gtk_label_new(messageInfo.text);
    gtk_label_set_xalign(GTK_LABEL(msgTextLabel), 0);
    gtk_label_set_line_wrap_mode(GTK_LABEL(msgTextLabel), PANGO_WRAP_WORD_CHAR);
    gtk_label_set_line_wrap(GTK_LABEL(msgTextLabel), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(msgTextLabel), 40);

    // Fill upper box
    gtk_box_pack_start(GTK_BOX(msgNameAndDateBox), msgNameLabel, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(msgNameAndDateBox), msgDateLabel, TRUE, TRUE, 0);

    // Insert upper box in main message box
    gtk_box_pack_start(GTK_BOX(msgMainBox), msgNameAndDateBox, TRUE, TRUE, 0);

    // Insert text label in main message box
    gtk_box_pack_start(GTK_BOX(msgMainBox), msgTextLabel, TRUE, TRUE, 0);

    // Append new message into a chat
    extern gdouble lastAdj;
    if (*currentDialogID == messageInfo.ID && *currentDialogID != -1)
        lastAdj = gtk_adjustment_get_upper(gtk_list_box_get_adjustment(currentDialog->msgList));

    gtk_container_add(GTK_CONTAINER(eventBox), msgMainBox);
    gtk_list_box_insert(currentDialog->msgList, eventBox, -1);

    g_signal_connect(eventBox, "button-press-event", (GCallback) processMsgSelecting, currentDialog->msgList);
    g_signal_connect(eventBox, "button-release-event", (GCallback) processMsgMenu, g_list_nth_data(additionalInfo, MSG_MENU));

    gtk_widget_show_all(eventBox);
}

void serverRequest_SendFriendRequest(FullUserInfo userInfo, GList *additionalInfo) {
    GtkListBox *friendRequestListBox = g_list_nth_data(additionalInfo, FRIEND_REQUEST_LIST_BOX);

    if (userInfo.ID == -1) {
        popupNotification("Request already exists", g_list_nth_data(additionalInfo, POPUP_LABEL));
        return;
    }
    if (userInfo.ID == -2) {
        popupNotification("User with this username doesn't exist", g_list_nth_data(additionalInfo, POPUP_LABEL));
        return;
    }

    // I'm a messenger of this request. All is alright
    if (userInfo.ID == -3) {
        popupNotification("Request has been sent", g_list_nth_data(additionalInfo, POPUP_LABEL));
        return;
    }

    // I'm a receiver of this request
    // Main gtkBox
    GtkWidget *mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *nameLabel = gtk_label_new(userInfo.additionalInfo);
    gtk_widget_set_size_request(nameLabel, -1, 50);
    gtk_box_pack_start(GTK_BOX(mainBox), nameLabel, TRUE, FALSE, 0);

    // Button box
    GtkWidget *buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 50);
    gtk_widget_set_halign(buttonBox, GTK_ALIGN_CENTER);
    GtkWidget *acceptButton = gtk_button_new_with_label("Принять");
    gtk_widget_set_size_request(acceptButton, 120, 10);
    g_object_set_data(G_OBJECT(acceptButton), "Data", &userInfo);
    GtkWidget *declineButton = gtk_button_new_with_label("Отклонить");
    gtk_widget_set_size_request(declineButton, 120, 10);
    g_object_set_data(G_OBJECT(declineButton), "Data", nameLabel);
    gtk_box_pack_start(GTK_BOX(buttonBox), acceptButton, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(buttonBox), declineButton, TRUE, FALSE, 0);

    // Adding button box into main box
    gtk_box_pack_start(GTK_BOX(mainBox), buttonBox, TRUE, FALSE, 0);

    // Adding new request to friend request listbox
    gtk_list_box_insert(friendRequestListBox, mainBox, -1);
    gtk_list_box_unselect_all(friendRequestListBox);

    // Output notification
    popupNotification("Friend request received", g_list_nth_data(additionalInfo, POPUP_LABEL));

    // Signals
    g_signal_connect(acceptButton, "clicked", (GCallback) acceptFriendRequest, additionalInfo);
    g_signal_connect(declineButton, "clicked", (GCallback) declineFriendRequest, additionalInfo);

    gtk_widget_show_all(mainBox);
}

void serverRequest_RemoveFriend(FullUserInfo userInfo, GList *additionalInfo) {
    GtkListBox *friendsListBox = g_list_nth_data(additionalInfo, FRIENDS_LIST_BOX);
    GList *friends = gtk_container_get_children(GTK_CONTAINER(friendsListBox));
    GList *temp = friends;
    while (temp != NULL) {
        FullUserInfo *user = g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(temp->data))))), "Data");
        if (!strcmp(user->username, userInfo.username)) {
            gtk_list_box_select_row(friendsListBox, temp->data);
            removeFriend(NULL, additionalInfo);
            break;
        }

        temp = temp->next;
    }
}

void serverRequest_LeaveDialog(FullUserInfo userInfo, GList *additionalInfo) {
    GList *dialogsList = g_list_nth_data(additionalInfo, DIALOGS_LIST);
    GList *temp = dialogsList;

    // Преобразуем строковое ID в число
    int ID = strtol(userInfo.additionalInfo, NULL, 10);
    Dialog *currentDialog;
    while (temp != NULL) {
        currentDialog = temp->data;
        if (currentDialog->ID == ID)
            break;

        temp = temp->next;
    }

    // Найдём этого пользователя в списке участников чата
    if (temp != NULL) {
        GList *users = gtk_container_get_children(GTK_CONTAINER(currentDialog->userList));
        GList *currentRow = users;
        while (currentRow != NULL) {
            if (!strcmp(gtk_label_get_text(GTK_LABEL(gtk_bin_get_child(GTK_BIN(currentRow->data)))), userInfo.username)) {
                gtk_widget_destroy(currentRow->data);
                break;
            }

            currentRow = currentRow->next;
        }
    }
}

void serverRequest_FriendIsOnline(FullUserInfo userInfo, GList *additionalInfo) {

}

void serverRequest_FriendDisconnect(FullUserInfo userInfo, GList *additionalInfo) {

}

gboolean serverRequest_Registration(GList *specialAdditionalServerData) {
    FullUserInfo *userInfo = g_list_nth_data(specialAdditionalServerData, 0);
    GList *additionalServerData = g_list_nth_data(specialAdditionalServerData, 1);
    if (userInfo->ID == -1) {
        printf("WARNING, file - 'clientCommand.c', foo - 'serverRequest_Registration': username is already in use\n");
        g_free(userInfo);
        return FALSE;
    }

    g_list_nth(additionalServerData, CURRENT_USER)->data = userInfo;

    GtkWidget *window = g_list_nth_data(additionalServerData, APPLICATION_WINDOW);
    GtkWidget *authWindow = g_list_nth_data(additionalServerData, AUTHENTICATION_WINDOW);

    gtk_widget_hide(authWindow);
    gtk_widget_show(window);
    gtk_widget_show_all(g_list_nth_data(additionalServerData, DIALOG_VIEWPORT));

    return FALSE;
}

gboolean serverRequest_Authorization(GList *specialAdditionalServerData) {
    AuthorizationPackage *startPackage = g_list_nth_data(specialAdditionalServerData, 0);
    GList *additionalServerData = g_list_nth_data(specialAdditionalServerData, 1);

    FullUserInfo *userInfo = g_malloc(sizeof(FullUserInfo));
    userInfo->request = AUTHORIZATION;
    userInfo->ID = startPackage->authorizedUser.ID;
    strcpy(userInfo->username, startPackage->authorizedUser.username);
    strcpy(userInfo->firstName, startPackage->authorizedUser.firstName);
    strcpy(userInfo->secondName, startPackage->authorizedUser.secondName);
    strcpy(userInfo->secondName, startPackage->authorizedUser.secondName);

    if (userInfo->ID == -1) {
        popupNotification("User doesn't exist", g_list_nth_data(additionalServerData, POPUP_LABEL));
        g_free(userInfo);
        return FALSE;
    }
    if (userInfo->ID == -2) {
        popupNotification("Incorrect password", g_list_nth_data(additionalServerData, POPUP_LABEL));
        g_free(userInfo);
        return FALSE;
    }

    g_list_nth(additionalServerData, CURRENT_USER)->data = userInfo;

    // Загрузим данные
    for (int i = 0; i < startPackage->dialogCount; ++i) {
        serverRequest_CreateDialog(startPackage->dialogList[i], additionalServerData);
        printf(">> CreateDialog %d\n", i + 1);
    }
    for (int i = 0; i < startPackage->friendCount; ++i) {
        addFriend(&startPackage->friends[i], additionalServerData);
        printf(">> AddFriend %d\n", i + 1);
    }
    for (int i = 0; i < startPackage->requestCount; ++i) {
        serverRequest_SendFriendRequest(startPackage->requests[i], additionalServerData);
        printf(">> FriendRequest %d\n", i + 1);
    }

    GtkWidget *window = g_list_nth_data(additionalServerData, APPLICATION_WINDOW);
    GtkWidget *authWindow = g_list_nth_data(additionalServerData, AUTHENTICATION_WINDOW);

    gtk_widget_hide(authWindow);
    gtk_widget_show(window);
    gtk_widget_show_all(g_list_nth_data(additionalServerData, DIALOG_VIEWPORT));

    g_free(startPackage);
    g_list_free(specialAdditionalServerData);

    return FALSE;
}

void serverRequestProcess(GList *additionalServerData) {
    // what's kind of message?
    SOCKET *serverSocket = g_list_nth_data(additionalServerData, SERVER_SOCKET);
    int i = 1;  // debug info
    while (TRUE) {
        /*char byte;
        int isAlive = recv(*serverSocket, &byte, 1, MSG_PEEK);
        if (isAlive == -1) {
            printf("ERROR: Server is offline\n");
            // TODO Try to connect again?
            return;
        }*/

        printf("Received request number %d\n", i++);
        void *data = malloc(820000);
        int bytesReceived = recv(*serverSocket, data, 820000, 0);
        if (bytesReceived < 0) {
            printf("ERROR: Server is offline\n");
            // TODO Try to connect again?
            return;
        }
        
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
                printf("LOG INFO, file - 'clientCommand.c', foo - 'serverRequestProcess': Registration success\n");

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
                for (int j = 0; j < temp->dialogCount; ++j)
                    startPackage->friends[j] = temp->friends[j];
                for (int j = 0; j < temp->friendCount; ++j)
                    startPackage->dialogList[j] = temp->dialogList[j];


                GList *list = NULL;
                list = g_list_append(list, startPackage);
                list = g_list_append(list, additionalServerData);

                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_Authorization), list);
                printf("LOG INFO, file - 'clientCommand.c', foo - 'serverRequestProcess': Authorization success\n");

                break;
            }
            case CREATE_DIALOG: {
                FullDialogInfo *dialogInfo = (FullDialogInfo *) data;
                serverRequest_CreateDialog(*dialogInfo, additionalServerData);
                printf("LOG INFO, file - 'clientCommand.c', foo - 'serverRequestProcess': Creating a dialog with ID '%d'\n", dialogInfo->ID);
                break;
            }
            case SEND_MESSAGE: {
                FullMessageInfo *messageInfo = (FullMessageInfo *) data;
                serverRequest_SendMessage(*messageInfo, additionalServerData);
                printf("LOG INFO, file - 'clientCommand.c', foo - 'serverRequestProcess': Receiving a message in dialog with ID '%d'\n", messageInfo->ID);

                break;
            }
            case SEND_FRIEND_REQUEST: {
                FullUserInfo *userInfo = (FullUserInfo *) data;
                serverRequest_SendFriendRequest(*userInfo, additionalServerData);
                printf("LOG INFO, file - 'clientCommand.c', foo - 'serverRequestProcess': Receiving a friend request from '%s'\n", userInfo->username);

                break;
            }
            case FRIEND_REQUEST_ACCEPTED: {
                FullUserInfo *userInfo = (FullUserInfo *) data;
                addFriend(userInfo, additionalServerData);
                printf("LOG INFO, file - 'clientCommand.c', foo - 'serverRequestProcess': Accepted a friend request with username '%s'\n", userInfo->username);

                break;
            }
            case REMOVE_FRIEND: {
                FullUserInfo *userInfo = (FullUserInfo *) data;
                serverRequest_RemoveFriend(*userInfo, additionalServerData);
                printf("LOG INFO, file - 'clientCommand.c', foo - 'serverRequestProcess': You was removed from friend-list of '%s'\n", userInfo->username);

                break;
            }
            case LEAVE_DIALOG: {
                FullUserInfo *userInfo = (FullUserInfo *) data;
                serverRequest_LeaveDialog(*userInfo, additionalServerData);
                printf("LOG INFO, file - 'clientCommand.c', foo - 'serverRequestProcess': User '%s' leave dialog with ID '%s'\n", userInfo->username, userInfo->additionalInfo);

                break;
            }
            default:
                printf("WARNING, file 'clientCommand.c', foo 'serverRequestProcess': Unknown type of request with code '%d' from server\n", *request);
        }
        free(data);
    }
}