#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#ifndef RCV_BF_LEN
#define RCV_BF_LEN 1492
#endif
class CliSock{
    private:
        static int i;
        SOCKET sck;
        sockaddr_in srvAddr;
        char rcvB[RCV_BF_LEN];
        char* s;
        WSADATA wsaData;        
    public:
        CliSock(char* ipAddress, int port);
        ~CliSock();
        void snd(char* Message);
        char* rcv();
        void clearString();
};

int CliSock::i=0;

CliSock::CliSock(char* ipAddress, int port){
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=NO_ERROR){WSACleanup();std::cout <<"WSA_Error";return;}
    sck = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sck==INVALID_SOCKET){WSACleanup();std::cout <<"Socket_Error";return;}
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(ipAddress);
    srvAddr.sin_port = htons(port);
    if(connect(sck,(SOCKADDR *) &srvAddr,sizeof srvAddr) == SOCKET_ERROR){closesocket(sck);sck = INVALID_SOCKET;WSACleanup();std::cout <<"Connection_Error";return;}
}   

CliSock::~CliSock(){
    shutdown(sck,SD_BOTH);
    closesocket(sck);
    WSACleanup();
}

void CliSock::snd(char* Message){
    if(send(sck,Message,(int)strlen(Message), 0)==SOCKET_ERROR){std::cout <<"SendError";}
}

char* CliSock::rcv(){
    do{
        i=recv(sck,rcvB,RCV_BF_LEN,0);
        strcat(s,rcvB);
        memset(rcvB, 0, sizeof rcvB);
        if(i==0){std::cout <<"ReceiveError";}
    } while (i==RCV_BF_LEN && WSAGetLastError()==WSAEMSGSIZE);
    return s;
}

void CliSock::clearString(){
    memset(s, 0, sizeof s);
    s=nullptr;
}