#ifndef _GWSIPHANDLESUBSCRIBER_H
#define _GWSIPHANDLESUBSCRIBER_H
#include "GWSip.h"

class GWSipHandleSubscriber
{
public:
	GWSipHandleSubscriber();
	~GWSipHandleSubscriber();
	void SendInsubscriptionAnswer(int tid, int status_code, osip_message_t* gw_answer);
	void HandleNotifyAnswer(eXosip_event_t* gw_event);
	void HandleSubscriberNew(eXosip_event_t* gw_event);
	int UpdateSendOkAlarm(void);
	int UpdateMapToOK();
	int ISSendOver();
	int CheckVLDid();
	int CheckMDDid();
	int CheckCDDid();
	int CheckSEDid();
};

#endif