cmake_minimum_required( VERSION 3.24 )
project( WT32_ETH01_MQTT )

set( CMAKE_CXX_STANDARD 17 )

add_executable( WT32_ETH01_MQTT
		WT32_ETH01_MQTT.ino
		Ethernet.cpp
		Ethernet.h
		WT32_ETH01_MQTT.h )

set_target_properties( WT32_ETH01_MQTT PROPERTIES LINKER_LANGUAGE CXX )
