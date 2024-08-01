## St4ltk3r

**St4ltk3r** is a fork of the [ESP8266-MAC-scanner]( ESP8266-MAC-scanner) project designed to transform your ESP8266 into a tracker for people and/or objects. It captures the MAC addresses of Access Points (APs) encountered by the ESP8266 and logs these values into a file using the LittleFS filesystem library.



**Proof of Concept:** https://youtu.be/-rMa3H_K3M4



### Features

- **MAC Address Capture**: Continuously scans and captures the MAC addresses of nearby Wi-Fi Access Points.
- **Logging with LittleFS**: Efficiently records captured MAC addresses into a file on the ESP8266's filesystem.
- **Portable Tracking**: Allows for tracking movement by logging which APs have been encountered over time.
- **Easy Deployment**: Simple to set up and deploy on any ESP8266 device.



### Usage

1. Power on your ESP8266 device;

2. Verify that you have all required librabries:

   - [ESP8266WiFi](https://github.com/esp8266/Arduino)

     [LittleFS](https://github.com/esp8266/Arduino/tree/master/libraries/LittleFS)

3. Upload the .ino to  your ESP8266 board;

4. The device will automatically start scanning for Wi-Fi APs and log the MAC addresses;

5. Access the logged data via the file stored on the ESP8266 using the Serial Monitor or UART;

6. In the "finder" directory, you have the script that find the latitude e longitude. Look the anoter readme file to understand the process;