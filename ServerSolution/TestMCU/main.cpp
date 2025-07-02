#include "CliSock.h"

#include <iostream>
#include <thread>
#include <unistd.h>
#include <string>
#include <mutex>

int main(int argc, char const *argv[]){

    char ip[]="192.168.137.1";
    // char ip[]="192.168.1.164";
    CliSock c(ip,54817);
    char loginQuery[82];

    std::cout << "Sending hello query\n";
    // std::string str = "!s-NodeMCU_here-dumbMCU-1-x-e!";
    //     str.replace(24, 1, 1, 6);
    //     str.replace(26, 1, 1, 187);
    // std::string str = "!s-NodeMCU_here-SmartMCU2-8-0-1-2-3-4-5-6-7-e!";
    //     str.replace(26, 1, 1, 8);
    //     str.replace(28, 1, 1, 25);
    //     str.replace(30, 1, 1, 25);
    //     str.replace(32, 1, 1, 25);
    //     str.replace(34, 1, 1, 25);
    //     str.replace(36, 1, 1, 25);
    //     str.replace(38, 1, 1, 25);
    //     str.replace(40, 1, 1, 25);
    //     str.replace(42, 1, 1, 25);

    strcpy(loginQuery, "!s-NodeMCU_here-Maroon-\x1B-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-e!");
    for (size_t cnt = 0; cnt < 27; cnt++){
      loginQuery[25+2*cnt]=(char)25;
    }

    // c.snd(str.data());
    c.snd(loginQuery);
    char* q;
    std::string ack = "!s-_ACK-c-e!";
        ack.replace(8,1,1,255);
    while(true){
        q=c.rcv();
        c.snd(ack.data());        
           usleep( (rand()%6)* 50000); // Wait 0-250 ms
    }

}
