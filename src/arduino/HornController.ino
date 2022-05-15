#include <mcp2515.h>

const int listenToNode = 0x02;

// Pin assignment
const int INT_PIN    = 0;
const int SPI_CS_PIN = 1;
const int hornPin    = 2;

MCP2515 mcp2515( SPI_CS_PIN );


// bit masks
const int hornMask = 1;

volatile bool interrupt = false;
struct can_frame frame;

void irqHandler() {
    interrupt = true;
}


void setup() {

  pinMode(hornPin,    OUTPUT);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  attachInterrupt( INT_PIN, irqHandler, FALLING);

}

void loop() {

  if (interrupt) {
    interrupt = false;

    struct can_frame frame;

    uint8_t irq = mcp2515.getInterrupts();

    if (irq & MCP2515::CANINTF_RX0IF) {
        if (mcp2515.readMessage(MCP2515::RXB0, &frame) == MCP2515::ERROR_OK) {
            // frame contains received from RXB0 message
           //if( frame.can_id == listenToNode ) 
              digitalWrite( hornPin,  ( frame.data[0] & hornMask )?HIGH:LOW );
        }
    }

    if (irq & MCP2515::CANINTF_RX1IF) {
        if (mcp2515.readMessage(MCP2515::RXB1, &frame) == MCP2515::ERROR_OK) {

            // frame contains received from RXB1 message
            //if( frame.can_id == listenToNode ) 
              digitalWrite( hornPin,  ( frame.data[0] & hornMask )?HIGH:LOW );
        }
    }
  }
}
