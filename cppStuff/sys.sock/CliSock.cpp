#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <cstring> //for memset
#include <iostream>

#define RCV_BF_LEN 1492

class CliSock{
    private:
        static int i;
        int sck;
        sockaddr_in srvAddr;
        char rcvB[RCV_BF_LEN];
        char* s;
        bool initialized;
    public:
        CliSock(char* ipAddress, int port);
        ~CliSock();
        void snd(char* Message);
        char* rcv();
        void clearString();
};

int CliSock::i = 0;

CliSock::CliSock(char* ipAddress, int port){
    initialized = false;
    sck = socket(AF_INET, SOCK_STREAM, 0);
    if(sck == -1){std::cout << "Socket_Error\n";return;}
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(ipAddress);
    srvAddr.sin_port = htons(port);
    if(connect(sck, (sockaddr*) &srvAddr, sizeof srvAddr) == -1){close(sck);std::cout << "Connection_Error\n";return;}
    initialized = true;
}

CliSock::~CliSock(){
    if(initialized){
        shutdown(sck, SHUT_RDWR);
        close(sck);
    }
}

void CliSock::snd(char* Message){
    if(send(sck, Message, strlen(Message), 0) == -1){
        std::cout << "SendError\n";
    }
}

char* CliSock::rcv() {
    memset(s, 0, RCV_BF_LEN);
    do{
        i = recv(sck, rcvB, RCV_BF_LEN, 0);
        strcat(s, rcvB);
        memset(rcvB, 0, sizeof rcvB);
        if(i == 0){std::cout << "ReceiveError\n";}
    }while(i == RCV_BF_LEN);
    return s;
}

void CliSock::clearString(){
    memset(s, 0, sizeof s);
    s=nullptr;
}
