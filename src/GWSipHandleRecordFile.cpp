#include "GWSipHandleRecordFile.h"
#include <string.h> 
#include <sys/stat.h> 
#include <time.h>
#include <sqlite.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/statfs.h>

GWSipHandleRecordFile::GWSipHandleRecordFile()
{
}
GWSipHandleRecordFile::~GWSipHandleRecordFile()
{
}
int GWSipHandleRecordFile::GetHistroyRecordFileXML(char xml_body[], char *ToIndex, char *beginTime, char *endTime, int type)
{
	GWSipTime MyGWSipTime;
    int toindex = atoi(ToIndex);
    long LbeginTime = MyGWSipTime.SipTimeToTimestamp(beginTime);
    long LendTime   = MyGWSipTime.SipTimeToTimestamp(endTime);
    vector<char *>  RecordList;
    int i = 0;
    if(type == 0x100000 || type == 0xFFFFFFFF)
        GetHistroyRecordList(LbeginTime, LendTime, RecordList);
    else if(type == 0x02)
        GetHistroyAlarmRecordList(LbeginTime, LendTime, RecordList);
    else if(type == 0x200000)
        GetManualRecordList(LbeginTime, LendTime, RecordList);
    if(RecordList.size() == 0)
    {
        snprintf(xml_body, 1024, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
                                 "<SIP_XML EventType=\"Response_History_Video\">\r\n"
                                 "<SubList RealNum=\"1\" SubNum=\"1\" FromIndex=\"1\" ToIndex=\"1\">\r\n"
                                 "<Item FileName=\"NULL\" FileUrl=\"NULL\" BeginTime=\"NULL\" EndTime=\"NULL\" Size=\"NULL\" DecoderTag=\"NULL\" Type=\"NULL\"/>\r\n"
                                 "</SubList>\r\n"
                                 "</SIP_XML>\r\n");
        return  200;
    }
    if(RecordList.size() < toindex) toindex = RecordList.size();
    char xml_head[200] = {0};
    char xml_middle[400] = {0};
    snprintf(xml_head, 200, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
                                "<SIP_XML EventType=\"Response_History_Video\">\r\n"
                                "<SubList RealNum=\"%d\" SubNum=\"%d\" FromIndex=\"1\" ToIndex=\"%d\">\r\n"
                                ,RecordList.size() , toindex, toindex);
    string s_xml_head = xml_head;
    string s_xml_end  = "</SubList>\r\n</SIP_XML>\r\n";
    string s_xml_middle;
    char send_BeginTime[30] = {0};
    char send_EndTime[30] = {0};
    long record_size = 0;
    char filepath[50] = {0};
    for(i=0; i < toindex; i++)
    {
        memset(send_BeginTime, 0, 30);
        memset(send_EndTime, 0, 30);
        memset(xml_middle, 0, 300);
        MyGWSipTime.NormalTimeToSipTime(RecordList[i], send_BeginTime);
        MyGWSipTime.NormalTimeToSipTime(RecordList[i], send_EndTime, 3600);
        
        if(type == 0x100000)
        {
            snprintf(filepath, 50, "/media/mmcblk0p1/1/%s", RecordList[i]);
            record_size = GetFileSize(filepath);
            snprintf(xml_middle, 400 ,"<Item FileName=\"%s\" FileUrl=\"rtsp://%s:554/media/mmcblk0p1/1/%s\" BeginTime=\"%s\" EndTime=\"%s\" Size=\"%ld\" DecoderTag=\"100\" Type=\"0x10000\"/>\r\n"
                        , RecordList[i], LocalIp, RecordList[i], send_BeginTime, send_EndTime, record_size);
        }
            
        else if(type == 0x02)
        {
            snprintf(filepath, 50, "/media/mmcblk0p1/2/%s", RecordList[i]);
            record_size = GetFileSize(filepath);
            snprintf(xml_middle, 400 ,"<Item FileName=\"%s\" FileUrl=\"rtsp://%s:554/media/mmcblk0p1/2/%s\" BeginTime=\"%s\" EndTime=\"%s\" Size=\"%ld\" DecoderTag=\"100\" Type=\"0x10000\"/>\r\n"
                        , RecordList[i], LocalIp, RecordList[i], send_BeginTime, send_EndTime, record_size);
        }
            
        else if(type == 0x200000)
        {
            snprintf(filepath, 50, "/media/mmcblk0p1/3/%s", RecordList[i]);
            record_size = GetFileSize(filepath);
            snprintf(xml_middle, 400 ,"<Item FileName=\"%s\" FileUrl=\"rtsp://%s:554/media/mmcblk0p1/3/%s\" BeginTime=\"%s\" EndTime=\"%s\" Size=\"%ld\" DecoderTag=\"100\" Type=\"0x10000\"/>\r\n"
                        , RecordList[i], LocalIp, RecordList[i], send_BeginTime, send_EndTime, record_size);
        }
            
        s_xml_middle = xml_middle;
        s_xml_head += s_xml_middle;
    }
    s_xml_head += s_xml_end;
    snprintf(xml_body, 30720, "%s", s_xml_head.data());
    RecordList.clear();
    vector<char *>().swap(RecordList); 
    return 0;
}
int GWSipHandleRecordFile::GetHistroyRecordList(long LbeginTime, long LendTime, vector<char *> &RecordList)
{
    DIR *RecordDir;
    struct dirent *File;
    struct stat buf; 
    if(!(RecordDir = opendir("/media/mmcblk0p1/1")))
    {
        printf("error opendir /media/mmcblk0p1/1!!!\n");
        return -1;
    }
    char FilePath[50] = {0};
    int i = 0;
    while((File = readdir(RecordDir)) != NULL)
    {
        if (strcmp(File->d_name, ".") == 0 ||   
                strcmp(File->d_name, "..") == 0 )    
                continue; 
        snprintf(FilePath, 50, "/media/mmcblk0p1/1/%s", File->d_name);
        if(stat(FilePath, &buf) >= 0 && !S_ISDIR(buf.st_mode) )
            if(buf.st_ctime > LbeginTime && buf.st_ctime < LendTime)
                RecordList.push_back(File->d_name);
    }
    return 0;
}
int GWSipHandleRecordFile::GetHistroyAlarmRecordList(long LbeginTime, long LendTime, vector<char *> &RecordList)
{
    DIR *RecordDir;
    struct dirent *File;
    struct stat buf; 
    if(!(RecordDir = opendir("/media/mmcblk0p1/2")))
    {
        printf("error opendir /media/mmcblk0p1/2!!!\n");
        return -1;
    }
    char FilePath[50] = {0};
    int i = 0;
    while((File = readdir(RecordDir)) != NULL)
    {
        if (strcmp(File->d_name, ".") == 0 ||   
                strcmp(File->d_name, "..") == 0 )    
                continue; 
        snprintf(FilePath, 50, "/media/mmcblk0p1/2/%s", File->d_name);
        if(stat(FilePath, &buf) >= 0 && !S_ISDIR(buf.st_mode) )
            if(buf.st_ctime > LbeginTime && buf.st_ctime < LendTime)
                RecordList.push_back(File->d_name);
    }
    return 0;
}
int GWSipHandleRecordFile::GetManualRecordList(long LbeginTime, long LendTime, vector<char *> &RecordList)
{
    DIR *RecordDir;
    struct dirent *File;
    struct stat buf; 
    if(!(RecordDir = opendir("/media/mmcblk0p1/3")))
    {
        printf("error opendir /media/mmcblk0p1/3!!!\n");
        return -1;
    }
    char FilePath[50] = {0};
    int i = 0;
    while((File = readdir(RecordDir)) != NULL)
    {
        if (strcmp(File->d_name, ".") == 0 ||   
                strcmp(File->d_name, "..") == 0 )    
                continue; 
        snprintf(FilePath, 50, "/media/mmcblk0p1/3/%s", File->d_name);
        if(stat(FilePath, &buf) >= 0 && !S_ISDIR(buf.st_mode) )
            if(buf.st_ctime > LbeginTime && buf.st_ctime < LendTime)
                RecordList.push_back(File->d_name);
    }
    return 0;
}
unsigned long GWSipHandleRecordFile::GetFileSize(const char *Path)  
{  
    unsigned long filesize = -1;      
    struct stat statbuff;  
    if(stat(Path, &statbuff) < 0)  
        return filesize;  
    else  
        filesize = statbuff.st_size;   
    return filesize;  
}

int GWSipHandleRecordFile::DelteAgoFile(const char* path)
{
    DIR *d; 
    struct dirent *file; 
    struct stat buf;    
    time_t MinTime = 0;
    char   MinName[50] = {0};
    if(!(d = opendir(path)))
    {
        printf("error opendir %s!!!\n",path);
        return -1;
    }
    char FilePath[50] = {0};
    int flag = 0;
    int count = 0;
    while((file = readdir(d)) != NULL)
    {
        if (strcmp(file->d_name, ".") == 0 ||   
                strcmp(file->d_name, "..") == 0 )    
                continue; 
        snprintf(FilePath, 50, "%s/%s", path, file->d_name);
        if(stat(FilePath, &buf) >= 0 && !S_ISDIR(buf.st_mode) )
        {
            count++;
            if(flag == 0)
            {
                MinTime = buf.st_ctime;
                flag = 1;
                memset(MinName, 0 , 50);
                memcpy(MinName, FilePath, 49);
            }   
            if(MinTime > buf.st_ctime)
            {
                MinTime = buf.st_ctime;
                memset(MinName, 0 , 50);
                memcpy(MinName, FilePath, 49);
            }
        }
    }
    cout<<"Delete ago file: "<<MinName<<endl;
    if(count > 5)
        remove(MinName);
    closedir(d);
    return 0;
}
int GWSipHandleRecordFile::GetSDCapability(void)
{
    struct statfs diskInfo;
    statfs("/media/mmcblk0p1", &diskInfo);
    unsigned long long blocksize = diskInfo.f_bsize;// 每个block里面包含的字节数
    unsigned long long totalsize = blocksize*diskInfo.f_blocks;//总的字节数
    
  //  printf("TOTAL_SIZE == %llu KB  %llu MB  %llu GB\n",totalsize>>10,totalsize>>20,totalsize>>30); // 分别换成KB,MB,GB为单位
    if((totalsize>>30) < 2 )
        return 401; //SD卡不存在

    char totalsize_GB[10]={0};
    sprintf(totalsize_GB,"%.2f",(float)(totalsize>>20)/1024);  
 //   printf("totalsize_GB=%s\n",totalsize_GB);
    unsigned long long freesize = blocksize*diskInfo.f_bfree; //再计算下剩余的空间大小  
      
 //   printf("DISK_FREE == %llu KB  %llu MB  %llu GB\n",freesize>>10,freesize>>20,freesize>>30);  
    if((freesize>>20) < 200)  //SD卡剩余空间小于200M，删除最早的录像文件
        return 402;
//    unsigned long long usedsize = blocksize*(diskInfo.f_blocks - diskInfo.f_bfree);  
 //   char usedsize_GB[10]={0};  
//    sprintf(usedsize_GB,"%.2f",(float)(usedsize>>20)/1024);  
 //   printf("usedsize_GB=%s\n",usedsize_GB);
    return 0;
}