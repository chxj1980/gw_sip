#ifndef _GWSIPHANDLHISTORYALARM_H
#define _GWSIPHANDLHISTORYALARM_H
#include "GWSip.h"
#include <libxml/parser.h>
#include <vector>
#include <iostream>
using namespace std;
class GWSipHandleHistoryAlarm
{
public:
    GWSipHandleHistoryAlarm();
    ~GWSipHandleHistoryAlarm();
    int GetHistoryAlarmXML(char xml_body[], char *ToIndex, char *beginTime, char *endTime, int type);
    
};

#endif