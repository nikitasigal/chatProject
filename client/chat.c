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
        gtk_list_box_set_adjustment(GTK_LIST_BOX(msgListBox), adjustment);
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
        gtk_list_box_set_adjustment(GTK_LIST_BOX(msgListBox), adjustment);
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

void processMsgMenu(GtkWidget *widget, GdkEvent *event, GtkMenu *msgMenu) {
    GtkWidget *row = gtk_widget_get_parent(widget);
    if (event->button.button == GDK_BUTTON_SECONDARY && gtk_list_box_row_is_selected(GTK_LIST_BOX_ROW(row)))
        gtk_menu_popup_at_pointer(GTK_MENU(msgMenu), event);
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
    strcpy(messageInfo.lastName, currentUser->lastName);
    strcpy(messageInfo.login, currentUser->login);
    strcpy(messageInfo.text, message);

    clientRequest_SendMessage(*serverDescriptor, messageInfo);
}
