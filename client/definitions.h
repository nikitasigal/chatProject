#ifndef TESTGTK_DEFINITIONS_H
#define TESTGTK_DEFINITIONS_H

#include <gtk/gtk.h>

#define TEXT_SIZE 5000
#define DATE_SIZE 32
#define NAME_SIZE 48
#define POPUP_LABEL_WIDTH 200
#define POPUP_LABEL_HEIGHT 100

typedef enum {
    CHAT_ENTRY,
    CHAT_BUTTON,
    DIALOG_USERS_VIEWPORT,
    DIALOG_USERS_SCROLLED_WINDOW,
    DIALOGS_LIST_BOX,
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

#endif //TESTGTK_DEFINITIONS_H
