// pseudo code

/*
 
Notes: This code is setup for direct-com (jump wires + pins) between arduinos. For xBees please use the below codes:

#include <SoftwareSerial.h> // at the beginning
SoftwareSerial XBee(2, 3); // XBeeTX(toArduinoRX), XBeeRX(toArduinoTX)

void setup() { XBee.begin(9600); }

void loop()
{
  if (Serial.available())
  { // If data comes in from serial monitor, send it out to XBee
    XBee.write(Serial.read());
  }
  if (XBee.available())
  { // If data comes in from XBee, send it out to serial monitor
    Serial.write(XBee.read());
  }
}

https://learn.sparkfun.com/tutorials/xbee-shield-hookup-guide/example-communication-test

*/


// Variables

int fsrPin = 0; // Set initial value
int fsrReading; // Force Resistor Reading
int lastReading = 0;

int moterPin = 1;
int speakerPin = 2;

int ZeroWeight = A; //FRS Reading when nothing is on top of it
int BottleWeight = B; //FRS Reading when empty bottle is on it

int BeforeLiftingBottle;
int Difference = 0; // Used to record differences in water-level change

int uncertaintyValue = D; // For adjusting sensitivity of fsr
String State = "Rest"; //1 = rest, 2 = action, start with rest state

int delayTime = 1000; // TODO: find appropriate interval

void setup(void) {
  Serial.begin(9600); // some number base on calibration of our water bottle weight)  
}


void loop(void) {

  float CommunicationFromAnother = analogRead(1); // or whatever pin we end up using


// Writing communications
    Serial.println("running......"+delayTime);
    fsrReading = analogRead(fsrPin); // when we are testing with inputs/outputs on one arduino
    
// If the fsrReading changed from last time: if the bottle has been lifted up

  if ((fsrReading < lastReading - uncertaintyValue) && (State == "Rest")) {
    //Change to active state
    State = "Active";
    //Record the fsrReading before the bottle has been lifted
    int fsrBeforeLifting = lastReading;
    //fan start running
    
  } else {
    State = "Rest";
    lastReading = fsrReading; // Update fsr Reading from another arduino
  }

  

  if (fsrReading < ZeroWeight - uncertaintyValue && State = 1){
    // wait 3 min
    // timer
    // insert code for playing notification sound for missing bottle
  }
  
// Activated State
  if (fsrReading <= ZeroWeight + uncertaintyValue) {
    analogWrite(1, spin); // Write.fanStateSignal to other arduino to start vortex;
    State = 2;
    }

  if (fsrReading > ZeroWeight + uncertaintyValue && State = 2){
    int Difference = analogRead(fsrPin) - BeforeLiftingBottle;

    if (Difference > 0){ // if drank
      analogWrite(1, drank); // Write.speakerStateSignal to other arduino to play <drinking> sound till finished;
      State = 1; // return resting
    } else {
      analogWrite(1, poured); // Write.speakerStateSignal to other arduino to play <pouring> sound till finished;
      State = 1; // return resting
    }
    delay(delayTime);//TODO edit delay time
    lastReading = fsrReading;

// Receiving communications
  if ( CommunicationFromAnother = spin){
    // insert code for Motor-On (using transistors / resistors or whichever method is available)
  } else if ( CommunicationFromAnother = drank ) {
    // insert code for playing pouring sound
  } else if ( CommunicationFromAnother = drank ) {
    // insert code for playing pouring sound
  } else {
    // everything is off  
  }
  
}
