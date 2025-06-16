#include "secrets.h"
#include "servoCalibration.h"

#define FORMAT_LEN 6
#define SERVOPAIR_LEN 4
#define SERVOCOUNT_OFFSET 3
#define SERVOID_OFFSET 5
#define SERVOPOS_OFFSET 7

#define ENABLE_TRACES

#include <Arduino.h>

#include <WiFi.h>
// #include <WiFiClientSecure.h>

#include <Adafruit_PWMServoDriver.h>

WiFiClient client;
String query = "";
uint8_t i = 0;
uint8_t cServoID = 0, cServoPos=0;
uint32_t flag = 0;
uint16_t pwm=0;

char loginQuery[82];

Adafruit_PWMServoDriver pwm0 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x42);

inline uint8_t getServoID(){return (query[SERVOID_OFFSET+(i*SERVOPAIR_LEN)] -1);}
inline uint8_t getServoPos(){return (query[SERVOPOS_OFFSET+(i*SERVOPAIR_LEN)] -1);}

void handleQuery(){

  if(!(query.startsWith("-")&&query.endsWith("-!"))){
    client.print("NACK-0"); /* Invalid query format */
    return;
  }
  switch(query.charAt(1)){
    case 'm': /* Move servos */
      #ifdef ENABLE_TRACES
        Serial.println("At servo movement execution");
      #endif
      
      if(query.length()<(FORMAT_LEN+SERVOPAIR_LEN*query[SERVOCOUNT_OFFSET])){client.print("NACK-1");break;} /* Would be Out of bounds */
      if(query[SERVOCOUNT_OFFSET]>SERVOCOUNT){client.print("NACK-2");break;} /* Servocounnt missmatch */
      for(i=0;i<query[SERVOCOUNT_OFFSET];i++){

        cServoID=getServoID(); cServoPos=getServoPos();
        pwm = minServoPos[cServoID] + ((float)cServoPos / 179.0f) * (maxServoPos[cServoID] - minServoPos[cServoID]);

        if(cServoID<6){/* LeftHand Servo Controls (PCA9685 board 0) - Servos 0-5 (6) */
          #ifdef ENABLE_TRACES
            Serial.printf("pwm0: ServoID -> %u, Position -> %u, PWM signal -> %u, min-maxPWM -> (%u,%u)\n", cServoID, cServoPos, pwm, minServoPos[cServoID], maxServoPos[cServoID]);
          #endif

          pwm0.setPWM(cServoID,0,pwm); 
          delayMicroseconds(40);
				}else if(cServoID<12){ /* RightHand Servo Controls (PCA9685 board 2) - Servos 6-11 (6) */
          #ifdef ENABLE_TRACES
            Serial.printf("pwm2: ServoID -> %u, OnBoardId -> %u, Position -> %u, PWM signal -> %u, min-maxPWM -> (%u,%u)\n", cServoID, (cServoID-6), cServoPos, pwm, minServoPos[cServoID], maxServoPos[cServoID]);
          #endif

          pwm2.setPWM((cServoID-6),0,pwm); 
          delayMicroseconds(40);
				}else{ /* Body/Head Servo Controls (PCA9685 board 1) - Servos 12-26 (15) */
          #ifdef ENABLE_TRACES
            Serial.printf("pwm1: ServoID -> %u, OnBoardId -> %u, Position -> %u, PWM signal -> %u, min-maxPWM -> (%u,%u)\n", cServoID, (cServoID-12), cServoPos, pwm, minServoPos[cServoID], maxServoPos[cServoID]);
          #endif

          pwm1.setPWM((cServoID-12),0,pwm);        
          delayMicroseconds(40);   
				}
      }
      client.print("_ACK");
      break;
    case 'u': /* Update target positions (For delayed mode) */
      #ifdef ENABLE_TRACES
        Serial.println("At Update servo positions");
      #endif

      if(query.length()<(FORMAT_LEN+SERVOPAIR_LEN*query[SERVOCOUNT_OFFSET])){client.print("NACK-1");break;} /* Would be Out of bounds */
      if(query[SERVOCOUNT_OFFSET]>SERVOCOUNT){client.print("NACK-2");break;} /* Servocounnt missmatch */
      for(i=0;i<query[SERVOCOUNT_OFFSET];i++){
        cServoID=getServoID();
        flag|=(0b1<<cServoID);
        targetServoPos[cServoID]=getServoPos();

        #ifdef ENABLE_TRACES
          Serial.printf("ServoID -> %u, Position -> %u\n", cServoID, targetServoPos[cServoID]);
        #endif       
      }
      client.print("_ACK");
      break;
    case 'e': /* Execute all movements (Delayed mode) */
      #ifdef ENABLE_TRACES
        Serial.println("Executing loaded movements");
      #endif
      
      if(flag==0){client.print("NACK-3");break;} /* No movements loaded - Should never happen but still */
      for(i=0;i<SERVOCOUNT;i++){
        if((flag&(0b1<<i))==((0b1<<i))){

          if(i<6){/* LeftHand Servo Controls (pwm board 0) - Servos 0-5 (6) */
          // pwm0.setPWM(cServoID,0,minServoPos[cServoID] + (getServoPos() * ((maxServoPos[cServoID]-minServoPos[cServoID])/180)) ); 
          }else if(i<12){ /* RightHand Servo Controls (pwm board 2) - Servos 6-11 (6) */
          // pwm2.setPWM((cServoID-6),0,minServoPos[cServoID] + (getServoPos() * ((maxServoPos[cServoID]-minServoPos[cServoID])/180)) );   
          }else{ /* Body/Head Servo Controls (pwm board 1) - Servos 12-26 (15) */
          // pwm1.setPWM((cServoID-12),0,minServoPos[cServoID] + (getServoPos() * ((maxServoPos[cServoID]-minServoPos[cServoID])/180)) );   
          }
            
          delayMicroseconds(40);
        }
      }
      flag=0;
      client.print("_ACK");
      break;
    default:
      client.print("NACK-5"); /* Invalid query format */
      break;
  }

}

void setup() {
  Serial.begin(9600);

  // WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSSW);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    #ifdef ENABLE_TRACES
      Serial.print("Stuck at wifi connection: ");
      Serial.println(WiFi.waitForConnectResult());
      // WiFi.reconnect();
    #endif
    delay(1000);
  }

  pwm0.begin();
  pwm0.setOscillatorFrequency(OSC_FREQ);
  pwm0.setPWMFreq(SERVO_FREQ); 

  pwm1.begin();
  pwm1.setOscillatorFrequency(OSC_FREQ);
  pwm1.setPWMFreq(SERVO_FREQ); 

  pwm2.begin();
  pwm2.setOscillatorFrequency(OSC_FREQ);
  pwm2.setPWMFreq(SERVO_FREQ); 

  strcpy(loginQuery, "!s-NodeMCU_here-Maroon-\x1B-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-e!");
    for (size_t cnt = 0; cnt < 27; cnt++){
      loginQuery[25+2*cnt]=(char)(restingServoPos[cnt] +1);
    }
  
}

void loop() {
  /* Check server connection - reconnect if necessary, and set servos to "resting position" */
  if(!client.connected()){
    if (!client.connect(SERVER_IP, SERVER_PORT)){      
      #ifdef ENABLE_TRACES
        Serial.println("Connection to server failed");
      #endif
      delay(1000);
      return;
    }else{
      /* Say hello to server and stop reseting the connection */
      #ifdef ENABLE_TRACES
        Serial.println("Connecting to server...");
      #endif

      delay(1000); /* Dont DoS the server */
      client.print(loginQuery);

      #ifdef ENABLE_TRACES
        Serial.println("Setting up servos...");
      #endif

      for(i=0;i<SERVOCOUNT;i++){

        cServoID=i;
        cServoPos=restingServoPos[i];
        pwm = minServoPos[cServoID] + ((float)cServoPos / 179.0f) * (maxServoPos[cServoID] - minServoPos[cServoID]);

        if(cServoID<6){/* LeftHand Servo Controls (PCA9685 board 0) - Servos 0-5 (6) */
          #ifdef ENABLE_TRACES
            Serial.printf("pwm0: ServoID -> %u, Position -> %u, PWM signal -> %u, min-maxPWM -> (%u,%u)\n", cServoID, cServoPos, pwm, minServoPos[cServoID], maxServoPos[cServoID]);
          #endif

          pwm0.setPWM(cServoID,0,pwm); 
				}else if(cServoID<12){ /* RightHand Servo Controls (PCA9685 board 2) - Servos 6-11 (6) */
          #ifdef ENABLE_TRACES
            Serial.printf("pwm2: ServoID -> %u, OnBoardId -> %u, Position -> %u, PWM signal -> %u, min-maxPWM -> (%u,%u)\n", cServoID, (cServoID-6), cServoPos, pwm, minServoPos[cServoID], maxServoPos[cServoID]);
          #endif

          pwm2.setPWM((cServoID-6),0,pwm); 
				}else{ /* Body/Head Servo Controls (PCA9685 board 1) - Servos 12-26 (15) */
          #ifdef ENABLE_TRACES
            Serial.printf("pwm1: ServoID -> %u, OnBoardId -> %u, Position -> %u, PWM signal -> %u, min-maxPWM -> (%u,%u)\n", cServoID, (cServoID-12), cServoPos, pwm, minServoPos[cServoID], maxServoPos[cServoID]);
          #endif

          pwm1.setPWM((cServoID-12),0,pwm);           
				}

        delayMicroseconds(10);
      }
      #ifdef ENABLE_TRACES
        Serial.println("READY");
        Serial.println("Listening for server orders ...");
      #endif
    }
  }
  /* Control loop */
  query="";
  if(client.available()){
    do{
      query+=(char)client.read();
    }while(client.available());

    handleQuery();

  }

}
