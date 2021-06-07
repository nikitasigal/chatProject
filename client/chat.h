#ifndef TESTGTK_CHAT_H
#define TESTGTK_CHAT_H

#include "definitions.h"

extern GtkWidget *msgMenu;
extern GtkWidget *msgList;

void sizeAllocate(GtkWidget *msgListBox, GdkRectangle *allocation, int *dialogIsJustOpened);

G_MODULE_EXPORT void processMsgSelecting(GtkWidget *widget, GdkEvent *event, gpointer user_data);

G_MODULE_EXPORT void processMsgMenu(GtkWidget *widget, GdkEvent *event, gpointer user_data);

G_MODULE_EXPORT void sendMessage(GtkWidget *button, GList *data);

#endif //TESTGTK_CHAT_H
