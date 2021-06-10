#ifndef CHATPROJECT_DEFINITIONS_H
#define CHATPROJECT_DEFINITIONS_H

#include <gtk/gtk.h>
#include <winsock2.h>

#define TEXT_SIZE 5000
#define DATE_SIZE 32
#define NAME_SIZE 32
#define DIALOG_SIZE 96
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
    LOAD_MESSAGES,
    LEAVE_DIALOG
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
    FRIEND_MENU_REMOVE_FRIEND,
    FRIEND_MENU_OPEN_DIALOG,
    MSG_MENU,
    POPUP_LABEL,
    DIALOG_MENU,
    DIALOG_MENU_LEAVE_DIALOG,
    SELECTED_ROW,
    DIALOG_NAME_LABEL
} AddServerDataType;

typedef struct {
    int ID;
    char name[NAME_SIZE];
    GtkListBox *userList;
    GtkListBox *msgList;
    char isOpened;
    char isGroup;
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
    char username[NAME_SIZE];
    char password[NAME_SIZE];
    char firstName[NAME_SIZE];
    char secondName[NAME_SIZE];
    char additionalInfo[NAME_SIZE];
} FullUserInfo;

typedef struct {
    Request request;
    int ID;
    char dialogName[DIALOG_SIZE];
    FullUserInfo users[MAX_NUMBER_OF_USERS];
    short usersNumber;
    char isGroup;
    char isSupposeToOpen;
} FullDialogInfo;

typedef struct {
    Request request;
    FullUserInfo authorizedUser;
    short dialogCount;
    FullDialogInfo dialogList[5];
    short friendCount;
    FullUserInfo friends[5];
    short requestCount;
    FullUserInfo requests[5];
} AuthorizationPackage;

#endif //CHATPROJECT_DEFINITIONS_H
