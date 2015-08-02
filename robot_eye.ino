// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 
 
#define TILT_SERVO_PIN 6
#define PAN_SERVO_PIN 5
#define LED_R_PIN 11 
#define LED_G_PIN 10 
#define LED_B_PIN 9
 
int minPosTilt = 0;
int maxPosTilt = 180;
int minPosPan = 0;
int maxPosPan = 180;
 
Servo tiltServo;
Servo panServo;


void writeColor(int red, int green, int blue) {
  analogWrite(LED_R_PIN, 255 - red);
  analogWrite(LED_G_PIN, 255 - green);
  analogWrite(LED_B_PIN, 255 - blue);
}

 
void setup() 
{ 
  pinMode(TILT_SERVO_PIN, OUTPUT);
  pinMode(PAN_SERVO_PIN, OUTPUT);
  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  tiltServo.attach(TILT_SERVO_PIN);  // attaches the servo on pin 9 to the servo object 
  panServo.attach(PAN_SERVO_PIN);  // attaches the servo on pin 9 to the servo object 

  setColor(0,0,0);  
} 
 
 
int randomRange(int startRange, int endRange) {
  return random(endRange - startRange) + startRange;
}
 
void randomPos() {
  tiltServo.write(randomRange(minPosTilt, maxPosTilt)); 
  panServo.write(randomRange(minPosPan, maxPosPan)); 
} 

void centerPos() {
  tiltServo.write(90); 
  panServo.write(90); 
} 

int r;
int g;
int b;
void setColor(int red, int green, int blue) {
  r = red;
  g = green;
  b = blue;
}

boolean alarmOn;
boolean blinkOn;
void updateColor() {
  blinkOn = !blinkOn;
    writeColor(r, g, b);
/*
if (blinkOn || !alarmOn) { 
    writeColor(r, g, b);
  }
  else  {
    writeColor(0,0,0);
  }
*/
}

void loop() 
{ 
  int moveProb = alarmOn  ? 75 : 25;
  
  if (random(100) < moveProb) {
    randomPos();
  }
  
  if (random(100) < 10) {
    setColor(255, 0, 0);
    alarmOn = true;
  }

  if (random(100) < 15) {
    setColor(0, 0, 255);
    alarmOn = false;
  }

  if (!alarmOn && random(100) < 25) {
    setColor(255, 0, 255);
    centerPos();
  }



  updateColor();

  int delayTime = 1000;
  if (alarmOn) {
    delayTime /= 2;
  }
    
  delay(delayTime);  
} 
