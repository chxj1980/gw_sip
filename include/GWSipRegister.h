#ifndef _GWSIPREGISTER_H
#define _GWSIPREGISTER_H
#include "GWSip.h"
class GWSipRegister
{
public:
	GWSipRegister();
	~GWSipRegister();
	int Register();
	int Unregister(int RegisterId, char *LogoutReason);

};

#endif