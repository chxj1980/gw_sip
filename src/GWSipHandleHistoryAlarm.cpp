#include "GWSipHandleHistoryAlarm.h"
#include <string.h> 
#include <sys/stat.h> 
#include <time.h>
#include <sqlite.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

GWSipHandleHistoryAlarm::GWSipHandleHistoryAlarm()
{
}
GWSipHandleHistoryAlarm::~GWSipHandleHistoryAlarm()
{
}
int GWSipHandleHistoryAlarm::GetHistoryAlarmXML(char xml_body[], char *ToIndex, char *beginTime, char *endTime, int type)
{
    GWSipTime MyGWSipTime;
    int toindex = atoi(ToIndex);
    long LbeginTime = MyGWSipTime.SipTimeToTimestamp(beginTime);
    long LendTime   = MyGWSipTime.SipTimeToTimestamp(endTime);
    int type_vl = 0;int type_md = 0;int type_cd = 0;int type_se = 0;
    //0x01 video lost  0x02 motiondetec  0x800 SD card error 0x1000 status event
    if((type&0x01) == 0x01 || (type&0x02) == 0x02 || (type&0x800) == 0x800 || (type&0x1000) == 0x1000)
    {
        if((type&0x01) == 0x01)
            type_vl = 1;
        if((type&0x02) == 0x02)
            type_md = 2;
        if((type&0x800) == 0x800)
            type_cd = 3;
        if((type&0x1000) == 0x1000)
            type_se = 4;
    }
    else return 404;
    sqlite *_db;
    char **data_alarm;
    int res;
    int row=0;
    int col=0;
    int count=0;
    char *err = 0;
    do
    {
        count ++;
        if(count == 10)
        {
            LOG("Read alarmevent.db failed!\n");
            return -1;
        }  
        _db = sqlite_open("/opt/app/dat/alarmevent.db", 2, &err);
    }while(!_db);
    res = sqlite_get_table(_db,"select * from alarm_event",&data_alarm,&row,&col,&err);
    if( res != SQLITE_OK )
    {
        sqlite_close(_db);
        return -1;
    }
    int alarm_begintime_pos = 1;
    int alarm_endtime_pos = 2;
    int alarm_status_pos = 3;
    int alarm_type_pos = 4;
    int item_count = 0;          //保存请求的报警文件的个数
    int alarm_id[201] ={0};//保存请求的报警文件在数据库中的ID
    int i = 1;
    int send_type = 0;
    long l_curtime = 0;
    char xml_middle[300] = {0};
    char send_BeginTime[30] = {0};
    char send_EndTime[30]   = {0};
    char xml_head[200] = {0};
    
    string s_xml_middle;
    string s_temp;
    while(i < row + 1)
    {
        l_curtime = MyGWSipTime.NormalTimeToTimestamp(data_alarm[i*col + alarm_begintime_pos]);
        if(l_curtime > LbeginTime && l_curtime < LendTime)
        {
            if(atoi(data_alarm[i*col + alarm_type_pos])==type_vl || atoi(data_alarm[i*col + alarm_type_pos])==type_md
            || atoi(data_alarm[i*col + alarm_type_pos])==type_cd || atoi(data_alarm[i*col + alarm_type_pos])==type_se)
            {
                item_count++;
                if(atoi(data_alarm[i*col+alarm_type_pos]) == 1) send_type = 0x01;
                else if(atoi(data_alarm[i*col+alarm_type_pos]) == 2) send_type = 0x02;
                else if(atoi(data_alarm[i*col+alarm_type_pos]) == 3) send_type = 0x800;
                else if(atoi(data_alarm[i*col+alarm_type_pos]) == 4) send_type = 0x1000;

                MyGWSipTime.NormalTimeToSipTime(data_alarm[i*col + alarm_begintime_pos], send_BeginTime);
                MyGWSipTime.NormalTimeToSipTime(data_alarm[i*col + alarm_endtime_pos], send_EndTime);
                memset(xml_middle, 0, 300);
                snprintf(xml_middle, 300, "<Item Code=\"%s\" BeginTime=\"%s\" EndTime=\"%s\" Status=\"%s\" Type=\"%d\"/>\r\n",
                                    IpcId,  send_BeginTime, send_EndTime, 
                                    data_alarm[i*col+alarm_status_pos], 
                                    send_type);
                s_temp = xml_middle;
                s_xml_middle += s_temp;

            } 
        }
        i++;
    }
    if(item_count == 0)
    {
        snprintf(xml_body, 1024, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
                                 "<SIP_XML EventType=\"Response_History_Alarm\">\r\n"
                                 "<SubList RealNum=\"1\" SubNum=\"1\" FromIndex=\"1\" ToIndex=\"1\">\r\n"
                                 "<Item Code=\"%s\" BeginTime=\"NULL\" EndTime=\"NULL\" Status=\"NULL\" Type=\"NULL\"/>\r\n"
                                 "</SubList>\r\n"
                                 "</SIP_XML>\r\n"
                                 , IpcId);
        sqlite_free_table(data_alarm);
        sqlite_close(_db);
        return  200;
    }
    if(toindex > item_count) toindex = item_count;
    snprintf(xml_head, 200, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
                                "<SIP_XML EventType=\"Response_History_Alarm\">\r\n"
                                "<SubList RealNum=\"%d\" SubNum=\"%d\" FromIndex=\"1\" ToIndex=\"%d\">\r\n"
                                ,item_count , toindex, toindex);
    string s_xml_head = xml_head;
    string s_xml_end  = "</SubList>\r\n</SIP_XML>\r\n";
    s_xml_head = s_xml_head + s_xml_middle + s_xml_end;
    snprintf(xml_body, 30720, "%s", s_xml_head.data());
    sqlite_free_table(data_alarm);
    sqlite_close(_db);
    
    return 0;
}