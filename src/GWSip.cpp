#include "GWSip.h"
#include <iostream>
#include <sqlite.h>
#include "Log.h"
char DeviceId[30];
char ServerIp[20];
char ServerPort[20];
char DevicePwd[20];
char ServerId[30];
char IpcId[30];

char LocalIp[30];

char Contact[100];
char To[100];
char From[100];

int  Expires;
int  LogoutFlag;

int  SaveRecordFlag = 0;

int video_status;
int online_status;
int sd_status;
int AlarmStatus = 0;
int SendSubNoWTime = 0;
int isSendSubNotifyOK = 0;
_SendSubInfo SendSubInfo ={0,0};
char  update_alarm_id[5] = {0};

_CallUser VCallUser[5]={{0,0,0,"0","0"},{0,0,0,"0","0"},{0,0,0,"0","0"},{0,0,0,"0","0"},{0,0,0,"0","0"}};
int CallUserNum = 0;
using namespace std;
map<int, int> map_alarm_vl_did;
map<int, int> map_alarm_md_did;
map<int, int> map_alarm_cd_did;
map<int, int> map_alarm_se_did;
GWSip::GWSip()
{
    LogoutFlag = 0;
    video_status = 1;
    online_status = 1;
    sd_status     = 1;
}
GWSip::~GWSip()
{
}
int GWSip::GWSipSetup(void)
{
	int result = 0;

	memset(DeviceId, 0, 30);
	memset(ServerIp, 0, 20);
	memset(ServerPort, 0, 20);
	memset(DevicePwd, 0, 20);
	memset(ServerId, 0, 30);
	memset(IpcId, 0, 30);
    Expires    = 3600;
	result = ReadConfig();
	if(result != 0)
		return -1;

    memset(LocalIp, 0, 30);
    result = GetLoaclIp();
    if(result != 0)
        return -1;

    memset(Contact, 0, 100);
    memset(To, 0, 100);
    memset(From, 0, 100);
    snprintf(Contact, 100, "sip:%s@%s:5060",DeviceId, LocalIp);
    snprintf(To, 100, "sip:%s@%s:%s",DeviceId, ServerIp, ServerPort);
    snprintf(From, 100, "sip:%s@%s:%s",DeviceId, ServerIp, ServerPort);

    return 0;
}
int GWSip::EXosipInit(void)
{
    int i = 0;
    i = eXosip_init();
    if(i != 0)
        return -1;
    i = eXosip_listen_addr (IPPROTO_UDP, NULL, 5060, AF_INET, 0); 
    printf("IPPROTO_UDP->%d\nAF_INET->%d\n", IPPROTO_UDP, AF_INET);
    if( i!=0 )
    {
        eXosip_quit();
        return -1;
    }
    return 0;
}

void GWSip::EventLoop(void)
{
    GWSipRegister MyGWSipRegister;
    GWSipPrint    MyGWSipPrint;
    int RegisterId = -1;
    int RefreshFailedCount = 0;
    int IsPushResourseOK = 0;
    int result = EXosipInit();
    int TimeCount = 0;
    if(result != 0)
        exit(result);
BEGIN:   

    while(LogoutFlag) 
        sleep(2);
    
    result = GWSipSetup();
    if(result != 0)
        exit(result);
    
    RegisterId = MyGWSipRegister.Register();
    if(RegisterId < 0)
        goto BEGIN;

    eXosip_event_t *gw_event  = NULL;
    while (1)
    {
        TimeCount++;
        if(LogoutFlag == 1)
        {
            MyGWSipRegister.Unregister(RegisterId, "Unregister");
            goto BEGIN;
        }
        TimeToCheck();
        if(RefreshFailedCount == 3)
            exit(-1);
        if(IsPushResourseOK == 0 || IsPushResourseOK == 1)
            HandlePushResourse();
        gw_event = eXosip_event_wait(0, 200);
        eXosip_lock();
        eXosip_default_action(gw_event);
        eXosip_automatic_refresh();
        eXosip_unlock();
        if(NULL == gw_event)
            continue;
        MyGWSipPrint.PrintSip(gw_event);
        switch (gw_event->type)
        {
            case EXOSIP_REGISTRATION_FAILURE: RefreshFailedCount++;break;
            case EXOSIP_REGISTRATION_SUCCESS: printf("refresh_register success\n");break;
            case EXOSIP_MESSAGE_ANSWERED:
                if ((NULL != gw_event->response)&&(200 == gw_event->response->status_code))
                    if(IsPushResourseOK == 0 || IsPushResourseOK == 1)
                        IsPushResourseOK ++;
                break;
            case EXOSIP_MESSAGE_NEW:HandleMessage(gw_event);break;
            case EXOSIP_CALL_INVITE:HandleCallInvite(gw_event); break;
            case EXOSIP_CALL_ACK:HandleCallAck(gw_event);break;
            case EXOSIP_CALL_CLOSED:HandleCallClosed(gw_event);break;
            case EXOSIP_IN_SUBSCRIPTION_NEW:HandleSubscriberNew(gw_event);break;
            case EXOSIP_NOTIFICATION_ANSWERED:HandleNotifyAnswer(gw_event);break;
            default: ;break;
        }
        eXosip_event_free(gw_event);
    }
}
void GWSip::TimeToCheck()
{
    GWSipHandleAlarm MyGWSipHandleAlarm;
    MyGWSipHandleAlarm.FindAlarm();
    if(SendSubNoWTime > 0)
        SendSubNoWTime--;
    if(SendSubNoWTime == 0)
        MyGWSipHandleAlarm.CheckAlarm();
}
void GWSip::HandleNotifyAnswer(eXosip_event_t *gw_event)
{
    GWSipHandleSubscriber MyGWSipHandleSubscriber;
    MyGWSipHandleSubscriber.HandleNotifyAnswer(gw_event);
}
void GWSip::HandleSubscriberNew(eXosip_event_t *gw_event)
{
    GWSipHandleSubscriber MyGWSipHandleSubscriber;
    MyGWSipHandleSubscriber.HandleSubscriberNew(gw_event);
}
void GWSip::HandleCallInvite(eXosip_event_t *gw_event)
{
    GWSipHandleCall MyGWSipHandleCall;
    MyGWSipHandleCall.HandleCallInvite(gw_event);
}
void GWSip::HandleCallAck(eXosip_event_t *gw_event)
{
    GWSipHandleVideo MyGWSipHandleVideo;
    MyGWSipHandleVideo.StartSendVideo(gw_event->did);
}
void GWSip::HandleCallClosed(eXosip_event_t *gw_event)
{
    GWSipHandleVideo MyGWSipHandleVideo;
    MyGWSipHandleVideo.CloseVideo(gw_event->did, 1);
}
void GWSip::HandleMessage(eXosip_event_t *gw_event)
{
    GWSipHandleMessage MyGWSipHandleMessage;
    MyGWSipHandleMessage.HandleMessage(gw_event);
}
void GWSip::HandlePushResourse(void) 
{
    GWSipPushResourse MyGWSipPushResourse;
    MyGWSipPushResourse.PushResourse();
}
    
int GWSip::ReadConfig(void)
{
    int count = 0;
    sqlite *_db;
    char **data;
    int res;
    int row, col;
    char *err = 0;
    do
    {
        count ++;
        if(count == 10)
        {
            LOG("Read sip.db failed!\n");
            return -1;
        }  
        _db = sqlite_open("/opt/app/dat/sip.db", 2, &err);
    }while(!_db);
    res = sqlite_get_table(_db,"select * from config",&data,&row,&col,&err);
    if( res != SQLITE_OK )
    {
        LOG("sqlite_get_table sip.db failed!\n");
        sqlite_close(_db);
        return -1;
    }
    snprintf(DeviceId, 30, data[9]);
    snprintf(ServerIp, 20, data[10]);
    snprintf(ServerPort, 20, data[11]);
    snprintf(DevicePwd, 20, data[12]);
    snprintf(ServerId, 30, data[14]);
    Expires = atoi(data[15]);
    LogoutFlag  = atoi(data[16]);
    snprintf(IpcId, 30, data[17]);  
    sqlite_free_table(data);
    sqlite_close(_db);
    return 0;
}
int GWSip::GetLoaclIp(void)
{
    int count = 0;
    sqlite *_db;
    char **data;
    int res;
    int row = 0;
    int col = 0;
    char *err = 0;
    do
    {
        count ++;
        if(count == 10)
        {
            LOG("Read nrx.db failed!\n");
            return -1;
        }  
        _db = sqlite_open("/opt/app/dat/nrx.db", 2, &err);
    }while(!_db);
    res = sqlite_get_table(_db,"select mip from inf",&data,&row,&col,&err);
    if( res != SQLITE_OK )
    {
        LOG("sqlite_get_table nrx.db failed!\n");
        sqlite_close(_db);
        return -1;
    }
    if(row != 0)
        sprintf(LocalIp, data[1]);
    sqlite_free_table(data);
    sqlite_close(_db);  
    return 0;   
}