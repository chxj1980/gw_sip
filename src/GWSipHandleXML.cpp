#include "GWSipHandleXML.h"
#include <iostream>
#include <string.h> 
#include <sys/stat.h> 
#include "Log.h"
#include <time.h>
#include <sqlite.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
using namespace std;
GWSipHandleXML::GWSipHandleXML()
{
}
GWSipHandleXML::~GWSipHandleXML()
{
}
int GWSipHandleXML::HandleXML(char *p_xml_body, int size, char xml_body[], int did)
{
    FILE *fp = NULL;
    xmlDocPtr  doc;           
    xmlNodePtr curNode;      
    char *EventType;
    int ret = 0;
    if(fp == NULL)
        fp = fopen("/opt/app/ram/handle_XML.xml","w+");
    if(fp != NULL)
    {
        fwrite(p_xml_body, size, 1, fp);
        fclose(fp);
        doc = xmlReadFile("/opt/app/ram/handle_XML.xml","UTF-8",XML_PARSE_RECOVER); 
        if (NULL == doc)
        { 
            fprintf(stderr,"Document not parsed successfully\n");
            return -1;
        }
        curNode = xmlDocGetRootElement(doc); 
        if (NULL == curNode)
        {
            fprintf(stderr,"empty document\n");
            xmlFreeDoc(doc);
            return -1;
        }
        if(xmlStrcmp(curNode->name, BAD_CAST "SIP_XML"))
        {
            fprintf(stderr,"document of the wrong type, root node != SIP_XML");
            xmlFreeDoc(doc);
            return -1;
        }
        if (xmlHasProp(curNode,BAD_CAST "EventType"))
        {
            EventType = (char *)xmlGetProp(curNode,BAD_CAST "EventType");
            if(strstr(EventType, "Request_Resource"))
            {
                ret = HandleXMLResource(xml_body, curNode);
                xmlFreeDoc(doc);
                return ret;
            }
            else if(strstr(EventType, "Request_History_Alarm"))
            {
                ret = HandleXMLHistoryAlarm(xml_body, curNode);
                xmlFreeDoc(doc);
                return ret;
            }
            else if(strstr(EventType, "Request_History_Video"))
            {
                if(sd_status == 0)
                {
                    xmlFreeDoc(doc);
                    return 500;
                }   
                ret = HandleXMLHistoryVideo(xml_body, curNode);
                xmlFreeDoc(doc);
                return  ret;
            }
            else if(strstr(EventType, "Control_Camera"))
            {
                ret = HandleXMLControlCamera(curNode);
                xmlFreeDoc(doc);
                return ret;
            }
            else if(strstr(EventType, "Subscribe_Alarm"))
            {
                ret = HandleSubscribeAlarm(curNode, did);
                xmlFreeDoc(doc);
                return ret;
            }
            else if(strstr(EventType, "Subscribe_Status"))
            {
                ret = HandleSubscribeStatus(curNode, did);
                xmlFreeDoc(doc);
                return ret;
            }
        }
        xmlFreeDoc(doc);
    }
    return 0;
}
int GWSipHandleXML::HandleSubscribeStatus(xmlNodePtr curNode, int did)
{
    char *code = NULL;

    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;
    while(curNode != NULL)
    {
        if (xmlHasProp(curNode,BAD_CAST "Code"))
            propNodePtr = curNode;
        curNode = curNode->next;
    }
    xmlAttrPtr attrPtr = propNodePtr->properties;
    while (attrPtr != NULL)
    {
        if (!xmlStrcmp(attrPtr->name, BAD_CAST "Code"))
            code = (char *)xmlGetProp(propNodePtr,BAD_CAST "Code");
        attrPtr = attrPtr->next;
    }
    if(code == NULL)
        return 400;
    if(!strstr(code, DeviceId) && !strstr(code, IpcId))
        return 404;
    map_alarm_se_did[did] = 1;
    return 200;
}
int GWSipHandleXML::HandleSubscribeAlarm(xmlNodePtr curNode, int did)
{
    char *code = NULL;
    char *type = NULL;

    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;
    while(curNode != NULL)
    {
        if (xmlHasProp(curNode,BAD_CAST "Code"))
            propNodePtr = curNode;
        curNode = curNode->next;
    }
    xmlAttrPtr attrPtr = propNodePtr->properties;
    while (attrPtr != NULL)
    {
        if (!xmlStrcmp(attrPtr->name, BAD_CAST "Code"))
            code = (char *)xmlGetProp(propNodePtr,BAD_CAST "Code");
        else if (!xmlStrcmp(attrPtr->name, BAD_CAST "Type"))
            type = (char *)xmlGetProp(propNodePtr,BAD_CAST "Type");
        attrPtr = attrPtr->next;
    }
    if(code == NULL)
        return 400;
    if(!strstr(code, DeviceId) && !strstr(code, IpcId))
        return 404;
    int32_t type32_i = atoi(type);
    int type_vl = 0;int type_md = 0;int type_cd = 0;int type_se = 0;
    //0x01 video lost  0x02 motiondetec  0x800 SD card error 0x1000 status event
    if((type32_i&0x01) == 0x01 || (type32_i&0x02) == 0x02 || (type32_i&0x800) == 0x800 || (type32_i&0x1000) == 0x1000)
    {
        if((type32_i&0x01) == 0x01)
            map_alarm_vl_did[did] = 1;
        if((type32_i&0x02) == 0x02)
            map_alarm_md_did[did] = 1;
        if((type32_i&0x800) == 0x800)
            map_alarm_cd_did[did] = 1;
        if((type32_i&0x1000) == 0x1000)
            map_alarm_se_did[did] = 1;
    }
    else if(type32_i == 0)
    {
        map_alarm_vl_did[did] = 1;map_alarm_md_did[did] = 1;map_alarm_cd_did[did] = 1;map_alarm_se_did[did] = 1;
    }
    else 
        return 404;
    return 200;
}
int GWSipHandleXML::HandleXMLControlCamera(xmlNodePtr curNode)
{
    char *command = NULL;
    char *commandpara1 = NULL;
    char *commandpara2 = NULL;

    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;
    while(curNode != NULL)
    {
        if (xmlHasProp(curNode,BAD_CAST "Command"))
            propNodePtr = curNode;
        curNode = curNode->next;
    }
    xmlAttrPtr attrPtr = propNodePtr->properties;
    while (attrPtr != NULL)
    {
        if (!xmlStrcmp(attrPtr->name, BAD_CAST "Command"))
            command = (char *)xmlGetProp(propNodePtr,BAD_CAST "Command");
        else if (!xmlStrcmp(attrPtr->name, BAD_CAST "CommandPara1"))
            commandpara1 = (char *)xmlGetProp(propNodePtr,BAD_CAST "CommandPara1");
        else if (!xmlStrcmp(attrPtr->name, BAD_CAST "CommandPara2"))
            commandpara2 = (char *)xmlGetProp(propNodePtr,BAD_CAST "CommandPara2");
        attrPtr = attrPtr->next;
    }
    if(command == NULL || commandpara1 == NULL || commandpara2 == NULL)
        return 400;
    GWSipParsePTZ MyGWSipParsePTZ;
    MyGWSipParsePTZ.ParsePTZ(command, commandpara1, commandpara2);
    return 2000;
}
int GWSipHandleXML::HandleXMLHistoryVideo(char xml_body[], xmlNodePtr curNode)
{
    char *code = NULL;
    char *type = NULL;
    char *beginTime = NULL;
    char *endTime = NULL;
    char *ToIndex = NULL;

    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;
    while(curNode != NULL)
    {
        if (xmlHasProp(curNode,BAD_CAST "Code"))
            propNodePtr = curNode;
        curNode = curNode->next;
    }
    xmlAttrPtr attrPtr = propNodePtr->properties;
    while (attrPtr != NULL)
    {
        if (!xmlStrcmp(attrPtr->name, BAD_CAST "Code"))
            code = (char *)xmlGetProp(propNodePtr,BAD_CAST "Code");
        else if (!xmlStrcmp(attrPtr->name, BAD_CAST "Type"))
            type = (char *)xmlGetProp(propNodePtr,BAD_CAST "Type");
        else if (!xmlStrcmp(attrPtr->name, BAD_CAST "BeginTime"))
            beginTime = (char *)xmlGetProp(propNodePtr,BAD_CAST "BeginTime");
        else if (!xmlStrcmp(attrPtr->name, BAD_CAST "EndTime"))
            endTime = (char *)xmlGetProp(propNodePtr,BAD_CAST "EndTime");
        else if (!xmlStrcmp(attrPtr->name, BAD_CAST "ToIndex"))
            ToIndex = (char *)xmlGetProp(propNodePtr,BAD_CAST "ToIndex");
        attrPtr = attrPtr->next;
    }
    if(code == NULL || type == NULL || beginTime == NULL || endTime == NULL) return 400;
    if(!strstr(code, IpcId)) return 404;

    int32_t type32_i = atoi(type);
    GWSipHandleRecordFile MyGWSipHandleRecordFile;
    int result = MyGWSipHandleRecordFile.GetHistroyRecordFileXML(xml_body, ToIndex, beginTime, endTime, type32_i);
    if(result != 0)
        return result;
    return 200;
}

int GWSipHandleXML::HandleXMLHistoryAlarm(char xml_body[], xmlNodePtr curNode)
{
    char *code = NULL;
    char *type = NULL;
    char *beginTime = NULL;
    char *endTime = NULL;
    char *ToIndex = NULL;
    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;
    while(curNode != NULL)
    {
        if (xmlHasProp(curNode,BAD_CAST "Code"))
            propNodePtr = curNode;
        curNode = curNode->next;
    }
    xmlAttrPtr attrPtr = propNodePtr->properties;
    while (attrPtr != NULL)
    {
        if (!xmlStrcmp(attrPtr->name, BAD_CAST "Code"))
            code = (char *)xmlGetProp(propNodePtr,BAD_CAST "Code");
        else if (!xmlStrcmp(attrPtr->name, BAD_CAST "Type"))
            type = (char *)xmlGetProp(propNodePtr,BAD_CAST "Type");
        else if (!xmlStrcmp(attrPtr->name, BAD_CAST "BeginTime"))
            beginTime = (char *)xmlGetProp(propNodePtr,BAD_CAST "BeginTime");
        else if (!xmlStrcmp(attrPtr->name, BAD_CAST "EndTime"))
            endTime = (char *)xmlGetProp(propNodePtr,BAD_CAST "EndTime");
        else if (!xmlStrcmp(attrPtr->name, BAD_CAST "ToIndex"))
            ToIndex = (char *)xmlGetProp(propNodePtr,BAD_CAST "ToIndex");
        attrPtr = attrPtr->next;
    }
    if(code == NULL || type == NULL || beginTime == NULL || endTime == NULL) return 400;
    int32_t type32_i = atoi(type);
    if(!strstr(code, IpcId)) return 404;

    GWSipHandleHistoryAlarm MyGWSipHandleHistoryAlarm;
    int result = MyGWSipHandleHistoryAlarm.GetHistoryAlarmXML(xml_body, ToIndex, beginTime, endTime, type32_i);
    if(result != 0)
        return result;
    return 200;
}
int GWSipHandleXML::HandleXMLResource(char xml_body[], xmlNodePtr curNode)
{
    char *code = NULL;
    char *fromindex = NULL;
    char *toindex = NULL;

    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;
    while(curNode != NULL)
    {
        if (xmlHasProp(curNode,BAD_CAST "Code"))
            propNodePtr = curNode;
        curNode = curNode->next;
    }
    xmlAttrPtr attrPtr = propNodePtr->properties; 
    while (attrPtr != NULL)
    {
        if (!xmlStrcmp(attrPtr->name, BAD_CAST "Code"))
            code = (char *)xmlGetProp(propNodePtr,BAD_CAST "Code");
        else if (!xmlStrcmp(attrPtr->name, BAD_CAST "FromIndex"))
            fromindex = (char *)xmlGetProp(propNodePtr,BAD_CAST "FromIndex");
        else if (!xmlStrcmp(attrPtr->name, BAD_CAST "ToIndex"))
            toindex = (char *)xmlGetProp(propNodePtr,BAD_CAST "ToIndex");
        attrPtr = attrPtr->next;
    }
    if(code == NULL)  return 400;
    if(!strstr(code, IpcId) && !strstr(code, DeviceId)) return 404;
    int status = 1;
    if(video_status == 0 || online_status == 0)
        status = 0;
    snprintf(xml_body, 4096,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
                                "<SIP_XML EventType=\"Response_Resource\">\r\n"
                                "<SubList Code=\"%s\" RealNum=\"1\" SubNum=\"1\" FromIndex=\"1\" ToIndex=\"1\">\r\n"
                                "<!--  %s  -->\r\n"
                                "<Item Code=\"%s\" Name=\"%s\" Status=\"%d\" DecoderTag=\"100\" Longitude=\" \" Latitude=\" \" SubNum=\"1\"/>\r\n"
                                "</SubList>\r\n"
                                "</SIP_XML>\r\n"
                                ,DeviceId, DeviceId, IpcId, "IPC1", status);
    return 200;
}
