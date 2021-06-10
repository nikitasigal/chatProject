#ifndef TESTGTK_FRIENDS_H
#define TESTGTK_FRIENDS_H

#include "../shared/definitions.h"

void sendFriendRequest(GtkButton *button, GList *additionalInfo);

void acceptFriendRequest(GtkWidget *button, GList *additionalInfo);

void declineFriendRequest(GtkWidget *button, GList *additionalInfo);

void addFriend(FullUserInfo *user, GList *additionalInfo);

void removeFriend(GtkMenuItem *menuitem, GList *additionalInfo);

void openPersonalDialog(GtkMenuItem *menuitem, GList *additionalInfo);

#endif //TESTGTK_FRIENDS_H
