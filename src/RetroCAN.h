#ifndef RETROCAN_h
#define RETROCAN_h

#include <mcp2515.h>

// the identity of the node in the network
const struct {
    int combinationSwitch ;
    int fuelSender        ;
    int speedSensor       ;
    int tachometerSensor  ;
    int oilTempSensor     ;
    int voltMeterSensor   ;
    int breakWarningSwitch;
    int reverseSwitch     ;
} network = { 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };

struct messagePosition {
    int index, mask;
};

const struct {
    messagePosition 
    headlight,
    highbeam,
    left,
    right,
    start,
    horn;
} comboSwitchMessage = { 
    messagePosition{0, 64}, 
    messagePosition{0, 32}, 
    messagePosition{0, 16}, 
    messagePosition{0, 8}, 
    messagePosition{0, 2}, 
    messagePosition{0, 1} 
};

const messagePosition fuelMessage  = {0, 0};
const messagePosition speedMessage = {0, 0};
const messagePosition tachMessage  = {0, 0};
const messagePosition voltMessage  = {0, 0};
const messagePosition breakMessage = {0, 0};
const messagePosition oilMessage   = {0, 0};

#endif