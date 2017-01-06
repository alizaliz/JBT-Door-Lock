



/*
*/


#include <DRV8834.h>
#include <QueueArray.h>
#include <SoftwareSerial.h>


/* Components */
// Driver
#define DIR 4
#define STEP 5
#define ENBL 6
#define M0 7
#define M1 8
// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
DRV8834 stepper(MOTOR_STEPS, DIR, STEP, ENBL, M0, M1);
//BTBee
/*
#define RX 2
#define TX 3
SoftwareSerial BTBee(RX, TX); */

/*
 * Message constants:
 *  Expected message structure STX x ETX
 *  Where 'x' is the control character.
 *  
 *  Control Characters:
 *    '+' Lock
 *    '-' Unlock
 *    '=' Query state
 *    '?' Settings
 *        
 *  '|' Delimmiter
*/
const char preamble = '2';//'\x02'; // STX preamble
const char postamble = '3';//'\x03'; // ETX postamble
const char lock = '+';
const char unlock = '-';
const char query = '=';
const char setting = '?';
const char delimmiter = '|';

QueueArray <String> inMessageQ;
QueueArray <String> outMessageQ;
int curPos, speed;
String inputString = "";         // a string to hold incoming data
String tmp;



void setup() {
  //BTBee.begin(38400);
  Serial.begin(9600);
  //Memory allocation
  inputString.reserve(20);
  tmp.reserve(20);

  // Default speed
  speed = 120;
  stepper.setRPM(speed);
  // make sure we are in full speed mode
  stepper.setMicrostep(1); 
  stepper.disable(); // Keep it loosey goosey
}

void loop() {

  if (!inMessageQ.isEmpty()) {
    tmp = inMessageQ.pop();
    switch(tmp.charAt(0)){
      case lock:
     
        move();
        break;
      case unlock:
 
        move();
        break;
      case query:
        // Return curpos
        break;
      case setting:
        // Further parsing of string for settings
        break;
    }
    
    /* Reset string paramters */
    //inputString = "";
  }
}

void move() {

 

}



// Message recieved function
int readState = 0;
void serialEvent() {
  /* Parse serial input from BTBee */
  //Serial.println("Recieved event");
  while (Serial.available()) { 

    char inChar = (char)Serial.read();

   Serial.write(inChar);

    switch (readState){
      case 0: // Search for preamble
        if(inChar == preamble){
          readState = 1;
        }
        break;
      case 1: // Collect characters
       if (inChar == postamble) { // Search for postamble - escape if found
          inMessageQ.push(inputString); // Set flag
          readState = 0;
        } else if (inputString.length() < 20){ 
          inputString += inChar;
        } else { // Escape if string too long
          readState = 0;
        }
        break;     
    }
  }
}

