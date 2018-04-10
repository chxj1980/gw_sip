#include "GWSipHandleSDP.h"
#include <iostream>
using namespace std;
GWSipHandleSDP::GWSipHandleSDP()
{
}
GWSipHandleSDP::~GWSipHandleSDP()
{
}
int GWSipHandleSDP::HandleSDP(eXosip_event_t *gw_event, char *sdp_body, sdp_message_t *sdp_msg)
{
	char  *RTP_sever_ip     = NULL;
    char  *RTP_sever_port   = NULL;
    char  *media_type       = NULL;
    char  *m_payload        = NULL;
	RTP_sever_ip    = sdp_message_c_addr_get(sdp_msg, -1, 0);//c在总的里面位置就是-1
    RTP_sever_port  = sdp_message_m_port_get(sdp_msg, 0);/*媒体服务器IP端口*/
    media_type      = sdp_message_m_media_get(sdp_msg, 0);
    m_payload       = sdp_message_m_payload_get(sdp_msg, 0, 0);
    if((RTP_sever_ip == NULL) || (RTP_sever_port == NULL) || (media_type == NULL))
        return 400;
    if(strstr(media_type, "video"))
    {
        if(atoi(m_payload) != 100)
        {
            return 488;
        }
        for (int i = 0; i < 5; ++i)
        {
            if(VCallUser[i].stat == 0)
            {
                VCallUser[i].CallId  = gw_event->cid;
                VCallUser[i].Did     = gw_event->did;
                snprintf(VCallUser[i].RTPIp, 20, "%s", RTP_sever_ip);
                snprintf(VCallUser[i].RTPPort, 6, "%s", RTP_sever_port);
                break;
            }
        }
        

        snprintf(sdp_body, 4096, "v=0\r\n"/*协议版本*/
                             "o=- 0 0 IN IP4 %s\r\n"/*会话源*//*用户名/会话ID/版本/网络类型/地址类型/地址*/
                             "s=-\r\n"/*会话名*/
                             "c=IN IP4 %s\r\n"/*连接信息*//*网络类型/地址信息/多点会议的地址*/
                             "m=video %s RTP/AVP 100\r\n"/*媒体/端口/传送层协议/格式列表*/
                             "a=rtpmap:100 H264/90000\r\n"/*收发模式*/
                             "a=fmtp:100 4CIF=1\r\n"/*净荷类型/编码名/时钟速率*/
                             "a=fmtp:100 profile-level-id=420028;sprop-parameter-sets=Z0IAKOkBQHsg\r\n"
                             "a=sendrecv\r\n"
                             ,LocalIp ,LocalIp , "4444");
    }
    else
        return 400;
    return 200;
}