#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <sqlite3.h>

#define DEFAULT_PORT "27015"

#define MESSAGE_LENGTH 256

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

    int bindError = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);   // Привязываем IP и порт к сокету
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



    SOCKET clientSocket = INVALID_SOCKET;   // Создаём временный сокет для принятия клиента
    clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        printf("Accepting connection error");
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return -5;
    }

    char message[MESSAGE_LENGTH] = {0};
    int recBytes = recv(clientSocket, message, MESSAGE_LENGTH, 0);  // Принимаем сообщение от клиента
    if (recBytes < 0) {
        printf("Receiving message error");
        closesocket(clientSocket);
        WSACleanup();
        return -6;
    }

    int sentBytes = send(clientSocket, message, MESSAGE_LENGTH, 0); // Отправляем сообщение клиенту обратно
    if (sentBytes < 0) {
        printf("Sending message error");
        closesocket(clientSocket);
        WSACleanup();
        return -7;
    }

    printf("Message from client: %s\n", message);

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
