/**
 * This program will use the Ethernet port on a WT32-ETH01 devkit to publish telemetry to a MQTT broker.
 */
#include "Ethernet.h"
#include "PubSubClient.h"
#include "WT32_ETH01_MQTT.h"


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
	if( eth_connected && !mqttClient.connected() )
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
