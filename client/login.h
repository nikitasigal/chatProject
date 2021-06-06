#ifndef TESTGTK_LOGIN_H
#define TESTGTK_LOGIN_H

#include "definitions.h"

void popupNotification(char *string);

void registrationButtonClicked(GtkWidget *button, GList *user_data);

void authorizationButtonClicked(GtkWidget *button, GList *user_data);

#endif //TESTGTK_LOGIN_H
