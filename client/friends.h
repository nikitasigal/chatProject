#ifndef TESTGTK_FRIENDS_H
#define TESTGTK_FRIENDS_H

#include "definitions.h"

void addFriend(int ID, char *firstName, char *lastName, char *login, GList **friendsList, GtkListBox *friendsListBox,
               GtkListBox *friendRequestListBoxDialogs);

#endif //TESTGTK_FRIENDS_H
