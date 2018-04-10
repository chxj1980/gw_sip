#include "GWSipHandleVideo.h"
#include <iostream>
using namespace std;
GWSipHandleVideo::GWSipHandleVideo()
{
}
GWSipHandleVideo::~GWSipHandleVideo()
{
}
int GWSipHandleVideo::StartSendVideo(int Did)
{
	FILE *Fp;
	int RTPLocalPort = 6666;
	for(int i=0; i<5; i++)
	    if(VCallUser[i].Did == Did)
	    {
	    	if(Did % 2 == 0)
	    		RTPLocalPort = 6666 + Did;
	    	else
	    		RTPLocalPort = 6666 - Did;
	    	VCallUser[i].stat = 1;
	    	CallUserNum ++;
	    	MyGWSipRTP.CreateNew(VCallUser[i].RTPIp, VCallUser[i].RTPPort, RTPLocalPort, i);
	    }
	return 0;
}
void GWSipHandleVideo::CloseVideo(int Did, int CloseType)
{
	for(unsigned int i=0; i<5; i++)
	{
		if(VCallUser[i].Did == Did)
		{
			CallUserNum --;
			VCallUser[i].stat = 0;
			if(CloseType == 0)
			    eXosip_call_terminate(VCallUser[i].CallId, VCallUser[i].Did);
			session[i].Destroy();
		}
	}
	
}