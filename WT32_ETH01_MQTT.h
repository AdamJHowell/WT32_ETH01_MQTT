/**
 * Created by Adam on 2023-03-22.
 */

#ifndef WT32_ETH01_MQTT_WT32_ETH01_MQTT_H
#define WT32_ETH01_MQTT_WT32_ETH01_MQTT_H

#include "Ethernet.h"
#include "PubSubClient.h"

const unsigned int RX_LED            = 5;     // The GPIO used by the receive LED.
const unsigned int TX_LED            = 17;    // The GPIO used by the transmit LED.
const unsigned int LED_OFF           = 0;     // This allows the program to accommodate boards that use non-standard HIGH and LOW values.
const unsigned int LED_ON            = 1;     // This allows the program to accommodate boards that use non-standard HIGH and LOW values.
unsigned long lastMqttConnectionTime = 0;     // The time of the last MQTT broker connection attempt.  Used by mqttConnect() to prevent swamping the broker with connection attempts.
unsigned long mqttCoolDownInterval   = 5000;  // The time between MQTT broker connection attempts.
unsigned long lastPrintTime          = 0;     // The last time stats were printed to the serial port.
unsigned long printInterval          = 5000;  // The time between prints.
unsigned long printCount             = 0;     // A count of how many times the printTelemetry() function has been called.


void setup();
void toggleLED();
void printTelemetry();
void loop();


#endif  //WT32_ETH01_MQTT_WT32_ETH01_MQTT_H
