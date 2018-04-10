#include "GWSipHandleMessage.h"
#include <iostream>
using namespace std;
GWSipHandleMessage::GWSipHandleMessage()
{
}
GWSipHandleMessage::~GWSipHandleMessage()
{
}
void GWSipHandleMessage::HandleMessage(eXosip_event_t *gw_event)
{
    GWSipHandleXML MyGWSipHandleXML;
	int i = 0;
    char xml_body[30720];
    
    osip_body_t *p_rqt_body  = NULL;
    osip_message_t *gw_answer = NULL;
    char *p_xml_body  = NULL;
    memset(xml_body, 0, 30720);
    osip_content_length_t *length;
    int size;
    osip_message_get_body(gw_event->request, 0, &p_rqt_body);
    if (NULL == p_rqt_body)
    	SendAnswer(gw_event->tid, 400, gw_answer);
    else 
    {
        p_xml_body = p_rqt_body->body;
        length = osip_message_get_content_length(gw_event->request);
        size   = atoi(length->value);
        i = MyGWSipHandleXML.HandleXML(p_xml_body, size, xml_body, gw_event->did);
        if(i == 200)
        {
            eXosip_lock();
            i = eXosip_message_build_answer(gw_event->tid, 200, &gw_answer);
            if (i != 0) 
            {
                eXosip_unlock ();
                return ;  
            }
            osip_message_set_body(gw_answer,xml_body,strlen(xml_body)); 
            osip_message_set_content_type(gw_answer, "application/xml");   
            i = eXosip_message_send_answer(gw_event->tid, 200, gw_answer);
            if (i != 0)  
            {
                eXosip_unlock ();
                return ;  
            }
            eXosip_unlock ();
        }
        else if(i == 2000)
            SendAnswer(gw_event->tid, 200, gw_answer);
        else if(i == 404)
            SendAnswer(gw_event->tid, 404, gw_answer);
        else if(i == 400)
            SendAnswer(gw_event->tid, 400, gw_answer);
        else 
        	SendAnswer(gw_event->tid, 500, gw_answer);
    } 
}
void GWSipHandleMessage::SendAnswer(int tid, int status_code, osip_message_t *gw_answer)
{
    eXosip_lock();
    int i = eXosip_message_build_answer(tid, status_code, &gw_answer);
    if(i != 0) 
    {
        eXosip_unlock ();
        return;
    }  
    i = eXosip_message_send_answer(tid, status_code, gw_answer);
    if (i != 0)  
    { 
        eXosip_unlock ();
        return;  
    }
    eXosip_unlock ();
}