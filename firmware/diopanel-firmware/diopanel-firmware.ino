// DIOPanel Firmware
// 
// Simple tool to provide control and monitoring 
// of digital I/O over serial
//
// Matt Ruffner 2020

#define CMD_GETPINLIST "getpinlist"
#define CMD_GETDIRMASK "getdirmask"
#define CMD_SETDIRMASK "setdirmask"
#define CMD_SETVALMASK "setvalmask"
#define CMD_GETVALMASK "getvalmask"

// could make a more abstract definition to make switching boards easier
// this defines them in the order they wrap around the promicro physically
// with the LED as the last pin
// https://golem.hu/pic/pro_micro_pinout.jpg
#define NUM_PROMICRO_PINS 19
uint8_t promicro_pins[NUM_PROMICRO_PINS] = {0,1,2,3,4,5,6,7,8,9,10,16,14,15,18,19,20,21,13};

// these are to be referenced in the code
#define NUM_PINS NUM_PROMICRO_PINS
uint8_t *pins = promicro_pins;

bool dirMask[NUM_PINS];
bool valMask[NUM_PINS];

// init pin directions and states to a know value
// let PC software take care of storing state and sending it back 
// to restore
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // set readString and other string utility timeout to 100ms
  // basically defining our max latency
  Serial.setTimeout(25);
  delay(1000);

  // initialze all pins to high z input i.e. internal pullup disabled
  for(int i=0; i<NUM_PINS; i++) {
    dirMask[i] = INPUT;
    valMask[i] = LOW;
  }

  applyDirMask();
  applyValMask();
}

// operation is polling based to wait for a command, parse, act and wait again
void loop() {
  // put your main code here, to run repeatedly:
  String cmd = Serial.readStringUntil('\n');

  if( cmd.equals(CMD_GETDIRMASK) ){
    sendDirMask();
  }
  else if( cmd.equals(CMD_GETVALMASK) ){
    updateValMask();
    sendValMask();
  } 
  else if( cmd.equals(CMD_SETDIRMASK) ){
    parseDirMask();
    applyDirMask();
  } 
  else if( cmd.equals(CMD_SETVALMASK)  ){
    parseValMask();
    applyValMask();
  }
  else if( cmd.equals(CMD_GETPINLIST) ){
    sendPinList();
  }
}

// send back list of supported pins for this device
void sendPinList() {
  Serial.print('{');
  for( int i=0; i<NUM_PINS; i++ ){
    Serial.print(pins[i], DEC);
    if( i < NUM_PINS-1 ) 
      Serial.print(',');
  }
  Serial.println('}');
}

// send plain text formatted direction mask as array looking thing
void sendDirMask() {
  Serial.print('{');
  for( int i=0; i<NUM_PINS; i++ ){
    Serial.print(dirMask[i], DEC);
    if( i < NUM_PINS-1 ) 
      Serial.print(',');
  }
  Serial.println('}');
}

// send plain text formatted value mask as array looking thing
void sendValMask() {
  Serial.print('{');
  for( int i=0; i<NUM_PINS; i++ ){
    Serial.print(valMask[i]);
    if( i < NUM_PINS-1 ) 
      Serial.print(',');
  }
  Serial.println('}');
}

void parseDirMask() {
  while(Serial.available()==0);
  
  if( Serial.read() == '{' ){
    for( int i=0; i<NUM_PINS; i++ ){
      char d = Serial.read();
      if( d == 'i' || d == 'I' || d == '0' ){
        dirMask[i] = INPUT;
      } else if( d == 'o' || d == 'O' || d == '1'){
        dirMask[i] = OUTPUT;
      }
      Serial.read(); // read comma or in last case trailing bracket
    }
  } 
}

void parseValMask() {
  while(Serial.available()==0);

  if( Serial.read() == '{' ){
    for( int i=0; i<NUM_PINS; i++ ){
      char d = Serial.read();
      if( d == '1' || d == 'h' || d == 'H' ){
        valMask[i] = HIGH;
      } else if( d == '0' || d == 'l' || d == 'L' ){
        valMask[i] = LOW;
      }
      Serial.read(); // read comma or in last case trailing bracket
    }
  } 
}

// if the pin direction is marked as an output, drive the pin to the 
// specified state. 
// if the pin is marked as an input, read the pin's value and update the 
// record in valMask
// tldr this function changes valMask if the corresponding pin is an input
void applyValMask() {
  for( int i=0; i<NUM_PINS; i++ ){
    if( dirMask[i] == OUTPUT ){
      digitalWrite(pins[i], valMask[i]);
    } else {
      valMask[i] = digitalRead(pins[i]);
    }
  }
}

void updateValMask() {
  for( int i=0; i<NUM_PINS; i++ ){
    if( dirMask[i] == INPUT ){
      valMask[i] = digitalRead(pins[i]);
    }
  }
}

// set the pin mode depending on the direction mask
// reading pin modes is not implemented we'll just start from a known state
void applyDirMask() {
  for( int i=0; i<NUM_PINS; i++ ) {
    pinMode(pins[i], dirMask[i]);
  }
}
