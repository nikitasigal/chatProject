#include "appMenu.h"

void gotoMessages(GtkWidget *button, GList *appDialogsMenuList) {
    // Раскроем список
    GtkWidget *chatEntry = g_list_nth_data(appDialogsMenuList, CHAT_ENTRY);
    GtkWidget *chatButton = g_list_nth_data(appDialogsMenuList, CHAT_BUTTON);
    GtkViewport *dialogViewport = g_list_nth_data(appDialogsMenuList, DIALOG_VIEWPORT);
    GtkWidget *dialogUsersScrolledWindow = g_list_nth_data(appDialogsMenuList, DIALOG_USERS_SCROLLED_WINDOW);
    GtkWidget *dialogUsersViewport = g_list_nth_data(appDialogsMenuList, DIALOG_USERS_VIEWPORT);
    GtkWidget *dialogMenuBox = g_list_nth_data(appDialogsMenuList, DIALOGS_MENU_BOX);
    GtkWidget *dialogNameLabel = g_list_nth_data(appDialogsMenuList, DIALOG_NAME_LABEL);

    gtk_container_remove(GTK_CONTAINER(dialogViewport), gtk_bin_get_child(GTK_BIN(dialogViewport)));
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(dialogUsersViewport));
    if (child != NULL)
        gtk_container_remove(GTK_CONTAINER(dialogUsersViewport), gtk_bin_get_child(GTK_BIN(dialogUsersViewport)));
    gtk_container_add(GTK_CONTAINER(dialogViewport), GTK_WIDGET(dialogMenuBox));

    gtk_widget_show_all(GTK_WIDGET(dialogViewport));
    gtk_widget_hide(chatEntry);
    gtk_widget_hide(chatButton);
    gtk_widget_hide(dialogUsersScrolledWindow);
    gtk_widget_hide(dialogNameLabel);
}

void gotoFriends(GtkWidget *button, GList *appDialogsMenuList) {
    // Разархивируем список
    GtkWidget *chatEntry = g_list_nth_data(appDialogsMenuList, CHAT_ENTRY);
    GtkWidget *chatButton = g_list_nth_data(appDialogsMenuList, CHAT_BUTTON);
    GtkViewport *dialogViewport = g_list_nth_data(appDialogsMenuList, DIALOG_VIEWPORT);
    GtkWidget *dialogUsersScrolledWindow = g_list_nth_data(appDialogsMenuList, DIALOG_USERS_SCROLLED_WINDOW);
    GtkBox *friendsBox = g_list_nth_data(appDialogsMenuList, FRIENDS_BOX);
    GtkWidget *dialogNameLabel = g_list_nth_data(appDialogsMenuList, DIALOG_NAME_LABEL);

    gtk_container_remove(GTK_CONTAINER(dialogViewport), gtk_bin_get_child(GTK_BIN(dialogViewport)));
    gtk_container_add(GTK_CONTAINER(dialogViewport), GTK_WIDGET(friendsBox));

    gtk_widget_show_all(GTK_WIDGET(dialogViewport));
    gtk_widget_hide(chatEntry);
    gtk_widget_hide(chatButton);
    gtk_widget_hide(dialogUsersScrolledWindow);
    gtk_widget_hide(dialogNameLabel);
}