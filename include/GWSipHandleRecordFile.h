#ifndef _GWSIPHANDLERECORDFILE_H
#define _GWSIPHANDLERECORDFILE_H
#include "GWSip.h"
#include <libxml/parser.h>
#include <vector>
#include <iostream>
using namespace std;
class GWSipHandleRecordFile
{
public:
	GWSipHandleRecordFile();
	~GWSipHandleRecordFile();

	int GetHistroyRecordFileXML(char xml_body[], char *ToIndex, char *beginTime, char *endTime, int type);
	int GetHistroyRecordList(long LbeginTime, long LendTime, vector<char *> &RecordList);
	int GetHistroyAlarmRecordList(long LbeginTime, long LendTime, vector<char *> &RecordList);
	int GetManualRecordList(long LbeginTime, long LendTime, vector<char *> &RecordList);
	unsigned long GetFileSize(const char *Path);
	int DelteAgoFile(const char* path);
	int GetSDCapability(void);
};

#endif