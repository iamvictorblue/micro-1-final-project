# micro-1-final-project
ESP32 project for reading temperature using Siri.

Introduction

The final project for the Microprocessors I - Fall 2022 course, done in the esp32 and Node-RED. The emphasis of this project is on having the user be able ask Siri for the temperature in the room where the ESP32 is. Siri will ask if one wants the temperature reading in Fahrenheit or Celsius. After that temperature is displayed in the user’s preferred unit.
Our project uses Amazon's AWS services and a ESP32 that takes the temperature readings .


General Architecture

In our project, the ESP32 microcontroller is connected to a 2-pin thermistor. We wanted to also use a DHT22, but our professor said no. Our own version of analogRead was then implemented.
The ESP32 is placed in the room where it will be connected to the internet which will connect to a remote cloud computer that has an MQTT Broker protocol. The Broker is used to retrieve data and send data to the RESTl API, which will give the phone  access to the data used in Siri.
 
 
Tool-Set List:

1. VSCode
2. MQTT Broker
3. NodeJS
4. Node Red
5. AWS cloud servers
6. Http protocol
7. SmartNoRA (Not used in final release but heavily used during research and development phase)
8. PIO (PlatformIO) ide for ESP32 development
9. Ubuntu
10. ESP32 board
11. iOS/Android device 
 
 
Cloud Architecture

The cloud machine was created using Amazon AWS Services, where MQTT, Node js, Node-red was run on. The MQTT let us communicate with the ESP-32 faster than most other services. 



Setting up node-red

For starters, one must use a cloud service like AWS,  which allows us to create a machine on the cloud. Then, one must install Node-Red and create a SSH connection with a key to connect to the device of choice.
Update server:
sudo apt-get update
sudo apt-get upgreade

Install nodejs, npm and node-red:
sudo apt-get install nodejs
node -v 
sudo apt-get install npm
npm -v 
sudo npm install -g unsafe-perm node-red node-red-admin

We download the dependencies necessary for the connection Siri and other Voice assistants through the node red ui. After that let the automation begin.
 
 
 
 
Temperature Architecture

The calibration of certain elements of the ESP32, like the thermistor, were a little tricky since manufacturer constants needed to be used to prepare the equations that would run in our main.cpp.
The following equations were formed using manufacturer constants:
float adcValue = analogRead(PIN_ANALOG_IN_TEMPERATURE);
float voltage = (float)adcValue / 4095.0 * 3.3;                 // read ADC pin and convert to voltage
float Rt = 10 * voltage / (3.3 - voltage);                      // calculate resistance value of thermistor
double tempK = 1 / (1 / (273.15 + 25) + log(Rt / 10) / 3950.0); // calculate temperature (Kelvin)
double tempC = tempK - 273.15;                                         // calculate temperature (Celsius)
double tempF = tempC * 9 / 5 + 32;
By instantiating the variables seen above, we could calculate useful elements of the math behind our project, such as: voltage, resistance, the adcValue, and temperature.









