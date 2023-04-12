cmake_minimum_required( VERSION 3.24 )
project( WT32_ETH01_MQTT )

set( CMAKE_CXX_STANDARD 17 )

include_directories( .
		"C:/Code/Arduino/libraries"
		"C:/Code/Arduino/libraries/ArduinoJson"
		"C:/Code/Arduino/libraries/ArduinoJson/src"
		"C:/Code/Arduino/libraries/ArduinoJson/Array"
		"C:/Code/Arduino/libraries/ArduinoJson/Collection"
		"C:/Code/Arduino/libraries/ArduinoJson/Deserialization"
		"C:/Code/Arduino/libraries/ArduinoJson/Document"
		"C:/Code/Arduino/libraries/ArduinoJson/Json"
		"C:/Code/Arduino/libraries/ArduinoJson/Memory"
		"C:/Code/Arduino/libraries/ArduinoJson/Misc"
		"C:/Code/Arduino/libraries/ArduinoJson/Serialization"
		"C:/Code/Arduino/libraries/ArduinoJson/Strings"
		"C:/Code/Arduino/libraries/ArduinoJson/src/ArduinoJson/Document"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.7/libraries/Ethernet/src"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.7/libraries/Ethernet"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.7/tools/sdk/esp32/include/esp_eth/include"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.7/cores/esp32"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.7/libraries/ESPmDNS/src"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.7/libraries/WiFi/src"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.7/libraries/ArduinoOTA/src"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.7/tools/sdk/esp32/include/esp_wifi/include"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/tools/xtensa-esp32-elf-gcc/gcc8_4_0-esp-2021r2-patch3/xtensa-esp32-elf/include/"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/tools/xtensa-esp32-elf-gcc/gcc8_4_0-esp-2021r2-patch3/xtensa-esp32-elf/include/c++/8.4.0/"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/tools/xtensa-esp32-elf-gcc/gcc8_4_0-esp-2021r2-patch3/xtensa-esp32-elf/"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.7/tools/sdk/esp32/include/esp32-camera/driver/include"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.7/tools/sdk/esp32/include/esp_common/include"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.7/tools/sdk/esp32/include/log/include"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.7/tools/sdk/esp32/include/esp_system/include"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.7/tools/sdk/esp32/include/hal/include/hal"
		"C:/Users/Adam Howell/AppData/Local/Arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/avr/include/"
		"C:/Program Files (x86)/Arduino/hardware/arduino/avr/cores/arduino/"
		"C:/Program Files (x86)/Arduino/hardware/tools/avr/lib/gcc/avr/7.3.0/include/"
		)

add_executable( WT32_ETH01_MQTT
		WT32_ETH01_MQTT.ino
		WT32_ETH01_MQTT.h
		Ethernet.ino
		Ethernet.h )

set_target_properties( WT32_ETH01_MQTT PROPERTIES LINKER_LANGUAGE CXX )
