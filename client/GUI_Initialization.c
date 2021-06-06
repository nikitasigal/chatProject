#include "GUI_Initialization.h"
#include "login.h"
#include "chat.h"
#include "messages.h"
#include "appMenu.h"
#include "friends.h"
#include "clientCommand.h"

GtkWidget *msgMenu;
GtkWidget *msgList;
GtkWidget *popupLabel;

void GUIInit(SOCKET serverSocket) {
    GtkBuilder *builder = gtk_builder_new_from_file("glade.glade");

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "registrationWindow"));
    GtkWidget *mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "testWindow"));
    GtkWidget *registrationButton = GTK_WIDGET(gtk_builder_get_object(builder, "registrationButton"));
    GtkWidget *authorizationButton = GTK_WIDGET(gtk_builder_get_object(builder, "authorizationButton"));
    GtkWidget *dialogViewport = GTK_WIDGET(gtk_builder_get_object(builder, "dialogViewport"));
    GtkWidget *dialogsButton = GTK_WIDGET(gtk_builder_get_object(builder, "dialogsButton"));
    GtkWidget *dialogMenuBox = GTK_WIDGET(gtk_builder_get_object(builder, "dialogMenuBox"));
    GtkWidget *dialogsWindow = GTK_WIDGET(gtk_builder_get_object(builder, "dialogsWindow"));
    GtkWidget *chatsViewport = GTK_WIDGET(gtk_builder_get_object(builder, "chatsViewport"));
    GtkWidget *friendsButton = GTK_WIDGET(gtk_builder_get_object(builder, "friendsButton"));
    GtkWidget *chatEntry = GTK_WIDGET(gtk_builder_get_object(builder, "chatEntry"));
    GtkWidget *chatButton = GTK_WIDGET(gtk_builder_get_object(builder, "chatButton"));
    GtkWidget *dialogUsersScrolledWindow = GTK_WIDGET(gtk_builder_get_object(builder, "dialogUsersScrolledWindow"));
    GtkWidget *dialogUsersViewport = GTK_WIDGET(gtk_builder_get_object(builder, "dialogUsersViewport"));
    GtkListBox *dialogsListBox = GTK_LIST_BOX(gtk_builder_get_object(builder, "dialogsListBox"));
    GtkWidget *friendsBox = GTK_WIDGET(gtk_builder_get_object(builder, "friendsBox"));
    GtkWidget *friendsWindow = GTK_WIDGET(gtk_builder_get_object(builder, "friendsWindow"));
    GtkWidget *friendsSwitcher = GTK_WIDGET(gtk_builder_get_object(builder, "friendsSwitcher"));
    GtkWidget *friendsStack = GTK_WIDGET(gtk_builder_get_object(builder, "friendsStack"));
    GtkWidget *friendsListBox = GTK_WIDGET(gtk_builder_get_object(builder, "friendsListBox"));
    GtkWidget *createDialogFriendsBoxList = GTK_WIDGET(gtk_builder_get_object(builder, "createDialogFriendsBoxList"));
    GtkWidget *createDialogButton = GTK_WIDGET(gtk_builder_get_object(builder, "createDialogButton"));
    GtkWidget *createDialogEntry = GTK_WIDGET(gtk_builder_get_object(builder, "createDialogEntry"));
    popupLabel = GTK_WIDGET(gtk_builder_get_object(builder, "popupLabel"));
    msgList = GTK_WIDGET(gtk_builder_get_object(builder, "MsgListBox"));

    // Message menu
    msgMenu = GTK_WIDGET(gtk_builder_get_object(builder, "msgMenu"));
    g_object_ref(msgMenu);

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

    GList *friendsList = NULL;
    //GtkListBox *friendsListBox = GTK_LIST_BOX(gtk_list_box_new());

    // Формируем список друзей
    /*User a = {1, "Kolya", "Ivanov", "Lololoshka"};
    User b = {2, "Kostya", "Rumyantsev", "Korostast"};
    friendsList = g_list_append(friendsList, &a);
    friendsList = g_list_append(friendsList, &b);*/

    addFriend(1, "Kolya", "Ivanov", "Lololoshka", &friendsList, GTK_LIST_BOX(friendsListBox),
              GTK_LIST_BOX(createDialogFriendsBoxList));
    addFriend(2, "Kostya", "Rumyantsev", "Korostast", &friendsList, GTK_LIST_BOX(friendsListBox),
              GTK_LIST_BOX(createDialogFriendsBoxList));

    /*User *a = g_malloc(sizeof(User));
    a->ID = 1;
    strcpy(a->firstName, "Kolya");
    strcpy(a->lastName, "Ivanov");
    strcpy(a->login, "Lololoshka");
    User *b = g_malloc(sizeof(User));
    b->ID = 2;
    strcpy(b->firstName, "Kostya");
    strcpy(b->lastName, "Rumyantsev");
    strcpy(b->login, "Korostast");
    friendsList = g_list_append(friendsList, a);
    friendsList = g_list_append(friendsList, b);*/

    /*newCreateChat(1, "Conversation1", userID, userCount, friendsList, dialogsList, dialogsListBox, additionalInfo,
                  GTK_BOX(dialogMenuBox));
    newCreateChat(2, "Conversation2", userID, userCount, friendsList, dialogsList, dialogsListBox, additionalInfo,
                  GTK_BOX(dialogMenuBox));*/
    //

    //GList *additionalInfo = NULL; // Дополнительная информация. Сейчас содержит entry и button
    //additionalInfo = g_list_append(additionalInfo, chatEntry);
    //additionalInfo = g_list_append(additionalInfo, chatButton);
    //additionalInfo = g_list_append(additionalInfo, dialogUsersScrolledWindow);

    /*GtkWidget *dialogList = gtk_list_box_new(); // Список бесед (внешний вид диалогов в "сообщениях")
    g_object_ref(dialogList);
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(dialogList), GTK_SELECTION_NONE);
    gtk_container_add(GTK_CONTAINER(dialogViewport), dialogList);*/

    //createChat("Lala", 1, GTK_LIST_BOX(dialogList), chatList, additionalInfo);
    //createChat("11A", 2, GTK_LIST_BOX(dialogList), chatList, additionalInfo);
    //createChat("Костя Румянцев", 3, GTK_LIST_BOX(dialogList), chatList, additionalInfo);

    GList *regList = NULL;
    regList = g_list_append(regList, GTK_ENTRY(gtk_builder_get_object(builder, "firstNameEntry")));
    regList = g_list_append(regList, GTK_ENTRY(gtk_builder_get_object(builder, "secondNameEntry")));
    regList = g_list_append(regList, GTK_ENTRY(gtk_builder_get_object(builder, "loginEntry")));
    regList = g_list_append(regList, GTK_ENTRY(gtk_builder_get_object(builder, "passwordEntry")));
    regList = g_list_append(regList, GTK_ENTRY(gtk_builder_get_object(builder, "passwordRepeatEntry")));
    regList = g_list_append(regList, &serverSocket);

    GList *authList = NULL;
    authList = g_list_append(authList, GTK_ENTRY(gtk_builder_get_object(builder, "loginAuthWindow")));
    authList = g_list_append(authList, gtk_builder_get_object(builder, "passwordAuthWindow"));
    authList = g_list_append(authList, &serverSocket);

    int *currentDialogID = malloc(sizeof(int));
    *currentDialogID = -1;

    // Current user information
    FullUserInfo *user = malloc(sizeof(FullUserInfo));

    GList *additionalInfo = NULL;
    additionalInfo = g_list_append(additionalInfo, &serverSocket);
    additionalInfo = g_list_append(additionalInfo, chatEntry);
    additionalInfo = g_list_append(additionalInfo, chatButton);
    additionalInfo = g_list_append(additionalInfo, dialogViewport);
    additionalInfo = g_list_append(additionalInfo, dialogMenuBox);
    additionalInfo = g_list_append(additionalInfo, dialogUsersScrolledWindow);
    additionalInfo = g_list_append(additionalInfo, dialogUsersViewport);
    additionalInfo = g_list_append(additionalInfo, dialogsListBox);
    additionalInfo = g_list_append(additionalInfo, friendsListBox);
    additionalInfo = g_list_append(additionalInfo, friendsSwitcher);
    additionalInfo = g_list_append(additionalInfo, friendsStack);
    additionalInfo = g_list_append(additionalInfo, friendsBox);
    additionalInfo = g_list_append(additionalInfo, createDialogFriendsBoxList);
    additionalInfo = g_list_append(additionalInfo, createDialogButton);
    additionalInfo = g_list_append(additionalInfo, createDialogEntry);
    additionalInfo = g_list_append(additionalInfo, dialogsList);
    additionalInfo = g_list_append(additionalInfo, currentDialogID);
    additionalInfo = g_list_append(additionalInfo, window);
    additionalInfo = g_list_append(additionalInfo, mainWindow);
    additionalInfo = g_list_append(additionalInfo, user);

    // Launch application
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(registrationButton, "clicked", (GCallback) registrationButtonClicked, regList);
    g_signal_connect(authorizationButton, "clicked", (GCallback) authorizationButtonClicked, authList);
    g_signal_connect(dialogsButton, "clicked", (GCallback) gotoMessages, additionalInfo);
    g_signal_connect(friendsButton, "clicked", (GCallback) gotoFriends, additionalInfo);
    g_signal_connect(createDialogButton, "clicked", (GCallback) createDialog, additionalInfo);

    //FullUserInfo user1 = {1, "Kolya", "Ivanov", "Lololoshka"};
    //FullUserInfo user2 = {2, "Kostya", "Rumyantsev", "Korostast"};
    //FullDialogInfo dialogInfo = {CREATE_DIALOG, 123, "DIALOG LOL", {user1, user2}, 2};
    //serverRequest_CreateDialog(dialogInfo, additionalInfo);

    //gdk_threads_add_timeout(500, (GSourceFunc) serverRequestProcess, additionalServerData);
    //serverRequestProcess(additionalServerData);
    //gdk_threads_add_idle(G_SOURCE_FUNC(serverRequestProcess), additionalServerData);
    g_thread_new("Server thread", (GThreadFunc) serverRequestProcess, additionalInfo);

    gtk_widget_show_all(window);
    gtk_widget_hide(chatEntry);
    gtk_widget_hide(chatButton);
    gtk_widget_hide(dialogUsersScrolledWindow);
    gtk_list_box_unselect_all(GTK_LIST_BOX(msgList));
}