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
    SEND_MESSAGE
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
    CREATE_DIALOG_FRIENDS_BOX_LIST,
    CREATE_DIALOG_BUTTON,
    CREATE_DIALOG_ENTRY,
    DIALOGS_LIST,
    CURRENT_DIALOG_ID,
    AUTHENTICATION_WINDOW,
    APPLICATION_WINDOW,
    CURRENT_USER
} AddServerDataType;

typedef struct {
    int ID;
    char firstName[NAME_SIZE];
    char lastName[NAME_SIZE];
    char login[NAME_SIZE];
} User;

typedef struct {
    int ID;
    char name[NAME_SIZE];
    GtkListBox *userList;
    GtkListBox *msgList;
} Dialog;

typedef struct {
    //char fullUserName[NAME_SIZE];
    Request request;
    int ID; // ID беседы
    char firstName[NAME_SIZE];
    char lastName[NAME_SIZE];
    char login[NAME_SIZE];
    char date[DATE_SIZE];
    char text[TEXT_SIZE];
} FullMessageInfo;

typedef struct {
    //char fullUserName[NAME_SIZE];
    Request request;
    int ID;
    char firstName[NAME_SIZE];
    char lastName[NAME_SIZE];
    char login[NAME_SIZE];
    char password[NAME_SIZE];
} FullUserInfo;

typedef struct {
    Request request;
    int ID;
    char dialogName[NAME_SIZE];
    FullUserInfo users[MAX_NUMBER_OF_USERS];
    int usersNumber;
    //GList *users;   // List of FullUserInfo
} FullDialogInfo;

#endif //TESTGTK_DEFINITIONS_H
