#ifndef TESTGTK_LOGIN_H
#define TESTGTK_LOGIN_H

#include "definitions.h"

void popupNotification(char *string, GtkWidget *popupLabel);

void registrationButtonClicked(GtkWidget *button, GList *additionalInfo);

void authorizationButtonClicked(GtkWidget *button, GList *additionalInfo);

#endif //TESTGTK_LOGIN_H
