#include "generalFunctions.h"
#include "GUI_Initialization.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_SERVER_IP "25.48.127.168"
#define DEFAULT_PORT "27015"

gboolean connectToServer() {
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
        return FALSE;
    }

    SOCKET serverSocket = INVALID_SOCKET;   // Значение сокета по умолчанию. Если он не изменится - ошибка
    serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol); // Создаём сокет, используя полученные данные о сети
    if (serverSocket == INVALID_SOCKET) {
        printf("Creating socket error. Shutting down...");
        WSACleanup(); // Убиваем WS2_32 DLL
        return FALSE;
    }

    int connectError = connect(serverSocket, result->ai_addr, (int)result->ai_addrlen); // Пытаемся подключиться к серверу
    if (connectError == SOCKET_ERROR) {
        printf("Socket connection error. Shutting down...");
        closesocket(serverSocket);  // Вообще, в документации написано, что надо проверить следующий IP, полученный в getaddrinfo, но поф
        freeaddrinfo(result);       // Очищаем result
        WSACleanup();
        return FALSE;
    }

    return TRUE;
}

int main(int argc, char *argv[]) {
    WSADATA wsaData;    // Хранит информацию об определении сокетов Windows (версия библиотеки, и т.п.)
    int WSInitError = WSAStartup(MAKEWORD(2, 2), &wsaData);// Инициализация WS2_32.dll версии 2.2
    if (WSInitError != 0) { // Если произошла ошибка, что WSAStartup вернёт значение не равное 0
        printf("WinSock initialization error. Shutting down...");
        return FALSE;
    }

    if (!connectToServer())
        return -1;


    gtk_init(&argc, &argv);

    GUIInit();

    gtk_main();

    return 0;
}
