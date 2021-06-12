#include "login.h"
#include "ServerHandler/clientCommands.h"

int notificationCount = 0;

typedef struct {
    short counter;
    GtkWidget *window;
} NotificationStruct;

NotificationStruct notifications[5];

gboolean fadeOutAnimation(short *i) {
    notifications[*i].counter++;

    if (notifications[*i].counter > 60)
        gtk_widget_set_opacity(notifications[*i].window, gtk_widget_get_opacity(notifications[*i].window) - 0.02);

    if (gtk_widget_get_opacity(notifications[*i].window) == 0) {
        gtk_widget_destroy(notifications[*i].window);
        notifications[*i].window = NULL;
        notificationCount--;
        g_free(i);
        return FALSE;
    }

    return TRUE;
}

void popupNotification(char *string, GtkWidget *popupLabel) {
    if (notificationCount == 5)
        return;

    notificationCount++;

    GtkWidget *popupWindow = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_resizable(GTK_WINDOW(popupWindow), FALSE);
    gtk_widget_set_size_request(popupWindow, 250, 140);

    GtkWidget *popupLabel2 = gtk_label_new(string);
    gtk_container_add(GTK_CONTAINER(popupWindow), popupLabel2);
    gtk_label_set_max_width_chars(GTK_LABEL(popupLabel2), 20);
    gtk_label_set_line_wrap(GTK_LABEL(popupLabel2), TRUE);
    gtk_label_set_line_wrap_mode(GTK_LABEL(popupLabel2), PANGO_WRAP_WORD_CHAR);
    gtk_label_set_xalign(GTK_LABEL(popupLabel2), (gfloat) 0.5);

    short *i = malloc(sizeof(short));
    *i = 0;
    for (; *i < 5; ++(*i))
        if (notifications[*i].window == NULL) {
            notifications[*i].window = popupWindow;
            notifications[*i].counter = 1;
            break;
        }

    int a = sizeof(FullMessageInfo);

    gtk_widget_set_opacity(popupWindow, 100);
    GdkRectangle workArea = {0};
    gdk_monitor_get_workarea(gdk_display_get_primary_monitor(gdk_display_get_default()), &workArea);
    gtk_window_move(GTK_WINDOW(popupWindow), workArea.width - 250, workArea.height - 140 * (*i + 1));

    gtk_widget_show_all(popupWindow);

    gdk_threads_add_timeout(20, G_SOURCE_FUNC(fadeOutAnimation), i);
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
    strcpy(newUser.lastName, gtk_entry_get_text(g_list_nth_data(additionalInfo, 1)));
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
        popupNotification("Incorrect username. Only Latin letters and numbers are allowed.",
                          g_list_nth_data(additionalInfo, 3));
        return;
    }

    if (!checkLoginAndPasswordCorrectness(gtk_entry_get_text(passwordEntry))) {
        printf("Incorrect password. Only Latin letters and numbers are allowed.\n");
        popupNotification("Incorrect password. Only Latin letters and numbers are allowed.",
                          g_list_nth_data(additionalInfo, 3));
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

