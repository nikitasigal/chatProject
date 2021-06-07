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

    // Заполним инфу о диалоге со стороны клиента TODO
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
        strcpy(dialogInfo.users[dialogInfo.usersNumber].lastName, tempUser->lastName);
        strcpy(dialogInfo.users[dialogInfo.usersNumber].login, tempUser->login);
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

void openDialog(GtkWidget *button, GList *data) {
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

    // Отправим запрос на подгрузку сообщений
    if (!newDialog->isOpened) {
        newDialog->isOpened = TRUE;
        FullDialogInfo dialogInfo;
        dialogInfo.ID = newDialog->ID;
        SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
        clientRequest_LoadMessages(*serverDescriptor, dialogInfo);
    }
}