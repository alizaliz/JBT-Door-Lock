
/*
*/

#include <Servo.h>
#include <SoftwareSerial.h>


/* Components */
const int servoPin = 9;
SoftwareSerial XBee(2, 3); // RX, TX

/*
 * Message constants:
 *  Expected message structure STX x ETX
 *  Where 'x' is the control character.
 *  Control Characters:
 *    '+' Lock
 *    '-' Unlock
 *    '=' Query state
 *    '?' Settings
*/
const char preamble = '\x02'; // STX preamble
const char postamble = '\x03'; // ETX postamble
const char lock = '+';
const char unlock = '-';
const char query = '=';
const char setting = '?';

/* Servo - lock positions */
int lockPos = 180;
int unlockPos = 0;


Servo servo; 
int curPos;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


void setup() {
  servo.attach(servoPin); 
  XBee.begin(38400);
  inputString.reserve(20);
  servo.write(unlockPos);
  curPos = unlockPos;
}

void loop() {

  
  // print the string when a newline arrives:
  if (stringComplete) {
    switch(inputString.charAt(0)){
      case lock:
        servo.write(lockPos);
        curPos = unlockPos;
        break;
      case unlock:
        servo.write(unlockPos);
        curPos = unlockPos;
        break;
      case query:
        // Return curpos
        break;
      case setting:
        // Further parsing of string for settings
        break;
    }
    
    /* Reset string paramters */
    inputString = "";
    stringComplete = false;
  }
}

int readState = 0;
void serialEvent() {
  /* Parse serial input from BTBee */
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
          stringComplete = true; // Set flag
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

