#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <gtk/gtk.h>
#include "definitions.h"

#include <sqlite3.h>

#define DEFAULT_PORT "27015"

void clientRequestReceiving(void *clientSocket) {
    SOCKET socket = (SOCKET) clientSocket;
    GDateTime *time;
    int testDialogID = 0;
    while (TRUE) {
        char byte;
        int isAlive = recv(socket, &byte, 1, MSG_PEEK);
        if (isAlive == -1) {
            printf("Client %d disconnected\n",
                   socket); // TODO send message to friends of this client, that friend is offline
            return;
        }

        time = g_date_time_new_now_local();
        gchar *temp = g_date_time_format(time, "%H:%M:%S, %d %b %Y, %a");
        void *userData = malloc(TEXT_SIZE * 2);
        int bytesReceived = recv(socket, userData, TEXT_SIZE * 2, 0);
        if (bytesReceived < 0) {
            printf("Warning: received < 0 bytes\n");
            free(userData);
            continue;
        }
        Request *request = userData;
        switch (*request) {
            case REGISTRATION: {
                printf("Creating new user (registration)..\n");
                FullUserInfo *userInfo = userData;

                // Check for login repetition in data-base
                // If login is unique (TEST FOR DEBUG)..
                userInfo->ID = 0;

                int bytesSent = send(socket, (void *) userInfo, sizeof(FullUserInfo), 0);
                if (bytesSent < 0)
                    printf("Warning: sent < 0 bytes\n");

                break;
            }
            case AUTHORIZATION: {
                printf("Searching for user (authorization)..\n");
                FullUserInfo *userInfo = userData;

                // Check for user's existing in data-base
                // If exist (TEST FOR DEBUG)..
                // Let's say we found user with received login. Check for password correction
                char foundPassword[NAME_SIZE] = {0};
                int userID = 0;
                strcpy(foundPassword, "666kostya666");

                if (!strcmp(userInfo->password, foundPassword)) {
                    // Give user his ID
                    userInfo->ID = userID;
                    strcpy(userInfo->firstName, "Kostya");
                    strcpy(userInfo->lastName, "Rumyantsev");
                } else {
                    // Put the value -1 at user ID, because it's wrong password
                    userInfo->ID = -1;
                }

                int bytesSent = send(socket, (void *) userInfo, sizeof(FullUserInfo), 0);
                if (bytesSent < 0)
                    printf("Warning: sent < 0 bytes\n");

                break;
            }
            case CREATE_DIALOG: {
                printf("Processing creating dialog..\n");
                FullDialogInfo *dialogInfo = userData;
                dialogInfo->ID = testDialogID++;

                int bytesSent = send(socket, (void *) dialogInfo, sizeof(FullDialogInfo), 0);
                if (bytesSent < 0)
                    printf("Warning: sent < 0 bytes\n");

                break;
            }
            case SEND_MESSAGE: {
                printf("Processing sending message..\n");
                FullMessageInfo *messageInfo = userData;
                strcpy(messageInfo->date, temp);

                int bytesSent = send(socket, (void *) messageInfo, sizeof(FullMessageInfo), 0);
                if (bytesSent < 0)
                    printf("Warning: sent < 0 bytes\n");

                break;
            }
            case SEND_FRIEND_REQUEST: {
                printf("Processing sending friend request..\n");
                FullUserInfo *userInfo = userData;
                // Допустим, обработали успешно
                userInfo->ID = -3;

                int bytesSent = send(socket, (void *) userInfo, sizeof(FullUserInfo), 0);
                if (bytesSent < 0)
                    printf("Warning: sent < 0 bytes\n");

                break;
            }
            case FRIEND_REQUEST_ACCEPTED: {
                printf("Processing friend request accepting..\n");
                FullUserInfo *userInfo = userData;
                // Обновлены базы данных
                int bytesSent = send(socket, (void *) userInfo, sizeof(FullUserInfo), 0);
                if (bytesSent < 0)
                    printf("Warning: sent < 0 bytes\n");

                break;
            }
            case FRIEND_REQUEST_DECLINED: {
                printf("Processing friend request declining..\n");
                // Обновлены базы данных
                // Не будем посылать никуда запрос

                break;
            }
            case REMOVE_FRIEND: {
                printf("Processing removing friend..\n");
                FullUserInfo *userInfo = userData;
                int bytesSent = send(socket, (void *) userInfo, sizeof(FullUserInfo), 0);
                if (bytesSent < 0)
                    printf("Warning: sent < 0 bytes\n");
                break;
            }
            default:
                printf("Warning in 'clientRequestReceiving': unknown request type\n");

                g_free(temp);
        }

        free(userData);
    }
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

        printf("Client thread is created\n");
        g_thread_new("Client's thread", (GThreadFunc) clientRequestReceiving, (void *) clientSocket);
    }

    return 0;
}
