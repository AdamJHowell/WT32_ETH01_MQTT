/**
 * C4:DE:E2:B2:C5:D3
 */
#include "PubSubClient.h"
#include <ETH.h>


const unsigned int RX_LED             = 5;                                      // The GPIO used by the receive LED.
const unsigned int TX_LED             = 17;                                     // The GPIO used by the transmit LED.
const unsigned int LED_OFF            = 0;                                      // This allows the program to accommodate boards that use non-standard HIGH and LOW values.
const unsigned int LED_ON             = 1;                                      // This allows the program to accommodate boards that use non-standard HIGH and LOW values.
const unsigned int BROKER_PORT        = 1883;                                   // The port to use when connecting to the MQTT broker.
const char *BROKER_ADDRESS            = "192.168.55.200";                       // The network address of the MQTT broker.  This can be an IP address or a hostname.
const char *COMMAND_TOPIC             = "test/wt32eth01/commands";              // The MQTT topic where the IP address will be published to.
const char *PUBLISH_TOPIC             = "test/wt32eth01/publishCount";          // The MQTT topic where the publish count will be published to.
const char *CALLBACK_COUNT_TOPIC      = "test/wt32eth01/networkCallbackCount";  // The MQTT topic where the network callback count will be published to.
const char *MQTT_CALLBACK_COUNT_TOPIC = "test/wt32eth01/mqttCallbackCount";     // The MQTT topic where the MQTT callback count will be published to.
const char *MAC_TOPIC                 = "test/wt32eth01/mac";                   // The MQTT topic where the MAC address will be published to.
const char *IP_TOPIC                  = "test/wt32eth01/ip";                    // The MQTT topic where the IP address will be published to.
const char *HOSTNAME                  = "wt32eth01";                            // The device hostname.
unsigned long lastMqttConnectionTime  = 0;                                      // The time of the last MQTT broker connection attempt.  Used by mqttConnect() to prevent swamping the broker with connection attempts.
unsigned long mqttCoolDownInterval    = 5000;                                   // The time between MQTT broker connection attempts.
unsigned long lastPrintTime           = 0;                                      // The last time stats were printed to the serial port.
unsigned long printInterval           = 5000;                                   // The time between prints.
unsigned long lastPublishTime         = 0;                                      // The last MQTT publish time.
unsigned long publishInterval         = 20000;                                  // The time between MQTT publishing.
unsigned long publishCount            = 0;                                      // A count of how many times telemetry has been published to the MQTT broker.
unsigned long mqttConnectCount        = 0;                                      // A count of how many times a connection to the MQTT broker has been attempted.
unsigned long mqttCallbackCount       = 0;                                      // A count of how many times the MQTT callback function has been called.
unsigned long networkCallbackCount    = 0;                                      // A count of how many times the network callback function has been called.
unsigned long printCount              = 0;                                      // A count of how many times the printTelemetry() function has been called.
char macAddress[18]                   = "AA:BB:CC:00:11:22";                    // A character array to hold the MAC address and a null terminator.
char ipAddress[16]                    = "127.0.0.1";                            // A character array to hold the IP address and a null terminator.
char duplex[12]                       = "HALF_DUPLEX";                          // A character array to hold the link duplex state and a null terminator.
bool eth_connected                    = false;                                  // A flag to indicate the connection state.
uint8_t linkSpeed                     = 42;                                     // This default value helps identify when the real value has not been assigned yet.


WiFiClient ethClient;
PubSubClient mqttClient( ethClient );


/**
 * @brief This is a callback function to handle network events.
 * @param event a pre-defined event to process.
 * Even though the event type is WiFiEvent_t, the same events work with the ETH.h library.
 */
void NetworkEvent( WiFiEvent_t event )
{
	networkCallbackCount++;
	switch( event )
	{
		case ARDUINO_EVENT_ETH_START:
			Serial.println( "ETH Started" );
			ETH.setHostname( HOSTNAME );
			break;
		case ARDUINO_EVENT_ETH_CONNECTED:
			Serial.println( "ETH Connected" );
			break;
		case ARDUINO_EVENT_ETH_GOT_IP:
			snprintf( macAddress, 18, "%s", ETH.macAddress().c_str() );
			snprintf( ipAddress, 16, "%d.%d.%d.%d", ETH.localIP()[0], ETH.localIP()[1], ETH.localIP()[2], ETH.localIP()[3] );
			if( ETH.fullDuplex() )
				snprintf( duplex, 12, "%s", "FULL_DUPLEX" );
			else
				snprintf( duplex, 12, "%s", "HALF_DUPLEX" );
			linkSpeed = ETH.linkSpeed();
			Serial.printf( "ETH MAC: %s, IPv4: %s, %s, %u Mbps", macAddress, ipAddress, duplex, linkSpeed );
			eth_connected = true;
			break;
		case ARDUINO_EVENT_ETH_DISCONNECTED:
			Serial.println( "ETH Disconnected" );
			eth_connected = false;
			break;
		case ARDUINO_EVENT_ETH_STOP:
			Serial.println( "ETH Stopped" );
			eth_connected = false;
			break;
		default:
			break;
	}
}  // End of the NetworkEvent() callback function.


/**
 * @brief lookupMQTTCode() will return the string for an integer state code.
 */
void lookupMQTTCode( int code, char *buffer )
{
	switch( code )
	{
		case -4:
			snprintf( buffer, 29, "%s", "Connection timeout" );
			break;
		case -3:
			snprintf( buffer, 29, "%s", "Connection lost" );
			break;
		case -2:
			snprintf( buffer, 29, "%s", "Connect failed" );
			break;
		case -1:
			snprintf( buffer, 29, "%s", "Disconnected" );
			break;
		case 0:
			snprintf( buffer, 29, "%s", "Connected" );
			break;
		case 1:
			snprintf( buffer, 29, "%s", "Bad protocol" );
			break;
		case 2:
			snprintf( buffer, 29, "%s", "Bad client ID" );
			break;
		case 3:
			snprintf( buffer, 29, "%s", "Unavailable" );
			break;
		case 4:
			snprintf( buffer, 29, "%s", "Bad credentials" );
			break;
		case 5:
			snprintf( buffer, 29, "%s", "Unauthorized" );
			break;
		default:
			snprintf( buffer, 29, "%s", "Unknown MQTT state code" );
	}
}  // End of the lookupMQTTCode() function.


/**
 * @brief mqttCallback() will process incoming messages on subscribed topics.
 * { "command": "publishTelemetry" }
 * { "command": "changeTelemetryInterval", "value": 10000 }
 * ToDo: Add more commands for the board to react to.
 */
void mqttCallback( char *topic, byte *payload, unsigned int length )
{
	mqttCallbackCount++;
	Serial.printf( "\nMessage arrived on Topic: '%s'\n", topic );
	Serial.printf( "Callback count: '%lu'\n", mqttCallbackCount );
}  // End of the mqttCallback() function.


void setup()
{
	delay( 1000 );
	Serial.begin( 115200 );
	if( !Serial )
		delay( 1000 );
	Serial.println( "\n" );
	Serial.println( "Function setup() is beginning." );
	WiFi.onEvent( NetworkEvent );
	Serial.println( "Network callbacks configured." );
	ETH.begin();
	// This delay give the Ethernet hardware time to initialize.
	delay( 300 );

	pinMode( RX_LED, OUTPUT );
	pinMode( TX_LED, OUTPUT );

	digitalWrite( RX_LED, LED_OFF );
	digitalWrite( TX_LED, LED_OFF );
}  // End of the setup() function.


/**
 * @brief mqttConnect() will connect to the MQTT broker.
 */
void mqttConnect( const char *broker, const int port )
{
	long time = millis();
	// Connect the first time.  Avoid subtraction overflow.  Connect after cool down.
	if( lastMqttConnectionTime == 0 || ( time > mqttCoolDownInterval && ( time - mqttCoolDownInterval ) > lastMqttConnectionTime ) )
	{
		mqttConnectCount++;
		digitalWrite( RX_LED, LED_OFF );
		digitalWrite( TX_LED, LED_OFF );
		Serial.printf( "Connecting to broker at %s:%d, using client ID '%s'...\n", broker, port, HOSTNAME );
		Serial.printf( "MQTT connection count: %lu\n", mqttConnectCount );
		mqttClient.setServer( broker, port );
		mqttClient.setCallback( mqttCallback );

		// Connect to the broker, using the MAC address for a MQTT client ID.
		if( mqttClient.connect( HOSTNAME ) )
		{
			Serial.println( "Connected to MQTT Broker." );
			mqttClient.subscribe( COMMAND_TOPIC );
			digitalWrite( RX_LED, LED_ON );
			digitalWrite( TX_LED, LED_ON );
		}
		else
		{
			int mqttStateCode = mqttClient.state();
			char buffer[29];
			lookupMQTTCode( mqttStateCode, buffer );
			Serial.printf( "MQTT state: %s\n", buffer );
			Serial.printf( "MQTT state code: %d\n", mqttStateCode );

			// This block increments the broker connection "cooldown" time by 10 seconds after every failed connection, and resets it once it is over 2 minutes.
			if( mqttCoolDownInterval > 60000 )
				mqttCoolDownInterval = 0;
			mqttCoolDownInterval += 15000;
			Serial.printf( "MQTT cooldown interval: %lu\n", mqttCoolDownInterval );
			toggleLED();
		}

		lastMqttConnectionTime = millis();
	}
}  // End of the mqttConnect() function.


/**
 * @brief toggleLED() will change the state of the LED.
 * This function does not manage any timings.
 */
void toggleLED()
{
	if( digitalRead( TX_LED ) != LED_ON )
	{
		digitalWrite( RX_LED, LED_OFF );
		digitalWrite( TX_LED, LED_ON );
		Serial.println( "LED on" );
	}
	else
	{
		digitalWrite( RX_LED, LED_ON );
		digitalWrite( TX_LED, LED_OFF );
		Serial.println( "LED off" );
	}
}  // End of toggleLED() function.


/**
 * @brief printTelemetry() will print the telemetry to the serial port.
 */
void printTelemetry()
{
	Serial.println();
	printCount++;
	Serial.println( __FILE__ );
	Serial.printf( "Print count %ld\n", printCount );
	Serial.println();

	if( eth_connected )
	{
		Serial.println( "Ethernet stats:" );
		Serial.printf( "  MAC address: %s\n", macAddress );
		Serial.printf( "  IP address: %s\n", ipAddress );
		Serial.printf( "  Duplex: %s\n", duplex );
		Serial.printf( "  Link speed: %u Mbps\n", linkSpeed );
		Serial.printf( "  Network callback count: %lu\n", networkCallbackCount );
		Serial.println();
	}

	Serial.println( "MQTT stats:" );
	Serial.printf( "  mqttConnectCount: %u\n", mqttConnectCount );
	Serial.printf( "  mqttCoolDownInterval: %lu\n", mqttCoolDownInterval );
	Serial.printf( "  Broker: %s:%d\n", BROKER_ADDRESS, BROKER_PORT );
	Serial.printf( "  Client ID: %s\n", HOSTNAME );
	char buffer[29];
	lookupMQTTCode( mqttClient.state(), buffer );
	Serial.printf( "  MQTT state: %s\n", buffer );
	Serial.printf( "  Publish count: %lu\n", publishCount );
	Serial.printf( "  MQTT callback count: %lu\n", mqttCallbackCount );
	Serial.println();
}  // End of the printTelemetry() function.


void loop()
{
	if( !mqttClient.connected() )
		mqttConnect( BROKER_ADDRESS, BROKER_PORT );
	else
		mqttClient.loop();

	unsigned int currentTime = millis();
	// Avoid overflow.  Print every interval.
	if( currentTime > printInterval && ( currentTime - printInterval ) > lastPrintTime )
	{
		toggleLED();
		printTelemetry();
		lastPrintTime = millis();
	}

	currentTime = millis();
	// Publish only if connected.  Publish the first time.  Avoid overflow.  Publish every interval.
	if( ( eth_connected && mqttClient.connected() ) && ( lastPublishTime == 0 || ( currentTime > publishInterval && ( currentTime - publishInterval ) > lastPublishTime ) ) )
	{
		publishCount++;
		char valueBuffer[25] = "";
		snprintf( valueBuffer, 25, "%lu", publishCount );
		if( mqttClient.publish( PUBLISH_TOPIC, valueBuffer ) )
			Serial.printf( "Successfully published to '%s' to '%s'\n", valueBuffer, PUBLISH_TOPIC );
		if( mqttClient.publish( MAC_TOPIC, macAddress ) )
			Serial.printf( "Successfully published to '%s' to '%s'\n", macAddress, MAC_TOPIC );
		if( mqttClient.publish( IP_TOPIC, ipAddress ) )
			Serial.printf( "Successfully published to '%s' to '%s'\n", ipAddress, IP_TOPIC );
		snprintf( valueBuffer, 25, "%lu", networkCallbackCount );
		if( mqttClient.publish( CALLBACK_COUNT_TOPIC, valueBuffer ) )
			Serial.printf( "Successfully published to '%s' to '%s'\n", valueBuffer, CALLBACK_COUNT_TOPIC );
		snprintf( valueBuffer, 25, "%lu", mqttCallbackCount );
		if( mqttClient.publish( MQTT_CALLBACK_COUNT_TOPIC, valueBuffer ) )
			Serial.printf( "Successfully published to '%s' to '%s'\n", valueBuffer, MQTT_CALLBACK_COUNT_TOPIC );
		Serial.printf( "Next publish in %u seconds.\n\n", publishInterval / 1000 );
		lastPublishTime = millis();
	}
}  // End of the loop() function.
