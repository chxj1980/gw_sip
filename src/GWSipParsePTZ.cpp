#include "GWSipParsePTZ.h"
#include "ONVIFUserInterface.h"
#include <iostream>
using namespace std;
using namespace yyhonviflib;
GWSipParsePTZ::GWSipParsePTZ()
{
}
GWSipParsePTZ::~GWSipParsePTZ()
{
}

void GWSipParsePTZ::ParsePTZ(char *command, char *commandpara1, char *commandpara2)
{
	int Cmd   = atoi(command);
	int CmdP1 = atoi(commandpara1);
	int CmdP2 = atoi(commandpara2);
	ONVIFUserInterface MyONVIFUserInterface;
	MyONVIFUserInterface.Path = "./dat/onvif.ini";
    MyONVIFUserInterface.User = "admin";
    MyONVIFUserInterface.Password= "carvedge123";
    MyONVIFUserInterface.Timeout = 5;
	MyONVIFUserInterface.PTZInit();
	switch(Cmd)
	{
		case 0x0101: ; break;
		case 0x0102: ; break;
		case 0x0103: ; break;
		case 0x0104: ; break;
		case 0x0201: ; break;
		case 0x0202: ; break;
		case 0x0203: ; break;
		case 0x0204: ; break;
		case 0x0301: MyONVIFUserInterface.PTZStopZoom(); break;
		case 0x0302: MyONVIFUserInterface.PTZZoom(-0.5); break;
		case 0x0303: MyONVIFUserInterface.PTZStopZoom(); break;
		case 0x0304: MyONVIFUserInterface.PTZZoom(0.5); break;
		case 0x0401: MyONVIFUserInterface.PTZStopPanTilt(); break;
		case 0x0402: MyONVIFUserInterface.PTZMoveY(float(CmdP2/10.0)); break;
		case 0x0403: MyONVIFUserInterface.PTZStopPanTilt(); break;
		case 0x0404: MyONVIFUserInterface.PTZMoveY(-float(CmdP2/10.0)); break;
		case 0x0501: MyONVIFUserInterface.PTZStopPanTilt(); break;
		case 0x0502: MyONVIFUserInterface.PTZMoveX(float(CmdP1/10.0)); break;
		case 0x0503: MyONVIFUserInterface.PTZStopPanTilt(); break;
		case 0x0504: MyONVIFUserInterface.PTZMoveX(-float(CmdP1/10.0)); break;
		case 0x0601: MyONVIFUserInterface.PTZSetPreset(commandpara1, commandpara1); break;
		case 0x0602: MyONVIFUserInterface.PTZGotoPreset(commandpara1); break;
		case 0x0603: MyONVIFUserInterface.PTZRemovePreset(commandpara1); break;
		case 0x0701: MyONVIFUserInterface.PTZStopPanTilt(); break;
		case 0x0702: MyONVIFUserInterface.PTZMoveXY(float(CmdP1/10.0), float(CmdP2/10.0)); break;
		case 0x0703: MyONVIFUserInterface.PTZStopPanTilt(); break;
		case 0x0704: MyONVIFUserInterface.PTZMoveXY(float(CmdP1/10.0), -float(CmdP2/10.0)); break;
		case 0x0801: MyONVIFUserInterface.PTZStopPanTilt(); break;
		case 0x0802: MyONVIFUserInterface.PTZMoveXY(-float(CmdP1/10.0), float(CmdP2/10.0)); break;
		case 0x0803: MyONVIFUserInterface.PTZStopPanTilt(); break;
		case 0x0804: MyONVIFUserInterface.PTZMoveXY(-float(CmdP1/10.0), -float(CmdP2/10.0)); break;
		case 0x0901: MyONVIFUserInterface.PTZStopAll(); break;
		case 0x0a01: ; break;
		case 0x0a02: ; break;
		case 0x0b01: ; break;
		case 0x0b02: ; break;
		case 0x0c01: ; break;
		case 0x0c02: ; break;
		case 0x0d01: ; break;
		case 0x0d02: ; break;
		case 0x0e01: ; break;
		case 0x0e02: ; break;
		case 0x0f01: ; break;
		case 0x0f02: ; break;
		case 0x1001: ; break;
		case 0x1002: ; break;
		case 0x1101: ; break;
		case 0x1102: ; break;
		default: ; break;
	}

}
