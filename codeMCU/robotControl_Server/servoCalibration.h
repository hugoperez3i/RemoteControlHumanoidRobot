#include <stdint.h>

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define OSC_FREQ 27000000 // The frequency the PCA9685 should use for frequency calculations

//Std Max-Min of servos
// #define SERVOMIN  71 // This is the 'minimum' pulse length count (out of 4096) AKA 0
// #define SERVOMAX  455 // This is the 'maximum' pulse length count (out of 4096) AKA 180º

//yellow color on servo // White marking on cable
#define SERVOMIN_Rotation_Biceps_L  71 
#define SERVOMAX_Rotation_Biceps_L  410 
#define SERVOMIN_Codo_L  220 
#define SERVOMAX_Codo_L  455 
#define SERVOMIN_Rotation_Antebrazo_L  71 
#define SERVOMAX_Rotation_Antebrazo_L  400 
//yellow color on servo // White marking on cable

//The other arm
#define SERVOMIN_Rotation_Biceps  90 
#define SERVOMAX_Rotation_Biceps  400 
#define SERVOMIN_Codo  100 
#define SERVOMAX_Codo  330 
#define SERVOMIN_Rotation_Antebrazo  90 
#define SERVOMAX_Rotation_Antebrazo  435 
//The other arm

//Rotation
#define SERVOMIDDLE_Rotation_Head 263 
#define SERVOMAX_Rotation_Head 349 
#define SERVOMIN_Rotation_Head 167
//Tilt With marker
#define SERVOMIDDLE_tiltHead_L 250
#define SERVOMIN_tiltHead_L 75
#define SERVOMAX_tiltHead_L 445
//Tilt NO marker
#define SERVOMIDDLE_tiltHead 260
#define SERVOMIN_tiltHead 75
#define SERVOMAX_tiltHead 455
//Head movements

//Shoulder movements
//Not marked side
#define SERVOMAX_shoulderarm 455 
#define SERVOMIN_shoulderarm 140 
#define SERVOMAX_shoulderbody 455 
#define SERVOMIN_shoulderbody 260 
//marked side
#define SERVOMAX_shoulderarm_L 200 
#define SERVOMIN_shoulderarm_L 440 
#define SERVOMAX_shoulderbody_L 455 
#define SERVOMIN_shoulderbody_L 320 
//Shoulder movements

//Right hand 
// thumb 0, index 1, middle 2, ring 3, little 4, palm 5
#define SERVOMIN_little_R 130 //green
#define SERVOMAX_little_R 370
#define SERVOMIN_ring_R 150 //blue
#define SERVOMAX_ring_R 400
#define SERVOMIN_middle_R 150 //yellow
#define SERVOMAX_middle_R 420 //dedo estirado
#define SERVOMIN_index_R 150 //red
#define SERVOMAX_index_R 400
#define SERVOMIN_thumb_R 100 // no color
#define SERVOMAX_thumb_R 400 
#define SERVOMIN_palm_R 230 //gray
#define SERVOMAX_palm_R 450
//Left Hand
#define SERVOMIN_little_L 170
#define SERVOMAX_little_L 370
#define SERVOMIN_ring_L 170 //blue
#define SERVOMAX_ring_L 370
#define SERVOMIN_middle_L 170 //Yellow
#define SERVOMAX_middle_L 370
#define SERVOMIN_index_L 170 //red
#define SERVOMAX_index_L 370
#define SERVOMIN_thumb_L 210 // no color
#define SERVOMAX_thumb_L 440
#define SERVOMIN_palm_L 280 //mano cerrada
#define SERVOMAX_palm_L 420

//MAX es dedo estirado, min es puño cerrado

//Torso
#define SERVOMIN_torso_L 90 //Max rotation up
#define SERVOMAX_torso_L 450 //Max rotation down
#define SERVOMIDDLE_torso_L 290 //Hold position-Middle position (100)

#define SERVOMIN_torso 90 //Max rotation down
#define SERVOMAX_torso 450 //Max rotation up
#define SERVOMIDDLE_torso 280 //Hold position-Middle position (95)

// Total Number of servos
#define SERVOCOUNT 27

// Servo Information for server control/connection
/** Minimum pulse lenght for the PWM signal of each servo -> minServoPos[servoID] >= 71 */
const uint16_t minServoPos[]={SERVOMIN_thumb_L,SERVOMIN_index_L,SERVOMIN_middle_L,SERVOMIN_ring_L,SERVOMIN_little_L,SERVOMIN_palm_L,
                              SERVOMIN_thumb_R,SERVOMIN_index_R,SERVOMIN_middle_R,SERVOMIN_ring_R,SERVOMIN_little_R,SERVOMIN_palm_R,
                              SERVOMIN_Rotation_Antebrazo_L,SERVOMIN_Rotation_Antebrazo,
                              SERVOMIN_Codo_L,SERVOMIN_Codo,
                              SERVOMIN_Rotation_Biceps_L,SERVOMIN_Rotation_Biceps,
                              SERVOMIN_shoulderarm_L,SERVOMIN_shoulderarm,
                              SERVOMIN_shoulderbody_L,SERVOMIN_shoulderbody,
                              SERVOMIN_torso_L,SERVOMIN_torso,
                              SERVOMIN_tiltHead_L,SERVOMIN_tiltHead,SERVOMIN_Rotation_Head};

/** Maximum pulse lenght for the PWM signal of each servo -> maxServoPos[servoID] <= 455 */
const uint16_t maxServoPos[]={SERVOMAX_thumb_L,SERVOMAX_index_L,SERVOMAX_middle_L,SERVOMAX_ring_L,SERVOMAX_little_L,SERVOMAX_palm_L,
                              SERVOMAX_thumb_R,SERVOMAX_index_R,SERVOMAX_middle_R,SERVOMAX_ring_R,SERVOMAX_little_R,SERVOMAX_palm_R,
                              SERVOMAX_Rotation_Antebrazo_L,SERVOMAX_Rotation_Antebrazo,
                              SERVOMAX_Codo_L,SERVOMAX_Codo,
                              SERVOMAX_Rotation_Biceps_L,SERVOMAX_Rotation_Biceps,
                              SERVOMAX_shoulderarm_L,SERVOMAX_shoulderarm,
                              SERVOMAX_shoulderbody_L,SERVOMAX_shoulderbody,
                              SERVOMAX_torso_L,SERVOMAX_torso,
                              SERVOMAX_tiltHead_L,SERVOMAX_tiltHead,SERVOMAX_Rotation_Head};
/** Target angle the servo should move to -> targetServoPos[servoID] = [0,179] */
uint8_t targetServoPos[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
/** Resting angle position the servo should be initialized to -> restingServoPos[servoID] = [0,179] */
const uint8_t restingServoPos[]={90,90,90,90,90,0, // left hand
                                  90,90,90,90,90,179, //right hand
                                  90,90, // forearm rotation
                                  170,60, // arm flex
                                  110,70, // arm rotation
                                  150,150, // shoulder flex
                                  0,80, // circonduction
                                  100,95, // torso tilt
                                  56,126, // head tilt
                                  100}; // head rot


//Board1 - Left Hand (6 servos) // Board3 - Right Hand (6 servos)
//Board1/3 ids
// thumb 0, index 1, middle 2, ring 3, little 4, palm 5


/* Board2 - Arms,Torso,Head (15 servos) */
//Board2 ids
// forearmRotationL - 0 / armFlexL - 2 / bicepsRotationL - 4 / shoulderArmRotationL - 6 / shoulderTorsoRotationL - 8
// forearmRotationR - 1 / armFlexR - 3 / bicepsRotationR - 5 / shoulderArmRotationR - 7 / shoulderTorsoRotationR - 9
// torsoTiltL - 10 / torsoTiltR - 11
// headTiltL - 13 / headTiltR - 14 / headRotation - 15

/*
HANDS - 0º max flex fingers -> 179º max extension {@90º resting position}
PALM - 0º thumb towards "the outside" -> 179º thumb towards the inside (of the palm) {@180º resting position RIGHT HAND}
PALM - 179º thumb towards "the outside" -> 0º thumb towards the inside (of the palm) {@0º resting position LEFT HAND}

ARM (RIGHT)
  FLEXION -> 180 fully extended, 60º resting position
  SHOULDER tilt -> 0º (T-pose), 150º resting positon
  ARM rotation -> 180º towards the body, 0º outward rotation; 70º resting position
  forearm rotation -> 180 palm down, 0 palm up; 90 resting position

ARM (LEFT)
  FLEXION -> 180º fully contracted, 0º fully extended; 170º resting position
  SHOULDER tilt -> 0º (T-pose), 150º resting positon
  ARM rotation -> 180 outwards, 0 towards body; 100 resting position
  forearm rotation -> 0 palm down, 180 palm up - 90 resting position

TORSO
  Circunduction RIGHT -> 180 (upwards), 0 (downwards), resting position 180
  Circunduction LEFT -> 0 (upwards), 180 (downwards), resting position 0

HEAD
  rotation -> 0º left, 180 right - 100 resting position
  tilt LEFT -> 0º
*/