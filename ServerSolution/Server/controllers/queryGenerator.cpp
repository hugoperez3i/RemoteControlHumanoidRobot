#include "queryGenerator.h"

QueryGenerator::QueryGenerator(/* args */){
}

QueryGenerator::~QueryGenerator(){
}

std::string QueryGenerator::ack(uint8_t code){
    std::string q = "!s-_ACK:0-e!";
    q.replace(8,1,1,code);
    return q;
}
std::string QueryGenerator::nack(uint8_t code){
    std::string q = "!s-NACK:0-e!";
    q.replace(8,1,1,code);
    return q;
}
std::string QueryGenerator::servoPos(uint32_t flag, std::vector<uint8_t> pos){
    /* [!s]-[SRVP]-[number of servos to update]-[servoid:servopos~servoid:servopos]-[e!] */
    /* [!s-]0-2 (3) Header [xxxx]3-6 (4) Type of Query [x]8(1) Number of servos [servoInfo]10-x((4*NumOfServos)-1) */
    std::string q = "!s-SRVP-0-";
    uint8_t count=0;
    for (size_t i = 0; i < 27; i++){
        if(flag&(0b1<<i)==(0b1<<i)){
            count++;
            q+=(i+1);q+=":";q+=pos[i];q+="-";
        }
    }
    q.replace(8,1,1,count);
    q+="e!";
    return q;
}

/* MCU functions Query[Server -> MCU] */

/* -- Query format --

    -<m/u/e/c>-s:<id>-p:<0-180>-!

        m: move servo (Update a desired position and move the servo)
        u: update servo positon (Update a desired position, dont move)
        e: move all servos to desired positions (No update)
        c: Set current servo positions (get the current positions from srv)
        -s:<id> : Servo to be moved/updated
        -p:<0-180> : Position the servo should be updated/moved to

*/

/* Support function for Dumb functions */

std::string QueryGenerator::divideIntoBytes(uint16_t u){
    std::string hl = "";
    hl+=(uint8_t)(u>>8);
    hl+=(uint8_t)(u&0x00ff);
    return hl;
}

/* Dumb functions - MCU*/

std::string QueryGenerator::dmb_mvServo(uint32_t flag, std::vector<uint8_t> pos, std::vector<std::vector<uint16_t>> minmax){
    std::string q = "-m-0-";
    uint8_t count=0;
    for (size_t i = 0; i < pos.size(); i++){
        if((flag&(0b1<<i))==(0b1<<i)){
            count++;
            q+=(i+1);q+=":";q+=divideIntoBytes((uint16_t) (1+(0b1<<15)+minmax[0][i]+(pos[i]-1)*(((minmax[1][i]-minmax[0][i])/180))));q+="-";
        }
    }
    q+="!";
    q.replace(3,1,1,count);
    return q;
}

/* Smart functions - MCU */

/* -m-<ServoCount>-<ServoId1>:<ServoPos>-<ServoId2>:<ServoPos>[-...]-! */
std::string QueryGenerator::smrt_mvServo(uint32_t flag,std::vector<uint8_t> pos){
    std::string q = "-m-0-";
    uint8_t count=0;
    for (size_t i = 0; i < pos.size(); i++){
        if((flag&(0b1<<i))==(0b1<<i)){
            count++;
            q+=(i+1);q+=":";q+=pos[i];q+="-";
        }
    }
    q+="!";
    q.replace(3,1,1,count);
    return q;
}

std::string QueryGenerator::smrt_updtServo(uint32_t flag,std::vector<uint8_t> pos){
    std::string q = "-u-0-";
    uint8_t count=0;
    for (size_t i = 0; i < pos.size(); i++){
        if((flag&(0b1<<i))==(0b1<<i)){
            count++;
            q+=(i+1);q+=":";q+=pos[i];q+="-";
        }
    }
    q+="!";
    q.replace(3,1,1,count);
    return q;
}
std::string QueryGenerator::smrt_currPos(){
    std::string q = "-c-s:";
    return q;
}