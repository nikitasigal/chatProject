#include "serverConnection.h"

SOCKET connectToServer() {
    WSADATA wsaData;    // Хранит информацию об определении сокетов Windows (версия библиотеки, и т.п.)
    int WSInitError = WSAStartup(MAKEWORD(2, 2), &wsaData);// Инициализация WS2_32.dll версии 2.2
    if (WSInitError != 0) { // Если произошла ошибка, что WSAStartup вернёт значение не равное 0
        printf("WinSock initialization error. Shutting down...\n");
        return -1;
    }

    struct addrinfo hints, *result;     // Информация о сокете
    ZeroMemory(&hints, sizeof(hints));  // Обнуление области памяти, в которой находится структура. Потому что дальше могут возникнуть ошибки
    hints.ai_family = AF_INET;          // Наш сокет будет принимать запросы в сети Интернет с протоколом 4-ой версии (IPv4)
    hints.ai_socktype = SOCK_STREAM;    // Соединение установим TCP (а точнее, настроим потоки). Разговор ведётся, пока не будет разорвано соединение
    hints.ai_protocol = IPPROTO_TCP;    // Установка настроек сокета для TCP-соединения

    printf("Enter IP:");
    char IP[32] = {0};
    scanf("%s", IP);
    int getAddressInfoError = getaddrinfo(IP, DEFAULT_PORT, &hints, &result);    // Преобразовать текстовую информацию о хосте в адресную
    // Вообще, возвращает в result корректную информацию о сети сервера
    if (getAddressInfoError != 0) {
        printf("Getting host address error. Shutting down...\n");
        WSACleanup();   // Убиваем WS2_32 DLL
        return -2;
    }

    SOCKET serverSocket = INVALID_SOCKET;   // Значение сокета по умолчанию. Если он не изменится - ошибка
    serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol); // Создаём сокет, используя полученные данные о сети
    if (serverSocket == INVALID_SOCKET) {
        printf("Creating socket error. Shutting down...\n");
        WSACleanup(); // Убиваем WS2_32 DLL
        return -3;
    }

    int connectError = connect(serverSocket, result->ai_addr, (int)result->ai_addrlen); // Пытаемся подключиться к серверу
    if (connectError == SOCKET_ERROR) {
        printf("Socket connection error. Shutting down...\n");
        closesocket(serverSocket);  // Вообще, в документации написано, что надо проверить следующий IP, полученный в getaddrinfo, но поф
        freeaddrinfo(result);       // Очищаем result
        WSACleanup();
        return -4;
    }

    return serverSocket;
}