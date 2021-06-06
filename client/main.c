#include "generalFunctions.h"
#include "GUI_Initialization.h"
#include "clientCommand.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_PORT "27015"

SOCKET connectToServer() {
    WSADATA wsaData;    // Хранит информацию об определении сокетов Windows (версия библиотеки, и т.п.)
    int WSInitError = WSAStartup(MAKEWORD(2, 2), &wsaData);// Инициализация WS2_32.dll версии 2.2
    if (WSInitError != 0) { // Если произошла ошибка, что WSAStartup вернёт значение не равное 0
        printf("WinSock initialization error. Shutting down...");
        return -1;
    }

    struct addrinfo hints, *result;     // Информация о сокете
    ZeroMemory(&hints, sizeof(hints));  // Обнуление области памяти, в которой находится структура. Потому что дальше могут возникнуть ошибки
    hints.ai_family = AF_INET;          // Наш сокет будет принимать запросы в сети Интернет с протоколом 4-ой версии (IPv4)
    hints.ai_socktype = SOCK_STREAM;    // Соединение установим TCP (а точнее, настроим потоки). Разговор ведётся, пока не будет разорвано соединение
    hints.ai_protocol = IPPROTO_TCP;    // Установка настроек сокета для TCP-соединения

    int getAddressInfoError = getaddrinfo(DEFAULT_SERVER_IP, DEFAULT_PORT, &hints, &result);    // Преобразовать текстовую информацию о хосте в адресную
    // Вообще, возвращает в result корректную информацию о сети сервера
    if (getAddressInfoError != 0) {
        printf("Getting host address error. Shutting down...");
        WSACleanup();   // Убиваем WS2_32 DLL
        return -2;
    }

    SOCKET serverSocket = INVALID_SOCKET;   // Значение сокета по умолчанию. Если он не изменится - ошибка
    serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol); // Создаём сокет, используя полученные данные о сети
    if (serverSocket == INVALID_SOCKET) {
        printf("Creating socket error. Shutting down...");
        WSACleanup(); // Убиваем WS2_32 DLL
        return -3;
    }

    int connectError = connect(serverSocket, result->ai_addr, (int)result->ai_addrlen); // Пытаемся подключиться к серверу
    if (connectError == SOCKET_ERROR) {
        printf("Socket connection error. Shutting down...");
        closesocket(serverSocket);  // Вообще, в документации написано, что надо проверить следующий IP, полученный в getaddrinfo, но поф
        freeaddrinfo(result);       // Очищаем result
        WSACleanup();
        return -4;
    }

    return serverSocket;
}

int main(int argc, char *argv[]) {
    /*SOCKET serverSocket = connectToServer();

    // Отправляем данные серверу (сейчас - себе)
    char message[MESSAGE_LENGTH] = {0};
    fgets(message, MESSAGE_LENGTH, stdin);
    int sentBytes = send(serverSocket, message, (int)strlen(message), 0);
    if (sentBytes < SOCKET_ERROR) {
        printf("Data sending error");
        WSACleanup();
        closesocket(serverSocket);
        return -5;
    }

    // Закрываем соединения для отправки данных
    int shutdownError = shutdown(serverSocket, SD_SEND);
    if (shutdownError == SOCKET_ERROR) {
        printf("Shutdown failed");
        WSACleanup();
        closesocket(serverSocket);
        return -6;
    }

    // Принимаем сообщение
    char recMessage[MESSAGE_LENGTH] = {0};
    int recBytes = recv(serverSocket, recMessage, MESSAGE_LENGTH, 0);
    if (recBytes == SOCKET_ERROR) {
        printf("Data receiving error");
        WSACleanup();
        closesocket(serverSocket);
        return -7;
    }

    // Печатаем сообщение
    printf("\nReceived message: %s\n", recMessage);

    closesocket(serverSocket);
    WSACleanup();*/

    SOCKET serverSocket = INVALID_SOCKET;
    serverSocket = connectToServer();
    if (serverSocket == INVALID_SOCKET) {
        printf("Can't connect to server\n");
    }
    //int size = 100;
    //FullUserInfo users[size];

    //unsigned long long bytes = recv(serverSocket, (void *) users, 10000, 0);
    //unsigned long long sizeOfStruct = sizeof(FullUserInfo);
    //unsigned long long number = bytes / sizeOfStruct; // Прислали два объекта

    //for (int i = 0; i < number; ++i)
    //    printf("%d\n", users[i].ID);
    

    gtk_init(&argc, &argv);

    GUIInit(serverSocket);

    gtk_main();

    return 0;
}
