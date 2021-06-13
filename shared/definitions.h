#ifndef CHATPROJECT_DEFINITIONS_H
#define CHATPROJECT_DEFINITIONS_H

#include <gtk/gtk.h>
#include <winsock2.h>

#define TEXT_SIZE 1000
#define DATE_SIZE 32
#define NAME_SIZE 32
#define DIALOG_SIZE 96
#define MAX_NUMBER_OF_USERS 30
#define MAX_PACKAGE_SIZE 614400 // 600 КБ
#define LOAD_MESSAGE_COUNT 500
#define AUTH_MAX_DIALOGS 100
#define AUTH_MAX_FRIENDS 100
#define AUTH_MAX_REQUESTS 100

#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_PORT "27015"

typedef enum {
    REGISTRATION = 1,
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
    LOGIN_ENTRY,
    PASSWORD_ENTRY,
    CHAT_ENTRY,
    CHAT_BUTTON,
    DIALOG_VIEWPORT,
    DIALOGS_MENU_BOX,
    DIALOG_USERS_SCROLLED_WINDOW,
    DIALOG_USERS_VIEWPORT,
    DIALOGS_LIST_BOX,
    FRIENDS_LIST_BOX,
    FRIENDS_BOX,
    CREATE_DIALOG_FRIENDS_LIST_BOX,
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
    MSG_MENU,
    POPUP_LABEL,
    DIALOG_MENU,
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
    int chatID; // chatID беседы
    char firstName[NAME_SIZE];
    char lastName[NAME_SIZE];
    char username[NAME_SIZE];
    char timestamp[DATE_SIZE];
    char text[TEXT_SIZE];
} FullMessageInfo;

typedef struct {
    Request request;
    short messagesCount;
    FullMessageInfo messagesList[LOAD_MESSAGE_COUNT];
} MessagesPackage;

typedef struct {
    Request request;
    int ID;
    char username[NAME_SIZE];
    char password[NAME_SIZE];
    char firstName[NAME_SIZE];
    char lastName[NAME_SIZE];
    char additionalInfo[NAME_SIZE];
} FullUserInfo;

typedef struct {
    Request request;
    int ID;
    char name[DIALOG_SIZE];
    short userCount;
    FullUserInfo userList[MAX_NUMBER_OF_USERS];
    char isGroup;
    char isSupposedToOpen;
} FullDialogInfo;

typedef struct {
    Request request;
    FullUserInfo authorizedUser;
    short dialogCount;
    FullDialogInfo dialogList[AUTH_MAX_DIALOGS];
    short friendCount;
    FullUserInfo friends[AUTH_MAX_FRIENDS];
    short requestCount;
    FullUserInfo requests[AUTH_MAX_REQUESTS];
} AuthorizationPackage;

#endif //CHATPROJECT_DEFINITIONS_H
