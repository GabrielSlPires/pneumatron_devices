# Pneumatron devices
Code to construct different devices and versions of the Pneumatron. 
The most recent version, and recommended to use, is the one using the ESP8266 microcontroller.
This version allows you to use several pneumatron devices at the same time due to wireless data acquisition.

For data acquisition and analysis scripts, please visit: [github.com/GabrielSlPires/pneumatron](https://github.com/GabrielSlPires/pneumatron)

## Devices and Version

- **pneumatron_arduino_usb**: Code for the first pneumatron version, using Arduino board and collecting data via UBS directly connected to a computer.
  
- **pneumatron_esp8266_receiver**: Code for pneumatron receiver, using ESP8266 board. You need only one receiver for your setup. The receiver must be connected to a computer for data acquisition.
  
- **pneumatron_esp8266_sender_bmp**: Code for pneumatron sender, using ESP8266 board and a BMP280 pressure sensor. You can use several senders with the same receiver. Note that you must change the MAC address in the code for your receiver MAC address. And also, you must change the pneumatron ID for a unique ID in your setup.

## Arduino IDE
To work with Arduino IDE the code is available inside the `arduino-ide` folder.

The other folders are to use with PlataformIO IDE.
