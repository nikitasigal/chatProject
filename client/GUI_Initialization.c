#include "GUI_Initialization.h"
#include "login.h"
#include "messages.h"
#include "appMenu.h"
#include "friends.h"
#include "generalFunctions.h"
#include "chat.h"

void GUIInit(SOCKET *serverSocket) {
    GtkBuilder *builder = gtk_builder_new_from_file("GUI/Glade/glade.glade");

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "registrationWindow"));
    GtkWidget *mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
    GtkWidget *chatEntry = GTK_WIDGET(gtk_builder_get_object(builder, "chatEntry"));
    GtkWidget *chatButton = GTK_WIDGET(gtk_builder_get_object(builder, "chatButton"));
    GtkWidget *dialogUsersScrolledWindow = GTK_WIDGET(gtk_builder_get_object(builder, "dialogUsersScrolledWindow"));
    GtkWidget *dialogUsersViewport = GTK_WIDGET(gtk_builder_get_object(builder, "dialogUsersViewport"));

    GtkWidget *registrationButton = GTK_WIDGET(gtk_builder_get_object(builder, "registrationButton"));
    GtkWidget *authorizationButton = GTK_WIDGET(gtk_builder_get_object(builder, "authorizationButton"));
    GtkWidget *dialogViewport = GTK_WIDGET(gtk_builder_get_object(builder, "dialogViewport"));
    GtkWidget *dialogsButton = GTK_WIDGET(gtk_builder_get_object(builder, "dialogsButton"));
    GtkWidget *dialogMenuBox = GTK_WIDGET(gtk_builder_get_object(builder, "dialogMenuBox"));
    GtkWidget *dialogsWindow = GTK_WIDGET(gtk_builder_get_object(builder, "dialogsWindow"));
    GtkWidget *friendsButton = GTK_WIDGET(gtk_builder_get_object(builder, "friendsButton"));
    GtkListBox *dialogsListBox = GTK_LIST_BOX(gtk_builder_get_object(builder, "dialogsListBox"));
    GtkWidget *friendsBox = GTK_WIDGET(gtk_builder_get_object(builder, "friendsBox"));
    GtkWidget *friendsWindow = GTK_WIDGET(gtk_builder_get_object(builder, "friendsWindow"));
    GtkWidget *friendsListBox = GTK_WIDGET(gtk_builder_get_object(builder, "friendsListBox"));
    GtkWidget *friendSendRequestButton = GTK_WIDGET(gtk_builder_get_object(builder, "friendSendRequestButton"));
    GtkWidget *friendSendRequestEntry = GTK_WIDGET(gtk_builder_get_object(builder, "friendSendRequestEntry"));
    GtkWidget *friendRequestListBox = GTK_WIDGET(gtk_builder_get_object(builder, "friendRequestListBox"));
    GtkWidget *createDialogFriendsListBox = GTK_WIDGET(gtk_builder_get_object(builder, "createDialogFriendsListBox"));
    GtkWidget *createDialogButton = GTK_WIDGET(gtk_builder_get_object(builder, "createDialogButton"));
    GtkWidget *createDialogEntry = GTK_WIDGET(gtk_builder_get_object(builder, "createDialogEntry"));
    GtkWidget *friendMenuRemoveFriend = GTK_WIDGET(gtk_builder_get_object(builder, "friendMenuRemoveFriend"));
    GtkWidget *friendMenuOpenDialog = GTK_WIDGET(gtk_builder_get_object(builder, "friendMenuOpenDialog"));
    GtkWidget *popupLabel = GTK_WIDGET(gtk_builder_get_object(builder, "popupLabel"));
    GtkWidget *dialogMenuLeaveDialog = GTK_WIDGET(gtk_builder_get_object(builder, "dialogMenuLeaveDialog"));
    GtkWidget *dialogNameLabel = GTK_WIDGET(gtk_builder_get_object(builder, "dialogNameLabel"));
    GtkWidget *timeLabel = GTK_WIDGET(gtk_builder_get_object(builder, "time"));

    // Dialog menu
    GtkWidget *dialogMenu = GTK_WIDGET(gtk_builder_get_object(builder, "dialogMenu"));
    g_object_ref(dialogMenu);

    // Message menu
    GtkWidget *msgMenu = GTK_WIDGET(gtk_builder_get_object(builder, "msgMenu"));
    g_object_ref(msgMenu);

    // Friend menu
    GtkWidget *friendMenu = GTK_WIDGET(gtk_builder_get_object(builder, "friendMenu"));
    g_object_ref(friendMenu);

    // Friends window:
    g_object_ref(friendsBox);
    gtk_container_remove(GTK_CONTAINER(friendsWindow), friendsBox);

    // Dialogs window
    g_object_ref(dialogMenuBox);
    gtk_container_remove(GTK_CONTAINER(dialogsWindow), GTK_WIDGET(dialogMenuBox));

    // Dialogs list and listBox
    GList *dialogsList = NULL;
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(dialogsListBox), GTK_SELECTION_NONE);
    gtk_container_add(GTK_CONTAINER(dialogViewport), GTK_WIDGET(dialogMenuBox));

    GList *regList = NULL;
    regList = g_list_append(regList, GTK_ENTRY(gtk_builder_get_object(builder, "firstNameEntry")));
    regList = g_list_append(regList, GTK_ENTRY(gtk_builder_get_object(builder, "secondNameEntry")));
    regList = g_list_append(regList, GTK_ENTRY(gtk_builder_get_object(builder, "loginEntry")));
    regList = g_list_append(regList, GTK_ENTRY(gtk_builder_get_object(builder, "passwordEntry")));
    regList = g_list_append(regList, GTK_ENTRY(gtk_builder_get_object(builder, "passwordRepeatEntry")));
    regList = g_list_append(regList, serverSocket);
    regList = g_list_append(regList, popupLabel);

    GList *authList = NULL;
    authList = g_list_append(authList, GTK_ENTRY(gtk_builder_get_object(builder, "loginAuthWindow")));
    authList = g_list_append(authList, gtk_builder_get_object(builder, "passwordAuthWindow"));
    authList = g_list_append(authList, serverSocket);
    authList = g_list_append(authList, popupLabel);

    int *currentDialogID = malloc(sizeof(int));
    *currentDialogID = -1;

    gboolean *dialogIsJustOpened = malloc(sizeof(gboolean));
    *dialogIsJustOpened = -1;

    // Current user information
    FullUserInfo *user = malloc(sizeof(FullUserInfo));

    // Dialog row selected by right mouse click
    GtkListBoxRow *selectedRow = malloc(sizeof(GtkListBoxRow));

    GList *additionalInfo = NULL;
    additionalInfo = g_list_append(additionalInfo, serverSocket);
    additionalInfo = g_list_append(additionalInfo, chatEntry);
    additionalInfo = g_list_append(additionalInfo, chatButton);
    additionalInfo = g_list_append(additionalInfo, dialogViewport);
    additionalInfo = g_list_append(additionalInfo, dialogMenuBox);
    additionalInfo = g_list_append(additionalInfo, dialogUsersScrolledWindow);
    additionalInfo = g_list_append(additionalInfo, dialogUsersViewport);
    additionalInfo = g_list_append(additionalInfo, dialogsListBox);
    additionalInfo = g_list_append(additionalInfo, friendsListBox);
    additionalInfo = g_list_append(additionalInfo, friendsBox);
    additionalInfo = g_list_append(additionalInfo, createDialogFriendsListBox);
    additionalInfo = g_list_append(additionalInfo, createDialogEntry);
    additionalInfo = g_list_append(additionalInfo, dialogsList);
    additionalInfo = g_list_append(additionalInfo, currentDialogID);
    additionalInfo = g_list_append(additionalInfo, window);
    additionalInfo = g_list_append(additionalInfo, mainWindow);
    additionalInfo = g_list_append(additionalInfo, user);
    additionalInfo = g_list_append(additionalInfo, friendSendRequestEntry);
    additionalInfo = g_list_append(additionalInfo, friendRequestListBox);
    additionalInfo = g_list_append(additionalInfo, dialogIsJustOpened);
    additionalInfo = g_list_append(additionalInfo, friendMenu);
    additionalInfo = g_list_append(additionalInfo, msgMenu);
    additionalInfo = g_list_append(additionalInfo, popupLabel);
    additionalInfo = g_list_append(additionalInfo, dialogMenu);
    additionalInfo = g_list_append(additionalInfo, selectedRow);
    additionalInfo = g_list_append(additionalInfo, dialogNameLabel);

    // CSS
    GdkScreen *screen = gdk_screen_get_default();
    GtkCssProvider *css = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css, "GUI/Themes/theme.css", NULL);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css);

    // Создадим пару друзей
    FullUserInfo *first = malloc(sizeof(FullUserInfo));
    first->ID = 3;
    first->request = 0;
    strcpy(first->firstName, "Ivan");
    strcpy(first->secondName, "Petrov");
    strcpy(first->username, "Lol2002");

    FullUserInfo *second = malloc(sizeof(FullUserInfo));
    second->ID = 4;
    second->request = 0;
    strcpy(second->firstName, "Masha");
    strcpy(second->secondName, "Nyasha");
    strcpy(second->username, "creeper_kitty");
    addFriend(first, additionalInfo);
    addFriend(second, additionalInfo);

    // Launch application
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(registrationButton, "clicked", (GCallback) registrationButtonClicked, regList);
    g_signal_connect(authorizationButton, "clicked", (GCallback) authorizationButtonClicked, authList);
    g_signal_connect(dialogsButton, "clicked", (GCallback) gotoMessages, additionalInfo);
    g_signal_connect(friendsButton, "clicked", (GCallback) gotoFriends, additionalInfo);
    g_signal_connect(createDialogButton, "clicked", (GCallback) createDialog, additionalInfo);
    g_signal_connect(friendSendRequestButton, "clicked", (GCallback) sendFriendRequest, additionalInfo);
    g_signal_connect(friendMenuRemoveFriend, "activate", (GCallback) removeFriend, additionalInfo);
    g_signal_connect(friendMenuOpenDialog, "activate", (GCallback) openPersonalDialog, additionalInfo);
    g_signal_connect(dialogMenuLeaveDialog, "activate", (GCallback) leaveDialog, additionalInfo);
    g_signal_connect(chatEntry, "activate", (GCallback) enterChatClicked, additionalInfo);
    g_signal_connect(gtk_builder_get_object(builder, "passwordAuthWindow"), "activate", (GCallback) authorizationButtonClicked, authList);
    g_signal_connect(gtk_builder_get_object(builder, "passwordRepeatEntry"), "activate", (GCallback) registrationButtonClicked, regList);

    // Launch a thread for server
    g_thread_new("Server thread", (GThreadFunc) serverRequestProcess, additionalInfo);

    // Launch a clock
    gdk_threads_add_timeout_seconds(1, (GSourceFunc) timer, timeLabel);

    g_object_unref(builder);

    gtk_widget_show_all(window);
    gtk_widget_hide(chatEntry);
    gtk_widget_hide(chatButton);
    gtk_widget_hide(dialogUsersScrolledWindow);
    gtk_widget_hide(dialogNameLabel);
}