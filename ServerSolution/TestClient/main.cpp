
#include "Client.h"

#include <iostream>
#include <thread>
#include <unistd.h>


int main(int argc, char const *argv[]){

    // char ip[]="10.60.125.20";
    char ip[]="192.168.1.58";

    /* Start connection */
    Client c(ip);
        usleep(500000);

    /* Change from RT(default) to delayed mode */
    // std::cout<<c.setDelayedMode(true) <<"\n";
    // std::cout<<c.setDelayedMode(false) <<"\n";

    /* Select the mcu to control */
    std::cout<<c.connectToMCU("Maroon") <<"\n";
    // std::cout<<c.connectToMCU("dumbMCU") <<"\n";

    /* Update all servos */
    std::vector<int> pVA(27,138);
    std::cout<<c.moveServos(pVA) <<"\n";
        usleep(5000000);

    // /* Update some servos only */
    std::vector<int> pV(2,17);
    std::cout<<c.moveServos(6,pV) <<"\n";
    
    /* Execute movements - Delayed mode only */
    // std::cout<<c.executeMovements() <<"\n";

    /* Upload Min/Max PWM signal parameters */
    // std::vector<uint16_t> miV(6,75);
    // std::vector<uint16_t> maV(6,450);
    // std::vector<std::vector<uint16_t>>mV;
    //     mV.emplace_back(miV);mV.emplace_back(maV);
    // std::cout<<c.uploadMINMAXinf(mV) <<"\n";
    
    /* Shutdown server */
    // std::cout<<c.shutdownServer() <<"\n";

}
