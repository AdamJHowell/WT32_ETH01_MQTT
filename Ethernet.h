/**
 * Created by Adam on 2023-03-22.
 */

#ifndef WT32_ETH01_MQTT_ETHERNET_H
#define WT32_ETH01_MQTT_ETHERNET_H


#include "PubSubClient.h"
#include "WT32_ETH01_MQTT.h"
#include <ETH.h>


WiFiClient ethClient;
PubSubClient mqttClient( ethClient );


const unsigned int BROKER_PORT        = 1883;                                   // The port to use when connecting to the MQTT broker.
const char *BROKER_ADDRESS            = "192.168.55.200";                       // The network address of the MQTT broker.  This can be an IP address or a hostname.
const char *COMMAND_TOPIC             = "test/wt32eth01/commands";              // The MQTT topic where the IP address will be published to.
const char *PUBLISH_TOPIC             = "test/wt32eth01/publishCount";          // The MQTT topic where the publish count will be published to.
const char *CALLBACK_COUNT_TOPIC      = "test/wt32eth01/networkCallbackCount";  // The MQTT topic where the network callback count will be published to.
const char *MQTT_CALLBACK_COUNT_TOPIC = "test/wt32eth01/mqttCallbackCount";     // The MQTT topic where the MQTT callback count will be published to.
const char *MAC_TOPIC                 = "test/wt32eth01/mac";                   // The MQTT topic where the MAC address will be published to.
const char *IP_TOPIC                  = "test/wt32eth01/ip";                    // The MQTT topic where the IP address will be published to.
const char *HOSTNAME                  = "wt32eth01";                            // The device hostname.
unsigned long lastPublishTime         = 0;                                      // The last MQTT publish time.
unsigned long publishInterval         = 20000;                                  // The time between MQTT publishing.
unsigned long publishCount            = 0;                                      // A count of how many times telemetry has been published to the MQTT broker.
unsigned long mqttConnectCount        = 0;                                      // A count of how many times a connection to the MQTT broker has been attempted.
unsigned long mqttCallbackCount       = 0;                                      // A count of how many times the MQTT callback function has been called.
unsigned long networkCallbackCount    = 0;                                      // A count of how many times the network callback function has been called.
char macAddress[18]                   = "AA:BB:CC:00:11:22";                    // A character array to hold the MAC address and a null terminator.
char ipAddress[16]                    = "127.0.0.1";                            // A character array to hold the IP address and a null terminator.
char duplex[12]                       = "HALF_DUPLEX";                          // A character array to hold the link duplex state and a null terminator.
bool eth_connected                    = false;                                  // A flag to indicate the connection state.
uint8_t linkSpeed                     = 42;                                     // This default value helps identify when the real value has not been assigned yet.


void NetworkEvent( WiFiEvent_t event );
void mqttCallback( char *topic, byte *payload, unsigned int length );
void lookupMQTTCode( int code, char *buffer );
void mqttConnect( const char *broker, int port );


#endif  //WT32_ETH01_MQTT_ETHERNET_H
