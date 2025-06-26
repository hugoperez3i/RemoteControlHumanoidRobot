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

    std::cout << "Sending hello query\n";
    // std::string str = "!s-NodeMCU_here-dumbMCU-1-x-e!";
    //     str.replace(24, 1, 1, 6);
    //     str.replace(26, 1, 1, 187);
    std::string str = "!s-NodeMCU_here-SmartMCU-6-0-1-2-3-4-5-e!";
        str.replace(25, 1, 1, 6);
        str.replace(27, 1, 1, 25);
        str.replace(29, 1, 1, 25);
        str.replace(31, 1, 1, 25);
        str.replace(33, 1, 1, 25);
        str.replace(35, 1, 1, 25);
        str.replace(37, 1, 1, 25);

    c.snd(str.data());
    char* q;
    std::string ack = "!s-_ACK-c-e!";
        ack.replace(8,1,1,255);
    while(true){
        q=c.rcv();
        c.snd(ack.data());
        //    usleep(500000); // Wait .5 seconds
    }

}
