#ifndef _GWSIPHANDLESDP_H
#define _GWSIPHANDLESDP_H
#include "GWSip.h"
class GWSipHandleSDP
{
public:
	GWSipHandleSDP();
	~GWSipHandleSDP();
	int HandleSDP(eXosip_event_t *gw_event, char *sdp_body, sdp_message_t *sdp_msg);
};

#endif