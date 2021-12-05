#include "SATCOMRelay.h"

uint32_t testModePrintTimer = millis();

SATCOMRelay relay;
uint32_t gpsTimer = millis();

void setup() {
  Serial.begin(115200);
  relay.initGPS();
}

void loop() {

  relay.readGPSSerial(); // we need to keep reading in main loop to keep GPS serial buffer clear
  if (millis() - gpsTimer > GPS_WAKEUP_INTERVAL) { // wake up the GPS until we get a fix or timeout
    relay.gpsWakeup();
    // TODO: double check this timeout logic
    if (relay.gpsFix() || (millis() - gpsTimer > GPS_WAKEUP_INTERVAL+GPS_LOCK_TIMEOUT)) { 
      relay.gpsStandby();
      gpsTimer = millis(); // reset the timer
      #if DEBUG_MODE
      Serial.print("DEBUG: ");if (relay.gpsFix()) {Serial.println("GOT GPS FIX");} else {Serial.println("GPS FIX TIMEOUT");}
      #endif
    } 
    //relay.printGPS();
  }

  #if TEST_MODE // print the state of the relay
  if (millis() - testModePrintTimer > TEST_MODE_PRINT_INTERVAL) {
    testModePrintTimer = millis(); // reset the timer
    relay.print();
  }
  #endif

}
