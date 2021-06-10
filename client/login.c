#include "login.h"
#include "ServerHandler/clientCommands.h"

gboolean isPopupShowed = FALSE;

gboolean fadeOutAnimation(gpointer caller) {
    static gboolean isShowed = FALSE;
    static int counter = 1;
    counter++;

    if (counter % 50 == 0)
        isShowed = TRUE;

    if (isShowed)
        gtk_widget_set_opacity(caller, gtk_widget_get_opacity(caller) - 0.02);

    if (gtk_widget_get_opacity(caller) == 0) {
        gtk_widget_hide(caller);
        isPopupShowed = FALSE;
        isShowed = FALSE;
        return FALSE;
    }
    return TRUE;
}

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

void popupNotification(char *string, GtkWidget *popupLabel) {
    if (isPopupShowed)
        return;

    GtkWidget *popupWindow = gtk_widget_get_parent(popupLabel);
    gtk_label_set_text(GTK_LABEL(popupLabel), string);

    gtk_widget_set_opacity(popupWindow, 100);
    GdkRectangle workArea = {0};
    gdk_monitor_get_workarea(gdk_display_get_primary_monitor(gdk_display_get_default()), &workArea);
    gtk_window_move(GTK_WINDOW(popupWindow), workArea.width - POPUP_LABEL_WIDTH, workArea.height - POPUP_LABEL_HEIGHT);

    gtk_widget_show(popupWindow);
    isPopupShowed = TRUE;

    gdk_threads_add_timeout(20, G_SOURCE_FUNC(fadeOutAnimation), popupWindow);
}

void registrationButtonClicked(GtkWidget *button, GList *additionalInfo) {
    if (!checkName(gtk_entry_get_text(g_list_nth_data(additionalInfo, 0)))) {
        printf("Incorrect firstname. Only Latin and Russian letters are allowed.\n");
        popupNotification("Incorrect firstname. Only Latin and Russian letters are allowed.",
                          g_list_nth_data(additionalInfo, 6));
        return;
    }

    if (!checkName(gtk_entry_get_text(g_list_nth_data(additionalInfo, 1)))) {
        printf("Incorrect lastname. Only Latin and Russian letters are allowed.\n");
        popupNotification("Incorrect lastname. Only Latin and Russian letters are allowed.",
                          g_list_nth_data(additionalInfo, 6));
        return;
    }

    if (!checkLoginAndPasswordCorrectness(gtk_entry_get_text(g_list_nth_data(additionalInfo, 2)))) {
        printf("Incorrect username. Only Latin letters and numbers are allowed.\n");
        popupNotification("Incorrect username. Only Latin letters and numbers are allowed.",
                          g_list_nth_data(additionalInfo, 6));
        return;
    }

    if (!checkLoginAndPasswordCorrectness(gtk_entry_get_text(g_list_nth_data(additionalInfo, 3)))) {
        printf("Incorrect password. Only Latin letters and numbers are allowed.\n");
        popupNotification("Incorrect password. Only Latin letters and numbers are allowed.",
                          g_list_nth_data(additionalInfo, 6));
        return;
    }

    if (!checkPasswordMatching(gtk_entry_get_text(g_list_nth_data(additionalInfo, 3)),
                               gtk_entry_get_text(g_list_nth_data(additionalInfo, 4)))) {
        printf("Incorrect password. Password mismatch.\n");
        popupNotification("Incorrect password. Password mismatch.", g_list_nth_data(additionalInfo, 6));
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
    if (!checkLoginAndPasswordCorrectness(gtk_entry_get_text(g_list_nth_data(additionalInfo, 0)))) {
        printf("Incorrect username. Only Latin letters and numbers are allowed.\n");
        popupNotification("Incorrect username. Only Latin letters and numbers are allowed.",
                          g_list_nth_data(additionalInfo, 3));
        return;
    }

    if (!checkLoginAndPasswordCorrectness(gtk_entry_get_text(g_list_nth_data(additionalInfo, 1)))) {
        printf("Incorrect password. Only Latin letters and numbers are allowed.\n");
        popupNotification("Incorrect password. Only Latin letters and numbers are allowed.",
                          g_list_nth_data(additionalInfo, 3));
        return;
    }

    // Fill user information
    FullUserInfo newUser;
    strcpy(newUser.username, gtk_entry_get_text(g_list_nth_data(additionalInfo, 0)));
    strcpy(newUser.password, gtk_entry_get_text(g_list_nth_data(additionalInfo, 1)));

    // Send information to server
    SOCKET *serverSocket = g_list_nth_data(additionalInfo, 2);
    clientRequest_Authorization(*serverSocket, newUser);
}

G_MODULE_EXPORT void nextField(GtkEntry *entry, GtkWidget *data) {
    gtk_widget_grab_focus(data);
}

