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
long long curChatIDGlobal;

void GUIInit() {
    //




    GtkBuilder *builder = gtk_builder_new_from_file("glade.glade");

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "registrationWindow"));
    GtkWidget *registrationButton = GTK_WIDGET(gtk_builder_get_object(builder, "registrationButton"));
    GtkWidget *authorizationButton = GTK_WIDGET(gtk_builder_get_object(builder, "authorizationButton"));
    GtkWidget *dialogViewport = GTK_WIDGET(gtk_builder_get_object(builder, "dialogViewport"));
    GtkWidget *dialogsButton = GTK_WIDGET(gtk_builder_get_object(builder, "dialogsButton"));
    GtkWidget *friendsButton = GTK_WIDGET(gtk_builder_get_object(builder, "friendsButton"));
    GtkWidget *chatEntry = GTK_WIDGET(gtk_builder_get_object(builder, "chatEntry"));
    GtkWidget *chatButton = GTK_WIDGET(gtk_builder_get_object(builder, "chatButton"));
    GtkWidget *dialogUsersScrolledWindow = GTK_WIDGET(gtk_builder_get_object(builder, "dialogUsersScrolledWindow"));
    GtkWidget *dialogUsersViewport = GTK_WIDGET(gtk_builder_get_object(builder, "dialogUsersViewport"));
    GtkListBox *dialogsListBox = GTK_LIST_BOX(gtk_builder_get_object(builder, "dialogsListBox"));
    GtkWidget *dialogsWindow = GTK_WIDGET(gtk_builder_get_object(builder, "dialogsWindow"));
    GtkWidget *friendsBox = GTK_WIDGET(gtk_builder_get_object(builder, "friendsBox"));
    GtkWidget *friendsWindow = GTK_WIDGET(gtk_builder_get_object(builder, "friendsWindow"));
    GtkWidget *friendsSwitcher = GTK_WIDGET(gtk_builder_get_object(builder, "friendsSwitcher"));
    GtkWidget *friendsStack = GTK_WIDGET(gtk_builder_get_object(builder, "friendsStack"));
    GtkWidget *friendsListBox = GTK_WIDGET(gtk_builder_get_object(builder, "friendsListBox"));
    popupLabel = GTK_WIDGET(gtk_builder_get_object(builder, "popupLabel"));
    msgList = GTK_WIDGET(gtk_builder_get_object(builder, "MsgListBox"));

    // Message menu
    msgMenu = GTK_WIDGET(gtk_builder_get_object(builder, "msgMenu"));
    g_object_ref(msgMenu);

    // Friends window:
    g_object_ref(friendsBox);
    gtk_container_remove(GTK_CONTAINER(friendsWindow), friendsBox);

    // Dialogs window
    g_object_ref(dialogsListBox);
    gtk_container_remove(GTK_CONTAINER(dialogsWindow), GTK_WIDGET(dialogsListBox));

    //
    GList *dialogsList = NULL;
    //GtkListBox *dialogsListBox = GTK_LIST_BOX(gtk_list_box_new());
    //g_object_ref(dialogsListBox);
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(dialogsListBox), GTK_SELECTION_NONE);
    gtk_container_add(GTK_CONTAINER(dialogViewport), GTK_WIDGET(dialogsListBox));

    GList *friendsList = NULL;
    //GtkListBox *friendsListBox = GTK_LIST_BOX(gtk_list_box_new());

    // Формируем список друзей
    /*User a = {1, "Kolya", "Ivanov", "Lololoshka"};
    User b = {2, "Kostya", "Rumyantsev", "Korostast"};
    friendsList = g_list_append(friendsList, &a);
    friendsList = g_list_append(friendsList, &b);*/
    addFriend(1, "Kolya", "Ivanov", "Lololoshka", &friendsList, GTK_LIST_BOX(friendsListBox));
    addFriend(2, "Kostya", "Rumyantsev", "Korostast", &friendsList, GTK_LIST_BOX(friendsListBox));

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

    int userID[30];
    int userCount = 2;
    userID[0] = 1;
    userID[1] = 2;
    GList *additionalInfo = NULL; // Дополнительная информация. Сейчас содержит entry и button
    additionalInfo = g_list_append(additionalInfo, chatEntry);
    additionalInfo = g_list_append(additionalInfo, chatButton);
    //additionalInfo = g_list_append(additionalInfo, dialogViewport);
    additionalInfo = g_list_append(additionalInfo, dialogUsersViewport);
    additionalInfo = g_list_append(additionalInfo, dialogUsersScrolledWindow);
    newCreateChat(1, "Conversation1", userID, userCount, friendsList, dialogsList, dialogsListBox, additionalInfo);
    newCreateChat(2, "Conversation2", userID, userCount, friendsList, dialogsList, dialogsListBox, additionalInfo);
    //

    GList *chatList = NULL; // Список чатов (историй) c информацией о беседе (ID, name, список сообщений)

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

    GList *authList = NULL;
    authList = g_list_append(authList, GTK_ENTRY(gtk_builder_get_object(builder, "loginAuthWindow")));
    authList = g_list_append(authList, GTK_ENTRY(gtk_builder_get_object(builder, "passwordAuthWindow")));

    GList *appDialogsMenuList = NULL;
    appDialogsMenuList = g_list_append(appDialogsMenuList, chatEntry);
    appDialogsMenuList = g_list_append(appDialogsMenuList, chatButton);
    appDialogsMenuList = g_list_append(appDialogsMenuList, dialogViewport);
    appDialogsMenuList = g_list_append(appDialogsMenuList, dialogUsersScrolledWindow);
    appDialogsMenuList = g_list_append(appDialogsMenuList, dialogsListBox);
    appDialogsMenuList = g_list_append(appDialogsMenuList, friendsListBox);
    appDialogsMenuList = g_list_append(appDialogsMenuList, friendsSwitcher);
    appDialogsMenuList = g_list_append(appDialogsMenuList, friendsStack);
    appDialogsMenuList = g_list_append(appDialogsMenuList, friendsBox);

    // Launch application
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(registrationButton, "clicked", (GCallback) registrationButtonClicked, regList);
    g_signal_connect(authorizationButton, "clicked", (GCallback) authorizationButtonClicked, authList);
    g_signal_connect(dialogsButton, "clicked", (GCallback) gotoMessages, appDialogsMenuList);
    g_signal_connect(friendsButton, "clicked", (GCallback) gotoFriends, appDialogsMenuList);


    // Collect data that we are going to use to process server requests
    GList *additionalServerData = NULL;
    additionalServerData = g_list_append(appDialogsMenuList, chatEntry);
    additionalServerData = g_list_append(appDialogsMenuList, chatButton);
    additionalServerData = g_list_append(appDialogsMenuList, dialogUsersViewport);
    additionalServerData = g_list_append(appDialogsMenuList, dialogUsersScrolledWindow);
    additionalServerData = g_list_append(appDialogsMenuList, dialogsListBox);

    serverRequestProcess(additionalServerData);


    gtk_widget_show_all(window);
    gtk_widget_hide(chatEntry);
    gtk_widget_hide(chatButton);
    gtk_widget_hide(dialogUsersScrolledWindow);
    gtk_list_box_unselect_all(GTK_LIST_BOX(msgList));
}