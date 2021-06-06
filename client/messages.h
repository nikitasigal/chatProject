#ifndef TESTGTK_MESSAGES_H
#define TESTGTK_MESSAGES_H

#include "definitions.h"
#include "clientCommand.h"

typedef struct {
    long long ID;
    char name[TEXT_SIZE];
    GtkWidget *msgList;
} ChatHistory;

typedef struct {
    long long ID;
    GList *chatList;
    GList *additionalInfo;
} ChatInfo;

void
newCreateChat(int ID, const char *name, const int usersID[30], int usersIDCount, GList *friendsList, GList *dialogsList,
              GtkListBox *dialogsListBox, GList *additionalInfo, GtkBox *dialogMenuBox);

void createChat(char *name, int ID, GtkListBox *dialogList, GList *chatList, GList *additionalInfo);

void openDialog(GtkWidget *button, ChatInfo *data);

void newOpenDialog(GtkWidget *button, GList *data);

void createDialog(GtkButton *button, GList *appDialogsMenuList);

#endif //TESTGTK_MESSAGES_H
