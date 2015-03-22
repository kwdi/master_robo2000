//////////LIB////////////////////LIB//////////
#include "Servo.h"
#include "NewPing.h"
#include <Wire.h>
//////////LIB////////////////////LIB//////////

//////////Ultrasonic-Servo////////////////////Ultrasonic-Servo//////////
#define TRIGGER_PIN  49  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     48  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

int leftscanval, centerscanval, rightscanval, ldiagonalscanval, rdiagonalscanval;
char choice;

int modecontrol=0;
int power=0;

const int distancelimit = 27; //Distance limit for obstacles in front           
const int sidedistancelimit = 12; //Minimum distance in cm to obstacles at both sides (the robot will allow a shorter distance sideways)

int distance;
int numcycles = 0;
char turndirection; //Gets 'l', 'r' or 'f' depending on which direction is obstacle free
const int turntime = 900; //Time the robot spends turning (miliseconds)
int thereis;
Servo head;
//////////Ultrasonic-Servo////////////////////Ultrasonic-Servo//////////

//////////TEMP////////////////////TEMP//////////
float tempC;
int LM35 = 1; //LM35 pin
//////////TEMP////////////////////TEMP//////////

//////////VOLT////////////////////VOLT//////////
int voltPin = 0; //voltometer pin
float vPow = 4.991;
float r1 = 100000;
float r2 = 10000;
//////////VOLT////////////////////VOLT//////////

//////////SPEAKER////////////////////SPEAKER//////////
int spkpin = 8;
int pause = 100;
int note = 440; // music note A4
//////////SPEAKER////////////////////SPEAKER//////////

void setup(){

	head.attach(7);
	head.write(80);

  pinMode(spkpin, OUTPUT);
}

float volt(){
  float v = (analogRead(0) * vPow) / 1024.0;
  float v2 = v / (r2 / (r1 + r2));
  return v2;
}

float temp(){
  tempC = analogRead(LM35);
  tempC = (5.0 * tempC * 100.0)/1024.0;
  return tempC;
}

//////////I2C////////////////////I2C//////////
void stopmove(){

  Wire.beginTransmission(2); // transmit to device #2 (wheels)
  Wire.write("stop");        // sends 4 bytes
  Wire.endTransmission();    // stop transmitting
  //change to number/signal for stopmove
}

void go(){

  Wire.beginTransmission(2); // transmit to device #2 (wheels)
  Wire.write("go");        // sends 4 bytes
  Wire.endTransmission();    // stop transmitting
  //change to number/signal for stopmove
}

void turnleft(){

  Wire.beginTransmission(2); // transmit to device #2 (wheels)
  Wire.write("turnleft");        // sends 4 bytes
  Wire.endTransmission();    // stop transmitting
  //change to number/signal for stopmove
}

void turnright(){

  Wire.beginTransmission(2); // transmit to device #2 (wheels)
  Wire.write("turnright");        // sends 4 bytes
  Wire.endTransmission();    // stop transmitting
  //change to number/signal for stopmove
}
//////////I2C////////////////////I2C//////////

void watchsurrounding(){ //Meassures distances to the right, left, front, left diagonal, right diagonal and asign them in cm to the variables rightscanval, 
                         //leftscanval, centerscanval, ldiagonalscanval and rdiagonalscanval (there are 5 points for distance testing)
  centerscanval = sonar.ping_cm();
  if(centerscanval<distancelimit){stopmove();}
  head.write(120);
  delay(100);
  ldiagonalscanval = sonar.ping_cm();
  if(ldiagonalscanval<distancelimit){stopmove();}
  head.write(160); //Didn't use 180 degrees because my servo is not able to take this angle
  delay(300);
  leftscanval = sonar.ping_cm();
  if(leftscanval<sidedistancelimit){stopmove();}
  head.write(120);
  delay(100);
  ldiagonalscanval = sonar.ping_cm();
  if(ldiagonalscanval<distancelimit){stopmove();}
  head.write(80); //I used 80 degrees because its the central angle of my 160 degrees span (use 90 degrees if you are moving your servo through the whole 180 degrees)
  delay(100);
  centerscanval = sonar.ping_cm();
  if(centerscanval<distancelimit){stopmove();}
  head.write(40);
  delay(100);
  rdiagonalscanval = sonar.ping_cm();
  if(rdiagonalscanval<distancelimit){stopmove();}
  head.write(0);
  delay(100);
  rightscanval = sonar.ping_cm();
  if(rightscanval<sidedistancelimit){stopmove();}

  head.write(80); //Finish looking around (look forward again)
  delay(300);
}

char decide(){
  watchsurrounding();
  if (leftscanval>rightscanval && leftscanval>centerscanval){
    choice = 'l';
  }
  else if (rightscanval>leftscanval && rightscanval>centerscanval){
    choice = 'r';
  }
  else{
    choice = 'f';
  }
  return choice;
}

void loop(){

  go();  // if nothing is wrong go forward using go() function above.
  ++numcycles;
  if(numcycles>130){ //Watch if something is around every 130 loops while moving forward 
    watchsurrounding();
    if(leftscanval<sidedistancelimit || ldiagonalscanval<distancelimit){
      turnright();
    }
    if(rightscanval<sidedistancelimit || rdiagonalscanval<distancelimit){
      turnleft();
    }
    numcycles=0; //Restart count of cycles
  }
  distance = sonar.ping_cm(); // use the watch() function to see if anything is ahead (when the robot is just moving forward and not looking around it will test the distance in front)
  if (distance<distancelimit){ // The robot will just stop if it is completely sure there's an obstacle ahead (must test 25 times) (needed to ignore ultrasonic sensor's false signals)
      ++thereis;}
  if (distance>distancelimit){
      thereis=0;} //Count is restarted
  if (thereis > 25){
    stopmove(); // Since something is ahead, stop moving.
    turndirection = decide(); //Decide which direction to turn.
    switch (turndirection){
      case 'l':
        turnleft();
        break;
      case 'r':
        turnright();
        break;
      case 'f':
        ; //Do not turn if there was actually nothing ahead
        break;
    }
    thereis=0;
  }
	
}