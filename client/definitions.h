#ifndef TESTGTK_DEFINITIONS_H
#define TESTGTK_DEFINITIONS_H

#include <gtk/gtk.h>
#include <winsock2.h>

#define TEXT_SIZE 5000
#define DATE_SIZE 32
#define NAME_SIZE 48
#define MAX_NUMBER_OF_USERS 30
#define POPUP_LABEL_WIDTH 200
#define POPUP_LABEL_HEIGHT 100

typedef enum {
    REGISTRATION,
    AUTHORIZATION,
    CREATE_DIALOG,
    SEND_MESSAGE,
    SEND_FRIEND_REQUEST,
    FRIEND_REQUEST_ACCEPTED,
    FRIEND_REQUEST_DECLINED,
    REMOVE_FRIEND,
    LOAD_MESSAGES
} Request;

typedef enum {
    SERVER_SOCKET,
    CHAT_ENTRY,
    CHAT_BUTTON,
    DIALOG_VIEWPORT,
    DIALOGS_MENU_BOX,
    DIALOG_USERS_SCROLLED_WINDOW,
    DIALOG_USERS_VIEWPORT,
    DIALOGS_LIST_BOX,
    FRIENDS_LIST_BOX,
    FRIENDS_SWITCHER,
    FRIENDS_STACK,
    FRIENDS_BOX,
    CREATE_DIALOG_FRIENDS_LIST_BOX,
    CREATE_DIALOG_BUTTON,
    CREATE_DIALOG_ENTRY,
    DIALOGS_LIST,
    CURRENT_DIALOG_ID,
    AUTHENTICATION_WINDOW,
    APPLICATION_WINDOW,
    CURRENT_USER,
    FRIEND_SEND_REQUEST_ENTRY,
    FRIEND_REQUEST_LIST_BOX,
    DIALOG_IS_JUST_OPENED,
    FRIEND_MENU,
    FRIEND_MENU_REMOVE_FRIEND
} AddServerDataType;

typedef struct {
    int ID;
    char name[NAME_SIZE];
    GtkListBox *userList;
    GtkListBox *msgList;
    gboolean isOpened;
} Dialog;

typedef struct {
    Request request;
    int ID; // ID беседы
    char firstName[NAME_SIZE];
    char lastName[NAME_SIZE];
    char login[NAME_SIZE];
    char date[DATE_SIZE];
    char text[TEXT_SIZE];
} FullMessageInfo;

typedef struct {
    Request request;
    int ID;
    char firstName[NAME_SIZE];
    char lastName[NAME_SIZE];
    char login[NAME_SIZE];
    char password[NAME_SIZE];
    char additionalInfo[NAME_SIZE];
} FullUserInfo;

typedef struct {
    Request request;
    int ID;
    char dialogName[NAME_SIZE];
    FullUserInfo users[MAX_NUMBER_OF_USERS];
    int usersNumber;
} FullDialogInfo;

#endif //TESTGTK_DEFINITIONS_H
