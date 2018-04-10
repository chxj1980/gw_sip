#ifndef _GWSIP_H
#define _GWSIP_H

#include "GWSipTime.h"
#include "GWSipPrint.h"
#include "GWSipRegister.h"
#include "GWSipPushResourse.h"
#include "GWSipHandleMessage.h"
#include "GWSipGlobalVariable.h"
#include "GWSipHandleRecordFile.h"
#include "GWSipHandleHistoryAlarm.h"
#include "GWSipParsePTZ.h"
#include "GWSipHandleCall.h"
#include "GWSipHandleSDP.h"
#include "GWSipHandleVideo.h"
#include "GWSipRTP.h"
#include "GWSipHandleSubscriber.h"
#include "GWSipHandleAlarm.h"
#include "eXosip2/eXosip.h"
#include "eXosip2.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "Log.h"

class GWSip
{
public:
	GWSip();
	~GWSip();
	void EventLoop(void);
private:
	int ReadConfig(void);
	int GWSipInit(void);
	int EXosipInit(void);
	int GetLoaclIp(void);
	int GWSipSetup(void);
	void TimeToCheck(void);
    void HandlePushResourse(void);
    void HandleMessage(eXosip_event_t *gw_event);
    void HandleCallInvite(eXosip_event_t *gw_event);
    void HandleCallAck(eXosip_event_t *gw_event);
    void HandleCallClosed(eXosip_event_t *gw_event);
    void HandleSubscriberNew(eXosip_event_t *gw_event);
    void HandleNotifyAnswer(eXosip_event_t *gw_event);
    

};

#endif
