#include "requestHandler.h"
#include "../chat.h"
#include "../messages.h"
#include "../friends.h"
#include "../login.h"

gboolean serverRequest_Registration(void **specialAdditionalServerData) {
    // Распакуем полученную информацию
    FullUserInfo *userInfo = specialAdditionalServerData[0];
    GList *additionalServerData = specialAdditionalServerData[1];

    // Если мы получили user ID == -1, то логин уже используется
    if (userInfo->ID == -1) {
        g_warning("File - 'requestHandler.c', foo - 'serverRequest_Registration': username is already in use");
        g_free(userInfo);
        return FALSE;
    }

    // Обновим информацию о текущем пользователе в глобальном списке
    g_list_nth(additionalServerData, CURRENT_USER)->data = userInfo;

    // Обновим интерфейс. Скроем окно авторизации и покажем окно приложения
    gtk_widget_hide(g_list_nth_data(additionalServerData, AUTHENTICATION_WINDOW));
    gtk_widget_show(g_list_nth_data(additionalServerData, APPLICATION_WINDOW));
    gtk_widget_show_all(g_list_nth_data(additionalServerData, DIALOG_VIEWPORT));

    return FALSE;
}

gboolean serverRequest_Authorization(void **specialAdditionalServerData) {
    // Распакуем полученную информацию
    AuthorizationPackage *startPackage = specialAdditionalServerData[0];
    GList *additionalServerData = specialAdditionalServerData[1];

    FullUserInfo *userInfo = g_malloc(sizeof(FullUserInfo));
    userInfo->request = AUTHORIZATION;
    userInfo->ID = startPackage->authorizedUser.ID;
    strcpy(userInfo->username, startPackage->authorizedUser.username);
    strcpy(userInfo->secondName, startPackage->authorizedUser.secondName);
    strcpy(userInfo->firstName, startPackage->authorizedUser.firstName);

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

    // Обновим информацию о текущем пользователе в глобальном списке
    g_list_nth(additionalServerData, CURRENT_USER)->data = userInfo;

    // Загрузим данные. Диалоги, друзья, запросы в друзья
    for (int i = 0; i < startPackage->dialogCount; ++i) {
        void *nextData[2] = {&startPackage->dialogList[i], additionalServerData};
        serverRequest_CreateDialog(nextData);
        g_message("Authorization package: createDialog %d", i + 1);
    }
    for (int i = 0; i < startPackage->friendCount; ++i) {
        void *nextData[2] = {&startPackage->friends[i], additionalServerData};
        addFriend(nextData);
        g_message("Authorization package: addFriend %d", i + 1);
    }
    for (int i = 0; i < startPackage->requestCount; ++i) {
        void *nextData[2] = {&startPackage->requests[i], additionalServerData};
        serverRequest_SendFriendRequest(nextData);
        g_message("Authorization package: friendRequest %d", i + 1);
    }

    // Обновим интерфейс. Скроем окно авторизации и покажем окно приложения
    gtk_widget_hide(g_list_nth_data(additionalServerData, AUTHENTICATION_WINDOW));
    gtk_widget_show(g_list_nth_data(additionalServerData, APPLICATION_WINDOW));
    gtk_widget_show_all(g_list_nth_data(additionalServerData, DIALOG_VIEWPORT));

    g_free(startPackage);

    return FALSE;
}

gboolean serverRequest_CreateDialog(void *data[2]) {
    // Распакуем полученную информацию
    FullDialogInfo *dialogInfo = data[0];
    GList *additionalInfo = data[1];

    // Распакуем нужную дополнительную информацию
    GtkListBox *dialogsListBox = g_list_nth_data(additionalInfo, DIALOGS_LIST_BOX);
    GtkMenu *dialogMenu = g_list_nth_data(additionalInfo, DIALOG_MENU);

    // Создадим новый диалог с полученной информацией
    Dialog *newDialog = g_malloc(sizeof(Dialog));
    newDialog->ID = dialogInfo->ID;
    newDialog->userList = GTK_LIST_BOX(gtk_list_box_new());
    newDialog->msgList = GTK_LIST_BOX(gtk_list_box_new());
    newDialog->isOpened = FALSE;
    newDialog->isGroup = dialogInfo->isGroup;
    strcpy(newDialog->name, dialogInfo->dialogName);

    // Установим виджет-заполнитель, который будет показываться, если список не содержит сообщений
    GtkWidget *msgListLabelNoMessages = gtk_label_new("Нет сообщений. Будь первым, напиши какую-нибудь чушь!");
    gtk_list_box_set_placeholder(newDialog->msgList, msgListLabelNoMessages);
    gtk_widget_show(msgListLabelNoMessages);

    // Мы можем выбирать сообщения, сколько хотим. Выбирать людей их списка участников мы не можем
    gtk_list_box_set_selection_mode(newDialog->msgList, GTK_SELECTION_MULTIPLE);
    gtk_list_box_set_selection_mode(newDialog->userList, GTK_SELECTION_NONE);

    // Сделаем виджеты "плавающими" - они не уничтожатся, если мы их уберём с экрана
    g_object_ref(newDialog->msgList);
    g_object_ref(newDialog->userList);

    // Обновим userList
    for (int i = 0; i < dialogInfo->usersNumber; ++i)
        gtk_list_box_insert(newDialog->userList, gtk_label_new(dialogInfo->users[i].username), -1);

    // Создаём окно с кнопкой беседы и ссылку на чат
    GtkWidget *dialogButton = gtk_button_new_with_label(dialogInfo->dialogName);
    gtk_widget_set_size_request(dialogButton, -1, 50);
    GtkWidget *dialogEventBox = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(dialogEventBox), dialogButton);
    g_object_set_data(G_OBJECT(dialogButton), "Data", newDialog);

    // Добавим созданный диалог в список диалогов
    gtk_list_box_insert(dialogsListBox, dialogEventBox, -1);

    // Обновим список диалогов
    GList *dialogList = g_list_nth(additionalInfo, DIALOGS_LIST);
    dialogList->data = g_list_append(dialogList->data, newDialog);

    // Данные для processDialogMenu
    GList *dialogMenuData = NULL;
    dialogMenuData = g_list_append(dialogMenuData, dialogMenu);
    dialogMenuData = g_list_append(dialogMenuData, additionalInfo);

    // Подключим сигналы: открытия диалога по нажатию, открытия меню по нажатию ПКМ по диалогу, отслеживание изменений размера listBox
    g_signal_connect(dialogButton, "clicked", (GCallback) openDialog, additionalInfo);
    g_signal_connect(dialogEventBox, "button-press-event", (GCallback) processDialogMenu, dialogMenuData);
    g_signal_connect(newDialog->msgList, "size-allocate", (GCallback) sizeAllocate, g_list_nth_data(additionalInfo, DIALOG_IS_JUST_OPENED));

    gtk_widget_show_all(dialogEventBox);

    // Надо ли нам его сейчас открыть?
    if (dialogInfo->isSupposeToOpen) {
        GList *tempList = NULL;
        tempList = g_list_append(tempList, newDialog);
        tempList = g_list_append(tempList, additionalInfo);
        openDialog(NULL, tempList);
    }

    // Освобождаем память, если это был запрос на создание диалога
    if (dialogInfo->request == CREATE_DIALOG)
        g_free(dialogInfo);

    return FALSE;
}

gboolean serverRequest_SendMessage(void *data[2]) {
    // Распакуем полученную информацию
    FullMessageInfo *messageInfo = data[0];
    GList *additionalInfo = data[1];

    // Распакуем данные
    GList *dialogsList = g_list_nth_data(additionalInfo, DIALOGS_LIST);
    int *currentDialogID = g_list_nth_data(additionalInfo, CURRENT_DIALOG_ID);

    // Ищем диалог
    GList *temp = dialogsList;
    Dialog *currentDialog = NULL;
    while (temp != NULL) {
        currentDialog = temp->data;
        if (messageInfo->ID == currentDialog->ID)
            break;

        temp = temp->next;
    }

    // Диалог не найден
    if (currentDialog == NULL) {
        printf("WARNING, file - 'requestHandler.c', foo - 'serverRequest_SendMessage': Dialog with ID '%d' was not found\n", messageInfo->ID);
        return FALSE;
    }

    // Если диалог ещё не был открыт, то не будем загружать сообщение
    if (!currentDialog->isOpened) {
        printf("LOG INFO: received message, but dialog wasn't opened\n");
        return FALSE;
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
    g_string_printf(fullName, "%s %s (%s)", messageInfo->firstName, messageInfo->lastName, messageInfo->login);
    GtkWidget *msgNameLabel = gtk_label_new(fullName->str);
    gtk_widget_set_halign(msgNameLabel, GTK_ALIGN_START);
    g_string_free(fullName, TRUE);

    // Message date
    GtkWidget *msgDateLabel = gtk_label_new(messageInfo->date);
    gtk_widget_set_halign(msgDateLabel, GTK_ALIGN_END);

    // Message text
    GtkWidget *msgTextLabel = gtk_label_new(messageInfo->text);
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
    if (*currentDialogID == messageInfo->ID && *currentDialogID != -1)
        lastAdj = gtk_adjustment_get_upper(gtk_list_box_get_adjustment(currentDialog->msgList));

    // Finish
    gtk_container_add(GTK_CONTAINER(eventBox), msgMainBox);
    gtk_list_box_insert(currentDialog->msgList, eventBox, -1);

    // Подсоединяем сигналы: выбор сообщения, открытие меню по ПКМ
    g_signal_connect(eventBox, "button-press-event", (GCallback) processMsgSelecting, currentDialog->msgList);
    g_signal_connect(eventBox, "button-release-event", (GCallback) processMsgMenu, g_list_nth_data(additionalInfo, MSG_MENU));

    gtk_widget_show_all(eventBox);

    // Освобождаем память, если был запрос на отправку сообщения
    if (messageInfo->request == SEND_MESSAGE)
        g_free(messageInfo);

    return FALSE;
}

gboolean serverRequest_SendFriendRequest(void *data[2]) {
    // Распакуем полученную информацию
    FullUserInfo *userInfo =  data[0];
    GList *additionalInfo = data[1];

    GtkListBox *friendRequestListBox = g_list_nth_data(additionalInfo, FRIEND_REQUEST_LIST_BOX);

    // Если пришёл запрос с user ID == -1, то запрос уже существует. Если ID == -2 -> пользователь не существует
    if (userInfo->ID == -1) {
        popupNotification("Request already exists", g_list_nth_data(additionalInfo, POPUP_LABEL));
        return FALSE;
    }
    if (userInfo->ID == -2) {
        popupNotification("User with this username doesn't exist", g_list_nth_data(additionalInfo, POPUP_LABEL));
        return FALSE;
    }

    // I'm a messenger of this request. All is alright
    if (userInfo->ID == -3) {
        popupNotification("Request has been sent", g_list_nth_data(additionalInfo, POPUP_LABEL));
        return FALSE;
    }

    // I'm a receiver of this request
    // Main gtkBox
    GtkWidget *mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *nameLabel = gtk_label_new(userInfo->username);
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

    // Освободим память, если был запрос на отправку
    if (userInfo->request == SEND_FRIEND_REQUEST)
        g_free(userInfo);

    gtk_widget_show_all(mainBox);

    return FALSE;
}

gboolean serverRequest_RemoveFriend(void *data[2]) {
    // Распакуем полученную информацию
    FullUserInfo *userInfo = data[0];
    GList *additionalInfo = data[1];

    GtkListBox *friendsListBox = g_list_nth_data(additionalInfo, FRIENDS_LIST_BOX);

    // Возьмём список друзей и найдём того, кого мы хотим удалить
    GList *friends = gtk_container_get_children(GTK_CONTAINER(friendsListBox));
    GList *currentRow = friends;
    while (currentRow != NULL) {
        FullUserInfo *user = g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(currentRow->data))))), "Data");
        if (!strcmp(user->username, userInfo->username)) {
            gtk_list_box_select_row(friendsListBox, currentRow->data);
            removeFriend(NULL, additionalInfo);
            break;
        }

        currentRow = currentRow->next;
    }

    // Освободим память, если был запрос на удаление друга
    if (userInfo->request == REMOVE_FRIEND)
        g_free(userInfo);

    return FALSE;
}

gboolean serverRequest_LeaveDialog(void *data[2]) {
    // Распакуем полученную информацию
    FullUserInfo *userInfo = data[0];
    GList *additionalInfo = data[1];

    GList *dialogsList = g_list_nth_data(additionalInfo, DIALOGS_LIST);
    GList *temp = dialogsList;

    // Преобразуем строковое ID в число
    int ID = strtol(userInfo->additionalInfo, NULL, 10);
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
            if (!strcmp(gtk_label_get_text(GTK_LABEL(gtk_bin_get_child(GTK_BIN(currentRow->data)))), userInfo->username)) {
                gtk_widget_destroy(currentRow->data);
                break;
            }

            currentRow = currentRow->next;
        }
    }

    // Освободим память, если был запрос выход из диалога
    if (userInfo->request == LEAVE_DIALOG)
        g_free(userInfo);

    return FALSE;
}

void serverRequest_FriendIsOnline(FullUserInfo userInfo, GList *additionalInfo) {

}

void serverRequest_FriendDisconnect(FullUserInfo userInfo, GList *additionalInfo) {

}