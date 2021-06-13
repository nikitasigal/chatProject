#include "friends.h"
#include "login.h"
#include "chat.h"
#include "messages.h"
#include "ServerHandler/clientCommands.h"

void processFriendSelecting(GtkWidget *widget, GdkEvent *event, GList *additionalInfo) {
    GtkListBox *friendsListBox = g_list_nth_data(additionalInfo, FRIENDS_LIST_BOX);
    GtkWidget *friendMenu = g_list_nth_data(additionalInfo, FRIEND_MENU);

    GtkWidget *row = gtk_widget_get_parent(widget);
    if (event->button.button == GDK_BUTTON_PRIMARY) {
        if (!gtk_list_box_row_is_selected(GTK_LIST_BOX_ROW(row))) {
            gtk_list_box_unselect_all(GTK_LIST_BOX(gtk_widget_get_parent(row)));
            gtk_list_box_select_row(friendsListBox, GTK_LIST_BOX_ROW(row));
        }
        else
            gtk_list_box_unselect_row(friendsListBox, GTK_LIST_BOX_ROW(row));
    }

    if (event->button.button == GDK_BUTTON_SECONDARY) {
        gtk_list_box_unselect_all(GTK_LIST_BOX(gtk_widget_get_parent(row)));
        gtk_list_box_select_row(friendsListBox, GTK_LIST_BOX_ROW(row));
        gtk_menu_popup_at_pointer(GTK_MENU(friendMenu), event);
    }
}

void sendFriendRequest(GtkButton *button, GList *additionalInfo) {
    // Распакуем данные
    SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
    GtkEntry *friendRequestSendEntry = g_list_nth_data(additionalInfo, FRIEND_SEND_REQUEST_ENTRY);
    FullUserInfo *currentUser = g_list_nth_data(additionalInfo, CURRENT_USER);
    strcpy(currentUser->additionalInfo, gtk_entry_get_text(GTK_ENTRY(friendRequestSendEntry)));

    // Проверим, не хотим ли мы случайно добавить себя же
    if (!strcmp(currentUser->username, gtk_entry_get_text(friendRequestSendEntry))) {
        popupNotification("You can't make friends with yourself. You just can't stand it");
        return;
    }

    // Проверим, не пустое ли поле имени диалога
    if (strlen(gtk_entry_get_text(friendRequestSendEntry)) == 0) {
        popupNotification("You should use username of the user you want to make friends");
        return;
    }

    // Создадим запрос и отправим его на сервер
    clientRequest_SendFriendRequest(*serverDescriptor, *currentUser);

    // Почистим entry
    gtk_entry_set_text(friendRequestSendEntry, "");
}

void acceptFriendRequest(GtkWidget *button, GList *additionalInfo) {
    SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
    FullUserInfo *currentUser = g_list_nth_data(additionalInfo, CURRENT_USER);

    // Delete request on interface
    GtkWidget *requestBox = gtk_widget_get_parent(gtk_widget_get_parent(button));
    strcpy(currentUser->additionalInfo, gtk_label_get_text(g_list_nth_data(gtk_container_get_children(GTK_CONTAINER(requestBox)), 0)));
    gtk_widget_destroy(requestBox);

    clientRequest_FriendRequestAccepted(*serverDescriptor, *currentUser);
}

void declineFriendRequest(GtkWidget *button, GList *additionalInfo) {
    SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
    FullUserInfo *currentUser = g_list_nth_data(additionalInfo, CURRENT_USER);

    // Delete request on interface
    GtkWidget *requestBox = gtk_widget_get_parent(gtk_widget_get_parent(button));
    strcpy(currentUser->additionalInfo, gtk_label_get_text(g_list_nth_data(gtk_container_get_children(GTK_CONTAINER(requestBox)), 0)));
    gtk_widget_destroy(requestBox);

    clientRequest_FriendRequestDeclined(*serverDescriptor, *currentUser);
}

gboolean addFriend(void *data[2]) {
    FullUserInfo *user = data[0];
    GList *additionalInfo = data[1];

    // Распакуем информацию
    GtkListBox *friendsListBox = g_list_nth_data(additionalInfo, FRIENDS_LIST_BOX);
    GtkListBox *createDialogFriendsListBox = g_list_nth_data(additionalInfo, CREATE_DIALOG_FRIENDS_LIST_BOX);

    // Добавим в список друзей во вкладке создания диалога и друзей
    GString *fullName = g_string_new("");
    g_string_append_printf(fullName, "%s %s (%s)", user->firstName, user->lastName, user->username);
    GtkWidget *friendLabel = gtk_label_new(fullName->str);
    gtk_widget_set_size_request(friendLabel, -1, 30);
    GtkWidget *friendLabel2 = gtk_label_new(fullName->str);
    gtk_widget_set_size_request(friendLabel2, -1, 30);

    GtkWidget *friendsEventBox = gtk_event_box_new();

    gtk_list_box_insert(friendsListBox, friendsEventBox, -1);
    gtk_container_add(GTK_CONTAINER(friendsEventBox), friendLabel);

    //
    FullUserInfo *tempUser = malloc(sizeof(FullUserInfo));
    tempUser->ID = user->ID;
    strcpy(tempUser->username, user->username);
    strcpy(tempUser->firstName, user->firstName);
    strcpy(tempUser->lastName, user->lastName);
    GtkWidget *createDialogEventBox = gtk_event_box_new();
    gtk_list_box_insert(createDialogFriendsListBox, createDialogEventBox, -1);
    gtk_container_add(GTK_CONTAINER(createDialogEventBox), friendLabel2);
    g_object_set_data(G_OBJECT(friendLabel), "Data", tempUser);
    g_object_set_data(G_OBJECT(friendLabel2), "Data", tempUser);

    g_string_free(fullName, TRUE);

    g_signal_connect(createDialogEventBox, "button-press-event", (GCallback) processMsgSelecting, createDialogFriendsListBox);
    g_signal_connect(friendsEventBox, "button-press-event", (GCallback) processFriendSelecting, additionalInfo);

    if (user->request == SEND_FRIEND_REQUEST)
        g_free(user);

    gtk_widget_show_all(GTK_WIDGET(friendsListBox));
    gtk_widget_show_all(createDialogEventBox);

    return FALSE;
}

void removeFriend(GtkMenuItem *menuitem, GList *additionalInfo) {
    GtkListBox *friendsListBox = g_list_nth_data(additionalInfo, FRIENDS_LIST_BOX);
    GtkListBox *createDialogFriendsListBox = g_list_nth_data(additionalInfo, CREATE_DIALOG_FRIENDS_LIST_BOX);
    GtkListBoxRow *row = gtk_list_box_get_selected_row(friendsListBox);

    // Найдём этого пользователя в другом списке
    GList *rows = gtk_container_get_children(GTK_CONTAINER(createDialogFriendsListBox));
    GList *temp = rows;
    FullUserInfo *searchingUser = g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(row))))), "Data");
    while (temp != NULL) {
        GtkListBoxRow *tempRow = temp->data;
        FullUserInfo *user = g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(tempRow))))), "Data");
        if (!strcmp(user->username, searchingUser->username)) {
            gtk_widget_destroy(GTK_WIDGET(temp->data));
            break;
        }

        temp = temp->next;
    }
    g_list_free(rows);

    gtk_widget_destroy(GTK_WIDGET(row));

    // Отправим запрос на сервер
    SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
    FullUserInfo *user = g_list_nth_data(additionalInfo, CURRENT_USER);
    clientRequest_RemoveFriend(*serverDescriptor, *user);
}

void openPersonalDialog(GtkMenuItem *menuitem, GList *additionalInfo) {
    GtkListBox *friendsListBox = g_list_nth_data(additionalInfo, FRIENDS_LIST_BOX);
    GList *dialogsList = g_list_nth_data(additionalInfo, DIALOGS_LIST);
    FullUserInfo *currentUser = g_list_nth_data(additionalInfo, CURRENT_USER);
    SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);

    GtkListBoxRow *row = gtk_list_box_get_selected_row(friendsListBox);

    FullUserInfo *currentFriend = g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(row))))), "Data");
    GString *currentUserFullName = g_string_new("");
    g_string_printf(currentUserFullName, "%s %s (%s)", currentFriend->firstName, currentFriend->lastName, currentFriend->username);

    // Поищем этот диалог в списке
    GList *currentDialog = dialogsList;
    while (currentDialog != NULL) {
        Dialog *dialog = (Dialog *) currentDialog->data;
        if (!dialog->isGroup && !strcmp(dialog->name, currentUserFullName->str)) {
            // Нашли
            GList *tempList = NULL;
            tempList = g_list_append(tempList, dialog);
            tempList = g_list_append(tempList, additionalInfo);
            openDialog(NULL, tempList);

            return;
        }

        currentDialog = currentDialog->next;
    }

    // Не нашли, тогда создадим
    FullDialogInfo newDialog;
    strcpy(newDialog.name, currentUserFullName->str);

    // Добавим друга в беседу
    newDialog.userList[0].ID = currentFriend->ID;
    strcpy(newDialog.userList[0].firstName, currentFriend->firstName);
    strcpy(newDialog.userList[0].lastName, currentFriend->lastName);
    strcpy(newDialog.userList[0].username, currentFriend->username);

    // Добавим себя в беседу
    newDialog.userList[1].ID = currentUser->ID;
    strcpy(newDialog.userList[1].firstName, currentUser->firstName);
    strcpy(newDialog.userList[1].lastName, currentUser->lastName);
    strcpy(newDialog.userList[1].username, currentUser->username);

    newDialog.usersCount = 2;
    newDialog.isGroup = FALSE;
    newDialog.isSupposedToOpen = TRUE;

    clientRequest_CreateDialog(*serverDescriptor, newDialog);

    // Очистим строку
    g_string_free(currentUserFullName, TRUE);
}