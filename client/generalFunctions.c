#include "generalFunctions.h"

G_MODULE_EXPORT void windowClose() {
    gtk_main_quit();
}

void timer(GtkLabel *timeLabel) {
    char *formatTime = g_date_time_format(g_date_time_new_now_local(), "%T");
    gtk_label_set_text(timeLabel, formatTime);
    g_free(formatTime);
}