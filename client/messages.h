#ifndef TESTGTK_MESSAGES_H
#define TESTGTK_MESSAGES_H

#include "definitions.h"
#include "clientCommand.h"

void leaveDialog(GtkMenuItem *menuitem, GList *additionalInfo);

void processDialogMenu(GtkWidget *widget, GdkEvent *event, GList *additionalInfo);

void openDialog(GtkWidget *button, GList *data);

void createDialog(GtkButton *button, GList *additionalInfo);

#endif //TESTGTK_MESSAGES_H
