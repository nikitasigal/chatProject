#include "login.h"
#include "ServerHandler/clientCommands.h"

gboolean checkLoginAndPasswordCorrectness(const gchar *field) {
    if (strlen(field) == 0)
        return FALSE;
    for (int i = 0; i < strlen(field); ++i)
        if (!g_unichar_isalnum(field[i]))
            return FALSE;
    return TRUE;
}

gboolean checkPasswordMatching(const gchar *firstPassword, const gchar *secondPassword) {
    return strcmp(firstPassword, secondPassword) == 0 ? TRUE : FALSE;
}

gboolean checkName(const gchar *field) {
    if (strlen(field) == 0)
        return FALSE;
    for (int i = 0; i < strlen(field); ++i) {
        if (field[i] == -47 || field[i] == -48) {   // Russian letters
            ++i;
            continue;
        }
        if (!g_unichar_isalpha(field[i]))
            return FALSE;
    }
    return TRUE;
}

void registrationButtonClicked(GtkWidget *button, GList *additionalInfo) {
    if (!checkName(gtk_entry_get_text(g_list_nth_data(additionalInfo, 0)))) {
        printf("Incorrect firstname. Only Latin and Russian letters are allowed.\n");
        popupNotification("Incorrect firstname. Only Latin and Russian letters are allowed.");
        return;
    }

    if (!checkName(gtk_entry_get_text(g_list_nth_data(additionalInfo, 1)))) {
        printf("Incorrect lastname. Only Latin and Russian letters are allowed.\n");
        popupNotification("Incorrect lastname. Only Latin and Russian letters are allowed.");
        return;
    }

    if (!checkLoginAndPasswordCorrectness(gtk_entry_get_text(g_list_nth_data(additionalInfo, 2)))) {
        printf("Incorrect username. Only Latin letters and numbers are allowed.\n");
        popupNotification("Incorrect username. Only Latin letters and numbers are allowed.");
        return;
    }

    if (!checkLoginAndPasswordCorrectness(gtk_entry_get_text(g_list_nth_data(additionalInfo, 3)))) {
        printf("Incorrect password. Only Latin letters and numbers are allowed.\n");
        popupNotification("Incorrect password. Only Latin letters and numbers are allowed.");
        return;
    }

    if (!checkPasswordMatching(gtk_entry_get_text(g_list_nth_data(additionalInfo, 3)),
                               gtk_entry_get_text(g_list_nth_data(additionalInfo, 4)))) {
        printf("Incorrect password. Password mismatch.\n");
        popupNotification("Incorrect password. Password mismatch.");
        return;
    }

    // Create new user
    FullUserInfo newUser;
    strcpy(newUser.firstName, gtk_entry_get_text(g_list_nth_data(additionalInfo, 0)));
    strcpy(newUser.secondName, gtk_entry_get_text(g_list_nth_data(additionalInfo, 1)));
    strcpy(newUser.username, gtk_entry_get_text(g_list_nth_data(additionalInfo, 2)));
    strcpy(newUser.password, gtk_entry_get_text(g_list_nth_data(additionalInfo, 3)));

    // Send information to server
    SOCKET *serverSocket = g_list_nth_data(additionalInfo, 5);
    clientRequest_Registration(*serverSocket, newUser);
}

void authorizationButtonClicked(GtkWidget *button, GList *additionalInfo) {
    // Распакуем данные
    GtkEntry *loginEntry;
    GtkEntry *passwordEntry;
    if (button == NULL) {
        loginEntry = g_list_nth_data(additionalInfo, LOGIN_ENTRY);
        passwordEntry = g_list_nth_data(additionalInfo, PASSWORD_ENTRY);
    } else {
        loginEntry = g_list_nth_data(additionalInfo, 0);
        passwordEntry = g_list_nth_data(additionalInfo, 1);
    }

    if (!checkLoginAndPasswordCorrectness(gtk_entry_get_text(loginEntry))) {
        printf("Incorrect username. Only Latin letters and numbers are allowed.\n");
        popupNotification("Incorrect username. Only Latin letters and numbers are allowed.");
        return;
    }

    if (!checkLoginAndPasswordCorrectness(gtk_entry_get_text(passwordEntry))) {
        printf("Incorrect password. Only Latin letters and numbers are allowed.\n");
        popupNotification("Incorrect password. Only Latin letters and numbers are allowed.");
        return;
    }

    // Fill user information
    FullUserInfo newUser;
    strcpy(newUser.username, gtk_entry_get_text(loginEntry));
    strcpy(newUser.password, gtk_entry_get_text(passwordEntry));

    // Send information to server
    SOCKET *serverSocket;
    if (button == NULL)
        serverSocket = g_list_nth_data(additionalInfo, SERVER_SOCKET);
    else
        serverSocket = g_list_nth_data(additionalInfo, 2);
    clientRequest_Authorization(*serverSocket, newUser);
}

G_MODULE_EXPORT void nextField(GtkEntry *entry, GtkWidget *data) {
    gtk_widget_grab_focus(data);
}

