#ifndef _GWSIPHANDLEVIDEO_H
#define _GWSIPHANDLEVIDEO_H
#include "GWSip.h"

class GWSipHandleVideo
{
public:
	GWSipHandleVideo();
	~GWSipHandleVideo();
	int StartSendVideo(int Did);
	void CloseVideo(int Did, int CloseType);
};

#endif