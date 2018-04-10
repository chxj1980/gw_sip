#ifndef _GWSIPHANDLEMESSAGE_H
#define _GWSIPHANDLEMESSAGE_H
#include "GWSip.h"
#include "GWSipHandleXML.h"
class GWSipHandleMessage
{
public:
	GWSipHandleMessage();
	~GWSipHandleMessage();
	void HandleMessage(eXosip_event_t *gw_event);
private:
	void SendAnswer(int tid, int status_code, osip_message_t *gw_answer);

};

#endif