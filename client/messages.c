#include "messages.h"
#include "chat.h"
#include "login.h"
#include "ServerHandler/clientCommands.h"

void leaveDialog(GtkMenuItem *menuitem, GList *additionalInfo) {
    GList *dialogsList = g_list_nth_data(additionalInfo, DIALOGS_LIST);
    GtkListBoxRow *selectedRow = g_list_nth_data(additionalInfo, SELECTED_ROW);
    SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
    FullUserInfo *currentUser = g_list_nth_data(additionalInfo, CURRENT_USER);

    // Диалог, который мы хотим удалить
    Dialog *selectedDialog = g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(selectedRow))))), "Data");
    sprintf(currentUser->additionalInfo, "%d", selectedDialog->ID);

    // Удалим диалог из списка диалогов
    GList *temp = dialogsList;
    while(temp != NULL) {
        Dialog *currentDialog = temp->data;
        if (selectedDialog->ID == currentDialog->ID) {
            if (!currentDialog->isGroup) {
                popupNotification("You can't leave personal dialog");
                return;
            }
            g_list_nth(additionalInfo, DIALOGS_LIST)->data = g_list_delete_link(dialogsList, temp);
            break;
        }

        temp = temp->next;
    }

    // Удалим кнопку диалога
    gtk_widget_destroy(GTK_WIDGET(selectedRow));

    // Установим chatID беседы и отправим запрос
    clientRequest_LeaveDialog(*serverDescriptor, *currentUser);
}

void processDialogMenu(GtkWidget *widget, GdkEvent *event, GList *additionalInfo) {
    GtkMenu *dialogMenu = g_list_nth_data(additionalInfo, 0);
    GList *data = g_list_nth_data(additionalInfo, 1);
    g_list_nth(data, SELECTED_ROW)->data = gtk_widget_get_parent(widget);

    if (event->button.button == GDK_BUTTON_SECONDARY)
        gtk_menu_popup_at_pointer(dialogMenu, event);
}

void createDialog(GtkButton *button, GList *additionalInfo) {
    // Распакуем данные
    GtkListBox *createDialogFriendsBoxList = g_list_nth_data(additionalInfo, CREATE_DIALOG_FRIENDS_LIST_BOX);
    GtkEntry *createDialogEntry = g_list_nth_data(additionalInfo, CREATE_DIALOG_ENTRY);
    SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
    FullUserInfo *currentUser = g_list_nth_data(additionalInfo, CURRENT_USER);

    // Проверим, не пустое ли поле имени диалога
    if (strlen(gtk_entry_get_text(createDialogEntry)) == 0) {
        printf("WARNING, file - 'messages.c', foo - 'createDialog': Dialog entry is empty.\n");
        popupNotification("Dialog name can't be empty.");
        return;
    }

    // Заполним инфу о диалоге со стороны клиента
    char dialogName[DIALOG_NAME_SIZE] = {0};
    strcpy(dialogName, gtk_entry_get_text(createDialogEntry));
    FullDialogInfo dialogInfo = {-1};
    dialogInfo.isGroup = TRUE;
    dialogInfo.isSupposedToOpen = FALSE;
    strcpy(dialogInfo.name, dialogName);

    // Соберём всех выбранных друзей
    GList *selectedFriends = gtk_list_box_get_selected_rows(createDialogFriendsBoxList);
    GList *temp = selectedFriends;
    while (temp != NULL) {
        FullUserInfo *tempUser = g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(temp->data))))), "Data");
        dialogInfo.userList[dialogInfo.userCount].ID = tempUser->ID;
        strcpy(dialogInfo.userList[dialogInfo.userCount].firstName, tempUser->firstName);
        strcpy(dialogInfo.userList[dialogInfo.userCount].lastName, tempUser->lastName);
        strcpy(dialogInfo.userList[dialogInfo.userCount].username, tempUser->username);
        ++dialogInfo.userCount;

        temp = temp->next;
    }


    // Добавим и себя
    dialogInfo.userList[dialogInfo.userCount].ID = currentUser->ID;
    strcpy(dialogInfo.userList[dialogInfo.userCount].firstName, currentUser->firstName);
    strcpy(dialogInfo.userList[dialogInfo.userCount].lastName, currentUser->lastName);
    strcpy(dialogInfo.userList[dialogInfo.userCount].username, currentUser->username);
    ++dialogInfo.userCount;

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
    Dialog *newDialog;
    GList *additionalInfo;
    if (button != NULL) {
        newDialog = g_object_get_data(G_OBJECT(button), "Data");
        additionalInfo = data;
    } else {
        // Персональный диалог
        newDialog = g_list_nth_data(data, 0);
        additionalInfo = g_list_nth_data(data, 1);
    }
    GtkWidget *chatEntry = g_list_nth_data(additionalInfo, CHAT_ENTRY);
    GtkWidget *chatButton = g_list_nth_data(additionalInfo, CHAT_BUTTON);
    GtkWidget *dialogViewport = g_list_nth_data(additionalInfo, DIALOG_VIEWPORT);
    GtkWidget *dialogUsersViewport = g_list_nth_data(additionalInfo, DIALOG_USERS_VIEWPORT);
    GtkWidget *dialogUsersScrolledWindow = g_list_nth_data(additionalInfo, DIALOG_USERS_SCROLLED_WINDOW);
    GtkWidget *dialogNameLabel = g_list_nth_data(additionalInfo, DIALOG_NAME_LABEL);
    GtkWidget *dialogTaskBar = g_list_nth_data(additionalInfo, DIALOG_TASK_BAR);
    GtkWidget *menuButton = g_list_nth_data(additionalInfo, MENU_BUTTON);
    GtkWidget *dialogAdditionalLabel = g_list_nth_data(additionalInfo, DIALOG_ADDITIONAL_LABEL);

    // Step 0.0
    gtk_label_set_text(GTK_LABEL(dialogNameLabel), newDialog->name);

    gtk_container_remove(GTK_CONTAINER(dialogViewport), gtk_bin_get_child(GTK_BIN(dialogViewport)));

    // Step 1
    gtk_container_add(GTK_CONTAINER(dialogViewport), GTK_WIDGET(newDialog->msgList));

    // Step 1.1
    g_list_nth(additionalInfo, CURRENT_DIALOG_ID)->data = &(newDialog->ID);

    // Step 2
    if (/*button != NULL &&*/ newDialog->isGroup)
        gtk_container_add(GTK_CONTAINER(dialogUsersViewport), GTK_WIDGET(newDialog->userList));

    // Step 3
    static unsigned signalHandler = 0;
    if (signalHandler != 0)
        g_signal_handler_disconnect(chatButton, signalHandler);

    gboolean *dialogIsJustOpened = g_list_nth_data(additionalInfo, DIALOG_IS_JUST_OPENED);
    *dialogIsJustOpened = TRUE;

    GList *temp = NULL;
    temp = g_list_append(temp, newDialog);
    temp = g_list_append(temp, additionalInfo);
    signalHandler = g_signal_connect(chatButton, "clicked", (GCallback) sendMessage, temp);

    gtk_widget_show_all(dialogViewport);
    gtk_widget_show(chatEntry);
    gtk_widget_show(chatButton);
    gtk_widget_show_all(dialogTaskBar);
    if (/*button != NULL &&*/ newDialog->isGroup) {
        gtk_widget_set_margin_start(dialogNameLabel, 95);
        gtk_widget_show_all(dialogUsersScrolledWindow);
        gtk_widget_show_all(menuButton);
        gtk_widget_show(dialogAdditionalLabel);
    }

    if (!newDialog->isGroup) {
        gtk_widget_set_margin_start(dialogNameLabel, 0);
        gtk_widget_hide(menuButton);
        gtk_widget_hide(dialogAdditionalLabel);
    }

    gtk_widget_grab_focus(chatEntry);

    // Отправим запрос на подгрузку сообщений
    if (!newDialog->isOpened) {
        newDialog->isOpened = TRUE;
        FullDialogInfo dialogInfo;
        dialogInfo.ID = newDialog->ID;
        SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
        clientRequest_LoadMessages(*serverDescriptor, dialogInfo);
    }
}