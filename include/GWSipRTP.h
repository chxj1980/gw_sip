#ifndef _GWSIPRTP_H
#define _GWSIPRTP_H
#include "rtpsession.h"
#include "rtpsessionparams.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtptimeutilities.h"
#include "rtppacket.h"
#include "rtcppacket.h"
#include "rtcpsrpacket.h"
#include "rtcpcompoundpacket.h"
#include "rtcpsrpacket.h"
#include "rtcprrpacket.h"
#include "rtcpbyepacket.h"
#include "rtprawpacket.h"
#define MAX_RTP_PKT_LENGTH     1360

using namespace jrtplib;
using namespace std;
class MyRTPSession : public RTPSession
{
public:
    MyRTPSession()
    {
        SetChangeIncomingData(true);
        flag_RR  = 0; 
        count_RR = 0; 
    }
    ~MyRTPSession()
    {
    }
    int flag_RR;
    int count_RR;
protected:
    void OnRTCPCompoundPacket(RTCPCompoundPacket *p, const RTPTime &receivetime, const RTPAddress *senderaddress)
    {   
        printf("%u.%06u RECEIVED\n",receivetime.GetSeconds(),receivetime.GetMicroSeconds());
        
        DumpCompoundPacket(stdout,p);
    }
    void DumpCompoundPacket(FILE *f, RTCPCompoundPacket *p)
    {
        RTCPPacket *pack;

        p->GotoFirstPacket();
        flag_RR  = 0;
        while ((pack = p->GetNextPacket()) != 0)
        {
            if (pack->GetPacketType() == RTCPPacket::RR)
            {
                fprintf(f,"GET RR success\n");
                flag_RR = 1;
                break;
            }
        }
//        fprintf(f,"\n");
        if(flag_RR == 1)
            count_RR = 0;
        fprintf(f,"count_RR is %d\n", count_RR);
    }
};
typedef struct {
    //byte 0
    unsigned char TYPE:5;
    unsigned char NRI:2;
    unsigned char F:1;    
         
} NALU_HEADER; /**//* 1 BYTES */

typedef struct {
    //byte 0
    unsigned char TYPE:5;
    unsigned char NRI:2; 
    unsigned char F:1;    
            
             
} FU_INDICATOR; /**//* 1 BYTES */

typedef struct {
    //byte 0
    unsigned char TYPE:5;
    unsigned char R:1;
    unsigned char E:1;
    unsigned char S:1;    
} FU_HEADER; /**//* 1 BYTES */
typedef struct
{
  int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
  unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
  unsigned max_size;            //! Nal Unit Buffer size
  int forbidden_bit;            //! should be always FALSE
  int nal_reference_idc;        //! NALU_PRIORITY_xxxx
  int nal_unit_type;            //! NALU_TYPE_xxxx
  unsigned char *buf;                    //! contains the first byte followed by the EBSP
  unsigned short lost_packets;  //! true, if packet loss is detected
} NALU_t;


class GWSipRTP
{
public:
	GWSipRTP();
	~GWSipRTP();
	void checkerror(int rtperr);
	void CreateNew(char *DestIp, char *DestPort, int LocalPort, int SessionID);
    void SendH264(int SessionID, u_int8_t* fReceiveBuffer ,int length, int Did);
    void free_n(void);
    void FreeNALU(NALU_t *n);
    void GetAnnexbNALU (NALU_t *nalu, u_int8_t *fReceiveBuffer, int length);
    NALU_t *AllocNALU(int buffersize);
    NALU_t *n;
};

#endif