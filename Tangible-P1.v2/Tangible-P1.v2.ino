
// Variables

#include <SoftwareSerial.h>
#include "pitches.h"

SoftwareSerial XBee(2, 3); // RX, TX

int fsrInputPin = 0;
int fsrReading; // Force Resistor Reading
int lastReading = 0;//Used for comparsion

const int motorOutputPin = 1;
const int speakerOutputPin = 2;

const int ZeroWeight = 0; //FRS Reading when nothing is on top of it
const int BottleWeight = 600; //FRS Reading when empty bottle is on it
int uncertaintyValue = 20; // For adjusting sensitivity of fsr

int fsrBeforeLifting; // Used to record differences before and after one lifts bottle

String State = "Rest"; //1 = rest, 2 = action, start with rest state
int delayTime = 1000; // TODO: find appropriate interval
int warningTime = 30000; // Time the bottle can be abscent from the base before start alarm
int Timer;

const char FILLING_WATER = 'l';
const char DRINKING_WATER = 'd';
const char FAN = 'f';

void setup(void) { 
   XBee.begin(9600);
  Serial.begin(9600);  
  pinMode(motorOutputPin, OUTPUT);
  pinMode(speakerOutputPin, OUTPUT);
}

int dataOutgoing(int mode) {
    if (XBee.available()) { // If data comes in from XBee, send it to serial monitor
      XBee.write(mode);
    }
}

void checkForRemote(){
  dataIncoming();
}

void loop(void) {
    // Writing communications
    Serial.println("running......"+delayTime);
    fsrReading = analogRead(fsrInputPin); // when we are testing with inputs/outputs on one arduino
    Serial.print("Analog reading = ");
    Serial.println(fsrReading);

    if (fsrReading != 0 && lastReading != 0){ 
      //ignore the starting and ending when lastReading is not updated yet
      
      int changesCheck = checkForLocalChanges(fsrReading,lastReading);
      if (changesCheck != -1){
        //changes in weight are detected
        
        if (State == "Active") {
          startFans();
          fsrBeforeLifting = changesCheck;

        } else if (State == "Rest"){
          Timer = 0;
          stopFans();
          evaluateDifference(fsrReading,fsrBeforeLifting);
        }
        
      }

      checkForRemote();
      
    }
    
    delay(delayTime);//TODO edit delay time
    lastReading = fsrReading;
    Timer = Timer + delayTime;
    if (Timer >= warningTime && State == "Active") {
       //TODO: Bottle hasn't been put back, play warning sound 
    }
}

int checkForLocalChanges(int current, int last) {
  if (current > last + uncertaintyValue) {
    State = "Rest"; // bottle put back
    return last;
  } else if (current < last - uncertaintyValue){
    State = "Active"; //bottle took off
    return current;
  } else {
    return -1;
  }
}

void evaluateDifference(int current, int last) {
  // is there a difference between fsrBeforeLifting and current
  
  if (last < current) {
    
     makeFillingSound();
     dataOutgoing(FILLING_WATER);
    
  } else if (last > current) {
    makeDrinkingSound();
    dataOutgoing(DRINKING_WATER);
  }

}

void startFans()  {
  digitalWrite(motorOutputPin, HIGH);
  //delay(5000);
  //pinMode(motorOutputPin, LOW);
  dataOutgoing(FAN);
}

void stopFans()  { // maybe we just have the fans stop asynchronously...?
  digitalWrite(motorOutputPin, LOW);
  //delay(5000);
  //pinMode(motorOutputPin, LOW);
  //dataOutgoing(FAN);
}




int dataIncoming() {
    if (XBee.available()) { // If data comes in from XBee, read it
      char c = XBee.read();
      switch (c) {
      case DRINKING_WATER:      // If received 'w'
        stopFans();
        makeDrinkingSound(); // Write analog/digital pin
        break;
      case FILLING_WATER:      // If received 'w'
        stopFans();
        makeFillingSound(); // Write analog pin
        break;
      case FAN:
        startFans();
        break;
      }
      
    }
} 


void makeDrinkingSound() {

  // Print a message to let the control know of our intentions:
  XBee.print("Drinking sound ");
  XBee.print(speakerOutputPin);
  XBee.print(" to ");
  XBee.println("HIGH");

  tone(speakerOutputPin, NOTE_B0, 300);
  noTone(speakerOutputPin);

}

void makeFillingSound() {

  tone(speakerOutputPin, NOTE_A2, 300);
  noTone(speakerOutputPin);

}





