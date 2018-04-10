#include "GWSipPrint.h"
#include <iostream>
using namespace std;
GWSipPrint::GWSipPrint()
{
}
GWSipPrint::~GWSipPrint()
{
}
int GWSipPrint::PrintSip(eXosip_event_t *gw_event)
{
	if(gw_event == NULL)
	{
		cout<<"PrintSip: gw_event is NULL."<<endl;
		return -1;
	}
    osip_message_t *clone_event = NULL;
    size_t length = 0;
    char *request_message  = NULL;
    char *response_message = NULL;

    printf("\r\n##############################################################\r\n");
    switch (gw_event->type)
    {
        case EXOSIP_REGISTRATION_NEW:
            printf("EXOSIP_REGISTRATION_NEW\r\n");
            break;
        case EXOSIP_REGISTRATION_SUCCESS:
            printf("EXOSIP_REGISTRATION_SUCCESS\r\n");
            break;
        case EXOSIP_REGISTRATION_FAILURE:
            printf("EXOSIP_REGISTRATION_FAILURE\r\n");
            break;
        case EXOSIP_REGISTRATION_REFRESHED:
            printf("EXOSIP_REGISTRATION_REFRESHED\r\n");
            break;
        case EXOSIP_REGISTRATION_TERMINATED:
            printf("EXOSIP_REGISTRATION_TERMINATED\r\n");
            break;
        case EXOSIP_CALL_INVITE:
            printf("EXOSIP_CALL_INVITE\r\n");
            break;
        case EXOSIP_CALL_REINVITE:
            printf("EXOSIP_CALL_REINVITE\r\n");
            break;
        case EXOSIP_CALL_NOANSWER:
            printf("EXOSIP_CALL_NOANSWER\r\n");
            break;
        case EXOSIP_CALL_PROCEEDING:
            printf("EXOSIP_CALL_PROCEEDING\r\n");
            break;
        case EXOSIP_CALL_RINGING:
            printf("EXOSIP_CALL_RINGING\r\n");
            break;
        case EXOSIP_CALL_ANSWERED:
            printf("EXOSIP_CALL_ANSWERED\r\n");
            break;
        case EXOSIP_CALL_REDIRECTED:
            printf("EXOSIP_CALL_REDIRECTED\r\n");
            break;
        case EXOSIP_CALL_REQUESTFAILURE:
            printf("EXOSIP_CALL_REQUESTFAILURE\r\n");
            break;
        case EXOSIP_CALL_SERVERFAILURE:
            printf("EXOSIP_CALL_SERVERFAILURE\r\n");
            break;
        case EXOSIP_CALL_GLOBALFAILURE:
            printf("EXOSIP_CALL_GLOBALFAILURE\r\n");
            break;
        case EXOSIP_CALL_ACK:
            printf("EXOSIP_CALL_ACK\r\n");
            break;
        case EXOSIP_CALL_CANCELLED:
            printf("EXOSIP_CALL_CANCELLED\r\n");
            break;
        case EXOSIP_CALL_TIMEOUT:
            printf("EXOSIP_CALL_TIMEOUT\r\n");
            break;
        case EXOSIP_CALL_MESSAGE_NEW:
            printf("EXOSIP_CALL_MESSAGE_NEW\r\n");
            break;
        case EXOSIP_CALL_MESSAGE_PROCEEDING:
            printf("EXOSIP_CALL_MESSAGE_PROCEEDING\r\n");
            break;
        case EXOSIP_CALL_MESSAGE_ANSWERED:
            printf("EXOSIP_CALL_MESSAGE_ANSWERED\r\n");
            break;
        case EXOSIP_CALL_MESSAGE_REDIRECTED:
            printf("EXOSIP_CALL_MESSAGE_REDIRECTED\r\n");
            break;
        case EXOSIP_CALL_MESSAGE_REQUESTFAILURE:
            printf("EXOSIP_CALL_MESSAGE_REQUESTFAILURE\r\n");
            break;
        case EXOSIP_CALL_MESSAGE_SERVERFAILURE:
            printf("EXOSIP_CALL_MESSAGE_SERVERFAILURE\r\n");
            break;
        case EXOSIP_CALL_MESSAGE_GLOBALFAILURE:
            printf("EXOSIP_CALL_MESSAGE_GLOBALFAILURE\r\n");
            break;
        case EXOSIP_CALL_CLOSED:
            printf("EXOSIP_CALL_CLOSED\r\n");
            break;
        case EXOSIP_CALL_RELEASED:
            printf("EXOSIP_CALL_RELEASED\r\n");
            break;
        case EXOSIP_MESSAGE_NEW:
            printf("EXOSIP_MESSAGE_NEW\r\n");
            break;
        case EXOSIP_MESSAGE_PROCEEDING:
            printf("EXOSIP_MESSAGE_PROCEEDING\r\n");
            break;
        case EXOSIP_MESSAGE_ANSWERED:
            printf("EXOSIP_MESSAGE_ANSWERED\r\n");
            break;
        case EXOSIP_MESSAGE_REDIRECTED:
            printf("EXOSIP_MESSAGE_REDIRECTED\r\n");
            break;
        case EXOSIP_MESSAGE_REQUESTFAILURE:
            printf("EXOSIP_MESSAGE_REQUESTFAILURE\r\n");
            break;
        case EXOSIP_MESSAGE_SERVERFAILURE:
            printf("EXOSIP_MESSAGE_SERVERFAILURE\r\n");
            break;
        case EXOSIP_MESSAGE_GLOBALFAILURE:
            printf("EXOSIP_MESSAGE_GLOBALFAILURE\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_UPDATE:
            printf("EXOSIP_SUBSCRIPTION_UPDATE\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_CLOSED:
            printf("EXOSIP_SUBSCRIPTION_CLOSED\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_NOANSWER:
            printf("EXOSIP_SUBSCRIPTION_NOANSWER\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_PROCEEDING:
            printf("EXOSIP_SUBSCRIPTION_PROCEEDING\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_ANSWERED:
            printf("EXOSIP_SUBSCRIPTION_ANSWERED\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_REDIRECTED:
            printf("EXOSIP_SUBSCRIPTION_REDIRECTED\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_REQUESTFAILURE:
            printf("EXOSIP_SUBSCRIPTION_REQUESTFAILURE\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_SERVERFAILURE:
            printf("EXOSIP_SUBSCRIPTION_SERVERFAILURE\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_GLOBALFAILURE:
            printf("EXOSIP_SUBSCRIPTION_GLOBALFAILURE\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_NOTIFY:
            printf("EXOSIP_SUBSCRIPTION_NOTIFY\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_RELEASED:
            printf("EXOSIP_SUBSCRIPTION_RELEASED\r\n");
            break;
        case EXOSIP_IN_SUBSCRIPTION_NEW:
            printf("EXOSIP_IN_SUBSCRIPTION_NEW\r\n");
            break;
        case EXOSIP_IN_SUBSCRIPTION_RELEASED:
            printf("EXOSIP_IN_SUBSCRIPTION_RELEASED\r\n");
            break;
        case EXOSIP_NOTIFICATION_NOANSWER:
            printf("EXOSIP_NOTIFICATION_NOANSWER\r\n");
            break;
        case EXOSIP_NOTIFICATION_PROCEEDING:
            printf("EXOSIP_NOTIFICATION_PROCEEDING\r\n");
            break;
        case EXOSIP_NOTIFICATION_ANSWERED:
            printf("EXOSIP_NOTIFICATION_ANSWERED\r\n");
            break;
        case EXOSIP_NOTIFICATION_REDIRECTED:
            printf("EXOSIP_NOTIFICATION_REDIRECTED\r\n");
            break;
        case EXOSIP_NOTIFICATION_REQUESTFAILURE:
            printf("EXOSIP_NOTIFICATION_REQUESTFAILURE\r\n");
            break;
        case EXOSIP_NOTIFICATION_SERVERFAILURE:
            printf("EXOSIP_NOTIFICATION_SERVERFAILURE\r\n");
            break;
        case EXOSIP_NOTIFICATION_GLOBALFAILURE:
            printf("EXOSIP_NOTIFICATION_GLOBALFAILURE\r\n");
            break;
        case EXOSIP_EVENT_COUNT:
            printf("EXOSIP_EVENT_COUNT\r\n");
            break;
        default:
            printf("..................\r\n");
            break;
    }
    osip_message_clone(gw_event->request, &clone_event);
    osip_message_to_str(clone_event, &request_message, &length);
    if(request_message != NULL)
    {
        printf("request_message is:\r\n %s\r\n", request_message);
    }
    osip_message_clone(gw_event->response, &clone_event);
    osip_message_to_str(clone_event, &response_message, &length);
    if(response_message != NULL)
    {
        printf("response_message is:\r\n %s\r\n", response_message);
    }
    printf("\r\n##############################################################\r\n");
}
