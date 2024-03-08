

#include <iostream>
#include "CliSock.cpp"
#include "SrvSock.cpp"
#include <thread>

void Server(){
    char ip[]="127.0.0.1";
    SrvSock s(ip,50500);
    s.runThreadServer();
}

void Client(char m[]){
    char ip[]="127.0.0.1";
    CliSock c(ip,50500);
    std::cout << "Sending stuff...\n";
    c.snd(m);
    Sleep(100);
    std::cout << "Cli out!\n";
}

int main(int argc, char const *argv[]){

    std::cout << "ServerUp!\n";
    std::thread s(Server);
    Sleep(500);
    std::cout << "ClientUp!\n";
    Client("Hello There!");

    Sleep(2500);
    std::cout << "Sending Shutdown!\n";
    Client("!gokys");

    s.join();

}

