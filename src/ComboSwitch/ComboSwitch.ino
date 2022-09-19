#include <SPI.h>
#include "RetroCAN.h"
#include <mcp2515.h>


// Input / Output pin assignments
const int hazard    = 0;
const int horn      = 1;
const int brights   = 2;
const int left      = 4;
const int right     = 3;
const int start     = 5;
const int running   = 6;
const int headlight = 7;
int sleepTimeMS = 100;

bool blinkOn = false;
int blinkRateMS = 500;
int blinkTime = blinkRateMS;

int lastMessage = 0;

struct can_frame frame;
MCP2515 mcp2515(10);

// pins for communicating with the CAN interface board 
const int can_tx     = 11;
const int can_rx     = 12;
const int SPI_CS_PIN = 10;
const int sck        = 13;
const int INT_PIN    = 9;
 
void setup()
{
  pinMode(hazard,    INPUT_PULLUP);
  pinMode(horn,      INPUT_PULLUP);
  pinMode(brights,   INPUT_PULLUP);
  pinMode(left,      INPUT_PULLUP);
  pinMode(right,     INPUT_PULLUP);
  pinMode(start,     INPUT_PULLUP);
  pinMode(running,   INPUT_PULLUP);
  pinMode(headlight, INPUT_PULLUP);

  frame.can_id  = network.combinationSwitch;
  frame.can_dlc = 1;

  while (!Serial);
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();  
}

void loop()
{
  // read each input pin
  bool hazardInput    = (HIGH != digitalRead(hazard));
  bool hornInput      = (HIGH != digitalRead(horn));
  bool headlightInput = (HIGH != digitalRead(headlight));
  bool brightsInput   = (HIGH != digitalRead(brights));
  bool leftInput      = (HIGH != digitalRead(left));
  bool rightInput     = (HIGH != digitalRead(right));
  bool runningInput   = (HIGH != digitalRead(running));
  bool startInput     = (HIGH != digitalRead(start));

  if ((blinkTime -= sleepTimeMS) <= 0) {
    blinkOn = !blinkOn;
    blinkTime = blinkRateMS;
  }

  int leftOn = false, rightOn = false;
  if (runningInput)
    leftOn = rightOn = true;

  if (leftInput)
    leftOn = blinkOn;

  if (rightInput)
    rightOn = blinkOn;

  if (hazardInput)
    leftOn = rightOn = blinkOn;

  int message = 0;
  // Serial.print("pre-message: ");
  // Serial.println( message );
  
  message |= (headlightInput)? comboSwitchMessage.headlight.mask: 0;
  message |= (!brights)?        comboSwitchMessage.highbeam.mask:  0;
  message |= (leftOn)?         comboSwitchMessage.left.mask:      0;
  message |= (rightOn)?        comboSwitchMessage.right.mask:     0;
  message |= (startInput)?     comboSwitchMessage.start.mask:     0;
  message |= (hornInput)?      comboSwitchMessage.horn.mask:      0;

  //Serial.print(" post message: ");Serial.println(message);

  // only send message if it is different than last message
  if (message != lastMessage)
  {
    Serial.print("Column combo switch sending. node: ");
    Serial.print(network.combinationSwitch);
    Serial.print(" message:");
    Serial.println(message);
    
    
    frame.data[0] = message;
    int status = mcp2515.sendMessage(&frame);
    if ( status == MCP2515::ERROR_OK) {
      Serial.println( "message sent!");
    }else {
      Serial.print( "error: " ); Serial.println(status);
    }

    lastMessage = message;
  }

  delay(sleepTimeMS);
}
