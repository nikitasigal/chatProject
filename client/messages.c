#include "messages.h"
#include "chat.h"
#include "login.h"

void createDialog(GtkButton *button, GList *appDialogsMenuList) {
    // Распакуем данные
    GtkListBox *createDialogFriendsBoxList = g_list_nth_data(appDialogsMenuList, CREATE_DIALOG_FRIENDS_LIST_BOX);
    GtkEntry *createDialogEntry = g_list_nth_data(appDialogsMenuList, CREATE_DIALOG_ENTRY);
    SOCKET *serverDescriptor = g_list_nth_data(appDialogsMenuList, SERVER_SOCKET);

    // Проверим, не пустое ли поле имени диалога
    if (strlen(gtk_entry_get_text(createDialogEntry)) == 0) {
        printf("Dialog entry is empty.\n");
        popupNotification("Dialog name can't be empty.");
        return;
    }

    // Заполним инфу о диалоге со стороны клиента
    char dialogName[NAME_SIZE] = {0};
    strcpy(dialogName, gtk_entry_get_text(createDialogEntry));
    FullDialogInfo dialogInfo = {-1};
    strcpy(dialogInfo.dialogName, dialogName);

    // Соберём всех выбранных друзей
    GList *selectedFriends = gtk_list_box_get_selected_rows(createDialogFriendsBoxList);
    GList *temp = selectedFriends;
    while (temp != NULL) {
        FullUserInfo *tempUser = g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(temp->data))))), "Data");
        dialogInfo.users[dialogInfo.usersNumber].ID = tempUser->ID;
        strcpy(dialogInfo.users[dialogInfo.usersNumber].firstName, tempUser->firstName);
        strcpy(dialogInfo.users[dialogInfo.usersNumber].secondName, tempUser->secondName);
        strcpy(dialogInfo.users[dialogInfo.usersNumber].username, tempUser->username);
        ++dialogInfo.usersNumber;

        temp = temp->next;
    }

    // Создадим запрос и отправим его на сервер
    clientRequest_CreateDialog(*serverDescriptor, dialogInfo);

    // Удалим список выбранных строк
    g_list_free(selectedFriends);
    gtk_list_box_unselect_all(createDialogFriendsBoxList);

    // Почистим entry
    gtk_entry_set_text(createDialogEntry, "");
}

void newOpenDialog(GtkWidget *button, GList *data) {
    // Step 0
    Dialog *newDialog = g_object_get_data(G_OBJECT(button), "Data");
    GList *additionalInfo = g_list_nth_data(data, 1);
    GtkWidget *chatEntry = g_list_nth_data(additionalInfo, CHAT_ENTRY);
    GtkWidget *chatButton = g_list_nth_data(additionalInfo, CHAT_BUTTON);
    GtkWidget *dialogViewport = g_list_nth_data(additionalInfo, DIALOG_VIEWPORT);
    GtkWidget *dialogUsersViewport = g_list_nth_data(additionalInfo, DIALOG_USERS_VIEWPORT);
    GtkWidget *dialogUsersScrolledWindow = g_list_nth_data(additionalInfo, DIALOG_USERS_SCROLLED_WINDOW);
    GtkWidget *dialogsMenuBox = g_list_nth_data(additionalInfo, DIALOGS_MENU_BOX);

    // Step 0.1
    gtk_container_remove(GTK_CONTAINER(dialogViewport), dialogsMenuBox);

    // Step 1
    gtk_container_add(GTK_CONTAINER(dialogViewport), GTK_WIDGET(newDialog->msgList));

    // Step 1.1
    g_list_nth(additionalInfo, CURRENT_DIALOG_ID)->data = &(newDialog->ID);

    // Step 2
    gtk_container_add(GTK_CONTAINER(dialogUsersViewport), GTK_WIDGET(newDialog->userList));

    // Step 3
    static unsigned signalHandler = 0;
    if (signalHandler != 0)
        g_signal_handler_disconnect(chatButton, signalHandler);

    gboolean *dialogIsJustOpened = g_list_nth_data(additionalInfo, DIALOG_IS_JUST_OPENED);
    *dialogIsJustOpened = TRUE;

    GList *temp = NULL;
    temp = g_list_append(temp, newDialog);
    temp = g_list_append(temp, data);
    signalHandler = g_signal_connect(chatButton, "clicked", (GCallback) sendMessage, temp);

    gtk_widget_show_all(dialogViewport);
    gtk_widget_show_all(dialogUsersScrolledWindow);
    gtk_widget_show(chatEntry);
    gtk_widget_show(chatButton);
}

void openDialog(GtkWidget *button, ChatInfo *data) {
    /*
     * Стадии открытия диалога:
     * 1) Загрузить сообщения (msgList)
     * 2) Загрузить пользователей диалога (userList)
     * 3) Показать строку ввода и кнопку отправки сообщения
     */
    // Разархивируем данные
    GList *chatList = data->chatList;
    GList *additionalInfo = data->additionalInfo;
    GtkWidget *chatEntry = g_list_nth_data(additionalInfo, 0);
    GtkWidget *chatButton = g_list_nth_data(additionalInfo, 1);
    GtkWidget *dialogUsersScrolledWindow = g_list_nth_data(additionalInfo, 2);

    GtkWidget *dialogBox = gtk_widget_get_parent(button);
    GtkWidget *dialogListBox = gtk_widget_get_parent(gtk_widget_get_parent(dialogBox));
    GtkWidget *dialogViewport = gtk_widget_get_parent(dialogListBox);
    gtk_container_remove(GTK_CONTAINER(dialogViewport), dialogListBox);

    GList *children = gtk_container_get_children(GTK_CONTAINER(dialogBox));
    const gchar *IDString = gtk_label_get_text(g_list_nth_data(children, 0));
    long long ID = g_ascii_strtoll(IDString, NULL, 10);

    // Найдём этот чат
    GList *temp = chatList;
    while (temp != NULL) {
        ChatHistory *tempHistory = temp->data;
        // Нашли?
        if (tempHistory->ID == ID) {
            gtk_container_add(GTK_CONTAINER(dialogViewport), tempHistory->msgList);
            //currentDialogListGlobal = tempHistory->msgList;
            //currentDialogIDGlobal = ID;
            break;
        } else {
            temp = temp->next;
        }
    }

    gtk_widget_show_all(dialogViewport);
    gtk_widget_show(chatEntry);
    gtk_widget_show(chatButton);
    gtk_widget_show(dialogUsersScrolledWindow);
}

void
newCreateChat(int ID, const char *name, const int usersID[30], int usersIDCount, GList *friendsList, GList *dialogsList,
              GtkListBox *dialogsListBox, GList *additionalInfo, GtkBox *dialogMenuBox) {
    // Step 1
    Dialog *newDialog = g_malloc(sizeof(Dialog));
    newDialog->ID = ID;
    newDialog->userList = GTK_LIST_BOX(gtk_list_box_new());
    newDialog->msgList = GTK_LIST_BOX(gtk_list_box_new());
    strcpy(newDialog->name, name);
    gtk_list_box_set_selection_mode(newDialog->msgList, GTK_SELECTION_MULTIPLE);
    g_object_ref(newDialog->msgList);
    gtk_list_box_set_selection_mode(newDialog->userList, GTK_SELECTION_NONE);
    g_object_ref(newDialog->userList);

    // Step 2
    for (int i = 0; i < usersIDCount; ++i) {
        GList *temp = friendsList;
        while (temp != NULL) {
            User *curUser = temp->data;
            if (usersID[i] == curUser->ID) {
                // Нашли
                // TODO fullName
                GtkWidget *newLabel = gtk_label_new(curUser->login);
                gtk_list_box_insert(newDialog->userList, newLabel, -1);
                break;
            } else {
                // Не нашли
                temp = temp->next;
            }
        }
    }

    // Step 3
    dialogsList = g_list_append(dialogsList, newDialog);

    // Step 4
    // Преобразовываем ID в строку
    char IDString[TEXT_SIZE] = {0};
    sprintf(IDString, "%d", ID);

    // Создаём окно с кнопкой беседы, ID и ссылку на чат
    GtkWidget *dialogBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget *dialogLabelWithID = gtk_label_new(IDString);
    GtkWidget *dialogButton = gtk_button_new_with_label(name);
    g_object_set_data(G_OBJECT(dialogButton), "Data", newDialog);
    gtk_container_add(GTK_CONTAINER(dialogBox), dialogLabelWithID);
    gtk_container_add(GTK_CONTAINER(dialogBox), dialogButton);

    // Добавим созданный диалог в список диалогов
    gtk_list_box_insert(dialogsListBox, dialogBox, -1);

    // Подключаем сигнал нажатия кнопки. При нажатии открывается диалог
    GList *data = NULL;
    data = g_list_append(data, newDialog);
    data = g_list_append(data, additionalInfo); // entry and send-button
    g_signal_connect(dialogButton, "clicked", (GCallback) newOpenDialog, data);
    //g_signal_connect(newDialog->msgList, "size-allocate", (GCallback) sizeAllocate, NULL);

    // Сделаем ID невидимым
    gtk_widget_set_no_show_all(dialogLabelWithID, TRUE);

    gtk_widget_show_all(dialogBox);
}

void createChat(char *name, int ID, GtkListBox *dialogList, GList *chatList, GList *additionalInfo) {
    /*
     * Создание чата:
     * 1) Создать переменную Dialog и заполнить поля: ID, name
     * 2) Заполнить переменную userList: label с fullName
     * 3) Внести переменную типа Dialog в dialogList
     * 4) Дополнить dialogListBox
     */
    // Преобразовываем ID в строку
    char IDString[TEXT_SIZE] = {0};
    sprintf(IDString, "%d", ID);

    // Создаём окно с кнопкой беседы, ID и ссылкой на чат
    GtkWidget *dialogBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *dialogLabelWithID = gtk_label_new(IDString);
    GtkWidget *dialogButton = gtk_button_new_with_label(name);
    gtk_container_add(GTK_CONTAINER(dialogBox), dialogLabelWithID);
    gtk_container_add(GTK_CONTAINER(dialogBox), dialogButton);

    // Создадим новый чат с информацией о нём
    ChatHistory *chat = (ChatHistory *) g_malloc(sizeof(ChatHistory));
    chat->msgList = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(chat->msgList), GTK_SELECTION_MULTIPLE);
    g_object_ref(chat->msgList);
    chat->ID = ID;
    strcpy(chat->name, name);
    chatList = g_list_append(chatList, chat);

    // DEBUG
    //insertMessageInChat("Kostya", "123", "Hello wordl!", chat->msgList);

    // Добавим созданный диалог в список диалогов
    gtk_list_box_insert(dialogList, dialogBox, -1);

    // Подключаем сигнал нажатия кнопки. При нажатии открывается диалог
    ChatInfo *data = g_malloc(sizeof(ChatInfo));
    data->ID = ID;
    data->chatList = chatList;
    data->additionalInfo = additionalInfo;
    g_signal_connect(dialogButton, "clicked", (GCallback) openDialog, data);
    //g_signal_connect(chat->msgList, "size-allocate", (GCallback) sizeAllocate, NULL);

    // Сделаем ID невидимым
    gtk_widget_set_no_show_all(dialogLabelWithID, TRUE);

    gtk_widget_show_all(dialogBox);
}