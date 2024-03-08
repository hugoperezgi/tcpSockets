#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#define NOT_THREADEDSERVER //Comment this line for threaded version instead of selector

#ifndef RCV_BF_LEN
#define RCV_BF_LEN 1492
#endif
class SrvSock{
    private:
        SOCKET sckListen;
        sockaddr_in srvAddr;
        char rcvB[RCV_BF_LEN];
        char* s;
        WSADATA wsaData;        
        FD_SET rSCK,allSCK;
        void cliHandler(SOCKET s);
        void closeSockets();
    public:
        bool srvUp;
        SrvSock(char* ipAddress, int port);
        ~SrvSock();
        void getNewClient();
        void runServer();
        void rmvSock(SOCKET s);

        #ifndef  NOT_THREADEDSERVER
            void runThreadServer();
        #endif
};

SrvSock::SrvSock(char* ipAddress, int port){
    srvUp=true;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=NO_ERROR){WSACleanup();std::cout <<"WSA_Error";}
    sckListen = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sckListen==INVALID_SOCKET){WSACleanup();std::cout <<"Socket_Error";}
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(ipAddress);
    srvAddr.sin_port = htons(port);
    if(bind(sckListen,(SOCKADDR *) &srvAddr,sizeof srvAddr) == SOCKET_ERROR){closesocket(sckListen);sckListen = INVALID_SOCKET;WSACleanup();std::cout <<"Bind_Error";}
    if(listen(sckListen,5)==SOCKET_ERROR){closesocket(sckListen);sckListen = INVALID_SOCKET;WSACleanup();std::cout <<"ListenError";}
        // Use backlog == SOMAXCONN for >>> client connections, else keep low (dont waste resources)
    FD_ZERO(&allSCK);
    FD_SET(sckListen,&allSCK);
}

SrvSock::~SrvSock(){

    closeSockets();
    Sleep(250);
    WSACleanup();

    std::cout<<"Server Out!";
}

void SrvSock::getNewClient(){
    SOCKET s = accept(sckListen,NULL,NULL);
    if(s==INVALID_SOCKET){std::cout << "AcceptError";}
    FD_SET(s,&allSCK);
}

/*this->rmvSock(sck); //Remove the socket if client dc'd*/
void SrvSock::rmvSock(SOCKET s){
    FD_CLR(s,&allSCK);
}

void SrvSock::runServer(){
    srvUp=true;
    int j=0;
    while (srvUp){
        //Fill the sock list that select will modify
        FD_ZERO(&rSCK);
        j=0;
        for (auto &&i : allSCK.fd_array){
            FD_SET(i,&rSCK);
            j++;
            if(j==allSCK.fd_count)break;
        }
        //Give the sck list to select
        switch (select(0,&rSCK,NULL,NULL,NULL)){
            case -1:
                /* Select Error - handle it? */
                break;
            case 0:
                /* Timeout - Shoould never happen as timeout is set to NULL */
                break;
            default:
                /* At least 1 sck is ready to accept/read */
                j=0;
                for (auto &&sck : rSCK.fd_array){
                    //Iterate through all the sockets ready to accept/read

                    FD_CLR(sck,&rSCK);//Remove the sck being handled from the list 

                    if(sck==sckListen){
                        //Main socket - Accept new connection
                        this->getNewClient();
                    }else{
                        //Client socket - Handle it
                        cliHandler(sck);
                    }
                    
                    if(rSCK.fd_count==0)break;
                        //Check whether all rdy sockets were handled
                }

                break;
        }

    }
    

}

void SrvSock::cliHandler(SOCKET s){
    int i;
    std::string str = "";
    memset(rcvB, 0, sizeof rcvB);

    do{
        i=recv(s,rcvB,RCV_BF_LEN,0);
        str+=rcvB;
        memset(rcvB, 0, sizeof rcvB);
        if(i==0){std::cout <<"ConnectionClosed";rmvSock(s);}
        if(i==SOCKET_ERROR){std::cout << WSAGetLastError();rmvSock(s);} 
    } while (i==RCV_BF_LEN && WSAGetLastError()==WSAEMSGSIZE);

    //Shutdown Query
    if(str=="!gokys"){
        this->srvUp=false;
    }else{
        std::cout << str << "\n";
    }

    /* Stuff -> handle your client query */    

}

void SrvSock::closeSockets(){
    int j=0;
    for (auto &&i : allSCK.fd_array){
        
        shutdown(i, SD_RECEIVE);
        send(i,"ServerShutdown",(int)strlen("ServerShutdown"), 0);
        closesocket(i);

        j++;
        if(j==allSCK.fd_count)break;
    }
}

#ifndef  NOT_THREADEDSERVER

    #include <thread>

    void threadHandler(SOCKET s,SrvSock* ss){
        int i;
        std::string str = "";
        char rcvB[RCV_BF_LEN];
        memset(rcvB, 0, sizeof rcvB);

        do{
            i=recv(s,rcvB,RCV_BF_LEN,0);
            str+=rcvB;
            memset(rcvB, 0, sizeof rcvB);
            if(i==0){std::cout <<"ConnectionClosed";}
            if(i==SOCKET_ERROR){std::cout << WSAGetLastError();} 
        } while (i==RCV_BF_LEN && WSAGetLastError()==WSAEMSGSIZE);

        //Shutdown Query
        if(str=="!gokys"){
            (*ss).srvUp=false;
        }else{
            std::cout << str << "\n";
        }

        /* Stuff -> handle your client query */    
    }

    void SrvSock::runThreadServer(){
        timeval timeout;
            timeout.tv_sec=5;
            timeout.tv_usec=0;
        SOCKET ss;
        std::thread t;
        while(srvUp){
            FD_ZERO(&rSCK);
            FD_SET(sckListen,&rSCK);

            if(select(0,&rSCK,NULL,NULL,&timeout)==0){continue;}

            ss = accept(sckListen,NULL,NULL);
            new std::thread(threadHandler,ss,this);
        }
    }


#endif