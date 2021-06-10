#include "generalFunctions.h"
#include "GUI_Initialization.h"
#include "ServerHandler/serverConnection.h"

int main(int argc, char *argv[]) {
    SOCKET serverSocket = INVALID_SOCKET;
    serverSocket = connectToServer();
    if (serverSocket == INVALID_SOCKET)
        printf("Can't connect to server\n");

    gtk_init(&argc, &argv);

    GUIInit(&serverSocket);

    gtk_main();

    return 0;
}
