#include <Serial_CAN_Module.h>
#include <SoftwareSerial.h>

Serial_CAN can;

// the identity of the combo switch node.
// which will be included in each CAN message
int nodeID = 0x02;

// Input / Output pin assignments
const int pass          = 2;
const int highbeam      = 3;
const int left          = 4;
const int right         = 5;
const int ignitionStart = 6;
const int runningLight  = 7;
const int headlight     = 8;
const int hazard        = 9;
const int horn          = 10;

// pins for communicating with the CAN interface board
const int can_tx        = 11;
const int can_rx        = 12;

const int headlightMask = 64;
const int highbeamMask  = 32;
const int leftMask      = 16;
const int rightMask     = 8;
const int startMask     = 2;
const int hornMask      = 1;

int sleepTimeMS = 25;

bool blinkOn = false;
int blinkRateMS = 500;  
int blinkTime   = blinkRateMS;

int lastMessage = 0;

void setup() {
  
  Serial.println("Inializing Column Combo Switch unit");
  pinMode(pass         , INPUT_PULLUP);
  pinMode(highbeam     , INPUT_PULLUP);
  pinMode(hazard       , INPUT_PULLUP);
  pinMode(left         , INPUT_PULLUP);
  pinMode(right        , INPUT_PULLUP);
  pinMode(ignitionStart, INPUT_PULLUP);
  pinMode(runningLight , INPUT_PULLUP);
  pinMode(headlight    , INPUT_PULLUP);
  pinMode(horn         , INPUT_PULLUP);

  Serial.begin(9600);
  while( !Serial);

  can.begin(can_tx, can_rx, 9600); 
  
  Serial.println("Column Combo Switch unit initialization complete");
}

void loop() {

  // read each input pin
  int headlightInput = ( HIGH == digitalRead( headlight     ))?0:1;
  int highbeamInput  = ( HIGH == digitalRead( highbeam      ))?0:1;
  int passInput      = ( HIGH == digitalRead( pass          ))?0:1;
  int hazardInput    = ( HIGH == digitalRead( hazard        ))?0:1;
  int leftInput      = ( HIGH == digitalRead( left          ))?0:1;
  int rightInput     = ( HIGH == digitalRead( right         ))?0:1;
  int runningInput   = ( HIGH == digitalRead( runningLight  ))?0:1;
  int startInput     = ( HIGH == digitalRead( ignitionStart ))?0:1;
  int hornInput      = ( HIGH == digitalRead( horn          ))?0:1;
    
  if ( ( blinkTime -= sleepTimeMS ) <= 0 ) {
    blinkOn = !blinkOn;
    blinkTime = blinkRateMS;
  }
  
  int leftOn = 0, rightOn = 0; 
  if ( runningInput ) 
    leftOn = rightOn = 1;

  if( leftInput )
    leftOn = ( blinkOn )?1:0;

  if( rightInput )
    rightOn = ( blinkOn )?1:0;
  
  if( hazardInput ) 
    leftOn = rightOn = ( blinkOn )?1:0;

  highbeamInput = ( passInput || highbeamInput );

  int message = 0;
  message |= (headlightInput)?headlightMask:0;
  message |= ( passInput || highbeamInput )?highbeamMask:0;
  message |= (leftOn)?leftMask:0;
  message |= (rightOn)?rightMask:0;
  message |= (startInput)?startMask:0;
  message |= (hornInput)?hornMask:0;

  // only send message if it is different than last message
  if( message != lastMessage ) {

    Serial.print("Column combo switch sending. node: "); 
    Serial.print( nodeID );
    Serial.print(" message:"); 
    Serial.println( message, BIN );
    
    unsigned char dta[1] = { (unsigned char)message };
    char result = can.send( nodeID, 0, 0, 1, dta );
    
    lastMessage = message;
  }

  delay( sleepTimeMS );
}
