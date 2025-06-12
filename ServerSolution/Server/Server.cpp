#include "srvCore.h"
#include "secrets.h"

#include <csignal>

#ifndef SERVER_IP
    #define SERVER_IP "0.0.0.0"
#endif

#ifndef SERVER_PORT
    #define SERVER_PORT 57573
#endif

int main(int argc, char const *argv[]){

    signal(SIGSEGV, srvCore::signalHandler);
    signal(SIGINT,  srvCore::signalHandler);
    signal(SIGTERM, srvCore::signalHandler);

    srvCore s(SERVER_IP,SERVER_PORT);
    s.runServer();
    
    return 0;
}

