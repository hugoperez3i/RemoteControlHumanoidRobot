#include "secrets.h"
#include "servoCalibration.h"

#define FORMAT_LEN 6
#define SERVOPAIR_LEN 4
#define SERVOCOUNT_OFFSET 3
#define SERVOID_OFFSET 5
#define SERVOPOS_OFFSET 7

#include <Arduino.h>

#include <WiFi.h>
// #include <WiFiClientSecure.h>

#include <Adafruit_PWMServoDriver.h>

WiFiClient client;
String query = "";
uint8_t i = 0;
uint8_t cServoID = 0;
uint32_t flag = 0;

Adafruit_PWMServoDriver pwm0 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x42);

inline uint8_t getServoID(){return (query[SERVOID_OFFSET+(i*SERVOPAIR_LEN)] -1);}
inline uint8_t getServoPos(){return (query[SERVOPOS_OFFSET+(i*SERVOPAIR_LEN)] -1);}

void handleQuery(){

  if(!(query.startsWith("-")&&query.endsWith("-!"))){
    client.print("NACK0"); /* Invalid query format */
    return;
  }
  switch(query.charAt(1)){
    case 'm': /* Move servos */
      // Serial.println("Move Servos");
      if(query.length()<(FORMAT_LEN+SERVOPAIR_LEN*query[SERVOCOUNT_OFFSET])){client.print("NACK1");break;} /* Would be Out of bounds */
      if(query[SERVOCOUNT_OFFSET]>SERVOCOUNT){client.print("NACK2");break;} /* Servocounnt missmatch */
      for(i=0;i<query[SERVOCOUNT_OFFSET];i++){
        cServoID=getServoID();

        if(cServoID<6){/* LeftHand Servo Controls (PCA9685 board 0) - Servos 0-5 (6) */
          Serial.println("pwm0: ServoID->"+String(cServoID)+", Position->"+String((uint8_t)(getServoPos())));
          // pwm0.setPWM(cServoID,0, getServoPos() * ((maxServoPos[cServoID]-minServoPos[cServoID])/180) ); 
				}else if(cServoID<12){ /* RightHand Servo Controls (PCA9685 board 2) - Servos 6-11 (6) */
          Serial.println("pwm2: ServoID->"+String(cServoID)+", Position->"+String((uint8_t)(getServoPos())));
          // pwm2.setPWM((cServoID-6),0, getServoPos() * ((maxServoPos[cServoID]-minServoPos[cServoID])/180) );   
				}else{ /* Body/Head Servo Controls (PCA9685 board 1) - Servos 12-26 (15) */
          Serial.println("pwm1: ServoID->"+String(cServoID)+", Position->"+String((uint8_t)(getServoPos())));
          // pwm1.setPWM((cServoID-12),0, getServoPos() * ((maxServoPos[cServoID]-minServoPos[cServoID])/180) );   
				}

        delayMicroseconds(10);
      }
      client.print("ACK");
      break;
    case 'u': /* Update target positions (For delayed mode) */
      // Serial.println("Update positions");
      if(query.length()<(FORMAT_LEN+SERVOPAIR_LEN*query[SERVOCOUNT_OFFSET])){client.print("NACK1");break;} /* Would be Out of bounds */
      if(query[SERVOCOUNT_OFFSET]>SERVOCOUNT){client.print("NACK2");break;} /* Servocounnt missmatch */
      for(i=0;i<query[SERVOCOUNT_OFFSET];i++){
        cServoID=getServoID();
        flag|=(0b1<<cServoID);
        targetServoPos[cServoID]=getServoPos();
        Serial.println("Servoid:"+String(cServoID)+"Pos:"+String(targetServoPos[cServoID]));
      }
      client.print("ACK");
      break;
    case 'e': /* Execute all movements (Delayed mode) */
      Serial.println("Execute movements");
      if(flag==0){client.print("NACK3");break;} /* No movements loaded - Should never happen but still */
      for(i=0;i<SERVOCOUNT;i++){
        if((flag&(0b1<<i))==((0b1<<i))){

          if(i<6){/* LeftHand Servo Controls (pwm board 0) - Servos 0-5 (6) */
            // pwm2.setPWM(i,0, getServoPos() * ((maxServoPos[i]-minServoPos[i])/180) );   
          }else if(i<12){ /* RightHand Servo Controls (pwm board 2) - Servos 6-11 (6) */
            // pwm0.setPWM((i-6),0, getServoPos() * ((maxServoPos[i]-minServoPos[i])/180) );   
          }else{ /* Body/Head Servo Controls (pwm board 1) - Servos 12-26 (15) */
            // pwm1.setPWM((i-12),0, getServoPos() * ((maxServoPos[i]-minServoPos[i])/180) );   
          }
            
          delayMicroseconds(10);
        }
      }
      flag=0;
      client.print("ACK");
      break;
    default:
      client.print("NACK5"); /* Invalid query format */
      break;
  }

}

void setup() {
  Serial.begin(9600);

  // WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSSW);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.print("Stuck at wifi connection: ");
    Serial.println(WiFi.waitForConnectResult());
    // WiFi.reconnect();
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
}

void loop() {
  /* Check server connection - reconnect if necessary */
  if(!client.connected()){
    if (!client.connect(SERVER_IP, SERVER_PORT)){
      Serial.println("Connection to server failed");
      delay(1000);
      return;
    }else{
      /* Say hello to server and stop reseting the connection */
      Serial.println("Connecting to server...");
      client.print("!s-NodeMCU_here-Maroon-\x1B-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-\1-e!");
      Serial.println("Setting up servos...");
      for(i=0;i<SERVOCOUNT;i++){
        if(i<6){/* LeftHand Servo Controls (PCA9685 board 0) - Servos 0-5 (6) */
          // pwm0.setPWM(i,0,restingServoPos[i]);   
        }else if(i<12){ /* RightHand Servo Controls (PCA9685 board 2) - Servos 6-11 (6) */
          // pwm2.setPWM((i-6),0,restingServoPos[i]);   
        }else{ /* Body/Head Servo Controls (PCA9685 board 1) - Servos 12-26 (15) */
          // pwm1.setPWM((i-12),0,restingServoPos[i]);   
        }
        delayMicroseconds(10);
      }
      Serial.println("READY");
    }
  }
  /* Control loop */
  query="";
  if(client.available()){
    do{
      query+=(char)client.read();
    }while(client.available());
    // Serial.println(query);
    handleQuery();

  }

}
