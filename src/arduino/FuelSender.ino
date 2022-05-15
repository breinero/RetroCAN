#include <Serial_CAN_Module.h>

Serial_CAN can;

// Pin assignments 
// analog pin used to connect the fuel float potentiometer
int floatPin = A0;  
int emptyPin = 1;
int can_tx   = 2;
int can_rx   = 3; 

int sleepInterval = 5000;

int lastMeasurement = 0;

int nodeID = 0x04;

void setup() {
  Serial.begin(9600);
  while(!Serial);

  can.begin(can_tx, can_rx, 9600);
}

void loop() {
  int measurement = analogRead( floatPin );           
  bool isEmpy = digitalRead( emptyPin );
  measurement = map(measurement, 0, 1023, 1, 180);

  unsigned char dta[2] = { 
    (unsigned char)measurement, 
    isEmpy
  };
  can.send( nodeID, 0, 0, 2, dta );
  delay(sleepInterval);                           // waits for the servo to get there
}