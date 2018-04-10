#include "GWSipPushResourse.h"
#include <iostream>
using namespace std;
GWSipPushResourse::GWSipPushResourse()
{
}
GWSipPushResourse::~GWSipPushResourse()
{
}

int GWSipPushResourse::PushResourse()
{
	osip_message_t *notify = NULL; 
    char xml_body[1024];  
    int i;  

    eXosip_lock ();  
    i = eXosip_message_build_request(&notify, "NOTIFY", To, From, NULL);  
    if (i == 0)  
    {  
        snprintf(xml_body, 1024,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
                                "<SIP_XML EventType=\"Push_Resourse\">\r\n"
                                "<Code>%s</Code>\r\n"
                                "<SubList SubNum=\"1\">\r\n"
                                "<!--  %s  -->\r\n"
                                "<Item Code=\"%s\" Name=\"%s\" Status=\"1\" DecoderTag=\"100\" Longitude=\" \" Latitude=\" \" SubNum=\"1\"/>\r\n"
                                "</SubList>\r\n"
                                "</SIP_XML>\r\n"
                                ,DeviceId, DeviceId, IpcId, "IPC1");
        osip_message_set_body(notify,xml_body,strlen(xml_body)); 
        osip_message_set_header(notify, "Contact", Contact);
        osip_message_set_content_type(notify, "application/xml");  
        i = eXosip_message_send_request(notify);  
        if (i != 0)  
        { 
            eXosip_unlock ();
            return -1;  
        }   
    } 
    else
    {
        eXosip_unlock ();
        return -1; 
    }     
    eXosip_unlock (); 
    return 0;      
}
