#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <gtk/gtk.h>
#include "sqlite.h"

#define DEFAULT_PORT "27015"

void clientRequestReceiving(void *clientSocket) {
    sqlite3 *sqliteConn;
    int sqlResult;
    sqlResult = sqlite3_open_v2("database.sqlite", &sqliteConn, SQLITE_OPEN_READWRITE, NULL);
    if (sqlResult != SQLITE_OK) {
        printf("<ERROR> clientRequestReceiving()//%d : Error while opening database - shutting thread down\n", clientSocket);
        return;
    }else
        printf("<LOG> clientRequestReceiving()//%d : Database connection established\n", clientSocket);

    SOCKET socket = (SOCKET) clientSocket;
    GDateTime *time;
    int testDialogID = 0;
    int test = 0;
    while (TRUE) {
        test++;
        time = g_date_time_new_now_local();
        gchar *temp = g_date_time_format(time, "%H:%M:%S, %d %b %Y, %a");
        void *userData = malloc(MAX_PACKAGE_SIZE);
        int bytesReceived = recv(socket, userData, MAX_PACKAGE_SIZE, 0);
        if (bytesReceived < 0) {
            printf("<WARNING> clientRequestReceiving()//%d : Socket received < 0 bytes\n", socket);
            free(userData);
            break;
        }
        Request *request = userData;
        switch (*request) {
            case REGISTRATION: {
                printf("<LOG> clientRequestReceiving()//%d : Registration of new user ...\n", socket);
                FullUserInfo *userInfo = userData;

                sqlRegister(sqliteConn, userInfo);

                int bytesSent = send(socket, (void *) userInfo, sizeof(FullUserInfo), 0);
                if (bytesSent < 0)
                    printf("<WARNING> clientRequestReceiving()//%d : Socket sent < 0 bytes\n", socket);

                break;
            }
            case AUTHORIZATION: {
                printf("<LOG> clientRequestReceiving()//%d : Authorizing user ...\n", socket);
                FullUserInfo *userInfo = userData;

                AuthorizationPackage auPackage;
                sqlAuthorize(sqliteConn, userInfo, &auPackage);

                auPackage.request = AUTHORIZATION;
                int bytesSent = send(socket, (void *) &auPackage, sizeof(AuthorizationPackage), 0);
                if (bytesSent < 0)
                    printf("<WARNING> clientRequestReceiving()//%d : Socket sent < 0 bytes\n", socket);

                break;
            }
            case CREATE_DIALOG: {
                printf("<LOG> clientRequestReceiving()//%d : Creating new dialog ...\n", socket);
                FullDialogInfo *dialogInfo = userData;
                dialogInfo->ID = testDialogID++;

                int bytesSent = send(socket, (void *) dialogInfo, sizeof(FullDialogInfo), 0);
                if (bytesSent < 0)
                    printf("<WARNING> clientRequestReceiving()//%d : Socket sent < 0 bytes\n", socket);

                break;
            }
            case SEND_MESSAGE: {
                printf("<LOG> clientRequestReceiving()//%d : Processing new message ...\n", socket);
                FullMessageInfo *messageInfo = userData;
                strcpy(messageInfo->timestamp, temp);

                int bytesSent = send(socket, (void *) messageInfo, sizeof(FullMessageInfo), 0);
                if (bytesSent < 0)
                    printf("<WARNING> clientRequestReceiving()//%d : Socket sent < 0 bytes\n", socket);

                break;
            }
            case SEND_FRIEND_REQUEST: {
                printf("<LOG> clientRequestReceiving()//%d : Processing new friend request ...\n", socket);
                FullUserInfo *userInfo = userData;
                // Допустим, обработали успешно
                userInfo->ID = -3;

                int bytesSent = send(socket, (void *) userInfo, sizeof(FullUserInfo), 0);
                if (bytesSent < 0)
                    printf("<WARNING> clientRequestReceiving()//%d : Socket sent < 0 bytes\n", socket);

                break;
            }
            case FRIEND_REQUEST_ACCEPTED: {
                printf("<LOG> clientRequestReceiving()//%d : Processing accepted friend request ...\n", socket);
                FullUserInfo *userInfo = userData;
                // Обновлены базы данных
                int bytesSent = send(socket, (void *) userInfo, sizeof(FullUserInfo), 0);
                if (bytesSent < 0)
                    printf("<WARNING> clientRequestReceiving()//%d : Socket sent < 0 bytes\n", socket);

                break;
            }
            case FRIEND_REQUEST_DECLINED: {
                printf("<LOG> clientRequestReceiving()//%d : Processing declined friend request ...\n", socket);
                // Обновлены базы данных
                // Не будем посылать никуда запрос

                break;
            }
            case REMOVE_FRIEND: {
                printf("<LOG> clientRequestReceiving()//%d : Deleting friend ...\n", socket);
                FullUserInfo *userInfo = userData;
                int bytesSent = send(socket, (void *) userInfo, sizeof(FullUserInfo), 0);
                if (bytesSent < 0)
                    printf("<WARNING> clientRequestReceiving()//%d : Socket sent < 0 bytes\n", socket);

                break;
            }
            case LEAVE_DIALOG: {
                printf("<LOG> clientRequestReceiving()//%d : Leaving dialog ...\n", socket);
                FullUserInfo *userInfo = userData;
                int bytesSent = send(socket, (void *) userInfo, sizeof(FullUserInfo), 0);
                if (bytesSent < 0)
                    printf("<WARNING> clientRequestReceiving()//%d : Socket sent < 0 bytes\n", socket);

                break;
            }
            case LOAD_MESSAGES: {
                printf("<LOG> clientRequestReceiving()//%d : Sending dialog messages ...\n", socket);
                FullDialogInfo *dialogInfo = userData;

                // Test
                FullMessageInfo messageInfo;
                messageInfo.chatID = dialogInfo->ID;
                strcpy(messageInfo.firstName, "Lol");
                strcpy(messageInfo.lastName, "Lol");
                strcpy(messageInfo.username, "Lol");
                strcpy(messageInfo.timestamp, "12 мая 22:01");
                strcpy(messageInfo.text, "Test message!!");

                MessagesPackage messagesPackage = {LOAD_MESSAGES, 5,
                                                   {messageInfo, messageInfo, messageInfo, messageInfo, messageInfo}};

                int bytesSent = send(socket, (void *) &messagesPackage, sizeof(MessagesPackage), 0);
                if (bytesSent < 0)
                    printf("<WARNING> clientRequestReceiving()//%d : Socket sent < 0 bytes\n", socket);

                break;
            }
            case DIALOG_ADD_USER: {
                /*printf("Adding new user to chat...\n");

                FullUserInfo user1 = {0, 1, "Lol", "Kek", "Check", "additionalInfo1"};
                FullUserInfo user2 = {0, 1, "Aba", "baba", "abababa", "additionalInfo2"};

                // Test
                FullDialogInfo dialogInfo = {DIALOG_ADD_USER, 4, "abc", {user1, user2}, 2, TRUE, 0};

                int bytesSent = send(socket, (void *) &dialogInfo, sizeof(FullDialogInfo), 0);
                if (bytesSent < 0)
                    printf("<WARNING> clientRequestReceiving()//%d : Socket sent < 0 bytes\n", socket);*/
            }
            default:
                printf("<ERROR> clientRequestReceiving()//%d : Request '%d' is not defined\n", socket, *request);
                g_free(temp);
        }

        free(userData);
    }

    sqlResult = sqlite3_close_v2(sqliteConn);
    if (sqlResult != SQLITE_OK)
        printf("<ERROR> clientRequestReceiving()//%d : Error while closing the database\n", socket);
    else
        printf("<LOG> clientRequestReceiving()//%d : Database connection closed\n", socket);
    printf("<LOG> clientRequestReceiving()//%d : Thread shutting down", socket);
}

int main() {
    struct WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    struct addrinfo hints, *result;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;    // Заполняет IP адрес самостоятельно
    int getAddressInfoError = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (getAddressInfoError != 0) {
        printf("Getting address information error");
        WSACleanup();
        return -1;
    }

    SOCKET listenSocket = INVALID_SOCKET;   // Создаём сокет для прослушивания клиентов
    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket == INVALID_SOCKET) {
        printf("Socket creation error");
        freeaddrinfo(result);
        WSACleanup();
        return -2;
    }

    int bindError = bind(listenSocket, result->ai_addr, (int) result->ai_addrlen);   // Привязываем IP и порт к сокету
    if (bindError == SOCKET_ERROR) {
        printf("Binding error");
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return -3;
    }
    freeaddrinfo(result);   // Очищаем информацию, она больше не нужна

    int listenError = listen(listenSocket, SOMAXCONN);
    if (listenError == SOCKET_ERROR) {
        printf("Listening error");
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return -4;
    }

    printf("Server is started\n");

    while (TRUE) {
        SOCKET clientSocket = INVALID_SOCKET;   // Создаём временный сокет для принятия клиента
        clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printf("Accepting connection error");
            freeaddrinfo(result);
            closesocket(listenSocket);
            WSACleanup();
            return -5;
        }

        printf("<LOG> main(): Client thread for client '%d' is created\n", clientSocket);
        char thread_name[30] = {0};
        sprintf(thread_name, "client_thread_%d", clientSocket);
        g_thread_new(thread_name, (GThreadFunc) clientRequestReceiving, (void *) clientSocket);
    }

    return 0;
}
