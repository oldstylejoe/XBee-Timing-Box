//Joe Snider
//6/15
//
//receive ascii from a Serial connection and send piulsed

float pulseTime = 100;//ms

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  
  //put in a delay for the XBee to negotiate
//  delay(1000);
}

int inByte;

void loop() {
  if(Serial.available() > 0) {
    inByte = Serial.read();
    
    int mask = 1; //todo - make global array for constant time deref
    for(int b = 0; b < 8; ++b) {
      if(mask & inByte) {
        digitalWrite(b+2, HIGH);
        //Serial.print("pin ");
        //Serial.println(b);
      }
      mask = mask << 1;
    }
    delay(pulseTime);
    for(int b = 0; b < 8; ++b, mask <<= 1) {
      digitalWrite(b+2, LOW);
    }
    
    //clear anything in serial
    while(Serial.available() > 0) {
      Serial.read();
    }
    
  }
}

