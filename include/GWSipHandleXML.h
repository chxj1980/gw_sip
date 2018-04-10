#ifndef _GWSIPHANDLEXML_H
#define _GWSIPHANDLEXML_H
#include "GWSip.h"
#include <libxml/parser.h>
class GWSipHandleXML
{
public:
	GWSipHandleXML();
	~GWSipHandleXML();
	int HandleXML(char *p_xml_body, int size, char xml_body[], int did);
	int HandleXMLResource(char xml_body[], xmlNodePtr curNode);
	int HandleXMLHistoryVideo(char xml_body[], xmlNodePtr curNode);
	int HandleXMLHistoryAlarm(char xml_body[], xmlNodePtr curNode);
	int HandleXMLControlCamera(xmlNodePtr curNode);
	int HandleSubscribeStatus(xmlNodePtr curNode, int did);
	int HandleSubscribeAlarm(xmlNodePtr curNode, int did);
};

#endif