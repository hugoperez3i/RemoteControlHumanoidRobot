#include "serverLogic.h"
#include <cstring>

inline bool checkHeader(std::string query){
    return (query.length()>=(TAILLEN+HEADERLEN+CODELEN) ? (query.compare(0,HEADERLEN,HEADERTXT)==0 && query.compare(query.length()-TAILLEN,TAILLEN,TAILTXT)==0) : false);
}

std::string serverLogic::dispatchEMOD(uint8_t eMOD,ControllerInfo* c){
    if(eMOD==_eMOD_Delayed){(*c).updateOnRealTime=false;return QueryGenerator().ack(_eMOD_Delayed);}
    if(eMOD==_eMOD_RealTime){(*c).updateOnRealTime=true;return QueryGenerator().ack(_eMOD_RealTime);}
    return QueryGenerator().nack(_NACK_InvalidParameter); 
}

std::string serverLogic::dispatchSRVP(char* bquery,ControllerInfo* c){

    if((*c).mcuInfo.mcuName.empty()){return QueryGenerator().nack(_NACK_NoActiveMCU);}
    if(!srvCore::isMCUOnline((*c).mcuInfo.mcuName.c_str())){return QueryGenerator().nack(_NACK_MCUOffline);}

    std::string query = "";
    query.clear();
    query.append(bquery);

    /* [!s]-[SRVP]-[number of servos to update]-[servoid:servopos~servoid:servopos]-[e!] */
    /* [!s-]0-2 (3) Header [xxxx]3-6 (4) Type of Query [x]8(1) Number of servos [servoInfo]10-x((4*NumOfServos)-1) */
    /* Servoid and servoposition are sent with a +1 offset as a zero would mean end of stream and cause issues */

    uint8_t numServ=query.at(8);
    uint8_t tmpID=0,tPos=0;
    if((*c).mcuInfo.servoCount<numServ){return QueryGenerator().nack(_NACK_ServoCountMissmatch);}
    if((*c).mcuInfo.servos_MIN_MAX.empty() && (!(*c).mcuInfo.smartMCU)){return QueryGenerator().nack(_NACK_NoMCUInfo);}
    query=query.substr(10,(4*numServ)-1);
    for (size_t i = 0; i < numServ; i++){
        tmpID=query.at(i*4)-1;
        tPos=query.at(2+i*4);
        if(tmpID>(*c).mcuInfo.servoCount || tPos>181){return QueryGenerator().nack(_NACK_InvalidParameter);}
        (*c).mcuInfo.updateFlag|=(0b1<<tmpID); 
        (*c).mcuInfo.targetPositions[tmpID]=tPos;
    }
    
    if((*c).mcuInfo.updateFlag==0){return QueryGenerator().nack(_NACK_InvalidParameter);}
    std::string rq="ACK";

    if((*c).updateOnRealTime){
        
        /* RealTime mode */
        if((*c).mcuInfo.smartMCU){
            rq=srvCore::contactMCU((*c).mcuInfo.mcuName.c_str(),QueryGenerator().smrt_mvServo((*c).mcuInfo.updateFlag,(*c).mcuInfo.targetPositions));
        }else{
            rq=srvCore::contactMCU((*c).mcuInfo.mcuName.c_str(),QueryGenerator().dmb_mvServo((*c).mcuInfo.updateFlag,(*c).mcuInfo.targetPositions,(*c).mcuInfo.servos_MIN_MAX));
        }

    }else{
        /* Non-RT/Delayed mode */

        if((*c).mcuInfo.smartMCU){
            rq=srvCore::contactMCU((*c).mcuInfo.mcuName.c_str(),QueryGenerator().smrt_updtServo((*c).mcuInfo.updateFlag,(*c).mcuInfo.targetPositions));
            // (*c).mcuInfo.updateFlag=0;
        }    
    }

    if(strcmp(&rq[0],"ACK")){return QueryGenerator().nack(_NACK_ErrorContactingMCU);}

    if((*c).updateOnRealTime){
        for(size_t i = 0; i < (*c).mcuInfo.servoCount; i++){
            if((((*c).mcuInfo.updateFlag & (0b1<<i)))==(0b1<<i)){
                (*c).mcuInfo.servoPositions[i]=(*c).mcuInfo.targetPositions[i];
            }
        }
        (*c).mcuInfo.updateFlag=0;
    }
    
    DBMAN::saveMCUInfo((*c).mcuInfo);
    
    return QueryGenerator().ack(_ACK_Generic);/* ACK query back to client*/    
}

std::string serverLogic::dispatchMALL(ControllerInfo* c){
    if((*c).mcuInfo.mcuName.empty()){return QueryGenerator().nack(_NACK_NoActiveMCU);}
    if(!srvCore::isMCUOnline((*c).mcuInfo.mcuName.c_str())){return QueryGenerator().nack(_NACK_MCUOffline);}

    std::string query="";

    if((*c).updateOnRealTime){return QueryGenerator().nack(_NACK_OnRTMode);}

    if((*c).mcuInfo.smartMCU){
        /* Smart mode */
        query=srvCore::contactMCU((*c).mcuInfo.mcuName.c_str(),QueryGenerator().smrt_mvAll());
    }else{
        /* Dumb mode */
        if((*c).mcuInfo.servos_MIN_MAX.empty()){return QueryGenerator().nack(_NACK_NoMCUInfo);}
        if((*c).mcuInfo.updateFlag==0){return QueryGenerator().nack(_NACK_InvalidParameter);}
        query=srvCore::contactMCU((*c).mcuInfo.mcuName.c_str(),QueryGenerator().dmb_mvServo((*c).mcuInfo.updateFlag,(*c).mcuInfo.targetPositions,(*c).mcuInfo.servos_MIN_MAX));
    }
    if(strcmp(&query[0],"ACK")){return QueryGenerator().nack(_NACK_ErrorContactingMCU);}

        for(size_t i = 0; i < (*c).mcuInfo.servoCount; i++){
            if((((*c).mcuInfo.updateFlag & (0b1<<i)))==(0b1<<i)){
                (*c).mcuInfo.servoPositions[i]=(*c).mcuInfo.targetPositions[i];
            }
        }
        (*c).mcuInfo.updateFlag=0;

    DBMAN::saveMCUInfo((*c).mcuInfo);

    return QueryGenerator().ack(_ACK_Generic);
}

std::string serverLogic::dispatchSMCU(char* bquery,ControllerInfo* c){
    std::string query = "";
    query.clear();
    query.append(bquery);

    /* !s-sMCU-[mcuName]-e! */
    query=query.substr(8,query.size()-11);

    (*c).mcuInfo=DBMAN::getMCUInfo(query.data());
    if((*c).mcuInfo.updateFlag!=0){(*c).updateOnRealTime=false;}
    if(!(*c).mcuInfo.mcuName.compare(query)){
        return QueryGenerator().ack(_ACK_Generic);
    }
    return QueryGenerator().nack(_NACK_NoActiveMCU);
}

std::string serverLogic::dispatchUINF(char* bquery,ControllerInfo* c){
    if((*c).mcuInfo.mcuName.empty()){return QueryGenerator().nack(_NACK_NoActiveMCU);}

    std::string query = "";
    query.clear();
    query.append(bquery);

    /* [!s]-[uINF]-[number of servos]-[servomin0:servomax0~servomin0:servomax0]-[e!] */
    /* [!s-]0-2 (3) Header [xxxx]3-6 (4) Type of Query [x]8(1) Number of servos [servoInfo]10-x((6*NumOfServos)-1) */

    uint8_t numServ=query.at(8);
    if((*c).mcuInfo.servoCount<numServ){return QueryGenerator().nack(_NACK_ServoCountMissmatch);}
    
    query=query.substr(10,(6*numServ)-1);
    std::vector<uint16_t> miV,maV;
    
    for (size_t i = 0; i < numServ; i++){
        miV.emplace_back((QueryGenerator().restore16int(query.at(i*6),query.at(1+(i*6))))-1-(0b1<<15));
        maV.emplace_back((QueryGenerator().restore16int(query.at(3+(i*6)),query.at(4+(i*6))))-1-(0b1<<15));
        if((miV[i]<SERVOMIN)||(maV[i]>SERVOMAX)){return QueryGenerator().nack(_NACK_InvalidParameter);}
    }

    (*c).mcuInfo.servos_MIN_MAX.clear();
    (*c).mcuInfo.servos_MIN_MAX.emplace_back(miV);
    (*c).mcuInfo.servos_MIN_MAX.emplace_back(maV);

    if(DBMAN::updateMCUInfo((*c).mcuInfo) == _DBMAN_ERROR){
        (*c).mcuInfo.servos_MIN_MAX.clear();
        return QueryGenerator().nack(_NACK_ErrorLoadingMINMAX);
    }

    return QueryGenerator().ack(_ACK_Generic);
}

int serverLogic::checkLogInQuery(std::string q){
    if(checkHeader(q)){
        if(q.compare(HEADERLEN,12,"NodeMCU_here")==0){return MCUHELLOQUERY;}
        if(q.compare(HEADERLEN,11,"Client_here")==0){return USRHELLOQUERY;}
    }
    return BADQUERY;
}

RobotInformation serverLogic::getQueryInformation(std::string q){
    //#To-Do: Check that servo positions are coherent (Within 1-181 range)

    /* !s-NodeMCU_here-[info]-e! */
    std::string tmp=q.substr(16,q.size()-19); 

    /* [info] mcuName-<servocount>-<pos0>-<pos1>...*/
        // dumbMCU-<servocount>-<187> {!s-NodeMCU_here-dumbMCU-<servocount>-<187>-e!}
    auto i=tmp.find('-');
    if (i == std::string::npos){
        throw std::invalid_argument("Malformed query string.");
    }
    std::string mcuName=tmp.substr(0,i);    

    if(tmp.at(i+3)==(char)187){return RobotInformation(mcuName,tmp.at(i+1),false);}

    std::vector<uint8_t> positions;
    positions.reserve(tmp.at(i+1));
    for (size_t j = 0; j < tmp.at(i+1); j++){
        positions.emplace_back(tmp.at(i+3+(2*j)));
    }

    return RobotInformation(mcuName,tmp.at(i+1),positions,true);
}

void serverLogic::handleQuery(std::string q, ControllerInfo* c){

    std::string qr = QueryGenerator().nack(_NACK_InvalidQuery);
    if(checkHeader(q)){

        char* code = new char[5];
        memcpy(code,q.data()+3,4);
        code[4]=0;
        //Retrocompatibility with firebase client
        if(!strcmp(code,"SRVP")){delete code;qr=dispatchSRVP(q.data(),c);}
            /* !s-SRVP-<Number of servos to update>-<servoid>:<position>-e! -> [!s][SRVP][number of servos to update][servoid:servopos~servoid:servopos][e!] */
        if(!strcmp(code,"eMOD")){delete code;qr=dispatchEMOD(q.at(PARAM1_POSITION),c);}
            /* !s-eMOD-[_eMOD_Delayed/_eMOD_RealTime]-e! ~ Used to change from Realtime updates to delayed mode */
        if(!strcmp(code,"mALL")){delete code;qr=dispatchMALL(c);} 
            /* !s-mALL-e! ~ IF on delayed mode, execute target movements */

        //New functionality
        if(!strcmp(code,"sMCU")){delete code;qr=dispatchSMCU(q.data(),c);}
            /* !s-sMCU-mcuName-e! ~ Swap active MCU */
        // if(!strcmp(code,"iMCU")){delete code;/* Get MCU info */;return 0;} #To-Do
            /* !s-iMCU-e! ~ Get MCU info */
        if(!strcmp(code,"uINF")){delete code;qr=dispatchUINF(q.data(),c);}
            /* !s-uINF-[data]-e! ~ Upload MCU info (ServoMin/ServoMax PWM parameters) */

        // Server Shutdown
        if(!strcmp(code,"sOFF")){delete code;srvCore::srvUp=false;qr=QueryGenerator().ack(_ACK_Generic);}
            /* !s-sOFF-e! ~ Shutdown server */
    }
    
    send((*c).controllerSCK,qr.c_str(),qr.size(), 0);
    return;
}