/*
    This code can be used to you own software or just compile to Windows.
    May the unique release to not compile.
    This is just a simple console software to 'test' network conection with a list of pings.
*/

#ifndef UNICODE
#define UNICODE
#endif
#ifdef _MSC_VER
#include <ws2tcpip.h>
#endif
#include <iphlpapi.h>
#include <icmpapi.h>
#include <stdio.h>

// CodeBlocks + Visual Studio[Include+Lib]  o Visual Studio (I use codeblocks with Visual Studio 15+ include and lib)
#ifdef _MSC_VER
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")
#else
//CodeBlocks mingw
#pragma comment(lib, "libIPHLPAPI.a")
#pragma comment(lib, "libws2_32.a")
#endif

#define MALLOC(x)   HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x)     HeapFree(GetProcessHeap(), 0, (x))
#define $TIMES$     2 //num of ping to test ip address.
#define $IPTEST$    5 //num of ip address and name to show.
#define $DATA$      "Data Buffer" //Data send.

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#ifdef _MSC_VER
void colorIt(int x,const wchar_t thisText[50]=L"")
#else
void colorIt(int x,const char* thisText=L"")
#endif
{
    SetConsoleTextAttribute(hConsole, (WORD)x); //set color, comment thist to quit colors
    wprintf(thisText); //text to color.
};

#ifdef _MSC_VER
void ipTest(wchar_t ipAddress[16], int posIP, int color1, int color2, int color3)
#else
void ipTest(char ipAddress, int posIP, int color1, int color2, int color3)
#endif
{
    colorIt(color1);
    wprintf(L"   #%d",posIP + 1);
    colorIt(color2,L" -> ");
    colorIt(color3);
    wprintf(L"%15s ",ipAddress);
};

void pingFail(void)
{
    colorIt(4,L"   [-]");
    colorIt(7,L" (");
    colorIt(6,L"  -- ms ");
    colorIt(7,L")");
};

int doPings(void)
{
#ifdef _MSC_VER //ping to
    wchar_t iplist[$IPTEST$][16] = {L"192.168.0.1",L"192.168.1.1",L"8.8.8.8",L"1.1.1.1",L"208.67.220.220"}; //same num of ip address
    wchar_t nameList[$IPTEST$][16] = { L"Modem",L"Router",L"Google",L"Cloudflare",L"OpenDNS"};              //same num of ip tagname
#else
    char iplist[IPTEST][16] = { "192.168.0.1", "192.168.1.1", "8.8.8.8", "1.1.1.1", "208.67.220.220"};
    char nameList[IPTEST][16] = { "Modem", "Router", "Google", "Cloudflare", "OpenDNS"};
#endif
    int ip=0,ping=0;
    HANDLE hIcmpFile;
    DWORD dwRetVal=0, replySize=0;
    char sendData[32]=$DATA$;
    LPVOID replyBuffer=NULL;
    PICMP_ECHO_REPLY pEchoReply;
    hIcmpFile=IcmpCreateFile();
    if(hIcmpFile == INVALID_HANDLE_VALUE)
    {
        colorIt(12,L"IcmpCreateFile error\n");
        return 1;
    }
    replySize=sizeof(ICMP_ECHO_REPLY)+sizeof(sendData)+8;
    replyBuffer=(VOID *) malloc(replySize);
    if(replyBuffer == NULL)
    {
        colorIt(12,L"Memory Buffer error\n");
        return 1;
    }
    colorIt(15,L"   PING     Address IPV4    Reply 1          Reply 2          Name\n\n"); //set your own try and error
    for(ip=0; ip<$IPTEST$; ip++)
    {
        for(ping=0; ping<$TIMES$; ping++)
        {
#ifdef _MSC_VER
            in_addr AddrBuf;
            InetPton(AF_INET, iplist[ip], &AddrBuf);
            dwRetVal=IcmpSendEcho2(hIcmpFile,NULL,NULL,NULL,AddrBuf.S_un.S_addr,sendData,sizeof(sendData),NULL,replyBuffer,replySize,1000);
#else
            dwRetVal=IcmpSendEcho(hIcmpFile, inet_addr(iplist[i]),sendData,sizeof(sendData),NULL, replyBuffer, replySize, 1000);
#endif

            if(dwRetVal != 0) //ping tryed
            {
                pEchoReply=(PICMP_ECHO_REPLY) replyBuffer; //receive reply
                if (ping == 0) ipTest(iplist[ip],ip,13,7,11);
                if(pEchoReply->Status == 0) //ping sucess
                {
                    colorIt(10,L"   [+]");
                    colorIt(7, L" (");
                    colorIt(14);
                    wprintf(L" %3.2ld ms ", pEchoReply->RoundTripTime);
                    colorIt(7,L")");
                    if(ip < 2) //first 2 set color 15 example white as local
                        colorIt(15);
                    else
                        colorIt(10); //3+ set color 10 example green as internet
                }
                else //ping fail
                {
                    pingFail();
                    if (ping == 1) colorIt(8);
                }
            }
            else //ping fail
            {
                if (ping == 0) ipTest(iplist[ip],ip,5,7,3);
                pingFail();
                if (ping == 1) colorIt(8);
            }
            Sleep(100); //wait before send new ping.
            dwRetVal=NULL, pEchoReply=NULL;
            if (ping == 1) wprintf(L"   %s\n",nameList[ip]);
            colorIt(7);
        };
        if(ip < 7) wprintf(L"\n");
    }
    if(replyBuffer != NULL) replyBuffer=NULL;
    if(hIcmpFile != NULL) IcmpCloseHandle(hIcmpFile), hIcmpFile= NULL;
    return 0;
};

int main(void)
{
    PIP_ADAPTER_INFO pAdapInfo=NULL;
    ULONG ulOutBufLen=sizeof(IP_ADAPTER_INFO);
    pAdapInfo=(IP_ADAPTER_INFO *) MALLOC(sizeof(IP_ADAPTER_INFO));
    if(pAdapInfo == NULL)
    {
        colorIt(12,L"IP_ADAPTER_INFO error on GetAdaptersinfo\n");
        return 1;
    }
    if(GetAdaptersInfo(pAdapInfo,&ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        FREE(pAdapInfo);
        pAdapInfo=(IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
        if(pAdapInfo == NULL)
        {
            colorIt(12,L"GetAdaptersinfo error\n");
            return 1;
        }
    }
    doPings();
    if (pAdapInfo != NULL)
    {
        FREE(pAdapInfo);
        pAdapInfo=NULL;
    }
    return 0;
}
