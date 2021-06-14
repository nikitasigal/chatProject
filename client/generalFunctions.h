#ifndef TESTGTK_GENERALFUNCTIONS_H
#define TESTGTK_GENERALFUNCTIONS_H

#include "../shared/definitions.h"

G_MODULE_EXPORT void windowClose();

void timer(GtkLabel *timeLabel);

gboolean popupNotification(char *string);

#endif //TESTGTK_GENERALFUNCTIONS_H
