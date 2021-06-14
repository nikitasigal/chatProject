#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <gtk/gtk.h>
#include "sqlite.h"

#define DEFAULT_PORT "27015"

typedef struct {
    int usID;
    SOCKET usSocket;
} ID_Socket;

ID_Socket connection[MAX_NUMBER_OF_USERS];
int connectionSize = 0;

void clientRequestReceiving(void *clientSocket) {
    sqlite3 *sqliteConn;
    int sqlResult = sqlite3_open_v2("database.sqlite", &sqliteConn, SQLITE_OPEN_READWRITE, NULL);
    if (sqlResult != SQLITE_OK) {
        g_critical("Thread %3d : Error while opening database - shutting thread down", clientSocket);
        return;
    } else
        g_message("Thread %3d : Database connection established", clientSocket);

    SOCKET socket = (SOCKET) clientSocket;
    while (TRUE) {
        void *userData = malloc(MAX_PACKAGE_SIZE);
        int bytesReceived = recv(socket, userData, MAX_PACKAGE_SIZE, 0);
        if (bytesReceived < 0) {
            g_message("Thread %3d : Client disconnected", socket);
            free(userData);
            break;
        }

        Request *request = userData;
        switch (*request) {
            case REGISTRATION: {
                g_message("Thread %3d : Registration of new user ...", socket);
                FullUserInfo *userInfo = userData;

                sqlRegister(sqliteConn, userInfo);
                if (userInfo->ID > 0) {
                    connection[connectionSize].usID = userInfo->ID;
                    connection[connectionSize].usSocket = socket;
                    connectionSize++;
                }

                int bytesSent = send(socket, (void *) userInfo, sizeof(FullUserInfo), 0);
                if (bytesSent < 0)
                    g_warning("Tread %3d : Socket sent < 0 bytes", socket);

                break;
            }
            case AUTHORIZATION: {
                g_message("Thread %3d : Authorizing user ...", socket);
                FullUserInfo *userInfo = userData;

                AuthorizationPackage authPackage;
                sqlAuthorize(sqliteConn, userInfo, &authPackage);
                short int exist = 0;
                for (int i = 0; i < connectionSize; i++) {
                    if (connection[i].usID == authPackage.authorizedUser.ID) {

                        if (connection[i].usSocket != 0) {
                            //отказ
                            //но может быть мусор - подумать
                            printf("Yup, he sure sucks");
                        } else {
                            connection[i].usSocket = socket;
                        }

                        exist = 1;
                        break;
                    }
                }

                if (!exist && authPackage.authorizedUser.ID > 0) {
                    connection[connectionSize].usID = authPackage.authorizedUser.ID;
                    connection[connectionSize].usSocket = socket;
                    connectionSize++;
                }

                authPackage.request = AUTHORIZATION;
                int bytesSent = send(socket, (void *) &authPackage, sizeof(AuthorizationPackage), 0);
                if (bytesSent < 0)
                    g_warning("Thread %3d : Socket sent < 0 bytes", socket);
                break;
            }
            case CREATE_DIALOG: {
                g_message("Thread %3d : Creating new dialog ...", socket);
                FullDialogInfo *dialogInfo = userData;

                sqlCreateDialog(sqliteConn, dialogInfo);
                if (dialogInfo->ID == -1) {
                    // error
                    break;
                }

                // TODO - Send dialogInfo to all users in dialogInfo->userList
                for (int i = 0; i < dialogInfo->userCount; i++) {
                    for (int j = 0; j < connectionSize; j++) {
                        if (connection[j].usID == dialogInfo->userList[i].ID && connection[j].usSocket != 0) {
                            if (socket == connection[j].usSocket)
                                dialogInfo->isSupposedToOpen = TRUE;
                            else
                                dialogInfo->isSupposedToOpen = FALSE;
                            int bytesSent = send(connection[j].usSocket, (void *) dialogInfo, sizeof(FullDialogInfo),0);

                            if (bytesSent < 0)
                                g_warning("Thread %3d : Socket sent < 0 bytes", socket);
                        }
                    }
                }
                break;
            }
            case SEND_MESSAGE: {
                g_message("Thread %3d : Processing new message ...", socket);
                FullMessageInfo *messageInfo = userData;

                FullDialogInfo dialogInfo;
                sqlSendMessage(sqliteConn, messageInfo, &dialogInfo);
                if (messageInfo->chatID == -1) {
                    // error
                    break;
                }

                // TODO - Send messageInfo to all users in membersList
                for (int i = 0; i < dialogInfo.userCount; i++) {
                    for (int j = 0; j < connectionSize; j++) {
                        if (dialogInfo.userList[i].ID == connection[j].usID && connection[j].usSocket != 0) {
                            int bytesSent = send(connection[j].usSocket, (void *) messageInfo, sizeof(FullMessageInfo),
                                                 0);
                            if (bytesSent < 0)
                                g_warning("Thread %3d : Socket '%d' sent < 0 bytes", socket);
                        }
                    }
                }
                break;
            }
            case SEND_FRIEND_REQUEST: {
                g_message("Thread %3d : Processing new friend request ...", socket);
                FullUserInfo *userInfo = userData;

                int requestID;
                sqlSendFriendRequest(sqliteConn, userInfo, &requestID);

                if (userInfo->ID == -1 || userInfo->ID == -2) {
                    int bytesSent = send(socket, (void *) userInfo, sizeof(FullUserInfo), 0);
                    if (bytesSent < 0)
                        g_warning("Thread %3d : Socket sent < 0 bytes", socket);

                    break;
                }

                for (int i = 0; i < connectionSize; i++) {
                    if (requestID == connection[i].usID) {
                        int bytesSent = send(connection[i].usSocket, (void *) userInfo, sizeof(FullUserInfo), 0);
                        if (bytesSent < 0)
                            g_warning("Thread %3d : Socket sent < 0 bytes", connection[i].usSocket);
                    }
                }

                if (userInfo->ID >= 0)
                    userInfo->ID = -3;

                int bytesSent = send(socket, (void *) userInfo, sizeof(FullUserInfo), 0);
                if (bytesSent < 0)
                    g_warning("Thread %3d : Socket sent < 0 bytes", socket);
                break;
            }
            case FRIEND_REQUEST_ACCEPTED: {
                g_message("Thread %3d : Processing accepted friend request ...", socket);
                FullUserInfo *userInfo = userData;

                FullUserInfo sender;
                sqlAcceptFriendRequest(sqliteConn, userInfo, &sender);
                if (userInfo->ID == -1) {
                    // error
                    break;
                }
                sender.request = FRIEND_REQUEST_ACCEPTED;
                // TODO - Proceed with logic from .txt file

                // temporary send to user, who answered the request
                int bytesSent = send(socket, (void *) &sender, sizeof(FullUserInfo), 0);
                if (bytesSent < 0)
                    g_warning("Thread %3d : Socket sent < 0 bytes", socket);

                for (int i = 0; i < connectionSize; i++) {
                    if (sender.ID == connection[i].usID) {
                        int bytesSent = send(connection[i].usSocket, (void *) userInfo, sizeof(FullUserInfo), 0);
                        if (bytesSent < 0)
                            g_warning("Thread %3d : Socket sent < 0 bytes", connection[i].usSocket);
                    }
                }

                break;
            }
            case FRIEND_REQUEST_DECLINED: {
                g_message("Thread %3d : Processing declined friend request ...", socket);
                FullUserInfo *userInfo = userData;

                sqlDeclineFriendRequest(sqliteConn, userInfo);
                if (userInfo->ID == -1) {
                    // error
                    break;
                }
                break;
            }
            case REMOVE_FRIEND: {
                g_message("Thread %3d : Deleting friend ...", socket);
                FullUserInfo *userInfo = userData;

                int friendID;
                sqlRemoveFriend(sqliteConn, userInfo, &friendID);
                if (userInfo->ID == -1) {
                    // error
                    break;
                }

                for (int i = 0; i < connectionSize; i++)
                    if (connection[i].usID == friendID) {
                        int bytesSent = send(connection[i].usSocket, (void *) userInfo, sizeof(FullUserInfo), 0);
                        if (bytesSent < 0)
                            g_warning("Thread %3d : Socket sent < 0 bytes", socket);
                        break;
                    }

                break;
            }
            case LEAVE_DIALOG: {
                g_message("Thread %3d : Leaving dialog ...", socket);
                FullUserInfo *userInfo = userData;

                FullDialogInfo dialogInfo;
                sqlLeaveDialog(sqliteConn, userInfo, &dialogInfo);
                if (userInfo->ID == -1) {
                    // error
                    break;
                }

                // TODO - Proceed with logic from .txt file
                // Send back to all users in dialogInfo->userList

                for (int i = 0; i < dialogInfo.userCount; i++) {
                    for (int j = 0; j < connectionSize; j++) {
                        if (dialogInfo.userList[i].ID == connection[j].usID && connection[j].usSocket != 0) {
                            int bytesSent = send(connection[j].usSocket, (void *) userInfo, sizeof(FullUserInfo), 0);
                            if (bytesSent < 0)
                                g_warning("Thread %3d : Socket sent < 0 bytes", connection[j].usSocket);
                        }
                    }
                }

                break;
            }
            case LOAD_MESSAGES: {
                g_message("Thread %3d : Loading messages ...", socket);
                FullDialogInfo *dialogInfo = userData;
                MessagesPackage msgPackage;

                sqlLoadMessages(sqliteConn, dialogInfo, &msgPackage);

                msgPackage.request = LOAD_MESSAGES;
                int bytesSent = send(socket, (void *) &msgPackage, sizeof(MessagesPackage), 0);
                if (bytesSent < 0)
                    g_warning("Thread %3d : Socket sent < 0 bytes", socket);
                break;
            }
            case DIALOG_ADD_USER: {
                g_message("Thread %3d : Adding user to dialog ...", socket);
                FullUserInfo *userInfo = userData;

                FullDialogInfo result;
                sqlJoinDialog(sqliteConn, userInfo, &result);
                if (userInfo->ID < 0) {
                    // error
                    break;
                }

                result.request = DIALOG_ADD_USER;
                result.isSupposedToOpen = -1;
                result.isGroup = TRUE;
                SOCKET current;

                for (int i = 0; i < connectionSize; i++) {
                    if (connection[i].usID == userInfo->ID && connection[i].usSocket != 0) {
                        current = connection[i].usSocket;
                        int bytesSent = send(connection[i].usSocket, (void *) &result, sizeof(FullDialogInfo), 0);
                        if (bytesSent < 0)
                            g_warning("Thread %3d : Socket sent < 0 bytes", connection[i].usSocket);
                        break;
                    }
                }
                result.isSupposedToOpen = 0;
                strcpy(result.name, userInfo->username);

                for (int i = 0; i < result.userCount; i++) {
                    for (int j = 0; j < connectionSize; j++) {
                        if (result.userList[i].ID == connection[j].usID && connection[j].usSocket != 0 && connection[j].usSocket != current) {
                            int bytesSent = send(connection[j].usSocket, (void *) &result, sizeof(FullDialogInfo), 0);
                            if (bytesSent < 0)
                                g_warning("Thread %3d : Socket sent < 0 bytes", connection[j].usSocket);
                        }
                    }
                }
                break;
            }
            default:
                g_critical("Thread %3d : Request '%d' is not defined", socket, *request);
        }

        free(userData);
    }

    for (int i = 0; i < connectionSize; i++) {
        if (connection[i].usSocket == (SOCKET) clientSocket) {
            connection[i].usSocket = 0;
            break;
        }
    }

    sqlResult = sqlite3_close_v2(sqliteConn);
    if (sqlResult != SQLITE_OK)
        g_critical("Thread %3d : Error while closing the database", socket);
    else
        g_message("Thread %3d : Database connection closed", socket);
    g_message("Thread %3d : Thread shutting down", socket);
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
        g_critical("Getting address information error");
        WSACleanup();
        return -1;
    }

    SOCKET listenSocket = INVALID_SOCKET;   // Создаём сокет для прослушивания клиентов
    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket == INVALID_SOCKET) {
        g_critical("Socket creation error");
        freeaddrinfo(result);
        WSACleanup();
        return -2;
    }

    int bindError = bind(listenSocket, result->ai_addr, (int) result->ai_addrlen);   // Привязываем IP и порт к сокету
    if (bindError == SOCKET_ERROR) {
        g_critical("Binding error");
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return -3;
    }
    freeaddrinfo(result);   // Очищаем информацию, она больше не нужна

    int listenError = listen(listenSocket, SOMAXCONN);
    if (listenError == SOCKET_ERROR) {
        g_critical("Listening error");
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return -4;
    }

    g_message("Server is online");

    while (TRUE) {
        SOCKET clientSocket = INVALID_SOCKET;   // Создаём временный сокет для принятия клиента
        clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            g_critical("Accepting connection error");
            freeaddrinfo(result);
            closesocket(listenSocket);
            WSACleanup();
            return -5;
        }

        g_message("main(): Created thread for client '%3d'", clientSocket);
        char thread_name[30] = {0};
        sprintf(thread_name, "client_thread_%3d", clientSocket);
        g_thread_new(thread_name, (GThreadFunc) clientRequestReceiving, (void *) clientSocket);
    }

    return 0;
}
