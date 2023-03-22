#include "PubSubClient.h"
#include <Ethernet.h>
#include <SPI.h>
#include <WebServer_WT32_ETH01.h>

#define RX_LED 5
#define TX_LED 17


/**
* Ethernet settings
*/
#define HOME
#ifdef WORK
// Select the IP address according to your work network.
IPAddress myIP( 10, 250, 250, 237 );
IPAddress myGW( 10, 250, 250, 1 );
IPAddress mySN( 255, 255, 255, 0 );
const char *brokerAddress = "10.250.250.205";
#else
// Select the IP address according to your home network.
IPAddress myIP( 192, 168, 55, 211 );
IPAddress myGW( 192, 168, 55, 1 );
IPAddress mySN( 255, 255, 255, 0 );
const char *brokerAddress = "192.168.55.200";
#endif// WORK
const unsigned int brokerPort        = 1883;
const char *commandTopic             = "test/wt32eth01/commands";
const char *publishTopic             = "test/wt32eth01";
const char *clientId                 = "wt32eth01";
unsigned long lastMqttConnectionTime = 0;
unsigned long mqttCoolDownInterval   = 7000;
unsigned long lastPublishTime        = 0;
unsigned long publishInterval        = 20000;
unsigned long publishCount           = 0;
unsigned long mqttConnectCount       = 0;
unsigned long callbackCount          = 0;
IPAddress myDNS( 8, 8, 8, 8 );// Google DNS Server IP


EthernetClient ethClient;
PubSubClient mqttClient( ethClient );

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
}// End of the lookupMQTTCode() function.


/**
 * @brief mqttCallback() will process incoming messages on subscribed topics.
 * { "command": "publishTelemetry" }
 * { "command": "changeTelemetryInterval", "value": 10000 }
 * ToDo: Add more commands for the board to react to.
 */
void mqttCallback( char *topic, byte *payload, unsigned int length )
{
	callbackCount++;
	Serial.printf( "\nMessage arrived on Topic: '%s'\n", topic );
	Serial.printf( "Callback count: '%lu'\n", callbackCount );
}// End of the mqttCallback() function.


void setup()
{
	Serial.begin( 115200 );

	Serial.print( "\nStarting BasicHttpClient on " + String( ARDUINO_BOARD ) );
	Serial.println( " with " + String( SHIELD_TYPE ) );
	Serial.println( WEBSERVER_WT32_ETH01_VERSION );

	// To be called before ETH.begin()
	WT32_ETH01_onEvent();
	ETH.begin( ETH_PHY_ADDR, ETH_PHY_POWER );
	ETH.config( myIP, myGW, mySN, myDNS );

	WT32_ETH01_waitForConnect();
	Serial.print( F( "HTTP EthernetWebServer is @ IP : " ) );
	Serial.println( ETH.localIP() );

	pinMode( RX_LED, OUTPUT );
	pinMode( TX_LED, OUTPUT );

	digitalWrite( RX_LED, LOW );
	digitalWrite( TX_LED, LOW );
}


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
		digitalWrite( RX_LED, 0 );
		Serial.printf( "Connecting to broker at %s:%d...\n", broker, port );
		mqttClient.setServer( broker, port );
		mqttClient.setCallback( mqttCallback );

		// Connect to the broker, using the MAC address for a MQTT client ID.
		if( mqttClient.connect( clientId ) )
		{
			Serial.println( "Connected to MQTT Broker." );
			mqttClient.subscribe( commandTopic );
			digitalWrite( RX_LED, 1 );
		}
		else
		{
			int mqttStateCode = mqttClient.state();
			char buffer[29];
			lookupMQTTCode( mqttStateCode, buffer );
			Serial.printf( "MQTT state: %s\n", buffer );
			Serial.printf( "MQTT state code: %d\n", mqttStateCode );

			// This block increments the broker connection "cooldown" time by 10 seconds after every failed connection, and resets it once it is over 2 minutes.
			if( mqttCoolDownInterval > 120000 )
				mqttCoolDownInterval = 0;
			mqttCoolDownInterval += 10000;
		}

		lastMqttConnectionTime = millis();
	}
}// End of the mqttConnect() function.


void loop()
{
	if( !mqttClient.connected() )
		mqttConnect( brokerAddress, brokerPort );
	else
		mqttClient.loop();

	unsigned int currentTime = millis();
	// Publish only if connected.  Publish the first time.  Avoid subtraction overflow.  Publish every interval.
	if( mqttClient.connected() && ( lastPublishTime == 0 || ( currentTime > publishInterval && ( currentTime - publishInterval ) > lastPublishTime ) ) )
	{
		publishCount++;
		char valueBuffer[25] = "";
		snprintf( valueBuffer, 25, "%lu", publishCount );
		Serial.printf( "Publishing '%s' to '%s'\n", valueBuffer, publishTopic );
		mqttClient.publish( publishTopic, valueBuffer );
		Serial.printf( "Next publish in %u seconds.\n\n", publishInterval / 1000 );
		lastPublishTime = millis();
	}
}
