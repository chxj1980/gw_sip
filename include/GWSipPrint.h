#ifndef _GWSIPPRINT_H
#define _GWSIPPRINT_H
#include "eXosip2/eXosip.h"
#include "eXosip2.h"
class GWSipPrint
{
public:
	GWSipPrint();
	~GWSipPrint();
	int PrintSip(eXosip_event_t *gw_event);
};

#endif
