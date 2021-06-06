#include "generalFunctions.h"
#include "clientCommand.h"
G_MODULE_EXPORT void windowClose() {
    gtk_main_quit();
}