#include "GWSipHandleSubscriber.h"
#include <iostream>
#include <sqlite.h>
#include "Log.h"
using namespace std;
GWSipHandleSubscriber::GWSipHandleSubscriber()
{
}
GWSipHandleSubscriber::~GWSipHandleSubscriber()
{
}
void GWSipHandleSubscriber::HandleSubscriberNew(eXosip_event_t* gw_event)
{
    cout<<"gw_event did is: "<<gw_event->did<<endl;
	GWSipHandleXML MyGWSipHandleXML;
	int i = 0;
    osip_body_t *msg_body     = NULL;
    char *p_xml_body          = NULL;
    char xml_body[128];     
    osip_header_t*  Header    = NULL;
    osip_message_t* gw_answer = NULL;
    osip_message_t* request   = NULL;
    char *rev_EventType       = NULL;
    string EventType;
    osip_content_length_t *length;
    int size;
    memset(xml_body, 0, 128);
    osip_message_header_get_byname(gw_event->request,"Event", 0, &Header);
    osip_from_t *req_from = NULL;
    req_from = osip_message_get_from(gw_event->request);
    if(NULL != Header)
        rev_EventType = Header->hvalue;
    if(strstr(rev_EventType, "alarm"))
        EventType = "alarm";    
    else if(strstr(rev_EventType, "presence"))
        EventType = "presence";
    else 
    {
        SendInsubscriptionAnswer(gw_event->tid, 400, gw_answer);
        return;
    }   
    osip_message_get_body(gw_event->request, 0, &msg_body);
    if(NULL == msg_body)
    {
        SendInsubscriptionAnswer(gw_event->tid, 200, gw_answer);   
        eXosip_lock();
        i = eXosip_insubscription_build_notify(gw_event->did, 2, NULL, &request);
        if(i != 0)
        {
            eXosip_unlock();
            return;
        }
        osip_message_set_contact(request, Contact);
        osip_message_set_header(request, "Event", EventType.c_str());
        i = eXosip_insubscription_send_request (gw_event->did, request);
        if(i != 0)
        {
            eXosip_unlock();
            return;
        }
        eXosip_unlock();
    }
    else
    { 
        p_xml_body = msg_body->body;
        length = osip_message_get_content_length(gw_event->request);
        size   = atoi(length->value);
        i = MyGWSipHandleXML.HandleXML(p_xml_body, size, xml_body, gw_event->did);  
        if(i == 200)
        {
            SendInsubscriptionAnswer(gw_event->tid, 200, gw_answer);
            
            eXosip_lock();
            i = eXosip_insubscription_build_notify(gw_event->did, 2, NULL, &request);
            if(i != 0)
            {
                eXosip_unlock();
                return;
            }
            osip_message_set_contact(request, Contact);
            osip_message_set_header(request, "Event", EventType.c_str());
            i = eXosip_insubscription_send_request (gw_event->did, request);
            if(i != 0)
            {
                eXosip_unlock();
                return;
            }
            eXosip_unlock(); 
        } 
        else if(i == 400)
            SendInsubscriptionAnswer(gw_event->tid, 400, gw_answer);
        else if(i == 404)
            SendInsubscriptionAnswer(gw_event->tid, 404, gw_answer);
        else
            SendInsubscriptionAnswer(gw_event->tid, 500, gw_answer);       
    } 
}
void GWSipHandleSubscriber::HandleNotifyAnswer(eXosip_event_t* gw_event)
{
	if ((NULL != gw_event->response)&&(200 == gw_event->response->status_code))
    {
        if(isSendSubNotifyOK == 1)
        {
            isSendSubNotifyOK = 0;
            if(ISSendOver() == 0)
            {
                UpdateSendOkAlarm();
                UpdateMapToOK();
            }         
        }      
    }
    else if ((NULL != gw_event->response)&&(400 == gw_event->response->status_code))
        printf("notify 400 error\n");
    else if ((NULL != gw_event->response)&&(403 == gw_event->response->status_code))
        printf("notify 403 error\n");
    else if ((NULL != gw_event->response)&&(481 == gw_event->response->status_code))
        printf("notify 481 error\n");
    else if ((NULL != gw_event->response)&&(500 == gw_event->response->status_code))
        printf("notify 500 error\n");
    else if ((NULL != gw_event->response)&&(503 == gw_event->response->status_code))
    {
        printf("notify 503 error\n");
        if(isSendSubNotifyOK == 1)
            SendSubNoWTime = 30;
    }
}
int GWSipHandleSubscriber::UpdateMapToOK()
{
    map<int, int>::iterator _it;
    _it = map_alarm_vl_did.begin();
    while(_it != map_alarm_vl_did.end())
    {
        _it->second = 1;
        ++_it;
    }
    _it = map_alarm_md_did.begin();
    while(_it != map_alarm_md_did.end())
    {
        _it->second = 1;
        ++_it;
    }
    _it = map_alarm_cd_did.begin();
    while(_it != map_alarm_cd_did.end())
    {
        _it->second = 1;
        ++_it;
    }
    _it = map_alarm_se_did.begin();
    while(_it != map_alarm_se_did.end())
    {
        _it->second = 1;
        ++_it;
    }
    return 0;
}
int GWSipHandleSubscriber::ISSendOver()
{
    int result = 0;
    switch(SendSubInfo.AlarmType)
    {
        case 1: result = CheckVLDid();break;
        case 2: result = CheckMDDid();break;
        case 3: result = CheckCDDid();break;
        case 4: result = CheckSEDid();break;
        default:;break;
    }
    return result;
}
int GWSipHandleSubscriber::CheckVLDid()
{
    map<int, int>::iterator _it;
    _it = map_alarm_vl_did.begin();
    int flag = 0;
    while(_it != map_alarm_vl_did.end())
    {
        if(_it->second == 1)
        {
            if((_it->first) == SendSubInfo.Did)
                _it->second = 0;
            else
            {
                flag = 1;
                break;
            }
        }
        ++_it;
    }
    return flag;
}
int GWSipHandleSubscriber::CheckMDDid()
{
    map<int, int>::iterator _it;
    _it = map_alarm_md_did.begin();
    int flag = 0;
    while(_it != map_alarm_md_did.end())
    {
        if(_it->second == 1)
        {
            if((_it->first) == SendSubInfo.Did)
                _it->second = 0;
            else
            {
                flag = 1;
                break;
            }
        }
        ++_it;
    }
    return flag;
}
int GWSipHandleSubscriber::CheckCDDid()
{
    map<int, int>::iterator _it;
    _it = map_alarm_cd_did.begin();
    int flag = 0;
    while(_it != map_alarm_cd_did.end())
    {
        if(_it->second == 1)
        {
            if((_it->first) == SendSubInfo.Did)
                _it->second = 0;
            else
            {
                flag = 1;
                break;
            }
        }
        ++_it;
    }
    return flag;
}
int GWSipHandleSubscriber::CheckSEDid()
{
    map<int, int>::iterator _it;
    _it = map_alarm_se_did.begin();
    int flag = 0;
    while(_it != map_alarm_se_did.end())
    {
        if(_it->second == 1)
        {
            if((_it->first) == SendSubInfo.Did)
                _it->second = 0;
            else
            {
                flag = 1;
                break;
            }
        }
        ++_it;
    }
    return flag;
}
int GWSipHandleSubscriber::UpdateSendOkAlarm(void)
{
    int count = 0;
    sqlite *_db;
    int res;
    char *err = 0;
    char _sql[100]={0};
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
    if(update_alarm_id == NULL)
    {
        sqlite_close(_db);
        return -1;
    }
    sprintf(_sql,"update alarm_event set alarm_status=0 where alarm_id=%s",update_alarm_id);
    res = sqlite_exec(_db, _sql, 0, 0, &err);
    if( res != SQLITE_OK )
    {
        LOG("update alarmevent.db failed!\n");
        sqlite_close(_db);
        return -1;
    }
    sqlite_close(_db);
    return 0;
}
void GWSipHandleSubscriber::SendInsubscriptionAnswer(int tid, int status_code, osip_message_t* gw_answer)
{
    eXosip_lock();
    int i = eXosip_insubscription_build_answer(tid, status_code, &gw_answer);
    if(i != 0)
    {
        eXosip_unlock();
        return;
    }
    i = eXosip_insubscription_send_answer(tid, status_code, gw_answer);
    if(i != 0)
    {
        eXosip_unlock();
        return;
    }
    eXosip_unlock();
}