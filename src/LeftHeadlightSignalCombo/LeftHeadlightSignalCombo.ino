#include "../RetroCAN.h"
// driver side headlight and signal unit
const int listenToNode = 0x02;

//pin assignment

const int transmit  = 2;
const int recieve   = 3;
const int highbeam  = 4;
const int signal    = 5;

//bit masks
const int highbeamMask  = 32;
const int leftMask      = 16;

void setup() {

  pinMode(highbeam,     OUTPUT);
  pinMode(signal,       OUTPUT);
  
  Serial.begin(9600);
  while (!Serial);
  //can.begin( transmit, recieve, 9600);

  Serial.println("CAN Receiver v2: driver side headlight and signal unit");
}

unsigned long id = 0;
unsigned char dta[8];

void loop() {
  
  delay(20);
  if( can.recv(&id, dta) ) {
        
    Serial.print("Received from ID: "); Serial.print(id);
    Serial.print(" message: "); Serial.println( dta[0], BIN );

    if ( id == listenToNode ) {
      int message = dta[0];
      
      Serial.print( "highbeam "); Serial.print( ( message & highbeamMask )?HIGH:LOW );
      Serial.print( ", signal "); Serial.println( ( message & leftMask )?HIGH:LOW );
      
      digitalWrite( highbeam, ( message & highbeamMask )?HIGH:LOW );
      digitalWrite( signal,   ( message & leftMask )?HIGH:LOW );
    } 
  }
}
