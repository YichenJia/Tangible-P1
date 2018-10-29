
// Variables

#include <SoftwareSerial.h>
#include "pitches.h"

SoftwareSerial XBee(2, 3); // RX, TX

int fsrInputPin = 0;
int fsrReading; // Force Resistor Reading
int lastReading = 0;//Used for comparsion

const int motorOutputPin = 9;
const int speakerOutputPin = 8;

const int ZeroWeight = 0; //FRS Reading when nothing is on top of it
const int BottleWeight = 600; //FRS Reading when empty bottle is on it
int uncertaintyValue = 200; // For adjusting sensitivity of fsr

int fsrBeforeLifting; // Used to record differences before and after one lifts bottle

String State = "Rest"; //1 = rest, 2 = action, start with rest state
int delayTime = 1000; // TODO: find appropriate interval
int warningTime = 30000; // Time the bottle can be abscent from the base before start alarm
int Timer;
int timeElapsed = 0;

const char FILLING_WATER = 'l';
const char DRINKING_WATER = 'd';
const char FAN = 'f';

void setup(void) { 

  XBee.begin(9600);
  Serial.begin(9600);  
  pinMode(motorOutputPin, OUTPUT);
  pinMode(speakerOutputPin, OUTPUT);
  delay(5000);
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
    //Writing communications
    //Serial.println("running......"+delayTime);
    fsrReading = analogRead(fsrInputPin); // when we are testing with inputs/outputs on one arduino
    Serial.print("Analog reading = ");
    Serial.println(fsrReading);
    
    //startFans();
    
    //ignore the starting and ending when lastReading is not updated yet
    int changesCheck = checkForLocalChanges(fsrReading,lastReading);
    if (changesCheck != -1){
      //changes in weight are detected
      Serial.println("Changes Detected");
      Serial.println(changesCheck);
      
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
    Serial.println("Bottle Put Back");
    return last;
  } else if (current < last - uncertaintyValue){
    State = "Active"; //bottle took off
    Serial.println("Bottle Took Off");
    return last;
  } else {
    return -1;
  }
}

void evaluateDifference(int current, int last) {
  // is there a difference between fsrBeforeLifting and current
  Serial.print("Difference:");
  Serial.println(last-current);
  if (last < current) {
    Serial.println("Filling Sound Playing");
    // makeFillingSound();
     dataOutgoing(FILLING_WATER);
    
  } else if (last > current) {
    Serial.println("Drinking Sound Playing");
    //makeDrinkingSound();
    dataOutgoing(DRINKING_WATER);
  }

}

void startFans()  {
  Serial.println("start fan motor");
  digitalWrite(motorOutputPin, HIGH);
  Serial.println(digitalRead(motorOutputPin));
  dataOutgoing(FAN);
}

void stopFans()  { // maybe we just have the fans stop asynchronously...?
  Serial.println("stop fan motor");
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
//  XBee.print("Drinking sound ");
//  XBee.print(speakerOutputPin);
//  XBee.print(" to ");
//  XBee.println("HIGH");

    int melody[] = {
      NOTE_C4, NOTE_C4, NOTE_C4, NOTE_C4, NOTE_C4, NOTE_C4, NOTE_C4
    };
    
    // note durations: 4 = quarter note, 8 = eighth note, etc.:
    int noteDurations[] = {
      4, 8, 8, 4, 4, 4, 4, 4
    };
    Serial.println("start melody motor");
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 8; thisNote++) {

      // to calculate the note duration, take one second divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(8, melody[thisNote], noteDuration);
  
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(8);
    }

}

void makeFillingSound() {

//  tone(speakerOutputPin, NOTE_A2, 1000);
//  noTone(speakerOutputPin);
    
    // notes in the melody:
    int melody[] = {
      NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
    };
    
    // note durations: 4 = quarter note, 8 = eighth note, etc.:
    int noteDurations[] = {
      4, 8, 8, 4, 4, 4, 4, 4
    };
    Serial.println("start melody motor");
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 8; thisNote++) {

      // to calculate the note duration, take one second divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(8, melody[thisNote], noteDuration);
  
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(8);
  }

}
