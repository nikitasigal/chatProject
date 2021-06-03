#ifndef TESTGTK_CHAT_H
#define TESTGTK_CHAT_H

#include "definitions.h"

extern GtkWidget *msgMenu;
extern GtkWidget *msgList;

G_MODULE_EXPORT void sizeAllocate(GtkWidget *something, gpointer user_data);

G_MODULE_EXPORT void processMsgSelecting(GtkWidget *widget, GdkEvent *event, gpointer user_data);

G_MODULE_EXPORT void processMsgMenu(GtkWidget *widget, GdkEvent *event, gpointer user_data);

G_MODULE_EXPORT void sendMessage(GtkWidget *button, GList *data);

void insertMessageInChat(char *name, char *date, char *message, GtkListBox *msgListBox);

#endif //TESTGTK_CHAT_H
