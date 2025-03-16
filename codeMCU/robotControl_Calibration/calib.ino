#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define OSC_FREQ 27000000 // The frequency the PCA9685 should use for frequency calculations

Adafruit_PWMServoDriver pwm0 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x42);

/* Min-Max values for servo */
#define SERVOMIN  260 //  71 -> This is the absolute 'minimum' pulse length count (out of 4096) AKA 0
#define SERVOMAX  400 // 455 -> This is the absolute 'maximum' pulse length count (out of 4096) AKA 180º

void setup() {
  Serial.begin(9600);
  Serial.println("Servo range check");

  pwm0.begin();
    pwm0.setOscillatorFrequency(OSC_FREQ);
    pwm0.setPWMFreq(SERVO_FREQ); 

  pwm1.begin();
    pwm1.setOscillatorFrequency(OSC_FREQ);
    pwm1.setPWMFreq(SERVO_FREQ); 

  pwm2.begin();
    pwm2.setOscillatorFrequency(OSC_FREQ);
    pwm2.setPWMFreq(SERVO_FREQ); 

  delay(10);
}

void loop() {

  pwm0.setPWM(0,0,SERVOMIN); 
  delay(1000);

  pwm0.setPWM(0,0,SERVOMAX); 
  delay(5000);

}