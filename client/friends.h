#ifndef TESTGTK_FRIENDS_H
#define TESTGTK_FRIENDS_H

#include "definitions.h"

void addFriendOld(int ID, char *firstName, char *lastName, char *login, GList **friendsList, GtkListBox *friendsListBox,
                  GtkListBox *friendRequestListBoxDialogs);

void sendFriendRequest(GtkButton *button, GList *additionalInfo);

void acceptFriendRequest(GtkWidget *button, GList *additionalInfo);

void declineFriendRequest(GtkWidget *button, GList *additionalInfo);

void addFriend(FullUserInfo *user, GList *additionalInfo);

void removeFriend(GtkMenuItem *menuitem, GList *data);

#endif //TESTGTK_FRIENDS_H
