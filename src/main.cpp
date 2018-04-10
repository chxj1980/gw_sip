#include "GWSip.h"
#include "ONVIFUserInterface.h"
#include "GWSipRTSP.h"
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include "Log.h"
using namespace std;
using namespace yyhonviflib;
GWSipRTP MyGWSipRTP;
void *RTSPThread(void *);
int main(int argc, char** argv)
{
    LOG("gw_sip start");
    int ret = 0;
    pthread_t RTSPId;
    ret = pthread_create(&RTSPId, NULL, RTSPThread, NULL);
    if(ret)   return -1;

    pthread_t id;
    ret = pthread_create(&id, NULL, MonitorThread, NULL);
    if(ret)   return -1;
    
    MyGWSipRTP.n = MyGWSipRTP.AllocNALU(5000000);
    GWSip MyGWSip;
    MyGWSip.EventLoop(); 
    
    
    return 0;
}
void *RTSPThread(void *)
{
    char RtspUri[200] = {0};
    char Root[20] = "subStream";
    ONVIFIni MyONVIFIni;
    MyONVIFIni.ReadIni("./dat/onvif.ini", Root, "StreamUri", RtspUri);
    const char *ProgName = "RecordMod";
    while(1)
    {
        TaskScheduler* scheduler = BasicTaskScheduler::createNew();
        UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);
        eventLoopWatchVariable = 0;
        openURL(*env, ProgName, RtspUri, "admin", "carvedge123"); 
        env->taskScheduler().doEventLoop(&eventLoopWatchVariable);
        env->reclaim(); env = NULL;
        delete scheduler; scheduler = NULL;
    }
}