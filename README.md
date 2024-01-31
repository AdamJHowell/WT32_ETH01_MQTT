# WT32_ETH01_MQTT

Uses the WT32-ETH01 devkit to communicate to a MQTT broker.

## Wiring for flashing the devkit

* Connect the RX pin of the USB-to-UART to the TX pin (IO1) on the devkit.
* Connect the TX pin of the USB-to-UART to the RX pin (IO3) on the devkit.
* Connect the ground pin of the USB-to-UART to any one of the ground pins on the devkit.
* Connect either
* * the 3.3 volt output of the USB-to-UART to the 3v3 pin on the devkit.
* * the 5 volt output of the USB-to-UART to the 5v pin on the devkit.

Do NOT connect both the 3.3 volt and the 5 volt pins at the same time!
