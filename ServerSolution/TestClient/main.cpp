
#include "Client.h"

#include <iostream>
#include <thread>
#include <unistd.h>


int main(int argc, char const *argv[]){

    // char ip[]="10.60.125.20";
    char ip[]="192.168.137.1";
    uint32_t flag=0;

    /* Start connection */
    Client c(ip);
        usleep(500000);

    /* Select the mcu to control */
    std::cout<<c.connectToMCU("Maroon") <<"\n";

    /* Change from RT(default) to delayed mode */
    // std::cout<<c.setDelayedMode(true) <<"\n";
    // std::cout<<c.setDelayedMode(false) <<"\n";

    /* Execute movements - Delayed mode only */
    // std::cout<<c.executeMovements() <<"\n";

    /* Move some servos only */
    std::vector<int> pV1;{
        flag=0;
        pV1.push_back(0); // Move servoId=13 to 0º
        flag = (flag | (1<<13)); 
        pV1.push_back(0); // Move servoId=15 to 0º
        flag = (flag | (1<<15)); 
        pV1.push_back(179); // Move servoId=17 to 179º
        flag = (flag | (1<<17)); 
    }
    std::cout<<c.moveServos(flag,pV1) <<" - Servo movemet comprobation\n";
    std::cout<<c.readServerResponse() <<" - MCU confirmation of movement\n";
        usleep(2500000); // Wait 2.5 seconds

    /* Move all servos */
    // std::vector<int> pVA(27,138);
    // std::cout<<c.moveServos(pVA) <<" - Servo movemet comprobation\n";
    // std::cout<<c.readServerResponse() <<" - MCU confirmation of movement\n";
    //     usleep(5000000);
    
    /* Upload Min/Max PWM signal parameters */
    // std::vector<uint16_t> miV(6,75);
    // std::vector<uint16_t> maV(6,450);
    // std::vector<std::vector<uint16_t>>mV;
    //     mV.emplace_back(miV);mV.emplace_back(maV);
    // std::cout<<c.uploadMINMAXinf(mV) <<"\n";

    /* Shutdown server */
    // std::cout<<c.shutdownServer() <<"\n";

}



