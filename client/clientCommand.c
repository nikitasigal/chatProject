#include "clientCommand.h"
#include "chat.h"
#include "messages.h"

/*typedef struct {
    int ID;
    char firstName[TEXT_SIZE];
    char lastName[TEXT_SIZE];
    char login[TEXT_SIZE];
} User;*/
// А также должны быть:
GList *friendList; // GtkListBox
GtkListBox *dialogListBox;

/*typedef struct {
    int ID;
    char name[TEXT_SIZE];
    GtkListBox *userList;
    GtkListBox *msgList;
} Dialog;*/
//GList *dialogList;

typedef struct {
    char date[TEXT_SIZE];
    char fullName[TEXT_SIZE]; // first/last names + login
    char text[TEXT_SIZE];
    //int ID; // ID беседы?
} Message;

/*
 * Мы приняли запрос от сервера. Есть несколько видов запросов:
 * 1) Создать беседу с именем name и идентификатором ID (с участниками array userID[] ?)
 * 2) Добавить сообщение в беседу с идентификатором ID, от name в время date c текстом text
 * 3) Запрос на добавление в друзья от пользователя ID с именем name
 * 4) Пользователь с именем name принял запрос на добавление в друзья
 * 5) Друг с именем name появился в сети
 */

/*
 * Мы хотим отправить запрос серверу. Есть несколько видов запросов:
 * 1) Создать беседу с именем name и идентификатором ID с участниками array userID[]
 * 2) Добавить сообщение в беседу с идентификатором ID, от name в время date c текстом text
 * 3) Запрос на добавление в друзья пользователя ID
 * 4) Мы приняли запрос на добавление в друзья пользователя ID
 * 5) Присоединиться к серверу с логином login и паролем password
 * 6) Зарегистрироваться (...)
 */

/*
 * Любой диалог должен содержать информацию:
 * 1) ID
 * 2) Name
 * 3) Список участников (name, login, ID (?))
 * 4) Список сообщений
 */

/*
 * Пользователь должен содержать информацию: (на сервере?)
 * 1) ID
 * 2) First name
 * 3) Last name
 * 4) Login
 * 5) Password (?)
 * 6) Список диалогов
 * 7) Список друзей
 */

/*
 * Сообщение содержит данные:
 * 1) Date
 * 2) Name (login)
 * 3) Text
 * 4) ID беседы (?)
 */

/*
 * Пользователь умеет:
 * 1) Отправлять сообщение в беседу ID
 * 2) Добавлять в друзья
 * 3) Переходить по вкладкам
 */

typedef struct {
    //char fullUserName[NAME_SIZE];
    char firstName[NAME_SIZE];
    char lastName[NAME_SIZE];
    char login[NAME_SIZE];
    char date[DATE_SIZE];
    char text[TEXT_SIZE];
} FullMessageInfo;

typedef struct {
    //char fullUserName[NAME_SIZE];
    int ID;
    char firstName[NAME_SIZE];
    char lastName[NAME_SIZE];
    char login[NAME_SIZE];
} FullUserInfo;

typedef struct {
    int ID;
    char dialogName[NAME_SIZE];
    GList *users;   // List of FullUserInfo
} FullDialogInfo;

void serverRequest_CreateDialog(FullDialogInfo dialogInfo, GList *additionalInfo) {
    // Распакуем нужную дополнительную информацию
    GtkListBox *dialogsListBox = g_list_nth_data(additionalInfo, DIALOGS_LIST_BOX);

    // Создадим новый диалог с информацией: ID, name, ссылка на историю чата, ссылка на виджет чата
    Dialog *newDialog = g_malloc(sizeof(Dialog));
    newDialog->ID = dialogInfo.ID;
    newDialog->userList = GTK_LIST_BOX(gtk_list_box_new());
    newDialog->msgList = GTK_LIST_BOX(gtk_list_box_new());
    strcpy(newDialog->name, dialogInfo.dialogName);

    gtk_list_box_set_selection_mode(newDialog->msgList, GTK_SELECTION_MULTIPLE);
    gtk_list_box_set_selection_mode(newDialog->userList, GTK_SELECTION_NONE);
    g_object_ref(newDialog->msgList);
    g_object_ref(newDialog->userList);

    // Обновим userList
    GList *currentUser = dialogInfo.users;
    while (currentUser != NULL) {
        // TODO fullName
        FullUserInfo *currentUserInfo = (FullUserInfo *) currentUser;
        GtkWidget *newLabel = gtk_label_new(currentUserInfo->login);
        gtk_list_box_insert(newDialog->userList, newLabel, -1);

        currentUser = currentUser->next;
    }

    // Преобразуем ID в строку
    char IDString[TEXT_SIZE] = {0};
    sprintf(IDString, "%d", dialogInfo.ID);

    // Создаём окно с кнопкой беседы, ID и ссылку на чат
    GtkWidget *dialogBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget *dialogLabelWithID = gtk_label_new(IDString);
    GtkWidget *dialogButton = gtk_button_new_with_label(dialogInfo.dialogName);
    gtk_container_add(GTK_CONTAINER(dialogBox), dialogLabelWithID);
    gtk_container_add(GTK_CONTAINER(dialogBox), dialogButton);

    // Добавим созданный диалог в список диалогов
    gtk_list_box_insert(dialogsListBox, dialogBox, -1);

    // Подключаем сигнал нажатия кнопки. При нажатии открывается диалог
    GList *data = NULL;
    data = g_list_append(data, newDialog);
    data = g_list_append(data, additionalInfo); // entry and send-button
    g_signal_connect(dialogButton, "clicked", (GCallback) newOpenDialog, data);
    g_signal_connect(newDialog->msgList, "size-allocate", (GCallback) sizeAllocate, NULL);

    // Сделаем ID невидимым
    gtk_widget_set_no_show_all(dialogLabelWithID, TRUE);

    gtk_widget_show_all(dialogBox);
}

void serverRequest_SendMessage(FullMessageInfo messageInfo) {

}

void serverRequest_AddFriend(FullUserInfo userInfo) {

}

void serverRequest_FriendIsOnline(FullUserInfo userInfo) {

}

void serverRequest_RemoveFriend(FullUserInfo userInfo) {

}

void serverRequest_UserLeaveDialog(FullUserInfo userInfo, FullDialogInfo dialogInfo) {

}

void serverRequest_FriendDisconnect(FullUserInfo userInfo, char *date) {

}

void serverRequestProcess(GList *additionalServerData) {
    // recv();
    // what's kind of message?

}