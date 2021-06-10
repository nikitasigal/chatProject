#ifndef TESTGTK_CLIENTCOMMAND_H
#define TESTGTK_CLIENTCOMMAND_H

#include "../shared/definitions.h"



void serverRequestProcess(GList *additionalServerData);

void serverRequest_CreateDialog(FullDialogInfo dialogInfo, GList *additionalInfo);

void serverRequest_SendMessage(FullMessageInfo messageInfo, GList *additionalInfo);

gboolean serverRequest_Registration(GList *specialAdditionalServerData);

gboolean serverRequest_Authorization(GList *specialAdditionalServerData);

#endif //TESTGTK_CLIENTCOMMAND_H
