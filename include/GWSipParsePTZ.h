#ifndef _GWSIPPARSEPTZ_H
#define _GWSIPPARSEPTZ_H
#include "GWSip.h"

class GWSipParsePTZ
{
public:
	GWSipParsePTZ();
	~GWSipParsePTZ();
	void ParsePTZ(char *command, char *commandpara1, char *commandpara2);
};

#endif