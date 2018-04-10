#include "GWSipRegister.h"
#include <iostream>
using namespace std;
GWSipRegister::GWSipRegister()
{
}
GWSipRegister::~GWSipRegister()
{
}
int GWSipRegister::Register()
{
	int i = 0;
	int registerId = -1;
    eXosip_event_t *je  = NULL;
    osip_message_t *reg = NULL;
    int RegisterCount = 0;
retry:
    eXosip_lock();
    registerId = eXosip_register_build_initial_register(From, To, Contact, Expires, &reg);
    if (0 > registerId)
    {
        eXosip_unlock();
        return -1;
    }
    i = eXosip_register_send_register(registerId, reg);
    eXosip_unlock();
    if (0 != i)
        return -1;

    for (;;)
    {
        RegisterCount++;
        if(RegisterCount == 5)
        {
            registerId = -1;
            break;
        }    
        je = eXosip_event_wait(0, 500);
        if (NULL == je)
            continue;
        if (EXOSIP_REGISTRATION_FAILURE == je->type)
        {
            if ((NULL != je->response)&&(401 == je->response->status_code))
            {
                printf("handle 401\n");
                reg = NULL;
                eXosip_lock();
                eXosip_clear_authentication_info();
                eXosip_add_authentication_info(DeviceId, DeviceId, DevicePwd, "MD5", NULL);
                eXosip_register_build_register(je->rid, Expires, &reg);
                i = eXosip_register_send_register(je->rid, reg);
                eXosip_unlock();
            }
            else if ((NULL != je->response)&&(404 == je->response->status_code))
            {
                printf("404 ERROR. user or pwd error\n");
                goto retry;
            }
            else if ((NULL != je->response)&&(500 == je->response->status_code))
            {
                printf("500 ERROR. server error can not register\n");
                goto retry;
            }
            else if ((NULL != je->response)&&(503 == je->response->status_code))
            {
                printf("503 ERROR. wait 30sec\n");
                int count = 0;
                while(count < 30)
                {
                    sleep(1);
                    count++;
                }
                eXosip_event_free(je);
                goto retry;
            } 
            else
            {
                eXosip_event_free(je);
                goto retry; 
            }
                  
        }
        else if (EXOSIP_REGISTRATION_SUCCESS == je->type)
        {
            registerId = je->rid;
            cout<<"Register OK."<<endl;
            break;
        }
    }
    eXosip_event_free(je);
    return registerId;
}
int GWSipRegister::Unregister(int RegisterId, char *LogoutReason)
{
    int ret = 0;
    osip_message_t *reg = NULL;
    eXosip_lock(); 
    ret = eXosip_register_build_register(RegisterId, 0, &reg);
    if(ret < 0)
    {
        eXosip_unlock(); 
        return -1;
    }
    osip_contact_t *contact = NULL;
    osip_message_set_header(reg, "Logout-Reason", LogoutReason);
    ret = eXosip_register_send_register(RegisterId,reg);
    if(ret < 0)
    {
        eXosip_unlock(); 
        return -1;
    }
    eXosip_unlock();
    return 0;  
}