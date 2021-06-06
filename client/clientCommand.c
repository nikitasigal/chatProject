#include "clientCommand.h"
#include "chat.h"
#include "messages.h"
#include <windows.h>

extern gdouble lastAdj;

/*
 * Мы приняли запрос от сервера. Есть несколько видов запросов:
 * 1) Создать беседу с именем name и идентификатором ID (с участниками array userID[] ?)
 * 2) Добавить сообщение в беседу с идентификатором ID, от name в время date c текстом text
 * 3) Запрос на добавление в друзья от пользователя ID с именем name
 * 4) Пользователь с именем name принял запрос на добавление в друзья
 * 5) Друг с именем name появился в сети
 */

/*
 * Мы хотим отправить запрос серверу. Есть несколько видов запросов:
 * 1) Создать беседу с именем name и идентификатором ID с участниками array userID[]
 * 2) Добавить сообщение в беседу с идентификатором ID, от name в время date c текстом text
 * 3) Запрос на добавление в друзья пользователя ID
 * 4) Мы приняли запрос на добавление в друзья пользователя ID
 * 5) Присоединиться к серверу с логином login и паролем password
 * 6) Зарегистрироваться (...)
 */

/*
 * Любой диалог должен содержать информацию:
 * 1) ID
 * 2) Name
 * 3) Список участников (name, login, ID (?))
 * 4) Список сообщений
 */

/*
 * Пользователь должен содержать информацию: (на сервере?)
 * 1) ID
 * 2) First name
 * 3) Last name
 * 4) Login
 * 5) Password (?)
 * 6) Список диалогов
 * 7) Список друзей
 */

/*
 * Сообщение содержит данные:
 * 1) Date
 * 2) Name (login)
 * 3) Text
 * 4) ID беседы (?)
 */

/*
 * Пользователь умеет:
 * 1) Отправлять сообщение в беседу ID
 * 2) Добавлять в друзья
 * 3) Переходить по вкладкам
 */

void clientRequest_CreateDialog(SOCKET serverSocket, FullDialogInfo dialogInfo) {
    // Хотим отправить запрос на создание диалога
    dialogInfo.request = CREATE_DIALOG;
    int bytes = send(serverSocket, (void *) &dialogInfo, sizeof(FullDialogInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_SendMessage': Can't create a dialog. Received 0 bytes of information\n");
}

void clientRequest_SendMessage(SOCKET serverSocket, FullMessageInfo messageInfo) {
    // Хотим отправить запрос на отправку сообщения
    messageInfo.request = SEND_MESSAGE;
    int bytes = send(serverSocket, (void *) &messageInfo, sizeof(FullMessageInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_SendMessage': Can't get a message. Received 0 bytes of information\n");
}

void clientRequest_Registration(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = REGISTRATION;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullMessageInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_Registration': Registration failed. Received 0 bytes of information\n");
}

void clientRequest_Authorization(SOCKET serverSocket, FullUserInfo userInfo) {
    userInfo.request = AUTHORIZATION;
    int bytes = send(serverSocket, (void *) &userInfo, sizeof(FullMessageInfo), 0);
    if (bytes < 0)
        printf("ERROR, file - 'clientCommand.c', foo - 'clientRequest_Authorization': Authorization failed. Received 0 bytes of information\n");
}

void serverRequest_CreateDialog(FullDialogInfo dialogInfo, GList *additionalInfo) {
    // Распакуем нужную дополнительную информацию
    GtkListBox *dialogsListBox = g_list_nth_data(additionalInfo, DIALOGS_LIST_BOX);

    // Создадим новый диалог с информацией: ID, name, ссылка на историю чата, ссылка на виджет чата
    Dialog *newDialog = g_malloc(sizeof(Dialog));
    newDialog->ID = dialogInfo.ID;
    newDialog->userList = GTK_LIST_BOX(gtk_list_box_new());
    newDialog->msgList = GTK_LIST_BOX(gtk_list_box_new());
    strcpy(newDialog->name, dialogInfo.dialogName);

    gtk_list_box_set_selection_mode(newDialog->msgList, GTK_SELECTION_MULTIPLE);
    GtkWidget *msgListLabelNoMessages = gtk_label_new("Нет сообщений. Будь первым, напиши какую-нибудь чушь!");
    gtk_list_box_set_placeholder(newDialog->msgList, msgListLabelNoMessages);
    gtk_list_box_set_selection_mode(newDialog->userList, GTK_SELECTION_NONE);
    g_object_ref(newDialog->msgList);
    g_object_ref(newDialog->userList);

    // Обновим userList
    for (int i = 0; i < dialogInfo.usersNumber; ++i)
        gtk_list_box_insert(newDialog->userList, gtk_label_new(dialogInfo.users[i].login), -1);

    // Преобразуем ID в строку
    char IDString[TEXT_SIZE] = {0};
    sprintf(IDString, "%d", dialogInfo.ID);

    // Создаём окно с кнопкой беседы, ID и ссылку на чат
    GtkWidget *dialogBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget *dialogLabelWithID = gtk_label_new(IDString);
    GtkWidget *dialogButton = gtk_button_new_with_label(dialogInfo.dialogName);
    g_object_set_data(G_OBJECT(dialogButton), "Data", newDialog);
    gtk_container_add(GTK_CONTAINER(dialogBox), dialogLabelWithID);
    gtk_container_add(GTK_CONTAINER(dialogBox), dialogButton);

    // Добавим созданный диалог в список диалогов
    gtk_list_box_insert(dialogsListBox, dialogBox, -1);

    GList *dialogList = g_list_nth(additionalInfo, DIALOGS_LIST);
    dialogList->data = g_list_append(dialogList->data, newDialog);

    // Подключаем сигнал нажатия кнопки. При нажатии открывается диалог
    GList *data = NULL;
    data = g_list_append(data, newDialog);
    data = g_list_append(data, additionalInfo); // entry and send-button
    g_signal_connect(dialogButton, "clicked", (GCallback) newOpenDialog, data);
    g_signal_connect(newDialog->msgList, "size-allocate", (GCallback) sizeAllocate, NULL);

    // Сделаем ID невидимым
    gtk_widget_set_no_show_all(dialogLabelWithID, TRUE);

    gtk_widget_show_all(dialogBox);
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

    // Вставим новое сообщение
    // Creating main message box
    GtkWidget *msgMainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_widget_set_margin_top(msgMainBox, 8);
    gtk_widget_set_margin_bottom(msgMainBox, 8);
    gtk_widget_set_margin_start(msgMainBox, 10);
    gtk_widget_set_margin_end(msgMainBox, 10);

    // Creating event box that catch clicking
    GtkWidget *eventBox = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(eventBox), msgMainBox);
    g_signal_connect(eventBox, "button-press-event", (GCallback) processMsgSelecting, currentDialog->msgList);
    g_signal_connect(eventBox, "button-release-event", (GCallback) processMsgMenu, NULL);

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
    if (*currentDialogID == messageInfo.ID && *currentDialogID != -1)
        lastAdj = gtk_adjustment_get_upper(gtk_list_box_get_adjustment(currentDialog->msgList));
    gtk_list_box_insert(currentDialog->msgList, eventBox, -1);

    gtk_widget_show_all(eventBox);
}

void serverRequest_AddFriend(FullUserInfo userInfo) {

}

void serverRequest_FriendIsOnline(FullUserInfo userInfo) {

}

void serverRequest_RemoveFriend(FullUserInfo userInfo) {

}

void serverRequest_UserLeaveDialog(FullUserInfo userInfo, FullDialogInfo dialogInfo) {

}

void serverRequest_FriendDisconnect(FullUserInfo userInfo, char *date) { // Change userInfo (date), maybe

}

gboolean serverRequest_Registration(GList *specialAdditionalServerData) {
    FullUserInfo *userInfo = g_list_nth_data(specialAdditionalServerData, 0);
    GList *additionalServerData = g_list_nth_data(specialAdditionalServerData, 1);
    if (userInfo->ID == -1) {
        printf("WARNING, file - 'clientCommand.c', foo - 'serverRequest_Registration': login is already in use\n");
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
    FullUserInfo *userInfo = g_list_nth_data(specialAdditionalServerData, 0);
    GList *additionalServerData = g_list_nth_data(specialAdditionalServerData, 1);

    if (userInfo->ID == -1) {
        printf("WARNING, file - 'clientCommand.c', foo - 'serverRequest_Authorization': incorrect password or login\n");
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

void serverRequestProcess(GList *additionalServerData) {
    // what's kind of message?
    SOCKET *serverSocket = g_list_nth_data(additionalServerData, SERVER_SOCKET);
    int i = 1;  // debug info
    while (TRUE) {
        char byte;
        int isAlive = recv(*serverSocket, &byte, 1, MSG_PEEK);
        if (isAlive == -1) {
            printf("ERROR: Server is offline\n");
            // TODO Try to connect again?
            return;
        }

        printf("Received request number %d\n", i++);
        void *data = malloc(10000);
        int bytesReceived = recv(*serverSocket, data, 10000, 0);
        if (bytesReceived < 0) {
            printf("ERROR, file - 'clientCommand.c', foo - 'serverRequestProcess': data is not received\n");
            continue;
        }
        
        Request *request = data;
        switch (*request) {
            case REGISTRATION: {
                FullUserInfo *userInfo = g_malloc(sizeof(FullUserInfo));
                FullUserInfo *temp = (FullUserInfo *) data;
                userInfo->request = AUTHORIZATION;
                userInfo->ID = temp->ID;
                strcpy(userInfo->firstName, temp->firstName);
                strcpy(userInfo->lastName, temp->lastName);
                strcpy(userInfo->login, temp->login);

                GList *list = NULL;
                list = g_list_append(list, userInfo);
                list = g_list_append(list, additionalServerData);
                gdk_threads_add_idle(G_SOURCE_FUNC(serverRequest_Registration), list);
                printf("LOG INFO, file - 'clientCommand.c', foo - 'serverRequestProcess': Registration success\n");

                break;
            }
            case AUTHORIZATION: {
                FullUserInfo *userInfo = g_malloc(sizeof(FullUserInfo));
                FullUserInfo *temp = (FullUserInfo *) data;
                userInfo->request = AUTHORIZATION;
                userInfo->ID = temp->ID;
                strcpy(userInfo->firstName, temp->firstName);
                strcpy(userInfo->lastName, temp->lastName);
                strcpy(userInfo->login, temp->login);

                GList *list = NULL;
                list = g_list_append(list, userInfo);
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
            default:
                printf("WARNING, file 'clientCommand.c', foo 'serverRequestProcess': Unknown type of request with code '%d' from server\n", *request);
        }
        free(data);
    }
}