#include "friends.h"

void addFriend(int ID, char *firstName, char *lastName, char *login, GList **friendsList, GtkListBox *friendsListBox) {
    User *a = g_malloc(sizeof(User));
    a->ID = ID;
    strcpy(a->firstName, firstName);
    strcpy(a->lastName, lastName);
    strcpy(a->login, login);
    *friendsList = g_list_append(*friendsList, a);
}