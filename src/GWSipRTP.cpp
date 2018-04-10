#include "GWSipRTP.h"
#include "GWSip.h"
#include <iostream>
using namespace std;
MyRTPSession session[5];
GWSipRTP::GWSipRTP()
{
}
GWSipRTP::~GWSipRTP()
{
}
void GWSipRTP::CreateNew(char *DestIp, char *DestPort, int LocalPort, int SessionID)
{
	uint16_t portbase, destport_r;
    uint32_t destip_r;

    cout<<DestIp<<endl;
    cout<<DestPort<<endl;
    cout<<LocalPort<<endl;
    RTPSessionParams sessionparams;
    sessionparams.SetOwnTimestampUnit(1.0/90000.0);

    RTPUDPv4TransmissionParams transparams;
    portbase = LocalPort;
    transparams.SetPortbase(portbase);
    
    int status = session[SessionID].Create(sessionparams,&transparams);
    checkerror(status);
    destip_r   = inet_addr(DestIp);
    destip_r   = ntohl(destip_r);
    destport_r = atoi(DestPort);
    RTPIPv4Address addr(destip_r,destport_r);

    status = session[SessionID].AddDestination(addr);
    checkerror(status);

    session[SessionID].SetDefaultPayloadType(100);
    session[SessionID].SetDefaultMark(false);
    session[SessionID].SetDefaultTimestampIncrement(90000.0 /25.0);
    session[SessionID].count_RR = 0;
}
void GWSipRTP::SendH264(int SessionID, u_int8_t* fReceiveBuffer ,int length, int Did)
{
    session[SessionID].count_RR ++;
    printf("count_RR is:%d\n", session[SessionID].count_RR);
    if(session[SessionID].count_RR >500)
    {
        cout<<"close video"<<endl;
        GWSipHandleVideo MyGWSipHandleVideo;
        MyGWSipHandleVideo.CloseVideo(Did, 0);
    }
    int status;
    NALU_HEADER     *nalu_hdr;
    FU_INDICATOR    *fu_ind;
    FU_HEADER       *fu_hdr;
    char sendbuf[1500];
    char* nalu_payload;
    unsigned int timestamp_increse=0,ts_current=0;
    bool start=false;
    int size=length+4;
    GetAnnexbNALU (n , fReceiveBuffer,  length);
    if(n->len<=MAX_RTP_PKT_LENGTH)
    {
        nalu_hdr =(NALU_HEADER*)&sendbuf[0]; 
        nalu_hdr->F=n->forbidden_bit;
        nalu_hdr->NRI=n->nal_reference_idc>>5;
        nalu_hdr->TYPE=n->nal_unit_type;
        nalu_payload=&sendbuf[1];
        memcpy(nalu_payload,n->buf+1,n->len-1);
        ts_current=ts_current+timestamp_increse;
    
        if(n->nal_unit_type==1 || n->nal_unit_type==5)
        {
            status = session[SessionID].SendPacket((void *)sendbuf,n->len,100,true,3600);
        }
        else
        {
            status = session[SessionID].SendPacket((void *)sendbuf,n->len,100,true,0);
        }
        checkerror(status);
    }
    else if(n->len>MAX_RTP_PKT_LENGTH)
    {
        
        int k=0,l=0;
        k=n->len/MAX_RTP_PKT_LENGTH;
        l=n->len%MAX_RTP_PKT_LENGTH;
        int t=0;
        ts_current=ts_current+timestamp_increse;
        while(t<=k)
            if(!t)
            {
                memset(sendbuf,0,1500);
               
                fu_ind =(FU_INDICATOR*)&sendbuf[0]; 
                fu_ind->F=n->forbidden_bit;
                fu_ind->NRI=n->nal_reference_idc>>5;
                fu_ind->TYPE=28;
                fu_hdr =(FU_HEADER*)&sendbuf[1];
                fu_hdr->E=0;
                fu_hdr->R=0;
                fu_hdr->S=1;
                fu_hdr->TYPE=n->nal_unit_type;
                nalu_payload=&sendbuf[2];
                memcpy(nalu_payload,n->buf+1,MAX_RTP_PKT_LENGTH);
                status = session[SessionID].SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH+2,100,false,0);
                checkerror(status);
                t++;
            }
            
            else if(k==t)
            {
                
                memset(sendbuf,0,1500);

                fu_ind =(FU_INDICATOR*)&sendbuf[0]; 
                fu_ind->F=n->forbidden_bit;
                fu_ind->NRI=n->nal_reference_idc>>5;
                fu_ind->TYPE=28;
                
                fu_hdr =(FU_HEADER*)&sendbuf[1];
                fu_hdr->R=0;
                fu_hdr->S=0;
                fu_hdr->TYPE=n->nal_unit_type;
                fu_hdr->E=1;
                nalu_payload=&sendbuf[2];
                memcpy(nalu_payload,n->buf+t*MAX_RTP_PKT_LENGTH+1,l-1);
                
                status = session[SessionID].SendPacket((void *)sendbuf,l+1,100,true,3600);
                checkerror(status);
                t++;
            }
            else if(t<k&&0!=t)
            {
                
                memset(sendbuf,0,1500);
                
                fu_ind =(FU_INDICATOR*)&sendbuf[0]; 
                fu_ind->F=n->forbidden_bit;
                fu_ind->NRI=n->nal_reference_idc>>5;
                fu_ind->TYPE=28;
                
                fu_hdr =(FU_HEADER*)&sendbuf[1];
                
                fu_hdr->R=0;
                fu_hdr->S=0;
                fu_hdr->E=0;
                fu_hdr->TYPE=n->nal_unit_type;
                nalu_payload=&sendbuf[2];
                memcpy(nalu_payload,n->buf+t*MAX_RTP_PKT_LENGTH+1,MAX_RTP_PKT_LENGTH);
                status = session[SessionID].SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH+2,100,false,0);
                checkerror(status);
                t++;
            }
    }    
}
void GWSipRTP::free_n(void)
{
    FreeNALU(n);
}
void GWSipRTP::FreeNALU(NALU_t *n)
{
  if (n)
  {
    if (n->buf)
    {
        free(n->buf);
        n->buf=NULL;
    }
    free (n);
  }
}
void GWSipRTP::checkerror(int rtperr)
{
    if (rtperr < 0)
        std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
}
void GWSipRTP::GetAnnexbNALU (NALU_t *nalu, u_int8_t *fReceiveBuffer, int length)
{
    nalu->startcodeprefix_len=4;
    nalu->len =length;
    nalu->buf = fReceiveBuffer;
    nalu->forbidden_bit = nalu->buf[0] & 0x80; 
    nalu->nal_reference_idc = nalu->buf[0] & 0x60; 
    nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;

}
NALU_t *GWSipRTP::AllocNALU(int buffersize)
{
    NALU_t *n;

    if ((n = (NALU_t*)calloc (1, sizeof (NALU_t))) == NULL)
    {
        printf("AllocNALU: n");
        exit(0);
    }

    n->max_size=buffersize;

    if ((n->buf = (unsigned char*)calloc (buffersize, sizeof (char))) == NULL)
    {
        free (n);
        printf ("AllocNALU: n->buf");
        exit(0);
    }

    return n;
}