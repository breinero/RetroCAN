#include <LiquidCrystal.h>
#include "RetroCAN.h"
#include <mcp2515.h>

MCP2515 mcp2515(10);

// pins for communicating with the CAN interface board 
const int can_tx     = 11;
const int can_rx     = 12;
const int SPI_CS_PIN = 10;
const int sck        = 13;
const int INT_PIN    = 9;

/*---- LCD constants ----*/
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/*---- Display Constants ----*/

struct displayPosition {
    int column, row, stringLen;
};

const struct {
  displayPosition 
    left,
    speedo,
    tach,
    fuel,
    right,
    headlight,
    highbeam,
    odometer,
    breakWarn,
    oil,
    volts;
} displayPositions = {
    displayPosition{ 0, 0, 1},   
    displayPosition{ 1, 0, 3},
    displayPosition{ 8, 0, 4},
    displayPosition{ 11, 1, 1},
    displayPosition{ 15, 0, 1},
    displayPosition{ 0, 1, 1},
    displayPosition{ 0, 1, 1},
    displayPosition{ 1, 1, 1},
    displayPosition{ 13, 1, 1},
    displayPosition{ 14, 1, 1},
    displayPosition{ 15, 1, 1} 
};

char left      = 'L';
char right     = 'R';
char headlight = 'B';
int speed      = 0;
int rpms       = 0;
int fuel       = 0;
int odometer   = 0;
char breakWarn = 'B';
char oil       = 'O';
char volts     = 'V';
// warning values
const int highOilTemp = 200;
const int lowVoltage  = 11;

long odomValue = 20000;

struct can_frame frame;

void handle( struct can_frame *frame ) {

  Serial.print("recieved a message! ");    
  
  if( frame->can_id == network.combinationSwitch ) {
    headlight = ' ';
    if ( frame->data[0] & comboSwitchMessage.headlight.mask  )
      headlight = 'L';
    else if ( frame->data[0] & comboSwitchMessage.highbeam.mask  )
      headlight = 'H';

    left  = ( frame->data[0] & comboSwitchMessage.left.mask )?'L':' ' ;
    right = ( frame->data[0] & comboSwitchMessage.right.mask )?'R':' ' ;
  }

  if( frame->can_id == network.fuelSender )
    fuel = frame->data[fuelMessage.index]; 

  if( frame->can_id == network.speedSensor )
    speed = frame->data[speedMessage.index]; 

  if( frame->can_id == network.tachometerSensor )
    rpms = frame->data[tachMessage.index]; 

  if( frame->can_id == network.oilTempSensor )
    oil = (frame->data[oilMessage.index] >= highOilTemp )?'O':' ';

  if( frame->can_id == network.voltMeterSensor )
    volts = ( frame->data[voltMessage.index] <= lowVoltage )?'V':' ';

  if( frame->can_id == network.breakWarningSwitch )
    breakWarn = ( frame->data[breakMessage.index] )?'B':' '; 

  display();
}

void display () {

  lcd.setCursor(displayPositions.left.column, displayPositions.left.row);
  lcd.print(left);
  lcd.print(" ");
  lcd.setCursor(displayPositions.speedo.column, displayPositions.speedo.row);
  lcd.print(speed);
  lcd.print("mph");
  lcd.setCursor(displayPositions.tach.column, displayPositions.tach.row);
  lcd.print(rpms);
  lcd.print("rpm");
  lcd.setCursor(15, 0);
  lcd.print(right);
  lcd.setCursor(displayPositions.headlight.column, displayPositions.headlight.row);
  lcd.print(headlight );
  lcd.print(" ");
  lcd.print(odometer);
  lcd.setCursor(displayPositions.fuel.column, displayPositions.fuel.row);
  lcd.print(fuel);
  lcd.print("f");
  lcd.setCursor(displayPositions.breakWarn.column, displayPositions.breakWarn.row);
  lcd.print(breakWarn);
  lcd.setCursor(displayPositions.oil.column, displayPositions.oil.row);
  lcd.print(oil);
  lcd.setCursor(displayPositions.volts.column, displayPositions.volts.row);
  lcd.print(volts);
}

volatile bool interrupt = false;
void irqHandler() {
    interrupt = true;
}


struct can_frame canMsg;

void setup() {

  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();

  attachInterrupt(0, irqHandler, FALLING);

  // set up the display
  lcd.begin(16, 2);
  lcd.clear();
  display();
  Serial.println( "Gauge Cluster initialized");
}

bool blink = false;

void loop() {

  if (interrupt) {
    interrupt = false;

    uint8_t irq = mcp2515.getInterrupts();

    if (irq & MCP2515::CANINTF_RX0IF) {
        if (mcp2515.readMessage(MCP2515::RXB0, &canMsg) == MCP2515::ERROR_OK) {
            // frame contains received from RXB0 message
            handle(&canMsg);
        }
    }

    if (irq & MCP2515::CANINTF_RX1IF) {
        if (mcp2515.readMessage(MCP2515::RXB1, &canMsg) == MCP2515::ERROR_OK) {
            // frame contains received from RXB1 message
            handle(&canMsg);
        }
    }
  }
}