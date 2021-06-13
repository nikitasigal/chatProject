#include "chat.h"
#include "login.h"
#include "ServerHandler/clientCommands.h"

gdouble lastAdj = 0;

gdouble g_abs(gdouble number) {
    return number < 0 ? -number : number;
}

void sizeAllocate(GtkWidget *msgListBox, GdkRectangle *allocation, int *dialogIsJustOpened) {
    if (*dialogIsJustOpened) {
        GtkAdjustment *adjustment = gtk_list_box_get_adjustment(GTK_LIST_BOX(msgListBox));
        gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment));
        gtk_list_box_set_adjustment(GTK_LIST_BOX(msgListBox), adjustment);
        *dialogIsJustOpened = FALSE;
        return;
    }

    GtkAdjustment *adjustment = gtk_list_box_get_adjustment(GTK_LIST_BOX(msgListBox));
    gdouble curAdj = gtk_adjustment_get_value(adjustment);
    gdouble pageSize = gtk_adjustment_get_page_size(adjustment);
    gdouble maxLastAdj = gtk_adjustment_get_upper(adjustment);
    gdouble result = maxLastAdj - (maxLastAdj - lastAdj) - curAdj - pageSize;
    if (g_abs(result) < 1e-6) {
        gtk_adjustment_set_value(adjustment, maxLastAdj);
        gtk_list_box_set_adjustment(GTK_LIST_BOX(msgListBox), adjustment);
    }
}

G_MODULE_EXPORT void processMsgSelecting(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    GtkWidget *row = gtk_widget_get_parent(widget);

    if (event->button.button == GDK_BUTTON_PRIMARY) {
        if (!gtk_list_box_row_is_selected(GTK_LIST_BOX_ROW(row)))
            gtk_list_box_select_row(user_data, GTK_LIST_BOX_ROW(row));
        else
            gtk_list_box_unselect_row(user_data, GTK_LIST_BOX_ROW(row));
    }
}

void processMsgMenu(GtkWidget *widget, GdkEvent *event, GtkMenu *msgMenu) {
    GtkWidget *row = gtk_widget_get_parent(widget);
    if (event->button.button == GDK_BUTTON_SECONDARY && gtk_list_box_row_is_selected(GTK_LIST_BOX_ROW(row)))
        gtk_menu_popup_at_pointer(GTK_MENU(msgMenu), event);
}

void enterChatClicked(GtkEntry *entry, GList *additionalInfo) {
    int *currentDialogID = g_list_nth_data(additionalInfo, CURRENT_DIALOG_ID);
    GList *dialogList = g_list_nth_data(additionalInfo, DIALOGS_LIST);
    GList *temp = dialogList;
    while (temp != NULL) {
        Dialog *currentDialog = temp->data;
        if (currentDialog->ID == *currentDialogID) {
            GList *data = NULL;
            data = g_list_append(data, currentDialog);
            data = g_list_append(data, additionalInfo);
            sendMessage(NULL, data);
            break;
        }

        temp = temp->next;
    }
}

void sendMessage(GtkWidget *button, GList *data) {
    // Разархивируем
    Dialog *currentDialog = g_list_nth_data(data, 0);
    GList *additionalInfo = g_list_nth_data(data, 1);
    SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
    GtkWidget *dialogEntry = g_list_nth_data(additionalInfo, CHAT_ENTRY);
    FullUserInfo *currentUser = g_list_nth_data(additionalInfo, CURRENT_USER);

    gtk_widget_grab_focus(GTK_WIDGET(dialogEntry));

    // Message from entry
    char message[TEXT_SIZE] = {0};
    strcpy(message, gtk_entry_get_text(GTK_ENTRY(dialogEntry)));
    if (strlen(message) == 0)
        return;
    gtk_entry_set_text(GTK_ENTRY(dialogEntry), "");

    // Соберём инфу о сообщении TODO инфа о пользователе
    FullMessageInfo messageInfo;
    messageInfo.ID = currentDialog->ID;
    strcpy(messageInfo.firstName, currentUser->firstName);
    strcpy(messageInfo.lastName, currentUser->secondName);
    strcpy(messageInfo.login, currentUser->username);
    strcpy(messageInfo.text, message);

    clientRequest_SendMessage(*serverDescriptor, messageInfo);
}

void dialogAddUser(GtkEntry *entry, GList *additionalInfo) {
    // Распакуем информацию
    SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
    FullUserInfo *currentUser = g_list_nth_data(additionalInfo, CURRENT_USER);
    int *currentDialogID = g_list_nth_data(additionalInfo, CURRENT_DIALOG_ID);
    GList *dialogsList = g_list_nth_data(additionalInfo, DIALOGS_LIST);
    GtkWidget *dialogUserViewport = g_list_nth_data(additionalInfo, DIALOG_USERS_VIEWPORT);
    GtkListBox *friendsListBox = g_list_nth_data(additionalInfo, FRIENDS_LIST_BOX);
    GList *friendsList = gtk_container_get_children(GTK_CONTAINER(friendsListBox));

    // Достанем текст из ввода
    char login[NAME_SIZE] = {0};
    strcpy(login, gtk_entry_get_text(entry));

    // Проверим корректность ввода
    if (strlen(login) == 0)
        return;

    if (!strcmp(currentUser->username, login)) {
        popupNotification("You are already in this chat");
        return;
    }

    // Найдём этот логин среди друзей
    GList *temp = friendsList;
    while (temp != NULL) {
        // Нашли
        GtkWidget *label = gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(temp->data))));
        FullUserInfo *currentFriend = g_object_get_data(G_OBJECT(label), "Data");
        if (!strcmp(currentFriend->username, login))
            break;

        temp = temp->next;
    }

    // Не нашли такого друга
    if (temp == NULL) {
        popupNotification("This user isn't your friend");
        return;
    }

    // Поищем среди участников диалога
    GtkWidget *listBox = gtk_bin_get_child(GTK_BIN(dialogUserViewport));
    GList *users = gtk_container_get_children(GTK_CONTAINER(listBox));
    GList *temp2 = users;
    while (temp2 != NULL) {
        if (!strcmp(gtk_label_get_text(GTK_LABEL(gtk_bin_get_child(GTK_BIN(temp2->data)))), login)) {
            popupNotification("This user already in the chat");
            return;
        }

        temp2 = temp2->next;
    }

    FullUserInfo user;
    strcpy(user.username, login);
    user.ID = *currentDialogID;

    clientRequest_DialogAddUser(*serverDescriptor, user);
}