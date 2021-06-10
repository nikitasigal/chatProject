#ifndef TESTGTK_LOGIN_H
#define TESTGTK_LOGIN_H

#include "../shared/definitions.h"

void popupNotification(char *string, GtkWidget *popupLabel);

void registrationButtonClicked(GtkWidget *button, GList *additionalInfo);

void authorizationButtonClicked(GtkWidget *button, GList *additionalInfo);

G_MODULE_EXPORT void nextField(GtkEntry *entry, GtkWidget *data);

#endif //TESTGTK_LOGIN_H
