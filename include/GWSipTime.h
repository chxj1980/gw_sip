#ifndef _GWSIPTIME_H
#define _GWSIPTIME_H
#include "GWSip.h"
#include <time.h>
class GWSipTime
{
public:
	GWSipTime();
	~GWSipTime();
	time_t SipTimeToTimestamp(const char *SipTime);
	time_t NormalTimeToTimestamp(const char *NormalTime);
	void   NormalTimeToSipTime(const char *NormalTime, char *SipTime);
	void   NormalTimeToSipTime(const char *NormalTime, char *SipTime, int Time);
};

#endif