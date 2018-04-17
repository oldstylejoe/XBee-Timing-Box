//Joe Snider
//6/15
//
//receive ascii from a Serial connection and send pulses out on digital pins.
//
//10/15
//Now goes into a default mode of sending a double pulse of 20ms every 1-5 seconds
// (parameters set below, double check) if it hasn't heard from the 
// XBee for a given time (100s).
//Potential problem: If this is actively sending the default pulse, then the wake
// pulse might be confused.
//
//3/16 updated for NYU to have default pulses every 20-30 seconds.
//
//11/16 updated to use a shift register. This turns on all the pins at the same time.

unsigned long pulseTime = 100;//ms
bool inPulseMode = false;
unsigned long pulseOffTime;
bool reportNextPulse = false;

unsigned long deafultPulseTime = 20;//ms
unsigned long deadTime = 100000;//ms //time to wait without anything from XBee
unsigned long lastPulseTime;
unsigned long nextDefaultPulse;
unsigned long defaultDelay[] = {20000, 21000, 22000, 23000, 24000, 25000, 26000, 27000, 28000, 29000, 30000};//ms
unsigned long nDelays = 11;//number of entries in the array above
bool inDefaultMode;

unsigned long latencyCheck = 0;

int latchPin = 7;
int clockPin = 8;
int dataPin = 9;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(A0, INPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);

  pinMode(13, OUTPUT);

  //start out in the default mode
  inDefaultMode = true;
  lastPulseTime = millis();
  randomSeed(analogRead(0));
  
  //while (!Serial);
  updateShiftRegister(0);
}

int inByte;

void loop() {
  if(Serial1.available() > 0 && !inPulseMode) {
    inByte = Serial1.read();

    updateShiftRegister(inByte);
    digitalWrite(13, HIGH);

    inDefaultMode = false;
    inPulseMode = true;

    lastPulseTime = millis();
    pulseOffTime = lastPulseTime + pulseTime;// may roll over

    //clear anything in serial
    while(Serial1.available() > 0) {
      Serial1.read();
    }
  }

  //turn off pulse, safe for roll-over
  if(inPulseMode && (long)(millis() - pulseOffTime) > 0) {
    updateShiftRegister(0);
    digitalWrite(13, LOW);
    inPulseMode = false;
  }
  
  //report the latency if we get a pulse on the standard serial port
  if(reportNextPulse && inPulseMode && analogRead(A0) > 255) {
    long latency = millis() - latencyCheck;
    reportNextPulse = false;
    Serial.print("latency: ");
    Serial.print(latency);
    Serial.println(" ms");
  }

  if(Serial.available()) {
    latencyCheck = millis(); //no worry about roll over, deal with it
    reportNextPulse = true;
    while(Serial.available() > 0) {
      Serial.read();
    }
  }

  //enter default mode after a delay, rollover safe
  if(!inDefaultMode && (long)(millis() - lastPulseTime - deadTime) > 0) {
    inDefaultMode = true;
    nextDefaultPulse = millis();
    updateShiftRegister(0);
    digitalWrite(13, LOW);
    inPulseMode = false;
  }

  if(inDefaultMode && (long)(millis() - nextDefaultPulse) > 0) {
    //this will block and delay the XBee trigger.
    //Have to add some sort of test to skip any allignment tags with a double tap within
    //  (about) 1us (10ms is probably reasonable, or just always skip the first one).

    digitalWrite(13, HIGH);
    updateShiftRegister(B11111111);
    delay(deafultPulseTime);
    updateShiftRegister(0);
    delay(deafultPulseTime);
    updateShiftRegister(B11111111);
    delay(deafultPulseTime);
    updateShiftRegister(0);
    digitalWrite(13, LOW);

    nextDefaultPulse = millis() + defaultDelay[random(nDelays)];
  }
}

//this sends whatever is stored in leds to the pins.
//It is big endian (I think, B00000001 fires pin 2 on the db25)
void updateShiftRegister(byte leds)
{
  byte mask = B10000000;
   digitalWrite(latchPin, LOW);
   for(int i = 0; i < 8; ++i) {
    digitalWrite(dataPin, LOW);
    if( (leds & mask) == mask) {
      digitalWrite(dataPin, HIGH);
    }
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
    leds <<= 1;
   }
   //shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}


