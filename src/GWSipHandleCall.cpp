#include "GWSipHandleCall.h"
#include <iostream>
using namespace std;
GWSipHandleCall::GWSipHandleCall()
{
}
GWSipHandleCall::~GWSipHandleCall()
{
}
void GWSipHandleCall::HandleCallInvite(eXosip_event_t *gw_event)
{
	osip_message_t *gw_answer = NULL;
    sdp_message_t  *sdp_msg = NULL;
    GWSipHandleSDP MyGWSipHandleSDP;
    char  sdp_body[4096];
    memset(sdp_body, 0, 4096);
    sdp_msg = eXosip_get_remote_sdp(gw_event->did);
    if(video_status == 0 || online_status == 0)
        SendCallAnswer(gw_event->tid, 500, gw_answer);
    else if (sdp_msg == NULL)
        SendCallAnswer(gw_event->tid, 400, gw_answer);
    else if (CallUserNum > 5)
        SendCallAnswer(gw_event->tid, 503, gw_answer);
    else
    {
        int i = MyGWSipHandleSDP.HandleSDP(gw_event, sdp_body, sdp_msg);
        if(i == 200)
        {
            eXosip_lock();
            i = eXosip_call_build_answer(gw_event->tid, 200, &gw_answer);
            if (i == 0) 
            {
                osip_message_set_body(gw_answer,sdp_body,strlen(sdp_body));  
                osip_message_set_content_type(gw_answer, "application/SDP");   
                i = eXosip_call_send_answer(gw_event->tid, 200, gw_answer);
                if (i != 0)  
                {
                    eXosip_unlock ();
                    return ;  
                }
            }
            else
            {
                eXosip_unlock ();
                return;
            }
            eXosip_unlock();
        }
        else if(i == 400)
            SendCallAnswer(gw_event->tid, 400, gw_answer);
        else if(i == 488)
            SendCallAnswer(gw_event->tid, 488, gw_answer);
    }
}
void GWSipHandleCall::SendCallAnswer(int tid, int status_code, osip_message_t *gw_answer)
{
    eXosip_lock();
    int i = eXosip_call_build_answer(tid, status_code, &gw_answer);
    if (i != 0) 
    {
        eXosip_unlock ();
        return;
    }
    if(status_code == 488)
        osip_message_set_header(gw_answer, "Warning", "not support payload type");
    osip_message_set_header(gw_answer, "Contact", Contact); 
    i = eXosip_call_send_answer(tid, status_code, gw_answer);/*按照规则回复200OK*/ 
    if (i != 0)  
    { 
        eXosip_unlock ();
        return ;  
    }
    eXosip_unlock ();
    return;
}