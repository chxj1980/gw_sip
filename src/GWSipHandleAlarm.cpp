#include "GWSipHandleAlarm.h"
#include <iostream>
#include <sqlite.h>
#include "Log.h"
using namespace std;
GWSipHandleAlarm::GWSipHandleAlarm()
{
}
GWSipHandleAlarm::~GWSipHandleAlarm()
{
}

void GWSipHandleAlarm::FindAlarm()
{
    if(AlarmStatus != 0)
        return;
	int res = 0;
    if(0 == access("/opt/app/ram/80/01.jpg", 0))
    {
        res = SaveAlarmBegin(1, 2);
        if(SaveRecordFlag == 0)
            SaveRecordFlag = 1;
        if(res == 0)
        {
            unlink("/opt/app/ram/80/01.jpg" );
            unlink("/opt/app/ram/80/02.jpg" );
            unlink("/opt/app/ram/80/03.jpg" );
        } 
        AlarmStatus = 2; 
        return ;
    }
    //sdcard
    if(0 == access("/opt/app/ram/sd.txt", 0) && sd_status == 1)
    {
        printf("SDcard is not exist.\n");
        sd_status = 0;
        SaveAlarmBegin(sd_status, 3);
        AlarmStatus = 3; 
        return ;
    }
    else if(0 != access("/opt/app/ram/sd.txt", 0) && sd_status == 0)
    {
        sd_status = 1;
        SaveAlarmEnd(sd_status, 3);
        AlarmStatus = 3; 
        return ;
    }
    //videolost
    if(0 == access("/opt/app/ram/videolost.txt", 0) && video_status == 1)
    {
        video_status = 0;
        SaveAlarmBegin(video_status, 4);
        AlarmStatus = 4; 
        return ;
    }
    else if(0 != access("/opt/app/ram/videolost.txt", 0) && video_status == 0)
    {
        video_status = 1;
        SaveAlarmEnd(video_status, 4);
        AlarmStatus = 4; 
        return ;
    }
    //device online
    if(0 == access("/opt/app/ram/offline.txt", 0) && online_status == 1)
    {
        online_status = 0;
        SaveAlarmBegin(online_status, 4);
        AlarmStatus = 4; 
        return ;
    }
    else if(0 != access("/opt/app/ram/offline.txt", 0) && online_status == 0)
    {
        online_status = 1;
        SaveAlarmEnd(online_status, 4);
        AlarmStatus = 4; 
        return ;
    }

}
int GWSipHandleAlarm::CheckInsubscriptionExpires(int Did)
{
	eXosip_notify_t *jn = NULL;
    eXosip_dialog_t *jd =NULL;
    eXosip_notify_dialog_find(Did, &jn,& jd);
    if(jd== NULL || jn==NULL)
    {
        printf("jd is NULL or jn is NULL********\n");
        return -1;
    }    
    else
    {
        time_t timep;
        time(&timep);
        printf("insubscription %d expires is :%li\n",Did, jn->n_ss_expires-timep);
        if((jn->n_ss_expires-timep) > 0)
            return 0;
        else
            return -1;
    }
}
int GWSipHandleAlarm::ISHaveSubscriber()
{
    if(map_alarm_vl_did.size() == 0 && map_alarm_md_did.size() == 0 &&
       map_alarm_cd_did.size() == 0 && map_alarm_se_did.size() == 0 )
        return 0;
    else
        switch(AlarmStatus)
        {
            case 2:if(map_alarm_md_did.size() == 0) return 0;break;
            case 3:if(map_alarm_cd_did.size() == 0) return 0;break;
            case 4:if(map_alarm_se_did.size() == 0) return 0;break;
            default:;break;
        }
    return 1;
}
void GWSipHandleAlarm::CheckAlarm()
{
    if(AlarmStatus == 0)
        return;
	int count = 0;
    sqlite *_db;
    char **data;
    int res;
    int row   = 0;
    int col   = 0;
    char *err = 0;
    do
    {
        count ++;
        if(count == 10)
        {
            LOG("Read alarmevent.db failed!\n");
            return ;
        }  
        _db = sqlite_open("/opt/app/dat/alarmevent.db", 2, &err);
    }while(!_db);
    res = sqlite_get_table(_db,"select * from alarm_event where alarm_status = 1",&data,&row,&col,&err);
    if( res != SQLITE_OK )
    {
        LOG("sqlite_get_table alarmevent.db failed!\n");
        sqlite_close(_db);
        return ;
    }
    int i = 1;
    int alarm_begintime_pos  = 1;
    int alarm_endtime_pos    = 2;
    int alarm_status_pos     = 3;
    int alarm_type_pos       = 4;
    int alarm_alarm_id_pos   = 5;
    if(row == 0)
    {
        AlarmStatus = 0;
        printf("no alarm event!!!!!\n");
        sqlite_free_table(data);
        sqlite_close(_db);
        return;
    }
    cout<<"row is :"<<endl;
    int ISSub = ISHaveSubscriber();
    char _sql[100]={0};
    while(i < row + 1)
    {
        if( ISSub == 0)
        {
            cout<<"donot have Subscriber"<<endl;
            sprintf(_sql,"update alarm_event set alarm_status=0 where alarm_id=%s",data[i*col + alarm_alarm_id_pos]);
            res = sqlite_exec(_db, _sql, 0, 0, &err);
            if( res != SQLITE_OK )
            {
                LOG("update alarmevent.db failed!\n");
                sqlite_free_table(data);
                sqlite_close(_db);
                return ;
            }
            i++;
            continue;
        }
        map<int, int>::iterator _it;
        if(atoi(data[i*col + alarm_type_pos]) == 1)
        {
            _it = map_alarm_vl_did.begin();
            while(_it != map_alarm_vl_did.end())
            {
                if(1 == (_it->second))
                {
                    if(CheckInsubscriptionExpires(_it->first) != 0)
                    {
                        map_alarm_vl_did.erase(_it++);
                        continue;
                    }
                    else
                    {
                        SendSubscriberNotify(_it->first, data[i*col + alarm_begintime_pos], data[i*col + alarm_endtime_pos] 
                                                      ,data[i*col + alarm_status_pos], data[i*col + alarm_type_pos], 0);
                        isSendSubNotifyOK = 1;
                        sprintf(update_alarm_id, data[i*col + alarm_alarm_id_pos]);
                        SendSubInfo.AlarmType = 1;SendSubInfo.Did = _it->first;
                        break;
                    }      
                }
                ++_it;
            }
            if(isSendSubNotifyOK == 1)
                break;
        }
        if(atoi(data[i*col + alarm_type_pos]) == 2)
        {
            _it = map_alarm_md_did.begin();
            while(_it != map_alarm_md_did.end())
            {
                if(1 == (_it->second))
                {
                    if(CheckInsubscriptionExpires(_it->first) != 0)
                    {
                        map_alarm_md_did.erase(_it++);
                        continue;
                    }
                    else
                    {
                        SendSubscriberNotify(_it->first, data[i*col + alarm_begintime_pos], data[i*col + alarm_endtime_pos]
                                                      ,data[i*col + alarm_status_pos], data[i*col + alarm_type_pos], 0);
                        isSendSubNotifyOK = 1;
                        sprintf(update_alarm_id, data[i*col + alarm_alarm_id_pos]);
                        SendSubInfo.AlarmType = 1;SendSubInfo.Did = _it->first;
                        break;
                    }     
                }
                ++_it;
            }
            if(isSendSubNotifyOK == 1)
                break;
        }
        if(atoi(data[i*col + alarm_type_pos]) == 3)
        {
            _it = map_alarm_cd_did.begin();
            while(_it != map_alarm_cd_did.end())
            {
                if(1 == (_it->second))
                {
                    if(CheckInsubscriptionExpires(_it->first) != 0)
                    {
                        map_alarm_cd_did.erase(_it++);
                        continue;
                    }
                    else
                    {
                        SendSubscriberNotify(_it->first, data[i*col + alarm_begintime_pos], data[i*col + alarm_endtime_pos] 
                                                      ,data[i*col + alarm_status_pos], data[i*col + alarm_type_pos], 0);
                        isSendSubNotifyOK = 1;
                        sprintf(update_alarm_id, data[i*col + alarm_alarm_id_pos]);
                        SendSubInfo.AlarmType = 1;SendSubInfo.Did = _it->first;
                        break;
                    }      
                }
                ++_it;
            }
            if(isSendSubNotifyOK == 1)
                break;
        }
        if(atoi(data[i*col + alarm_type_pos]) == 4)
        {
            _it = map_alarm_se_did.begin();
            while(_it != map_alarm_se_did.end())
            {
                if(1 == (_it->second))
                {
                    if(CheckInsubscriptionExpires(_it->first) != 0)
                    {
                        map_alarm_se_did.erase(_it++);
                        continue;
                    }
                    else
                    {
                        SendSubscriberNotify(_it->first, data[i*col + alarm_begintime_pos], data[i*col + alarm_endtime_pos] 
                                                      ,data[i*col + alarm_status_pos], data[i*col + alarm_type_pos], 1);
                        isSendSubNotifyOK = 1;
                        sprintf(update_alarm_id, data[i*col + alarm_alarm_id_pos]);
                        SendSubInfo.AlarmType = 1;SendSubInfo.Did = _it->first;
                        break;
                    }       
                }
                ++_it;
            }
            if(isSendSubNotifyOK == 1)
                break;
        }
        i++;
    }
    sqlite_free_table(data);
    sqlite_close(_db);

}
int GWSipHandleAlarm::SaveAlarmEnd(int status, int type)
{
    printf("alarm_event cancle.\n");
    int count = 0;
    sqlite *_db;
    char _sql[200] = {0};
    int res;
    int alarm_id = 0;
    char *err = 0;
    struct tm *p;
    do
    {
        count ++;
        if(count == 10)
        {
            LOG("SaveAlarmEnd/Read alarmevent.db failed!\n");
            return -1;
        }  
        _db = sqlite_open("/opt/app/dat/alarmevent.db", 2, &err);
    }while(!_db);
    time_t timep;
    time(&timep);
    p = gmtime(&timep);
    sprintf(_sql, "update alarm_event set endtime='%d%02d%02d%02d%02d%02d',status=%d,alarm_status=1 where type='%d' and status=0"
                     ,1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, 
                     status, type);
    res = sqlite_exec(_db, _sql, 0, 0, &err);
    if( res != SQLITE_OK )
    {
        LOG("SaveAlarmEnd/update alarm_event failed!\n");
        sqlite_close(_db);
        return -1;
    }
    sqlite_close(_db);
    return 0;
}
void GWSipHandleAlarm::SendSubscriberNotify(int did, char *_starttime, char*_endtime, char *satus, char *type, int eventType)
{
    printf("did is %d time is %s\n", did, _starttime);
    char starttime[20]={0};
    char endtime[20]={0};
    sprintf(starttime, _starttime);
    sprintf(endtime, _endtime);
    string EventType;
    if(eventType == 0)
        EventType = "alarm";
    else if(eventType == 1)
        EventType = "presence";

    osip_message_t* request   = NULL; 
    char xml_body[1024];  
    memset(xml_body, 0, 1024);

    eXosip_lock();
    int i = eXosip_insubscription_build_notify(did, 2, NULL, &request);
    printf("osip_from_t is :%s %s %s\n", request->to->url->username, request->to->url->host, request->to->url->port);
    if (i != 0)
    {
        eXosip_unlock ();
        return;
    }
    else  
    {  
        printf("SendSubscriberNotify\r\n");
        if(eventType == 0)  //0 Notify_Alarm  1 Notify_Status
        {
            char send_starttime[30]={0};
            char send_endtime[30]={0};
            sprintf(send_starttime, "%c%c%c%c-%c%c-%c%cT%c%c:%c%c:%c%cZ",starttime[0],starttime[1],starttime[2],starttime[3],
                    starttime[4],starttime[5],starttime[6],starttime[7],starttime[8],starttime[9]
                    ,starttime[10],starttime[11],starttime[12],starttime[13]);
            sprintf(send_endtime, "%c%c%c%c-%c%c-%c%cT%c%c:%c%c:%c%cZ",endtime[0],endtime[1],endtime[2],endtime[3],
                    endtime[4],endtime[5],endtime[6],endtime[7],endtime[8],endtime[9]
                    ,endtime[10],endtime[11],endtime[12],endtime[13]);
            snprintf(xml_body, 1024, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
                             "<!-- %s  -->"
                             "<SIP_XML EventType=\"Notify_Alarm\">\r\n"
                             "<Item Code=\"%s\" Name=\"IPC1\" Type=\"%s\" StartTime=\"%s\" StopTime=\"%s\"/>\r\n"
                             "</SIP_XML>\r\n"
                             ,DeviceId, IpcId, type, send_starttime, send_endtime);
        }
        else if(eventType == 1) 
        {
            printf("send  Notify_Status!\r\n");
            snprintf(xml_body, 1024, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
                             "<!-- %s  -->"
                             "<SIP_XML EventType=\"Notify_Status\">\r\n"
                             "<Item Code=\"%s\" Name=\"IPC1\" Status=\"%s\"/>\r\n"
                             "</SIP_XML>\r\n"
                             ,DeviceId, IpcId, satus);
        }
        osip_message_set_header(request, "Event", EventType.c_str());
        osip_message_set_body(request, xml_body, strlen(xml_body)); 
        osip_message_set_content_type(request, "application/xml");    
        i = eXosip_insubscription_send_request (did, request); 
        if (i != 0)  
        {  
            eXosip_unlock ();
            return ;  
        }   
    } 
    eXosip_unlock ();
}
int GWSipHandleAlarm::SaveAlarmBegin(int status, int type)
{
    printf("alarm_event is exist.\n");
    int count = 0;
    sqlite *_db;
    char _sql[200] = {0};
    char _sql_stat[200] = {0};
    char **data;
    int res;
    int row, col;
    int alarm_id = 0;
    char *err = 0;
    struct tm *p;
    do
    {
        count ++;
        if(count == 10)
        {
            LOG("SaveAlarmBegin/Read alarmevent.db failed!\n");
            return -1;
        }  
        _db = sqlite_open("/opt/app/dat/alarmevent.db", 2, &err);
    }while(!_db);
    res = sqlite_get_table(_db,"select * from count_status",&data,&row,&col,&err);
    if( res != SQLITE_OK )
    {
        LOG("SaveAlarmBegin/sqlite_get_table alarmevent.db failed!\n");
        sqlite_close(_db);
        return -1;
    }
    int num = atoi(data[2]);
    int isfull= atoi(data[3]);
    printf("num and isfull %d %d\n",num, isfull);
    time_t timep;
    time(&timep);
    p = gmtime(&timep);
    if(num < 200 && isfull == 1)
    {
        num++;
        alarm_id = num;
        sprintf(_sql, "update alarm_event set begintime='%d%02d%02d%02d%02d%02d',endtime='',status=%d,type='%d',alarm_status=1 where alarm_id=%d"
                     ,1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, 
                     status, type, alarm_id);
        sprintf(_sql_stat, "update count_status set count=%d,isfull=1",alarm_id);   
    }
    else if(num < 199 && isfull == 0)
    {
        num++;
        alarm_id = num;
        sprintf(_sql, "insert into alarm_event (begintime,status,type,alarm_id,alarm_status) "
                     "values ('%d%02d%02d%02d%02d%02d', %d, '%d', %d, 1)"
                     ,1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, 
                     status, type, alarm_id);
        sprintf(_sql_stat, "update count_status set count=%d,isfull=0",alarm_id);
    }
    else if(num == 199 && isfull == 0)
    {
        num ++;
        alarm_id = num;
        sprintf(_sql, "insert into alarm_event (begintime,status,type,alarm_id,alarm_status) "
                     "values ('%d%02d%02d%02d%02d%02d', %d, '%d', %d, 1)"
                     ,1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, 
                     status, type, alarm_id);
        sprintf(_sql_stat, "update count_status set count=%d,isfull=1",alarm_id);
    }
    else if(num == 200)
    {
        num = 0;
        alarm_id = num;
        sprintf(_sql, "update alarm_event set begintime='%d%02d%02d%02d%02d%02d',status=%d,type='%d',alarm_status=1 where alarm_id=%d"
                     ,1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, 
                     status, type, alarm_id);
        sprintf(_sql_stat, "update count_status set count=%d,isfull=1",alarm_id);
    }
    res = sqlite_exec(_db, _sql, 0, 0, &err);
    if( res != SQLITE_OK )
    {
        LOG("SaveAlarmBegin/sqlite_exec update alarm_event failed!\n");
        sqlite_free_table(data);
        sqlite_close(_db);
        return -1;
    }
    res = sqlite_exec(_db, _sql_stat, 0, 0, &err);
    if( res != SQLITE_OK )
    {
        LOG("SaveAlarmBegin/sqlite_exec update count_status failed!\n");
        sqlite_free_table(data);
        sqlite_close(_db);
        return -1;
    }
    sqlite_free_table(data);
    sqlite_close(_db);
    return 0;
}