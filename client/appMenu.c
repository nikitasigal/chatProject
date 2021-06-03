#include "appMenu.h"

void gotoMessages(GtkWidget *button, GList *appDialogsMenuList) {
    // Раскроем список
    GtkWidget *chatEntry = g_list_nth_data(appDialogsMenuList, 0);
    GtkWidget *chatButton = g_list_nth_data(appDialogsMenuList, 1);
    GtkViewport *dialogViewport = g_list_nth_data(appDialogsMenuList, 2);
    GtkWidget *dialogUsersScrolledWindow = g_list_nth_data(appDialogsMenuList, 3);
    GtkListBox *dialogListBox = g_list_nth_data(appDialogsMenuList, 4);
    GtkWidget *dialogUsersViewport = gtk_bin_get_child(GTK_BIN(dialogUsersScrolledWindow));

    gtk_container_remove(GTK_CONTAINER(dialogViewport), gtk_bin_get_child(GTK_BIN(dialogViewport)));
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(dialogUsersViewport));
    if (child != NULL)
        gtk_container_remove(GTK_CONTAINER(dialogUsersViewport), gtk_bin_get_child(GTK_BIN(dialogUsersViewport)));
    gtk_container_add(GTK_CONTAINER(dialogViewport), GTK_WIDGET(dialogListBox));

    gtk_widget_hide(chatEntry);
    gtk_widget_hide(chatButton);
    gtk_widget_hide(dialogUsersScrolledWindow);
}

void gotoFriends(GtkWidget *button, GList *appDialogsMenuList) {
    // Разархивируем список
    GtkWidget *chatEntry = g_list_nth_data(appDialogsMenuList, 0);
    GtkWidget *chatButton = g_list_nth_data(appDialogsMenuList, 1);
    GtkViewport *dialogViewport = g_list_nth_data(appDialogsMenuList, 2);
    GtkWidget *dialogUsersScrolledWindow = g_list_nth_data(appDialogsMenuList, 3);
    GtkListBox *dialogListBox = g_list_nth_data(appDialogsMenuList, 4);
    GtkListBox *friendsListBox = g_list_nth_data(appDialogsMenuList, 5);
    GtkStackSwitcher *friendsStackSwitcher = g_list_nth_data(appDialogsMenuList, 6);
    GtkStack *friendsStack = g_list_nth_data(appDialogsMenuList, 7);
    GtkStack *friendsBox = g_list_nth_data(appDialogsMenuList, 8);
    GtkWidget *dialogUsersViewport = gtk_bin_get_child(GTK_BIN(dialogUsersScrolledWindow));

    gtk_container_remove(GTK_CONTAINER(dialogViewport), gtk_bin_get_child(GTK_BIN(dialogViewport)));
    gtk_container_add(GTK_CONTAINER(dialogViewport), GTK_WIDGET(friendsBox));

    gtk_widget_hide(chatEntry);
    gtk_widget_hide(chatButton);
    gtk_widget_hide(dialogUsersScrolledWindow);
}