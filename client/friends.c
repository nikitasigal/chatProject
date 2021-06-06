#include "friends.h"

void addFriend(int ID, char *firstName, char *lastName, char *login, GList **friendsList, GtkListBox *friendsListBox,
               GtkListBox *friendRequestListBoxDialogs) {
    FullUserInfo *friend = g_malloc(sizeof(User));
    friend->ID = ID;
    strcpy(friend->firstName, firstName);
    strcpy(friend->lastName, lastName);
    strcpy(friend->login, login);
    *friendsList = g_list_append(*friendsList, friend);

    // Отобразим в friendsListBox
    GString *fullName = g_string_new("");
    g_string_append_printf(fullName, "%s %s (%s)", firstName, lastName, login);
    GtkWidget *friendLabel = gtk_label_new(fullName->str);
    GtkWidget *friendLabel2 = gtk_label_new(fullName->str);
    gtk_list_box_insert(friendsListBox, friendLabel, -1);
    gtk_list_box_insert(friendRequestListBoxDialogs, friendLabel2, -1);
    g_object_set_data(G_OBJECT(friendLabel), "Data", friend);
    g_object_set_data(G_OBJECT(friendLabel2), "Data", friend);
    FullUserInfo *info = g_object_get_data(G_OBJECT(friendLabel), "Data");

    g_string_free(fullName, TRUE);
}