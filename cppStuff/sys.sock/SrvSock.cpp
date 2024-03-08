#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <cstring> //for memcpy
#include <iostream>

// #define NOT_THREADEDSERVER //Comment this line for threaded version instead of selector

#define RCV_BF_LEN 1492

class SrvSock{
    private:
        int sckListen, nfds;
        sockaddr_in srvAddr;
        char rcvB[RCV_BF_LEN];
        char* s;
        fd_set rSCK,allSCK;
        void cliHandler(int s);
        void closeSockets();
    public:
        bool srvUp;
        SrvSock(char* ipAddress, int port);
        ~SrvSock();
        void getNewClient();
        void runServer();
        void rmvSock(int s);

        #ifndef  NOT_THREADEDSERVER
            void runThreadServer();
        #endif
};

SrvSock::SrvSock(char* ipAddress, int port){
    srvUp=true;
    sckListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sckListen==-1) std::cout << "SocketError\n";
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(ipAddress);
    srvAddr.sin_port = htons(port);
    if(bind(sckListen, (sockaddr*) &srvAddr, sizeof(srvAddr)) == -1){close(sckListen);std::cout << "Bind_Error\n";return;}
    if(listen(sckListen, 5) == -1){close(sckListen);std::cout << "ListenError\n";return;}
    FD_ZERO(&allSCK);
    FD_SET(sckListen,&allSCK);
}

SrvSock::~SrvSock(){
    closeSockets();
    std::cout<<"Server Out!";
}

void SrvSock::getNewClient(){
    int s = accept(sckListen, NULL, NULL);
    if(s == -1){std::cout << "AcceptError\n";return;}
    FD_SET(s, &allSCK);
    nfds++;
}

void SrvSock::rmvSock(int s){
    FD_CLR(s, &allSCK);
    nfds--;
}

void SrvSock::runServer(){
    srvUp = true;
    nfds=1;
    while(srvUp){
        FD_ZERO(&rSCK);
        memcpy(&rSCK,&allSCK,sizeof allSCK);        
        switch(select(nfds+1, &rSCK, NULL, NULL, NULL)){
            case -1:
                std::cout << "Select Error\n";
                break;
            case 0:
                std::cout << "Timeout\n";
                break;
            default:
                for(int s = 0; s < (nfds+1); ++s){
                    if(FD_ISSET(s, &rSCK)){
                        if(s == sckListen){
                            getNewClient();
                        }else{
                            cliHandler(s);
                        }
                    }
                }
                break;
        }
    }
}

void SrvSock::cliHandler(int s){
    int i;
    std::string str = "";
    memset(rcvB, 0, sizeof(rcvB));
    do{
        i=recv(s,rcvB,RCV_BF_LEN,0);
        str+=rcvB;
        memset(rcvB, 0, sizeof(rcvB));
        if(i==0){std::cout << "ConnectionClosed\n";rmvSock(s);close(s);return;}
        if(i==-1){std::cout << "Receive Error\n";rmvSock(s);close(s);return;}
    }while(i==RCV_BF_LEN);

    if(str == "!gokys"){
        srvUp = false;
    }else{
        std::cout << str << "\n";
    }
}

void SrvSock::closeSockets(){
    for (int s = 0; s < (nfds+1); ++s) {
        if(FD_ISSET(s, &allSCK)){
            shutdown(s, SHUT_RD);
            send(s, "ServerShutdown", strlen("ServerShutdown"), 0);
            close(s);
        }
    }
}

#ifndef  NOT_THREADEDSERVER

    #include <thread>

    void threadHandler(int s, SrvSock* ss){
        int i;
        std::string str = "";
        char rcvB[RCV_BF_LEN];
        memset(rcvB, 0, sizeof(rcvB));

        do{
            i = recv(s, rcvB, RCV_BF_LEN, 0);
            str += rcvB;
            memset(rcvB, 0, sizeof(rcvB));
            if(i == 0){std::cout << "ConnectionClosed\n";close(s);}
            if(i==-1){std::cout << "Receive Error\n";close(s);}
        }while(i==RCV_BF_LEN);

        if(str=="!gokys"){
            (*ss).srvUp = false;
        } else{
            std::cout << str << "\n";
        }

        /* Stuff -> handle your client query */
    }

    void SrvSock::runThreadServer(){
        timeval timeout;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;
        int ss;
        while(srvUp){
            FD_ZERO(&rSCK);
            FD_SET(sckListen, &rSCK);

            if(select(sckListen + 1, &rSCK, NULL, NULL, &timeout) == 0){continue;}

            ss = accept(sckListen, NULL, NULL);
            if(ss==-1){std::cout << "AcceptError\n";continue;}
            new std::thread(threadHandler, ss, this);
        }
    }

#endif