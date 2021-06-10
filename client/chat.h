#ifndef TESTGTK_CHAT_H
#define TESTGTK_CHAT_H

#include "../shared/definitions.h"

void sizeAllocate(GtkWidget *msgListBox, GdkRectangle *allocation, int *dialogIsJustOpened);

void enterChatClicked(GtkEntry *entry, GList *additionalInfo);

G_MODULE_EXPORT void processMsgSelecting(GtkWidget *widget, GdkEvent *event, gpointer user_data);

G_MODULE_EXPORT void processMsgMenu(GtkWidget *widget, GdkEvent *event, GtkMenu *msgMenu);

G_MODULE_EXPORT void sendMessage(GtkWidget *button, GList *data);

#endif //TESTGTK_CHAT_H
