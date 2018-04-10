#include "GWSipTime.h"

GWSipTime::GWSipTime()
{
}
GWSipTime::~GWSipTime()
{
}

time_t GWSipTime::SipTimeToTimestamp(const char *SipTime)
{
	char clock_T_y[6];
	memset(clock_T_y, 0, 6);
	strncpy(clock_T_y, SipTime, 4);
	char clock_T_m[4];
	memset(clock_T_m, 0, 4);
	strncpy(clock_T_m, SipTime + 5, 2);
	char clock_T_d[4];
	memset(clock_T_d, 0, 4);
	strncpy(clock_T_d, SipTime + 8, 2);
	char clock_Z_h[4];
	memset(clock_Z_h, 0, 4);
	strncpy(clock_Z_h, SipTime + 11, 2);
	char clock_Z_m[4];
	memset(clock_Z_m, 0, 4);
	strncpy(clock_Z_m, SipTime + 14, 2);
	char clock_Z_s[4];
	memset(clock_Z_s, 0, 4);
	strncpy(clock_Z_s, SipTime + 17, 2);
	time_t timep;    
    struct tm tm; 
    tm.tm_year = atoi(clock_T_y)-1900;
    tm.tm_mon  = atoi(clock_T_m)-1;
    tm.tm_mday = atoi(clock_T_d);
    tm.tm_hour = atoi(clock_Z_h);
    tm.tm_min  = atoi(clock_Z_m);
    tm.tm_sec  = atoi(clock_Z_s);
    timep = mktime(&tm);
    return timep;
}
time_t GWSipTime::NormalTimeToTimestamp(const char *NormalTime)
{
	char clock_T_y[6];
	memset(clock_T_y, 0, 6);
	strncpy(clock_T_y, NormalTime, 4);
	char clock_T_m[4];
	memset(clock_T_m, 0, 4);
	strncpy(clock_T_m, NormalTime + 4, 2);
	char clock_T_d[4];
	memset(clock_T_d, 0, 4);
	strncpy(clock_T_d, NormalTime + 6, 2);
	char clock_Z_h[4];
	memset(clock_Z_h, 0, 4);
	strncpy(clock_Z_h, NormalTime + 8, 2);
	char clock_Z_m[4];
	memset(clock_Z_m, 0, 4);
	strncpy(clock_Z_m, NormalTime + 10, 2);
	char clock_Z_s[4];
	memset(clock_Z_s, 0, 4);
	strncpy(clock_Z_s, NormalTime + 12, 2);
	time_t timep;    
    struct tm tm; 
    tm.tm_year = atoi(clock_T_y)-1900;
    tm.tm_mon  = atoi(clock_T_m)-1;
    tm.tm_mday = atoi(clock_T_d);
    tm.tm_hour = atoi(clock_Z_h);
    tm.tm_min  = atoi(clock_Z_m);
    tm.tm_sec  = atoi(clock_Z_s);
    timep = mktime(&tm);
    return timep;
}
void GWSipTime::NormalTimeToSipTime(const char *NormalTime, char *SipTime)
{
	char clock_T_y[6];
	memset(clock_T_y, 0, 6);
	strncpy(clock_T_y, NormalTime, 4);
	char clock_T_m[4];
	memset(clock_T_m, 0, 4);
	strncpy(clock_T_m, NormalTime + 4, 2);
	char clock_T_d[4];
	memset(clock_T_d, 0, 4);
	strncpy(clock_T_d, NormalTime + 6, 2);
	char clock_Z_h[4];
	memset(clock_Z_h, 0, 4);
	strncpy(clock_Z_h, NormalTime + 8, 2);
	char clock_Z_m[4];
	memset(clock_Z_m, 0, 4);
	strncpy(clock_Z_m, NormalTime + 10, 2);
	char clock_Z_s[4];
	memset(clock_Z_s, 0, 4);
	strncpy(clock_Z_s, NormalTime + 12, 2);
	sprintf(SipTime, "%s-%s-%sT%s:%s:%sZ",
		clock_T_y, clock_T_m, clock_T_d,
		clock_Z_h, clock_Z_m, clock_Z_s);
}
void GWSipTime::NormalTimeToSipTime(const char *NormalTime, char *SipTime, int Time)
{
	struct tm *tm;
	time_t timep = NormalTimeToTimestamp(NormalTime) + time_t(Time);
    tm = localtime(&timep);
	sprintf(SipTime, "%d-%02d-%02dT%02d:%02d:%02dZ",
		tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
}
