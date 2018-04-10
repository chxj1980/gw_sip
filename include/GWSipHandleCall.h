#ifndef _GWSIPHANDLECALL_H
#define _GWSIPHANDLECALL_H
#include "GWSip.h"

class GWSipHandleCall
{
public:
	GWSipHandleCall();
	~GWSipHandleCall();
	void SendCallAnswer(int tid, int status_code, osip_message_t *gw_answer);
	void HandleCallInvite(eXosip_event_t *gw_event);
};

#endif