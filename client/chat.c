#include "chat.h"
#include "clientCommand.h"

gdouble lastAdj = 0;

gdouble g_abs(gdouble number) {
    return number < 0 ? -number : number;
}

void sizeAllocate(GtkWidget *msgListBox, GdkRectangle *allocation, int *dialogIsJustOpened) {
    if (*dialogIsJustOpened) {
        GtkAdjustment *adjustment = gtk_list_box_get_adjustment(GTK_LIST_BOX(msgListBox));
        gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment));
        gtk_list_box_set_adjustment(GTK_LIST_BOX(msgList), adjustment);
        *dialogIsJustOpened = FALSE;
        return;
    }

    GtkAdjustment *adjustment = gtk_list_box_get_adjustment(GTK_LIST_BOX(msgListBox));
    gdouble curAdj = gtk_adjustment_get_value(adjustment);
    gdouble pageSize = gtk_adjustment_get_page_size(adjustment);
    gdouble maxLastAdj = gtk_adjustment_get_upper(adjustment);
    gdouble result = maxLastAdj - (maxLastAdj - lastAdj) - curAdj - pageSize;
    if (g_abs(result) < 1e-6) {
        gtk_adjustment_set_value(adjustment, maxLastAdj);
        gtk_list_box_set_adjustment(GTK_LIST_BOX(msgList), adjustment);
    }
}

G_MODULE_EXPORT void processMsgSelecting(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    GtkWidget *row = gtk_widget_get_parent(widget);

    if (event->button.button == GDK_BUTTON_PRIMARY) {
        if (!gtk_list_box_row_is_selected(GTK_LIST_BOX_ROW(row)))
            gtk_list_box_select_row(user_data, GTK_LIST_BOX_ROW(row));
        else
            gtk_list_box_unselect_row(user_data, GTK_LIST_BOX_ROW(row));
    }
}

void processMsgMenu(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    GtkWidget *row = gtk_widget_get_parent(widget);
    if (event->button.button == GDK_BUTTON_SECONDARY && gtk_list_box_row_is_selected(GTK_LIST_BOX_ROW(row)))
        gtk_menu_popup_at_pointer(GTK_MENU(msgMenu), event);
}

void insertMessageInChat(char *name, char *date, char *message, GtkListBox *msgListBox) {
    // Creating main message box
    GtkWidget *msgMainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_widget_set_margin_top(msgMainBox, 8);
    gtk_widget_set_margin_bottom(msgMainBox, 8);
    gtk_widget_set_margin_start(msgMainBox, 10);
    gtk_widget_set_margin_end(msgMainBox, 10);

    // Creating event box that catch clicking
    GtkWidget *eventBox = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(eventBox), msgMainBox);
    g_signal_connect(eventBox, "button-press-event", (GCallback) processMsgSelecting, msgListBox);
    g_signal_connect(eventBox, "button-release-event", (GCallback) processMsgMenu, NULL);

    // Name and date message box
    GtkWidget *msgNameAndDateBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    // Sender name
    GtkWidget *msgNameLabel = gtk_label_new(name);
    gtk_widget_set_halign(msgNameLabel, GTK_ALIGN_START);

    // Message date
    GtkWidget *msgDateLabel = gtk_label_new(date);
    gtk_widget_set_halign(msgDateLabel, GTK_ALIGN_END);

    // Message text
    GtkWidget *msgTextLabel = gtk_label_new(message);
    gtk_label_set_xalign(GTK_LABEL(msgTextLabel), 0);
    gtk_label_set_line_wrap_mode(GTK_LABEL(msgTextLabel), PANGO_WRAP_WORD_CHAR);
    gtk_label_set_line_wrap(GTK_LABEL(msgTextLabel), gtk_true());
    gtk_label_set_max_width_chars(GTK_LABEL(msgTextLabel), 40);

    // Fill upper box
    gtk_box_pack_start(GTK_BOX(msgNameAndDateBox), msgNameLabel, gtk_true(), gtk_true(), 0);
    gtk_box_pack_start(GTK_BOX(msgNameAndDateBox), msgDateLabel, gtk_true(), gtk_true(), 0);

    // Insert upper box in main message box
    gtk_box_pack_start(GTK_BOX(msgMainBox), msgNameAndDateBox, gtk_true(), gtk_true(), 0);

    // Insert text label in main message box
    gtk_box_pack_start(GTK_BOX(msgMainBox), msgTextLabel, gtk_true(), gtk_true(), 0);

    // Append new message into a chat
    lastAdj = gtk_adjustment_get_upper(gtk_list_box_get_adjustment(msgListBox));
    gtk_list_box_insert(msgListBox, eventBox, -1);

    gtk_widget_show_all(eventBox);
}

void sendMessage(GtkWidget *button, GList *data) {
    // Разархивируем
    Dialog *currentDialog = g_list_nth_data(data, 0);
    GList *additionalInfoTemp = g_list_nth_data(data, 1);
    GList *additionalInfo = g_list_nth_data(additionalInfoTemp, 1);
    SOCKET *serverDescriptor = g_list_nth_data(additionalInfo, SERVER_SOCKET);
    GtkWidget *dialogEntry = g_list_nth_data(additionalInfo, CHAT_ENTRY);
    FullUserInfo *currentUser = g_list_nth_data(additionalInfo, CURRENT_USER);

    gtk_widget_grab_focus(GTK_WIDGET(dialogEntry));

    // Name TODO имя текущего пользователя

    // Message from entry
    char message[TEXT_SIZE] = {0};
    strcpy(message, gtk_entry_get_text(GTK_ENTRY(dialogEntry)));
    if (strlen(message) == 0)
        return;
    gtk_entry_set_text(GTK_ENTRY(dialogEntry), "");

    // Соберём инфу о сообщении TODO инфа о пользователе
    FullMessageInfo messageInfo;
    messageInfo.ID = currentDialog->ID;
    strcpy(messageInfo.firstName, currentUser->firstName);
    strcpy(messageInfo.lastName, currentUser->secondName);
    strcpy(messageInfo.login, currentUser->username);
    strcpy(messageInfo.text, message);

    clientRequest_SendMessage(*serverDescriptor, messageInfo);

    //insertMessageInChat("Korostast", timeFormat, message, currentDialog->msgList);
}
