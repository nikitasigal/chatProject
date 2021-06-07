#include "friends.h"
#include "clientCommand.h"
#include "login.h"
#include "chat.h"

void removeFriend(GtkMenuItem *menuitem, GList *data) {
    GtkListBox *friendsListBox = g_list_nth_data(data, FRIENDS_LIST_BOX);
    GtkListBox *createDialogFriendsListBox = g_list_nth_data(data, CREATE_DIALOG_FRIENDS_LIST_BOX);
    GtkListBoxRow *row = gtk_list_box_get_selected_row(friendsListBox);

    // Найдём этого пользователя в другом списке
    GList *rows = gtk_container_get_children(GTK_CONTAINER(createDialogFriendsListBox));
    GList *temp = rows;
    FullUserInfo *searchingUser = g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(row))))), "Data");
    while (temp != NULL) {
        GtkListBoxRow *tempRow = temp->data;
        FullUserInfo *user = g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(tempRow))))), "Data");
        if (!strcmp(user->login, searchingUser->login)) {
            gtk_widget_destroy(GTK_WIDGET(temp->data));
            break;
        }

        temp = temp->next;
    }
    g_list_free(rows);

    gtk_widget_destroy(GTK_WIDGET(row));

    // Отправим запрос на сервер
    SOCKET *serverDescriptor = g_list_nth_data(data, SERVER_SOCKET);
    FullUserInfo *user = g_list_nth_data(data, CURRENT_USER);
    clientRequest_RemoveFriend(*serverDescriptor, *user);
}

void processFriendMenu(GtkWidget *widget, GdkEvent *event, GtkMenu *friendMenu) {
    GtkWidget *row = gtk_widget_get_parent(widget);
    if (event->button.button == GDK_BUTTON_SECONDARY && gtk_list_box_row_is_selected(GTK_LIST_BOX_ROW(row)))
        gtk_menu_popup_at_pointer(friendMenu, event);
}

void processFriendSelecting(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    GtkWidget *row = gtk_widget_get_parent(widget);

    if (event->button.button == GDK_BUTTON_PRIMARY) {
        if (!gtk_list_box_row_is_selected(GTK_LIST_BOX_ROW(row))) {
            gtk_list_box_unselect_all(GTK_LIST_BOX(gtk_widget_get_parent(row)));
            gtk_list_box_select_row(user_data, GTK_LIST_BOX_ROW(row));
        }
        else
            gtk_list_box_unselect_row(user_data, GTK_LIST_BOX_ROW(row));
    }
}

void acceptFriendRequest(GtkWidget *button, GList *additionalInfo) {
    SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
    FullUserInfo *currentUser = g_list_nth_data(additionalInfo, CURRENT_USER);

    // Delete request on interface
    GtkWidget *requestBox = gtk_widget_get_parent(gtk_widget_get_parent(button));
    gtk_widget_destroy(requestBox);

    clientRequest_FriendRequestAccepted(*serverDescriptor, *currentUser);
}

void declineFriendRequest(GtkWidget *button, GList *additionalInfo) {
    SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
    FullUserInfo *currentUser = g_list_nth_data(additionalInfo, CURRENT_USER);

    // Delete request on interface
    GtkWidget *requestBox = gtk_widget_get_parent(gtk_widget_get_parent(button));
    gtk_widget_destroy(requestBox);

    clientRequest_FriendRequestDeclined(*serverDescriptor, *currentUser);
}

void sendFriendRequest(GtkButton *button, GList *additionalInfo) {
    // Распакуем данные
    SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
    GtkEntry *friendRequestSendEntry = g_list_nth_data(additionalInfo, FRIEND_SEND_REQUEST_ENTRY);
    FullUserInfo *currentUser = g_list_nth_data(additionalInfo, CURRENT_USER);
    strcpy(currentUser->additionalInfo, gtk_entry_get_text(GTK_ENTRY(friendRequestSendEntry)));

    // Проверим, не хотим ли мы случайно добавить себя же
    if (!strcmp(currentUser->login, gtk_entry_get_text(friendRequestSendEntry))) {
        popupNotification("You can't make friends with yourself. You just can't stand it");
        return;
    }

    // Проверим, не пустое ли поле имени диалога
    if (strlen(gtk_entry_get_text(friendRequestSendEntry)) == 0) {
        popupNotification("You should use login of the user you want to make friends");
        return;
    }

    // Создадим запрос и отправим его на сервер
    clientRequest_SendFriendRequest(*serverDescriptor, *currentUser);

    // Почистим entry
    gtk_entry_set_text(friendRequestSendEntry, "");
}

void addFriend(FullUserInfo *user, GList *additionalInfo) {
    // Распакуем информацию
    GtkListBox *friendsListBox = g_list_nth_data(additionalInfo, FRIENDS_LIST_BOX);
    GtkListBox *createDialogFriendsListBox = g_list_nth_data(additionalInfo, CREATE_DIALOG_FRIENDS_LIST_BOX);
    GtkMenu *friendMenu = g_list_nth_data(additionalInfo, FRIEND_MENU);

    // Добавим в список друзей во вкладке создания диалога и друзей
    GString *fullName = g_string_new("");
    g_string_append_printf(fullName, "%s %s (%s)", user->firstName, user->lastName, user->login);
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
    strcpy(tempUser->firstName, user->firstName);
    strcpy(tempUser->lastName, user->lastName);
    strcpy(tempUser->login, user->login);
    GtkWidget *createDialogEventBox = gtk_event_box_new();
    gtk_list_box_insert(createDialogFriendsListBox, createDialogEventBox, -1);
    gtk_container_add(GTK_CONTAINER(createDialogEventBox), friendLabel2);
    g_object_set_data(G_OBJECT(friendLabel), "Data", tempUser);
    g_object_set_data(G_OBJECT(friendLabel2), "Data", tempUser);

    g_string_free(fullName, TRUE);

    g_signal_connect(createDialogEventBox, "button-press-event", (GCallback) processMsgSelecting, createDialogFriendsListBox);
    g_signal_connect(friendsEventBox, "button-press-event", (GCallback) processFriendSelecting, friendsListBox);
    g_signal_connect(friendsEventBox, "button-release-event", (GCallback) processFriendMenu, friendMenu);

    gtk_widget_show_all(GTK_WIDGET(friendsListBox));
    gtk_widget_show_all(createDialogEventBox);
}