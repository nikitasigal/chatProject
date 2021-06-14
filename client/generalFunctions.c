#include "generalFunctions.h"

typedef struct {
    short counter;
    GtkWidget *window;
} NotificationStruct;

int notificationCount = 0;

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

gboolean popupNotification(char *string) {
    if (notificationCount == 5)
        return FALSE;

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

    gtk_widget_set_opacity(popupWindow, 100);
    GdkRectangle workArea = {0};
    gdk_monitor_get_workarea(gdk_display_get_primary_monitor(gdk_display_get_default()), &workArea);
    gtk_window_move(GTK_WINDOW(popupWindow), workArea.width - 250, workArea.height - 140 * (*i + 1));

    gtk_widget_show_all(popupWindow);

    gdk_threads_add_timeout(20, G_SOURCE_FUNC(fadeOutAnimation), i);

    return FALSE;
}

G_MODULE_EXPORT void windowClose() {
    gtk_main_quit();
}

void timer(GtkLabel *timeLabel) {
    char *formatTime = g_date_time_format(g_date_time_new_now_local(), "%T");
    gtk_label_set_text(timeLabel, formatTime);
    g_free(formatTime);
}