#ifndef _GWSIPHANDLEALARM_H
#define _GWSIPHANDLEALARM_H
#include "GWSip.h"

class GWSipHandleAlarm
{
public:
	GWSipHandleAlarm();
	~GWSipHandleAlarm();
	void FindAlarm();
	void CheckAlarm();
	int CheckInsubscriptionExpires(int Did);
	int SaveAlarmBegin(int status, int type);
	int SaveAlarmEnd(int status, int type);
	void SendSubscriberNotify(int did, char *_starttime, char*_endtime, char *satus, char *type, int eventType);
	int ISHaveSubscriber();
};

#endif