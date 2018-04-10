#ifndef _GWSSIPGLOBALVARIABLE_H
#define _GWSSIPGLOBALVARIABLE_H
#include "GWSip.h"
#include "GWSipRTP.h"
#include <vector>
#include <map>


using namespace std;
typedef struct __CallUser
{
	int stat;
	int Did;
	int CallId;
	char RTPIp[20];
	char RTPPort[6];
}_CallUser;
typedef struct __SendSubInfo
{
	int AlarmType;
	int Did;
}_SendSubInfo;
extern _SendSubInfo SendSubInfo;
extern GWSipRTP MyGWSipRTP;
extern MyRTPSession session[5];
extern _CallUser VCallUser[5];
extern int CallUserNum;
extern map<int, int> map_alarm_vl_did;
extern map<int, int> map_alarm_md_did;
extern map<int, int> map_alarm_cd_did;
extern map<int, int> map_alarm_se_did;
extern char DeviceId[30];
extern char ServerIp[20];
extern char ServerPort[20];
extern char DevicePwd[20];
extern char ServerId[30];
extern char IpcId[30];

extern char LocalIp[30];

extern char Contact[100];
extern char To[100];
extern char From[100];

extern int  Expires;
extern int  LogoutFlag;

extern int video_status;
extern int online_status;
extern int sd_status;
extern int AlarmStatus;

extern int SendSubNoWTime;
extern int isSendSubNotifyOK;
extern char  update_alarm_id[5];

extern int  SaveRecordFlag;
#endif
