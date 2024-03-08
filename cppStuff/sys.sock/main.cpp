#include "CliSock.cpp"
#include "SrvSock.cpp"

#include <iostream>
#include <thread>
#include <unistd.h>

void Server(){
    char ip[]="127.0.0.1";
    SrvSock s(ip,50500);
    #ifdef NOT_THREADEDSERVER
        s.runServer();
    #else
        s.runThreadServer();
    #endif
}

void Client(char m[]){
    char ip[]="127.0.0.1";
    CliSock c(ip,50500);
    std::cout << "Sending stuff...\n";
    c.snd(m);
    usleep(100000);
    std::cout << "Cli out!\n";
}


int main(int argc, char const *argv[]){

    std::cout<<"Fuck you <3\n";
    
    std::cout << "ServerUp!\n";
    std::thread s(Server);
    usleep(500000);
    std::cout << "ClientUp!\n";
    Client("Hello There!");

    usleep(500000);
    std::cout << "Sending Shutdown!\n";
    Client("!gokys");

    s.join();


}
