#include "login.h"

extern GtkWidget *popupLabel;
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
        counter = 1;
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
        if (field[i] == -47 || field[i] == -48) {
            ++i;
            continue;
        }
        if (!g_unichar_isalpha(field[i]))
            return FALSE;
    }
    return TRUE;
}

void popupNotification(char *string) {
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

void registrationButtonClicked(GtkWidget *button, gpointer user_data) {
    if (!checkName(gtk_entry_get_text(g_list_nth_data(user_data, 0)))) {
        printf("Incorrect firstname. Only Latin and Russian letters are allowed.\n");
        popupNotification("Incorrect firstname. Only Latin and Russian letters are allowed.");
        return;
    }

    if (!checkName(gtk_entry_get_text(g_list_nth_data(user_data, 1)))) {
        printf("Incorrect lastname. Only Latin and Russian letters are allowed.\n");
        popupNotification("Incorrect lastname. Only Latin and Russian letters are allowed.");
        return;
    }

    if (!checkLoginAndPasswordCorrectness(gtk_entry_get_text(g_list_nth_data(user_data, 2)))) {
        printf("Incorrect login. Only Latin letters and numbers are allowed.\n");
        popupNotification("Incorrect login. Only Latin letters and numbers are allowed.");
        return;
    }

    if (!checkLoginAndPasswordCorrectness(gtk_entry_get_text(g_list_nth_data(user_data, 3)))) {
        printf("Incorrect password. Only Latin letters and numbers are allowed.\n");
        popupNotification("Incorrect password. Only Latin letters and numbers are allowed.");
        return;
    }

    if (!checkPasswordMatching(gtk_entry_get_text(g_list_nth_data(user_data, 3)),
                               gtk_entry_get_text(g_list_nth_data(user_data, 4)))) {
        printf("Incorrect password. Password mismatch.\n");
        popupNotification("Incorrect password. Password mismatch.");
        return;
    }

    printf("Registration success!\n");
    popupNotification("Registration success!");
}

void authorizationButtonClicked(GtkWidget *button, gpointer user_data) {
    if (!checkLoginAndPasswordCorrectness(gtk_entry_get_text(g_list_nth_data(user_data, 0)))) {
        printf("Incorrect login. Only Latin letters and numbers are allowed.\n");
        popupNotification("Incorrect login. Only Latin letters and numbers are allowed.");
        return;
    }

    if (!checkLoginAndPasswordCorrectness(gtk_entry_get_text(g_list_nth_data(user_data, 1)))) {
        printf("Incorrect password. Only Latin letters and numbers are allowed.\n");
        popupNotification("Incorrect password. Only Latin letters and numbers are allowed.");
        return;
    }

    printf("Authorization success!\n");
    popupNotification("Authorization success!");
}