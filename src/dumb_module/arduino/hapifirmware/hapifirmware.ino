/*
#*********************************************************************
#Copyright 2016 Maya Culpa, LLC
#
#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.
#*********************************************************************
*/


//**** Begin Board Selection Section ****
//#define RTU_ENET          // Define for Arduino 2560 via Ethernet shield
#define RTU_USB           // Define for Arduino 2560 via USB
//#define RTU_UNO           // Define for Arduino UNO via USB
//#define RTU_ESP           // Define for NodeMCU, or ESP8266-based device

// Required for ESP (WiFi) connection
//#define HAPI_SSID "your_ssid"
//#define HAPI_PWD  "your_pwd"

//**** Begin Board Selection Section ****


#include <DHT.h>
#include <SPI.h>
#ifdef RTU_ESP
#include <ESP8266WiFi.h>
#endif
#ifdef RTU_ENET
#include <Ethernet.h>
#endif
#include <stdlib.h>
#include <math.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

enum pin_mode_enum {
  UNUSED_PIN, // or reserved
  DIGITAL_INPUT_PIN,
  DIGITAL_INPUT_PULLUP_PIN,
  DIGITAL_OUTPUT_PIN,
  ANALOG_OUTPUT_PIN,
  ANALOG_INPUT_PIN
};

struct pin_config_struct {
  pin_mode_enum mode;
  int default_value;
};

#ifdef RTU_ENET
#define NUM_DIGITAL 54    // Number of digital I/O pins
#define PIN_MAP_SIZE NUM_DIGITAL*2   // Array size for default digital state data
                                     // 2 bytes per digital I/O pin, 1st byte = State, 2nd byte = Value

// Default pin allocation
#define ONE_WIRE_BUS 8   // Reserved pin for 1-Wire bus
#define PH_SENSORPIN A1  // Reserved pin for pH probe
#define DHTTYPE DHT22    // Sets DHT type
#define DHTPIN 12        // Reserved pin for DHT-22 sensor

// Analog input pins are assumed to be used as analog input pins
const struct pin_config_struct pin_configurations[] = {
  // digital
  {UNUSED_PIN,               LOW }, //  0
  {UNUSED_PIN,               LOW }, //  1
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  2
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  3
  {UNUSED_PIN,               LOW }, //  4
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  5
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  6
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  7
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  8
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  9
  {UNUSED_PIN,               LOW }, // 10
  {DIGITAL_INPUT_PULLUP_PIN, LOW }, // 11
  {DIGITAL_INPUT_PIN,        LOW }, // 12
  {DIGITAL_OUTPUT_PIN,       LOW }, // 13
  {UNUSED_PIN,               LOW }, // 14
  {UNUSED_PIN,               LOW }, // 15
  {UNUSED_PIN,               LOW }, // 16
  {UNUSED_PIN,               LOW }, // 17
  {UNUSED_PIN,               LOW }, // 18
  {UNUSED_PIN,               LOW }, // 19
  {UNUSED_PIN,               LOW }, // 20
  {UNUSED_PIN,               LOW }, // 21
  {DIGITAL_OUTPUT_PIN,       LOW }, // 22
  {DIGITAL_OUTPUT_PIN,       LOW }, // 23
  {DIGITAL_OUTPUT_PIN,       LOW }, // 24
  {DIGITAL_OUTPUT_PIN,       LOW }, // 25
  {DIGITAL_OUTPUT_PIN,       LOW }, // 26
  {DIGITAL_OUTPUT_PIN,       LOW }, // 27
  {DIGITAL_INPUT_PIN,        LOW }, // 28
  {DIGITAL_INPUT_PIN,        LOW }, // 29
  {DIGITAL_INPUT_PIN,        LOW }, // 30
  {DIGITAL_INPUT_PIN,        LOW }, // 31
  {DIGITAL_INPUT_PIN,        LOW }, // 32
  {DIGITAL_INPUT_PIN,        LOW }, // 33
  {DIGITAL_INPUT_PIN,        LOW }, // 34
  {DIGITAL_INPUT_PIN,        LOW }, // 35
  {DIGITAL_INPUT_PIN,        LOW }, // 36
  {DIGITAL_INPUT_PIN,        LOW }, // 37
  {DIGITAL_INPUT_PIN,        LOW }, // 38
  {DIGITAL_INPUT_PIN,        LOW }, // 39
  {DIGITAL_INPUT_PIN,        LOW }, // 40
  {DIGITAL_INPUT_PIN,        LOW }, // 41
  {DIGITAL_INPUT_PIN,        LOW }, // 42
  {DIGITAL_INPUT_PIN,        LOW }, // 43
  {DIGITAL_INPUT_PIN,        LOW }, // 44
  {DIGITAL_INPUT_PIN,        LOW }, // 45
  {DIGITAL_INPUT_PIN,        LOW }, // 46
  {DIGITAL_INPUT_PIN,        LOW }, // 47
  {DIGITAL_INPUT_PULLUP_PIN, LOW }, // 48
  {DIGITAL_INPUT_PULLUP_PIN, LOW }, // 49
  {UNUSED_PIN,               LOW }, // 50
  {UNUSED_PIN,               LOW }, // 51
  {UNUSED_PIN,               LOW }, // 52
  {UNUSED_PIN,               LOW }, // 53
  // analog
  {ANALOG_INPUT_PIN,         LOW }, // 54
  {ANALOG_INPUT_PIN,         LOW }, // 55
  {ANALOG_INPUT_PIN,         LOW }, // 56
  {ANALOG_INPUT_PIN,         LOW }, // 57
  {ANALOG_INPUT_PIN,         LOW }, // 58
  {ANALOG_INPUT_PIN,         LOW }, // 59
  {ANALOG_INPUT_PIN,         LOW }, // 60
  {ANALOG_INPUT_PIN,         LOW }, // 61
  {ANALOG_INPUT_PIN,         LOW }, // 62
  {ANALOG_INPUT_PIN,         LOW }, // 63
  {ANALOG_INPUT_PIN,         LOW }, // 64
  {ANALOG_INPUT_PIN,         LOW }, // 65
  {UNUSED_PIN,               LOW }, // 66
  {UNUSED_PIN,               LOW }, // 67
  {UNUSED_PIN,               LOW }, // 68
  {UNUSED_PIN,               LOW }  // 69
};
#endif


#ifdef RTU_USB
#define NUM_DIGITAL 54    // Number of digital I/O pins
#define PIN_MAP_SIZE NUM_DIGITAL*2   // Array size for default digital state data
                                     // 2 bytes per digital I/O pin, 1st byte = State, 2nd byte = Value

// Default pin allocation
#define ONE_WIRE_BUS 8   // Reserved pin for 1-Wire bus
#define PH_SENSORPIN A1  // Reserved pin for pH probe
#define DHTTYPE DHT22    // Sets DHT type
#define DHTPIN 12        // Reserved pin for DHT-22 sensor

// Analog input pins are assumed to be used as analog input pins
const struct pin_config_struct pin_configurations[] = {
  // digital
  {UNUSED_PIN,               LOW }, //  0
  {UNUSED_PIN,               LOW }, //  1
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  2
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  3
  {UNUSED_PIN,               LOW }, //  4
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  5
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  6
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  7
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  8
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  9
  {UNUSED_PIN,               LOW }, // 10
  {DIGITAL_INPUT_PULLUP_PIN, LOW }, // 11
  {DIGITAL_INPUT_PIN,        LOW }, // 12
  {DIGITAL_OUTPUT_PIN,       LOW }, // 13
  {UNUSED_PIN,               LOW }, // 14
  {UNUSED_PIN,               LOW }, // 15
  {UNUSED_PIN,               LOW }, // 16
  {UNUSED_PIN,               LOW }, // 17
  {UNUSED_PIN,               LOW }, // 18
  {UNUSED_PIN,               LOW }, // 19
  {UNUSED_PIN,               LOW }, // 20
  {UNUSED_PIN,               LOW }, // 21
  {DIGITAL_OUTPUT_PIN,       LOW }, // 22
  {DIGITAL_OUTPUT_PIN,       LOW }, // 23
  {DIGITAL_OUTPUT_PIN,       LOW }, // 24
  {DIGITAL_OUTPUT_PIN,       LOW }, // 25
  {DIGITAL_OUTPUT_PIN,       LOW }, // 26
  {DIGITAL_OUTPUT_PIN,       LOW }, // 27
  {DIGITAL_INPUT_PIN,        LOW }, // 28
  {DIGITAL_INPUT_PIN,        LOW }, // 29
  {DIGITAL_INPUT_PIN,        LOW }, // 30
  {DIGITAL_INPUT_PIN,        LOW }, // 31
  {DIGITAL_INPUT_PIN,        LOW }, // 32
  {DIGITAL_INPUT_PIN,        LOW }, // 33
  {DIGITAL_INPUT_PIN,        LOW }, // 34
  {DIGITAL_INPUT_PIN,        LOW }, // 35
  {DIGITAL_INPUT_PIN,        LOW }, // 36
  {DIGITAL_INPUT_PIN,        LOW }, // 37
  {DIGITAL_INPUT_PIN,        LOW }, // 38
  {DIGITAL_INPUT_PIN,        LOW }, // 39
  {DIGITAL_INPUT_PIN,        LOW }, // 40
  {DIGITAL_INPUT_PIN,        LOW }, // 41
  {DIGITAL_INPUT_PIN,        LOW }, // 42
  {DIGITAL_INPUT_PIN,        LOW }, // 43
  {DIGITAL_INPUT_PIN,        LOW }, // 44
  {DIGITAL_INPUT_PIN,        LOW }, // 45
  {DIGITAL_INPUT_PIN,        LOW }, // 46
  {DIGITAL_INPUT_PIN,        LOW }, // 47
  {DIGITAL_INPUT_PULLUP_PIN, LOW }, // 48
  {DIGITAL_INPUT_PULLUP_PIN, LOW }, // 49
  {UNUSED_PIN,               LOW }, // 50
  {UNUSED_PIN,               LOW }, // 51
  {UNUSED_PIN,               LOW }, // 52
  {UNUSED_PIN,               LOW }, // 53
  // analog
  {ANALOG_INPUT_PIN,         LOW }, // 54
  {ANALOG_INPUT_PIN,         LOW }, // 55
  {ANALOG_INPUT_PIN,         LOW }, // 56
  {ANALOG_INPUT_PIN,         LOW }, // 57
  {ANALOG_INPUT_PIN,         LOW }, // 58
  {ANALOG_INPUT_PIN,         LOW }, // 59
  {ANALOG_INPUT_PIN,         LOW }, // 60
  {ANALOG_INPUT_PIN,         LOW }, // 61
  {ANALOG_INPUT_PIN,         LOW }, // 62
  {ANALOG_INPUT_PIN,         LOW }, // 63
  {ANALOG_INPUT_PIN,         LOW }, // 64
  {ANALOG_INPUT_PIN,         LOW }, // 65
  {UNUSED_PIN,               LOW }, // 66
  {UNUSED_PIN,               LOW }, // 67
  {UNUSED_PIN,               LOW }, // 68
  {UNUSED_PIN,               LOW }  // 69
};
#endif


#ifdef RTU_UNO
#define NUM_DIGITAL 14    // Number of digital I/O pins
#define PIN_MAP_SIZE NUM_DIGITAL*2   // Array size for default digital state data
                                     // 2 bytes per digital I/O pin, 1st byte = State, 2nd byte = Value

// Default pin allocation
#define ONE_WIRE_BUS 8   // Reserved pin for 1-Wire bus
#define PH_SENSORPIN A1  // Reserved pin for pH probe
#define DHTTYPE DHT22    // Sets DHT type
#define DHTPIN 12        // Reserved pin for DHT-22 sensor
#define THERMISTOR 2     // Analog Read Temperature

// Analog input pins are assumed to be used as analog input pins
const struct pin_config_struct pin_configurations[] = {
  // digital
  {UNUSED_PIN,               LOW }, //  0
  {UNUSED_PIN,               LOW }, //  1
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  2
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  3
  {DIGITAL_OUTPUT_PIN,       LOW }, //  4
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  5
  {DIGITAL_OUTPUT_PIN,       HIGH}, //  6
  {UNUSED_PIN,               HIGH}, //  7
  {DIGITAL_INPUT_PULLUP_PIN, HIGH}, //  8
  {DIGITAL_INPUT_PIN,        HIGH}, //  9
  {DIGITAL_INPUT_PIN,        LOW }, // 10
  {DIGITAL_INPUT_PIN,        LOW }, // 11
  {DIGITAL_INPUT_PULLUP_PIN, LOW }, // 12
  {DIGITAL_OUTPUT_PIN,       LOW }, // 13
  // analog
  {ANALOG_INPUT_PIN,         LOW }, // 14
  {ANALOG_INPUT_PIN,         LOW }, // 15
  {ANALOG_INPUT_PIN,         LOW }, // 16
  {ANALOG_INPUT_PIN,         LOW }, // 17
  {ANALOG_INPUT_PIN,         LOW }, // 18
  {ANALOG_INPUT_PIN,         LOW }  // 19
};
#endif

#ifdef RTU_ESP
#define NUM_DIGITAL 17    // Number of digital I/O pins
#define PIN_MAP_SIZE NUM_DIGITAL*2   // Array size for default state data
                                     // 2 bytes per digital I/O pin, 1st byte = State, 2nd byte = Value

// Default pin allocation
#define ONE_WIRE_BUS 13   // Reserved pin for 1-Wire bus
#define PH_SENSORPIN 14   // Reserved pin for pH probe
#define DHTTYPE DHT22     // Sets DHT type
#define DHTPIN 12         // Reserved pin for DHT-22 sensor

// Analog input pins are assumed to be used as analog input pins
const struct pin_config_struct pin_configurations[] = {
  // digital i/o
  {DIGITAL_OUTPUT_PIN, HIGH}, //  0
  {DIGITAL_OUTPUT_PIN, HIGH}, //  1
  {DIGITAL_OUTPUT_PIN, HIGH}, //  2
  {DIGITAL_INPUT_PIN,  HIGH}, //  3
  {DIGITAL_OUTPUT_PIN, HIGH}, //  4
  {DIGITAL_OUTPUT_PIN, HIGH}, //  5
  {UNUSED_PIN,         LOW }, //  6
  {UNUSED_PIN,         LOW }, //  7
  {UNUSED_PIN,         LOW }, //  8
  {UNUSED_PIN,         LOW }, //  9
  {UNUSED_PIN,         LOW }, // 10
  {UNUSED_PIN,         LOW }, // 11
  {DIGITAL_OUTPUT_PIN, HIGH}, // 12
  {DIGITAL_OUTPUT_PIN, HIGH}, // 13
  {DIGITAL_OUTPUT_PIN, HIGH}, // 14
  {DIGITAL_OUTPUT_PIN, HIGH}, // 15
  {DIGITAL_OUTPUT_PIN, HIGH}, // 16
  // analog
  {ANALOG_INPUT_PIN,   5   }  // 17 // A0 input0
};
#endif

#define NUM_ANALOG (ARRAY_LENGTH(pin_configurations) - NUM_DIGITAL) // Number of analog I/O pins

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature wp_sensors(&oneWire);

//**** Begin Main Variable Definition Section ****
String HAPI_CLI_VERSION = "v2.2";  // The version of the firmware the RTU is running
#ifdef RTU_ENET
String RTUID = "RTU001";             // This RTUs Unique ID Number - unique across site
#endif
#ifdef RTU_USB
String RTUID = "RTU101";             // This RTUs Unique ID Number - unique across site
#endif
#ifdef RTU_ESP
String RTUID = "RTU301";             // This RTUs Unique ID Number - unique across site
#endif
#ifdef RTU_UNO
String RTUID = "RTU201";             // This RTUs Unique ID Number - unique across site
#endif
idle_mode = false;         // a boolean representing the idle mode of the RTU
boolean metric = true;             // should values be returned in metric or US customary units
String inputString = "";           // A string to hold incoming data
String inputCommand = "";          // A string to hold the command
String inputPort = "";             // A string to hold the port number of the command
String inputControl = "";          // A string to hold the requested action of the command
String inputTimer = "0";           // A string to hold the length of time to activate a control
boolean stringComplete = false;    // A boolean indicating when received string is complete (a \n was received)
//**** End Main Variable Definition Section ****

//**** Begin Communications Section ****
// the media access control (ethernet hardware) address for the shield:
byte mac[] = { 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 };
#ifdef RTU_ENET
EthernetServer rtuServer = EthernetServer(80);
EthernetClient client;
#endif
#ifdef RTU_ESP
const char *ssid = HAPI_SSID;
const char *password = HAPI_PWD;
int WiFiStatus = 0;
WiFiServer rtuServer = WiFiServer(80);
WiFiClient client;
#endif
//**** End Communications Section ****


//Define the Reset function
void (*resetFunc)(void) = 0; //declare reset function @ address 0

//**** Begin DHT Device Section ****
//Define DHT devices and allocate resources
DHT dht1(DHTPIN, DHT22); //For each DHT, create a new variable given the pin and Type
DHT dhts[] = {dht1};

//**** Begin Custom Functions Section ****
//Custom functions are special functions for reading sensors or controlling devices. They are
//used when setting or a reading a pin isn't enough, as in the instance of library calls.
#define CUSTOM_FUNCTIONS 5 //The number of custom functions supported on this RTU

typedef float (* GenericFP)(int); //generic pointer to a function that takes an int and returns a float

struct FuncDef {   //define a structure to associate a Name to generic function pointer.
  String fName;
  String fType;
  int fPort;
  GenericFP fPtr;
};

//Create a FuncDef for each custom function
//Format: abbreviation, context, pin, function
FuncDef func1 = {"tmp", "dht", -1, &readTemperature};
FuncDef func2 = {"hmd", "dht", -1, &readHumidity};
FuncDef func3 = {"trm", "thermistor", 2, &readThermistorTemp};
FuncDef func4 = {"res1tmp", "DS18B20", ONE_WIRE_BUS, &readWaterTemperature};
FuncDef func5 = {"phl", "pH Sensor", PH_SENSORPIN, &readpH};

FuncDef functions[CUSTOM_FUNCTIONS] = {func1, func2, func3, func4, func5};
//**** End Custom Functions Section ****

String getPinArray() {
  // Returns all pin configuration information
  String response = "";
  for (int i = 0; i < ARRAY_LENGTH(pin_configurations); i++) {
    if (i < NUM_DIGITAL) {
      response += String(i) + String(pin_configurations[i].mode);
    }
    else {
      response += "A" + String(i - NUM_DIGITAL) + String(pin_configurations[i].mode);
    }
  }
  return response;
}

void assembleResponse(String &responseString, String varName, String value) {
  // Helper function for building response strings
  if (responseString.equals("")) {
    responseString = "{";
  }

  if (!varName.equals("")) {
    responseString += "\"" + varName + "\"" + ":" + "\"" + value + "\"" + ",";
  }
  else {
    if (responseString.endsWith(",")) {
      responseString = responseString.substring(0, responseString.length() - 1);
    }
    responseString += "}";
  }
}

void writeLine(String response, boolean EOL) {
  // Writes a response line to the network connection

  char inChar;

  for (int i = 0; i < response.length(); i++) {
    inChar = (char)response.charAt(i);
#ifdef RTU_ENET
    rtuServer.write(inChar);
#endif
#ifdef RTU_ESP
    rtuServer.write(inChar);
#endif
#ifdef RTU_USB
    Serial.write(inChar);
#endif
#ifdef RTU_UNO
    Serial.write(inChar);
#endif
  }
  if ((String)inChar != "\n") {
    if (EOL) {
#ifdef RTU_ENET
      rtuServer.write(inChar);
#endif
#ifdef RTU_ESP
      rtuServer.write(inChar);
#endif
#ifdef RTU_USB
      Serial.write(inChar);
#endif
#ifdef RTU_UNO
      Serial.write(inChar);
#endif
    }
  }
}


float readHumidity(int iDevice) {
  // readHumidity  - Uses the DHT Library to read the current humidity
  float returnValue;
  float h;
  //h = dhts[iDevice].readHumidity();
  h = dht1.readHumidity();

  if (isnan(h)) {
    returnValue = (-1);
  }
  else {
    returnValue = h;
  }
  return returnValue;
}

float readTemperature(int iDevice) {
  // readTemperature  - Uses the DHT Library to read the current temperature
  float returnValue;
  float h;
  //h = dhts[iDevice].readTemperature();
  h = dht1.readTemperature();

  if (isnan(h)) {
    returnValue = (-1);
  }
  else {
    returnValue = h;
    if (!metric) {
      returnValue = (returnValue * 9.0)/ 5.0 + 32.0; // Convert Celsius to Fahrenheit
    }
  }
  return returnValue;
}

float readWaterTemperature(int iDevice) {
  // readWaterTemperature  - Uses the Dallas Temperature library to read the waterproof temp sensor
  float returnValue;
  wp_sensors.requestTemperatures();
  returnValue = wp_sensors.getTempCByIndex(0);
  if (isnan(returnValue)) {
    returnValue = (-1);
  }
  else {
    if (!metric) {
      returnValue = (returnValue * 9.0)/ 5.0 + 32.0; // Convert Celsius to Fahrenheit
    }
  }
  return returnValue;
}

float readpH(int iDevice) {
  // readpH - Reads pH from an analog pH sensor (Robot Mesh SKU: SEN0161, Module version 1.0)
  unsigned long int avgValue;  //Store the average value of the sensor feedback
  float b;
  int buf[10], temp;

  for (int i = 0; i < 10; i++) { //Get 10 sample value from the sensor for smooth the value
    buf[i] = analogRead(iDevice);
    delay(10);
  }
  for (int i = 0; i < 9; i++) { //sort the analog from small to large
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)               //take the average value of 6 center sample
    avgValue += buf[i];
  float phValue = (float)avgValue * 5.0 / 1024 / 6; //convert the analog into millivolt
  phValue = 3.5 * phValue;                  //convert the millivolt into pH value
  return phValue;
}

float readThermistorTemp(int iDevice) {
  // Simple code to read a temperature value from a 10k thermistor with a 10k pulldown resistor
  float Temp;
  int RawADC = analogRead(iDevice);

  Temp = log(10000.0*((1024.0/RawADC-1)));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp -= 273.15;            // Convert Kelvin to Celsius
  if (!metric) {
     Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celsius to Fahrenheit
  }

  return Temp;
}

#ifdef RTU_USB
String getCommand() {
#endif
#ifdef RTU_UNO
String getCommand() {
#endif
#ifdef RTU_ENET
String getCommand(EthernetClient client) {
#endif
#ifdef RTU_ESP
String getCommand(WiFiClient client) {
#endif

  // Retrieves a command from the cuurent serial or network connection
  stringComplete = false;
  char inChar;
  inputString = "";

#if defined(RTU_USB) || defined(RTU_UNO)
  while (Serial.available() > 0 && !stringComplete) {
    inChar = (char)Serial.read();  // read the bytes incoming from the client:
#elif defined(RTU_ENET) || defined(RTU_ESP)
  while (client.available() > 0 && !stringComplete) {
    inChar = (char)client.read();  // read the bytes incoming from the client:
#endif
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
    delay(2);                       // small delay to receive any further characters
  }

  Serial.println(inputString);
  return inputString;
}

String buildResponse() {
  // Assembles a response with values from pins and custom functions
  // Returns a JSON string  ("pinnumber":value,"custom function abbreviation":value}
  String response = "buildResponse\r\n";
  assembleResponse(response, "name", RTUID);
  assembleResponse(response, "version", HAPI_CLI_VERSION);
  //  assembleResponse(response, "lastcmd", lastCommand);
  //Process digital pins
  for (int i = 0; i < NUM_DIGITAL; i++) {
    switch (pin_configurations[i].mode) {
    case DIGITAL_INPUT_PIN:
    case DIGITAL_INPUT_PULLUP_PIN:
    case DIGITAL_OUTPUT_PIN:
    case ANALOG_OUTPUT_PIN: // ^^^ does not jive with NUM_DIGITAL
      assembleResponse(response, (String)i, (String)digitalRead(i));
      break;
    default:
      break;
    }
  }

  //Process analog pins
  for (int i = 0; i < NUM_ANALOG; i++) {
    assembleResponse(response, (String)(i + NUM_DIGITAL), (String)analogRead(i));
  }

  //Process custom functions
  FuncDef f;
  float funcVal = (-1.0);
  String funcStr = "";
  String tempVal;
  char cFuncVal[10];
  String str;

  for (int i = 0; i < CUSTOM_FUNCTIONS; i++) {
    f = functions[i];

    if (f.fType.equals("dht")) {
      for (int j = 0; j < ARRAY_LENGTH(dhts); j++) {
        funcVal = f.fPtr(j);
        assembleResponse(response, f.fName, String((int)funcVal));
      }
    }
    else {
      funcVal = f.fPtr(f.fPort);
      dtostrf(funcVal, 4, 3, cFuncVal);
      str = cFuncVal;
      assembleResponse(response, f.fName, str);
    }
  }

  assembleResponse(response, "", ""); //closes up the response string
  return response;
}

String getStatus() {
  // Returns the current status of the RTU itself
  // Includes firmware version, MAC address, IP Address, Free RAM and Idle Mode
  String retval = "getStatus\r\n";
  String macstring = (char*)mac;

  retval = RTUID + "\r\n";
  retval += "Firmware " + HAPI_CLI_VERSION + "\r\n";
  Serial.println(retval);
#ifdef RTU_USB
  retval += "Connected on USB\r\n";
#endif
#ifdef RTU_UNO
  retval += "Connected on USB\r\n";
#endif
#ifdef RTU_ENET
  retval += "MAC=";
  retval += "0x" + String(mac[0], HEX) + ":";
  retval += "0x" + String(mac[1], HEX) + ":";
  retval += "0x" + String(mac[2], HEX) + ":";
  retval += "0x" + String(mac[3], HEX) + ":";
  retval += "0x" + String(mac[4], HEX) + ":";
  retval += "0x" + String(mac[5], HEX) + "\n";
  Serial.println(retval);
  retval += "IP=";
  retval += String(Ethernet.localIP()[0]) + ".";
  retval += String(Ethernet.localIP()[1]) + ".";
  retval += String(Ethernet.localIP()[2]) + ".";
  retval += String(Ethernet.localIP()[3]) + "\n";
  Serial.println(retval);
#endif
#ifdef RTU_ESP
  retval += "MAC=";
  retval += "0x" + String(mac[0], HEX) + ":";
  retval += "0x" + String(mac[1], HEX) + ":";
  retval += "0x" + String(mac[2], HEX) + ":";
  retval += "0x" + String(mac[3], HEX) + ":";
  retval += "0x" + String(mac[4], HEX) + ":";
  retval += "0x" + String(mac[5], HEX) + "\n";
  Serial.println(retval);
  retval += "IP=";
  retval += String(WiFi.localIP()[0]) + ".";
  retval += String(WiFi.localIP()[1]) + ".";
  retval += String(WiFi.localIP()[2]) + ".";
  retval += String(WiFi.localIP()[3]) + "\n";
  Serial.println(retval);
#endif

  retval += "Digital= " + String(NUM_DIGITAL) + "\n";
  retval += "Analog= " + String(NUM_ANALOG) + "\n";
  retval += "Free SRAM: " + String(freeRam()) + "k\n";

  retval += "Idle Mode: " + (idle_mode ? "True" : "False");

  return retval;
}


int freeRam() {
#ifdef RTU_ESP
  // Gets free ram on the ESP8266
  return ESP.getFreeHeap();
#else
  // Gets free ram on the Arduino
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
#endif
}

void setup() {

  // Initialize pins
  for (int i = 0; i < ARRAY_LENGTH(pin_configurations); i++) {
    switch (pin_configurations[i].mode) {
    case DIGITAL_INPUT_PIN:
      pinMode(i, INPUT);
      break;
    case DIGITAL_INPUT_PULLUP_PIN:
      pinMode(i, INPUT_PULLUP);
      break;
    case DIGITAL_OUTPUT_PIN:
      pinMode(i, OUTPUT);
      digitalWrite(i, pin_configurations[i].default_value);
      break;
    case ANALOG_OUTPUT_PIN:
      pinMode(i, OUTPUT);
      break;
    default:
      break;
    }
  }

  dht1.begin(); // Start the DHT-22
  /*for (int i = 0; i < ARRAY_LENGTH(dhts); i++) {
    dhts[i].begin();
  }*/

  wp_sensors.begin(); // Start the DS18B20

  inputString.reserve(200);  // reserve 200 bytes for the inputString:

  Serial.begin(115200);

#ifdef RTU_ENET
  Serial.println("Initializing network....");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to obtain IP address  ...");
  } else {
    Serial.print("IP address: ");
    Serial.println(Ethernet.localIP());
  }
  rtuServer.begin();
#endif
#ifdef RTU_ESP
  Serial.println("Initializing WiFi network....");
  WiFiStatus = WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.macAddress(mac);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    rtuServer.begin();
#endif

  Serial.println("Starting communications  ...");
  Serial.println(getStatus()); //Send Status (incl. IP Address) to the Serial Monitor
  Serial.println("Setup Complete. Listening for connections.");

}

void loop() {
#if defined(RTU_USB) || defined(RTU_UNO)
  if (Serial.available()) {
    inputString = getCommand();
#elif defined(RTU_ENET) || defined(RTU_ESP)
  // Wait for a new client to connect
  client = rtuServer.available();
  if (client) {
    inputString = getCommand(client);
#endif
    inputString.trim();
    inputString.toLowerCase();
    inputTimer = "0";

    if (inputString != "" && inputString != "\r\n") {
      inputCommand = inputString.substring(0, 3);
      boolean cmdFound = false;

      Serial.println(inputCommand);

      if (inputCommand == "aoc" && !idle_mode) {
        cmdFound = true;
        inputPort = inputString.substring(3, 6);
        inputControl = inputString.substring(6, 9);
        if (pin_configurations[inputPort.toInt()].mode == ANALOG_INPUT_PIN) {
          analogWrite(inputPort.toInt(), inputControl.toInt());
        } // END OF if pin_configurations == ANALOG_INPUT_PIN
      }  // END Of aoc

      // doc (Digital Output Control) Sets a single digital output
      if (inputCommand == "doc" && !idle_mode) {
        cmdFound = true;
        inputPort = inputString.substring(4, 6);
        inputControl = inputString.substring(6, 7);
        inputTimer = inputString.substring(7, 10);
        if (pin_configurations[inputPort.toInt()].mode == DIGITAL_OUTPUT_PIN) {
          if (inputTimer.toInt() > 0) {
            int currVal = digitalRead(inputPort.toInt());
            digitalWrite(inputPort.toInt(), inputControl.toInt());
            delay(inputTimer.toInt() * 1000);
            digitalWrite(inputPort.toInt(), currVal);
          }
          else {
            digitalWrite(inputPort.toInt(), inputControl.toInt());
          }

        } // END OF if pin_configurations == DIGITAL_OUTPUT_PIN
      }  // END Of doc

      // Get pin modes
      if (inputCommand == "gpm") {
        cmdFound = true;
        String response = getPinArray();
        writeLine(response, true); //Send pin mode information back to client
      }

      // Enable/Disable Idle Mode
      if (inputCommand == "idl") {
        cmdFound = true;
        if (inputString.substring(3, 4) == "0") {
          idle_mode = false;
        }
        else if (inputString.substring(3, 4) == "1") {
          idle_mode = true;
        }
      }

      // res  - resets the Arduino
      if (inputCommand == "res" && !idle_mode) {
        cmdFound = true;
        for (int x = 0; x < ARRAY_LENGTH(pin_configurations); x++) {
          if (pin_configurations[x].mode == DIGITAL_OUTPUT_PIN) {
            digitalWrite(x, LOW);
          }
          if (pin_configurations[inputPort.toInt()].mode == ANALOG_OUTPUT_PIN) {
            analogWrite(x, 0);
          }
        }
        delay(100);
        resetFunc();  //call reset
      }

      // Get the RTU Status
      if (inputCommand == "sta") {
        cmdFound = true;
        writeLine(getStatus(), true);
      }

      String response = buildResponse();
      writeLine(response, true);
#ifdef RTU_ENET
      client.stop();
#endif
#ifdef RTU_ESP
      client.stop();
#endif
    }
  }
}
