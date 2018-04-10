#include "GWSipRTSP.h"
#include "GWSipGlobalVariable.h"
#include "iostream"
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <time.h>
#include <sqlite.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
using namespace std;
static unsigned rtspClientCount = 0;
int LifeLive555 = 10;
char eventLoopWatchVariable = 0;
RTSPClient* rtspClient;
int IsConnect = 0;
int SDIsExist = 1;
int SDIsFull  = 0;
time_t RecordBeginTime = 0;
UsageEnvironment& operator<<(UsageEnvironment& env, const RTSPClient& rtspClient) 
{
    return env << "[URL:\"" << rtspClient.url() << "\"]: ";
}
UsageEnvironment& operator<<(UsageEnvironment& env, const MediaSubsession& subsession) 
{
    return env << subsession.mediumName() << "/" << subsession.codecName();
}
void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL, char const* User, char const* Password) 
{
    rtspClient = ourRTSPClient::createNew(env, rtspURL, RTSP_CLIENT_VERBOSITY_LEVEL, progName);
    if (rtspClient == NULL) 
    {
        env << "Failed to create a RTSP client for URL \"" << rtspURL << "\": " << env.getResultMsg() << "\n";
        return;
    }
    ++rtspClientCount;
    Authenticator* authenticator = new Authenticator(User, Password);
    rtspClient->sendDescribeCommand(continueAfterDESCRIBE, authenticator); 
}

void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) 
{
    do 
    {
        UsageEnvironment& env = rtspClient->envir(); // alias
        StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias
        if (resultCode != 0) 
        {
            env << *rtspClient << "Failed to get a SDP description: " << resultString << "\n";
            delete[] resultString;
            break;
        }
        char* const sdpDescription = resultString;
        env << *rtspClient << "Got a SDP description:\n" << sdpDescription << "\n";
        // Create a media session object from this SDP description:
        scs.session = MediaSession::createNew(env, sdpDescription);
        delete[] sdpDescription; // because we don't need it anymore
        if (scs.session == NULL) 
        {
            env << *rtspClient << "Failed to create a MediaSession object from the SDP description: " << env.getResultMsg() << "\n";
            break;
        } 
        else if (!scs.session->hasSubsessions()) 
        {
            env << *rtspClient << "This session has no media subsessions (i.e., no \"m=\" lines)\n";
            break;
        }
        scs.iter = new MediaSubsessionIterator(*scs.session);
        setupNextSubsession(rtspClient);
        return;
    } while (0);

    // An unrecoverable error occurred with this stream.
    shutdownStream(rtspClient);
}
void setupNextSubsession(RTSPClient* rtspClient) 
{
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias
  
    scs.subsession = scs.iter->next();
    if (scs.subsession != NULL) 
    {
        if (!scs.subsession->initiate()) 
        {
            env << *rtspClient << "Failed to initiate the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\n";
            setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
        } 
        else 
        {
            env << *rtspClient << "Initiated the \"" << *scs.subsession << "\" subsession (";
            if (scs.subsession->rtcpIsMuxed()) 
            {
                env << "client port " << scs.subsession->clientPortNum();
            } 
            else 
            {
                env << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1;
            }
            env << ")\n";

            // Continue setting up this subsession, by sending a RTSP "SETUP" command:
            rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, REQUEST_STREAMING_OVER_TCP);
        }
        return;
    }

  // We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
    if (scs.session->absStartTime() != NULL) 
    {
        // Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
        rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime());
    } 
    else 
    {
        scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
        rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
    }
}
void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString) 
{
    do 
    {
        UsageEnvironment& env = rtspClient->envir(); // alias
        StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

        if (resultCode != 0) 
        {
            env << *rtspClient << "Failed to set up the \"" << *scs.subsession << "\" subsession: " << resultString << "\n";
            break;
        }
        env << *rtspClient << "Set up the \"" << *scs.subsession << "\" subsession (";
        if (scs.subsession->rtcpIsMuxed()) 
        {
            env << "client port " << scs.subsession->clientPortNum();
        } 
        else 
        {
            env << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1;
        }
        env << ")\n";
        scs.subsession->sink = DummySink::createNew(env, *scs.subsession, rtspClient->url());
        // perhaps use your own custom "MediaSink" subclass instead
        if (scs.subsession->sink == NULL)
        {
            env << *rtspClient << "Failed to create a data sink for the \"" << *scs.subsession
            << "\" subsession: " << env.getResultMsg() << "\n";
            break;
        }
        env << *rtspClient << "Created a data sink for the \"" << *scs.subsession << "\" subsession\n";
        scs.subsession->miscPtr = rtspClient; // a hack to let subsession handle functions get the "RTSPClient" from the subsession 
        scs.subsession->sink->startPlaying(*(scs.subsession->readSource()),
                       subsessionAfterPlaying, scs.subsession);
        // Also set a handler to be called if a RTCP "BYE" arrives for this subsession:
        if (scs.subsession->rtcpInstance() != NULL) 
        {
            scs.subsession->rtcpInstance()->setByeHandler(subsessionByeHandler, scs.subsession);
        }
    } while (0);
    delete[] resultString;
    // Set up the next subsession, if any:
    setupNextSubsession(rtspClient);
}

void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString) 
{
    Boolean success = False;
    do 
    {
        UsageEnvironment& env = rtspClient->envir(); // alias
        StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias
        if (resultCode != 0) 
        {
            env << *rtspClient << "Failed to start playing session: " << resultString << "\n";
            break;
        }
        if (scs.duration > 0) 
        {
            unsigned const delaySlop = 2; // number of seconds extra to delay, after the stream's expected duration.  (This is optional.)
            scs.duration += delaySlop;
            unsigned uSecsToDelay = (unsigned)(scs.duration*1000000);
            scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
        }
        env << *rtspClient << "Started playing session";
        if (scs.duration > 0) 
        {
            env << " (for up to " << scs.duration << " seconds)";
        }
        env << "...\n";
        success = True;
    } while (0);
    delete[] resultString;
    if (!success) 
    {
    // An unrecoverable error occurred with this stream.
        shutdownStream(rtspClient);
    }
}
// Implementation of the other event handlers:

void subsessionAfterPlaying(void* clientData) 
{
    MediaSubsession* subsession = (MediaSubsession*)clientData;
    RTSPClient* rtspClient = (RTSPClient*)(subsession->miscPtr);
    // Begin by closing this subsession's stream:
    Medium::close(subsession->sink);
    subsession->sink = NULL;
    // Next, check whether *all* subsessions' streams have now been closed:
    MediaSession& session = subsession->parentSession();
    MediaSubsessionIterator iter(session);
    while ((subsession = iter.next()) != NULL) 
    {
        if (subsession->sink != NULL) 
        return; // this subsession is still active
    }
    // All subsessions' streams have now been closed, so shutdown the client:
    shutdownStream(rtspClient);
}

void subsessionByeHandler(void* clientData) 
{
    MediaSubsession* subsession = (MediaSubsession*)clientData;
    RTSPClient* rtspClient = (RTSPClient*)subsession->miscPtr;
    UsageEnvironment& env = rtspClient->envir(); // alias

    env << *rtspClient << "Received RTCP \"BYE\" on \"" << *subsession << "\" subsession\n";

    // Now act as if the subsession had closed:
    subsessionAfterPlaying(subsession);
}

void streamTimerHandler(void* clientData) 
{
    ourRTSPClient* rtspClient = (ourRTSPClient*)clientData;
    StreamClientState& scs = rtspClient->scs; // alias
    scs.streamTimerTask = NULL;

    // Shut down the stream:
    shutdownStream(rtspClient);
}

void shutdownStream(RTSPClient* rtspClient, int exitCode) 
{
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

    // First, check whether any subsessions have still to be closed:
    if (scs.session != NULL) 
    { 
        Boolean someSubsessionsWereActive = False;
        MediaSubsessionIterator iter(*scs.session);
        MediaSubsession* subsession;

        while ((subsession = iter.next()) != NULL) 
        {
            if (subsession->sink != NULL) 
            {
                Medium::close(subsession->sink);
                subsession->sink = NULL;

                if (subsession->rtcpInstance() != NULL) 
                {
                    subsession->rtcpInstance()->setByeHandler(NULL, NULL); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
                }

                someSubsessionsWereActive = True;
            }
        }

        if (someSubsessionsWereActive) 
        {
            // Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
            // Don't bother handling the response to the "TEARDOWN".
            rtspClient->sendTeardownCommand(*scs.session, NULL);
        }
    }

    env << *rtspClient << "Closing the stream.\n";
    Medium::close(rtspClient);
    // Note that this will also cause this stream's "StreamClientState" structure to get reclaimed.

    if (--rtspClientCount == 0) 
    {
        // The final stream has ended, so exit the application now.
        // (Of course, if you're embedding this code into your own application, you might want to comment this out,
        // and replace it with "eventLoopWatchVariable = 1;", so that we leave the LIVE555 event loop, and continue running "main()".)
        //exit(exitCode);  //modify by yyl
        rtspClientCount ++;
        cout<<"NO connect"<<endl;
    }
}


// Implementation of "ourRTSPClient":

ourRTSPClient* ourRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL,
                    int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum) 
{
    return new ourRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

ourRTSPClient::ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
                 int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
  : RTSPClient(env,rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1) 
{}

ourRTSPClient::~ourRTSPClient() 
{}


// Implementation of "StreamClientState":

StreamClientState::StreamClientState()
  : iter(NULL), session(NULL), subsession(NULL), streamTimerTask(NULL), duration(0.0) 
{}

StreamClientState::~StreamClientState() 
{
    delete iter;
    if (session != NULL) 
    {
        // We also need to delete "session", and unschedule "streamTimerTask" (if set)
        UsageEnvironment& env = session->envir(); // alias

        env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
        Medium::close(session);
    }
}
DummySink* DummySink::createNew(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId) 
{
    return new DummySink(env, subsession, streamId);
}

DummySink::DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId)
  : MediaSink(env),
    fSubsession(subsession) 
{
    fStreamId = strDup(streamId);
    fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
    Begin = 0;
    RecordTime = 0;
    FpRecord = NULL;
}

DummySink::~DummySink() 
{
    RecordBeginTime = 0;
    if(FpRecord != NULL)
            fclose(FpRecord);
    delete[] fReceiveBuffer;
    delete[] fStreamId;
}

void DummySink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
                  struct timeval presentationTime, unsigned durationInMicroseconds) 
{
    DummySink* sink = (DummySink*)clientData;
    sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}




void DummySink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
                  struct timeval presentationTime, unsigned /*durationInMicroseconds*/) 
{
  // We've just received a frame of data.  (Optionally) print out information about it:
    #ifdef DEBUG_PRINT_EACH_RECEIVED_FRAME
    if (fStreamId != NULL) 
        envir() << "Stream \"" << fStreamId << "\"; ";
    envir() << fSubsession.mediumName() << "/" << fSubsession.codecName() << ":\tReceived " << frameSize << " bytes";
    if (numTruncatedBytes > 0) 
        envir() << " (with " << numTruncatedBytes << " bytes truncated)";
    char uSecsStr[6+1]; // used to output the 'microseconds' part of the presentation time
    sprintf(uSecsStr, "%06u", (unsigned)presentationTime.tv_usec);
    envir() << ".\tPresentation time: " << (int)presentationTime.tv_sec << "." << uSecsStr;
    if (fSubsession.rtpSource() != NULL && !fSubsession.rtpSource()->hasBeenSynchronizedUsingRTCP()) 
    {
        envir() << "!"; // mark the debugging output to indicate that this presentation time is not RTCP-synchronized
    }
    #ifdef DEBUG_PRINT_NPT
        envir() << "\tNPT: " << fSubsession.getNormalPlayTime(presentationTime);
    #endif
        envir() << "\n";
    #endif
    LifeLive555 = 10;
    IsConnect = 1;
    for(unsigned int i = 0; i < 5; i++)
        if(VCallUser[i].stat == 1)
            MyGWSipRTP.SendH264(i, fReceiveBuffer, frameSize, VCallUser[i].Did);
    if(0 == access("/opt/app/ram/videolost.txt", 0))
        remove("/opt/app/ram/videolost.txt");
    
    SaveRecord(frameSize);
    // Then continue, to request the next frame of data:
    continuePlaying();
}

Boolean DummySink::continuePlaying() 
{
    if (fSource == NULL) return False; // sanity check (should not happen)

    // Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
    fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE,
                        afterGettingFrame, this,
                        onSourceClosure, this);
    return True;
}

int DummySink::SaveRecord(unsigned frameSize)
{
    u_int8_t H264_StartCode[] = { 0x00, 0x00, 0x00, 0x01 };
    time_t CurTime;
    time(&CurTime);
    if(RecordTime > 600)
    {
        Begin = 0;
        RecordTime = 0;
        if(FpRecord != NULL)
            fclose(FpRecord);
        if(SaveRecordFlag == 1)
        {
            SaveRecordFlag = 0;
            char CopyCmd[150] = {0};
            snprintf(CopyCmd, 150, "cp -rf %s /media/mmcblk0p1/2/", FileName);
            FILE *pp = popen(CopyCmd, "w"); 
            if (!pp) 
                return -1;
            pclose(pp);
        }
    }
    if(Begin == 0)
    {
        Begin = 1;
        RecordBeginTime = CurTime;                                                                
        struct tm tm;
        gmtime_r(&CurTime, &tm);
        memset(FileName, 0, 50);
        snprintf(FileName, 50, "/media/mmcblk0p1/1/%04d%02d%02d%02d%02d%02d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        FpRecord = fopen(FileName, "wb+");
    }
    RecordTime = CurTime - RecordBeginTime;
    if(SDIsExist == 1)
    {
        fwrite(H264_StartCode, 4, 1, FpRecord);
        fwrite(fReceiveBuffer, frameSize, 1, FpRecord);
    }  
    return 0; 
}
void *MonitorThread(void *)
{
    int ret = 0;
    GWSipHandleRecordFile MyGWSipHandleRecordFile;
    while(1)
    {
        ret = MyGWSipHandleRecordFile.GetSDCapability();
        if(ret == 401)
        {
            SDIsExist = 0;
            cout<<"SDCARD doesn't exist."<<endl;
        }  
        else if(ret == 402)  
        {
            SDIsFull = 1;
            cout<<"SDCARD out of disk space."<<endl;
            MyGWSipHandleRecordFile.DelteAgoFile("/media/mmcblk0p1/1");
            MyGWSipHandleRecordFile.DelteAgoFile("/media/mmcblk0p1/2");
            MyGWSipHandleRecordFile.DelteAgoFile("/media/mmcblk0p1/3");
        }
        cout<<"LifeLive555: "<<LifeLive555<<endl;
        LifeLive555--;
        sleep(1);
        if(LifeLive555 < 0)
        {
            if(creat("/opt/app/ram/videolost.txt",0777)<0)
            {
                cout<<"create file videolost.txt failure!"<<endl;
            }
            if(IsConnect == 1)
            {
                shutdownStream(rtspClient);
                IsConnect = 0;
            }
                
            eventLoopWatchVariable = 1;
            LifeLive555 = 10;
        }  
    }  
}



