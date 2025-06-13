
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
        usleep(2500000); // Wait 2.5 seconds

    /* Change from RT(default) to delayed mode */
    // std::cout<<c.setDelayedMode(true) <<"\n";
    // std::cout<<c.setDelayedMode(false) <<"\n";

    /* Execute movements - Delayed mode only */
    // std::cout<<c.executeMovements() <<"\n";

    /* Combined movement 1 - Rise hand towards face, tilt head */
    std::vector<int> pV1;{
        flag=0;
        pV1.push_back(0); // rotate palm towards face
        flag = (flag | (1<<13)); 
        pV1.push_back(0); // flex arm towards face
        flag = (flag | (1<<15)); 
        pV1.push_back(100); // Rotate arm towards body
        flag = (flag | (1<<17)); 
        pV1.push_back(179); // Tilt head towards hand
        flag = (flag | (1<<25)); 
        pV1.push_back(141); // Rotate head towards hand
        flag = (flag | (1<<26)); 
    }
    std::cout<<c.moveServos(flag,pV1) <<" - Servo movemet comprobation\n";
    std::cout<<c.readServerResponse() <<" - MCU confirmation of movement\n";
        usleep(1500000); // Wait 5 seconds

    /* Combined movement 2 - Open hand */
    // std::vector<int> pV2;{
    //     flag=0;
    //     pV2.push_back(179); // Thumb
    //     flag = (flag | (1<<6)); 
    //     pV2.push_back(179); // Index
    //     flag = (flag | (1<<7)); 
    //     pV2.push_back(179); // Middle
    //     flag = (flag | (1<<8)); 
    //     pV2.push_back(179); // Ring
    //     flag = (flag | (1<<9)); 
    //     pV2.push_back(179); // Little
    //     flag = (flag | (1<<10)); 
    // }
    // std::cout<<c.moveServos(flag,pV2) <<" - Servo movemet comprobation\n";
    // std::cout<<c.readServerResponse() <<" - MCU confirmation of movement\n";
    //     usleep(2500000); // Wait 2.5 second

    /* Combined movement 3 - Close hand */
    std::vector<int> pV3;{
        flag=0;
        pV3.push_back(0); // Thumb
        flag = (flag | (1<<6)); 
        pV3.push_back(0); // Index
        flag = (flag | (1<<7)); 
        pV3.push_back(0); // Middle
        flag = (flag | (1<<8)); 
        pV3.push_back(0); // Ring
        flag = (flag | (1<<9)); 
        pV3.push_back(0); // Little
        flag = (flag | (1<<10));     }
    std::cout<<c.moveServos(flag,pV3) <<" - Servo movemet comprobation\n";
    std::cout<<c.readServerResponse() <<" - MCU confirmation of movement\n";
        usleep(1500000); // Wait 2.5 seconds

    /* Combined movement 4 - rest hand */
    std::vector<int> pV4;{
        flag=0;
        pV4.push_back(90); // Thumb
        flag = (flag | (1<<6)); 
        pV4.push_back(90); // Index
        flag = (flag | (1<<7)); 
        pV4.push_back(90); // Middle
        flag = (flag | (1<<8)); 
        pV4.push_back(90); // Ring
        flag = (flag | (1<<9)); 
        pV4.push_back(90); // Little
        flag = (flag | (1<<10)); 
    }
    std::cout<<c.moveServos(flag,pV4) <<" - Servo movemet comprobation\n";
    std::cout<<c.readServerResponse() <<" - MCU confirmation of movement\n";
        usleep(1500000); // Wait 2.5 seconds
        
    /* Combined movement 5 - Move arm and head back to "resting" position */
    std::vector<int> pV5;{
        flag=0;
        pV5.push_back(90); // rotate palm towards face
        flag = (flag | (1<<13)); 
        pV5.push_back(60); // flex arm towards face
        flag = (flag | (1<<15)); 
        pV5.push_back(70); // Rotate arm towards body
        flag = (flag | (1<<17)); 
        pV5.push_back(126); // Tilt head towards hand
        flag = (flag | (1<<25)); 
        pV5.push_back(100); // Rotate head towards hand
        flag = (flag | (1<<26)); 
    }
    std::cout<<c.moveServos(flag,pV5) <<" - Servo movemet comprobation\n";
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



