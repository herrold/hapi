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

HAPI Remote Terminal Unit Firmware Code v3.0.0
Authors: Tyler Reed, Mark Miller
ESP Modification: John Archbold

Sketch Date: May 2nd 2017
Sketch Version: v3.0.0
Implement of MQTT-based HAPInode (HN) for use in Monitoring and Control
Implements mDNS discovery of MQTT broker
Implements definitions for
  ESP-NodeMCU
  ESP8266
  WROOM32
Communications Protocol
  WiFi
Communications Method
  MQTT        Listens for messages on Port 1883
*/


//**** Begin Board Configuration Section ****

// Board Type
// ==========
//**ESP Based
// Board Type
#define HN_ESP8266
//#define HN_ESP32

// Connection Type
// ===============
#define HN_WiFi
//#define HN_ENET          // Define for Ethernet shield

// Protocol Type
// =============
#define HN_MQTT

//**** End Board Configuration Section ****

#include <DHT.h>
#include <SPI.h>
#ifdef HN_WiFi
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>        // for avahi
#include <WiFiUdp.h>            // For ntp
#endif
#ifdef HN_ENET
#include <Ethernet.h>
#include <EthernetBonjour.h>
#endif
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT brokerinclude <stdlib.h>
#include <ArduinoJson.h>  // JSON library for MQTT strings

#include <math.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Bounce2.h> // Used for "debouncing" inputs

// =====================================================================
// Make sure to update these files for your own WiFi and/or MQTT Broker!
// =====================================================================
#include "nodewifi.h"       // WiFi and MQTT setup
#include "nodeboard.h"      // Node default pin allocations

//**** Begin Main Variable Definition Section ****
int loopcount;
String HAPI_FW_VERSION = "v3.0";    // The version of the firmware the HN is running
#ifdef HN_ESP8266
String HN_base = "HN3";             // Prefix for mac address
#endif
#ifdef HN_ESP32
String HN_base = "HN4";             // Prefix for mac address
#endif

String HN_id = "HNx";              // HN address
String HN_status = "Online";

boolean idle_mode = false;         // a boolean representing the idle mode of the HN
boolean metric = true;             // should values be returned in metric or US customary units
String inputString = "";           // A string to hold incoming data
String inputCommand = "";          // A string to hold the command
String inputPort = "";             // A string to hold the port number of the command
String inputControl = "";          // A string to hold the requested action of the command
String inputTimer = "0";           // A string to hold the length of time to activate a control
boolean stringComplete = false;    // A boolean indicating when received string is complete (a \n was received)
//**** End Main Variable Definition Section ****

//**** Begin Communications Section ****
// the media access control (ethernet hardware) address for the shield
// Need to manually change this for USB, Ethernet
byte mac[] = { 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 };
char mac_str[16] = "555555555555";
char hostString[32] = {0};              // for mDNS Hostname

#ifdef HN_ENET
EthernetClient HNClient;
#endif

#ifdef HN_WiFi
// Local wifi network parameters (set in nodewifi.h)
const char* ssid = HAPI_SSID;
const char* password = HAPI_PWD;
// WiFi config
int WiFiStatus = 0;
WiFiClient HNClient;
// ntp config
IPAddress timeServerIP;               // Place to store IP address of mqttbroker.local
const char* ntpServerName = "mqttbroker";
const int NTP_PACKET_SIZE = 48;       // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE];  //buffer to hold incoming and outgoing packets
unsigned int localPort = UDP_port;    // local port to listen for UDP packets
WiFiUDP udp;                          // A UDP instance to let us send and receive packets over UDP
unsigned long epoch;

#endif
//**** End Communications Section ****

//**** Begin MQTT Section ****
const char* clientID = "HAPInode";
const char* mqtt_topic_status = "STATUS/RESPONSE";         // General Status topic
const char* mqtt_topic_asset = "ASSET/RESPONSE";           // Genral Asset topic
char mqtt_topic_command[256] = "COMMAND/";              // Command topic for this HN
const char* mqtt_topic_exception = "EXCEPTION/";        // General Exception topic

StaticJsonBuffer<128> hn_topic_exception;               // Exception data for this HN
char MQTTOutput[256];                                   // String storage for the JSON data
char MQTTInput[256];                                    // String storage for the JSON data
boolean mqtt_broker_connected = false;

// Callback function header
void MQTTcallback(char* topic, byte* payload, unsigned int length);
PubSubClient MQTTClient(HNClient);                          // 1883 is the listener port for the Broker

// Prepare JSON string objects

JsonObject& exception_topic = hn_topic_exception.createObject();
//**** End MQTT Section ****

//**** Begin Sensors Section ****
// Definitions related to sensor operations
#define SENSORID_DIO 0    // DIGITAL I/O
#define SENSORID_AIO 1    // ANALOG I/O
#define SENSORID_FN 2     // FUNCTION I/O

// Initialise the Pushbutton Bouncer object
const int ledPin = 2; // This code uses the built-in led for visual feedback that the button has been pressed
const int buttonPin = 5; // Connect your button to pin #gpio13
int ledflash = 0;
Bounce bouncer = Bounce();

// Use bouncer object to measure flow rate
Bounce flowrate = Bounce();
int WaterFlowRate = 0;

//LIGHT Devices

//oneWire Devices
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature wp_sensors(&oneWire);

//Define DHT devices and allocate resources
#define NUM_DHTS 1        //total number of DHTs on this device
#define DHTTYPE DHT22     // Sets DHT type

DHT dht1(DHT_SENSORPIN, DHT22);  //For each DHT, create a new variable given the pin and Type
DHT dhts[1] = {dht1};     //add the DHT device to the array of DHTs

//Custom functions are special functions for reading sensors or controlling devices. They are
//used when setting or a reading a pin isn't enough, as in the instance of library calls.
typedef float (* GenericFP)(int); //generic pointer to a function that takes an int and returns a float
struct FuncDef {   //define a structure to associate a Name to generic function pointer.
  String fName;
  String fType;
  String fUnit;
  int fPin;
  GenericFP fPtr;
};


//The number of custom functions supported on this HN
#define CUSTOM_FUNCTIONS 7
//Create a FuncDef for each custom function
//Format: abbreviation, context, pin, function
FuncDef func1 = {"tmp", "dht", "C", -1, &readTemperatured};
FuncDef func2 = {"hum", "dht", "%", -1, &readHumidity};

FuncDef func3 = {"lux", "light", "lux", LIGHT_SENSORPIN, &readLightSensorTemp};
FuncDef func4 = {"tmp1", "DS18B20", "C", ONE_WIRE_BUS, &read1WireTemperature};
FuncDef func5 = {"ph", "pH Sensor", "pH", PH_SENSORPIN, &readpH};
FuncDef func6 = {"tds", "TDS Sensor", "ppm", TDS_SENSORPIN, &readTDS};
FuncDef func7 = {"flow", "Flow Rate Sensor", "lpm", FLOW_SENSORPIN, &readFlow};
FuncDef HapiFunctions[CUSTOM_FUNCTIONS] = {func1, func2, func3, func4, func5, func6, func7};

//**** End Sensors Section ****

void b2c(byte* bptr, char* cptr, int len) {
  int i;
  char c;
  for (i=0; i<len; i++) {
    c = (bptr[i] >> 4) & 0x0f;
    c += '0';
    if (c > '9') c += ('A' - '9' - 1);
    *cptr++ = c;
//    Serial.print(c, HEX);
    c = bptr[i] & 0x0f;
    c += '0';
    if (c > '9') c += ('A' - '9' - 1);
    *cptr++ = c;
//    Serial.print(c, HEX);
  }
  *cptr++ = '\0';
//  Serial.println("");
}

int freeRam (){
#ifdef HN_ESP8266
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
// Switch the on-board LED off to start with
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

// TESTING
// Setup pushbutton Bouncer object
  pinMode(buttonPin, INPUT);
  bouncer.attach(buttonPin);
  bouncer.interval(5);
// end TESTING

// Start Debug port and sensors
// ============================
  setupSensors();             // Initialize I/O and start devices
  inputString.reserve(200);   // reserve 200 bytes for the inputString
  Serial.begin(115200);       // Debug port

#ifdef HN_ENET
  Serial.println("Initializing network....");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to obtain IP address  ...");
  } else
  {
    Serial.print("IP address: ");
    Serial.println(Ethernet.localIP());
  }

  //TODO
  // Bonjour support
#endif

#ifdef HN_WiFi
  Serial.println("Initializing WiFi network....");
  WiFiStatus = WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.macAddress(mac);


  b2c(&mac[3], &mac_str[0], 3);         //convert mac to ASCII value for unique station ID
  HN_id = HN_base + String(mac_str);
  HN_id.toCharArray(hostString,(HN_id.length()+1));
  
  Serial.println();
  Serial.print("IP  address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Hostname   : ");
  Serial.println(WiFi.hostname());
  Serial.println(WiFi.hostname(hostString));
  Serial.print("NewHostname: ");
  Serial.println(WiFi.hostname());
  
// Start mDNS support
// ==================

  Serial.print("HN_id:      ");
  Serial.println(HN_id);
  Serial.print("hostString: ");
  Serial.println(hostString);

 if (!MDNS.begin(hostString)) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.print("Hostname: ");
  Serial.print(hostString);
  Serial.println(" mDNS responder started");

  Serial.println("Sending mDNS query");
  int n = MDNS.queryService("workstation", "tcp"); // Send out query for workstation tcp services
  Serial.println("mDNS query done");
  if (n == 0) {
    Serial.println("no services found");
  }
  else {
    Serial.print(n);
    Serial.println(" service(s) found");
    for (int i = 0; i < n; ++i) {
      // Print details for each service found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(MDNS.hostname(i));
      Serial.print(" (");
      Serial.print(MDNS.IP(i));
      Serial.print(":");
      Serial.print(MDNS.port(i));
      Serial.println(")");
    }
  }
  Serial.println();
#endif

// Start NTP support
// =================
  Serial.println("Starting UDP");                 // Start UDP
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  WiFi.hostByName(ntpServerName, timeServerIP);   // Get mqttbroker's IP address
  Serial.print("Local IP:   ");
  Serial.println(timeServerIP);
  getNTPTime();

// Start MQTT support
// ==================
  MQTTClient.setServer(MQTT_broker_address, MQTT_port);
  MQTTClient.setCallback(MQTTcallback);

  exception_topic["AssetId"] = HN_id;

  // Wait until connected to MQTT Broker
  // client.connect returns a boolean value
  mqtt_broker_connected = false;
  Serial.println("Connecting to MQTT broker ...");
  do {
    if (sendMQTTStatus()) {
      mqtt_broker_connected = true;
    }
  } while (mqtt_broker_connected == false);

// Subscribe to the TOPICs

  while (!(MQTTClient.subscribe("STATUS/QUERY"))) {
    Serial.println(" .. subscribing to STATUS/QUERY");
    MQTTClient.loop();
    delay(100);
  }
  while (!(MQTTClient.subscribe("ASSET/QUERY"))) {
    MQTTClient.loop();
    Serial.println(" .. subscribing to ASSET/QUERY");
    delay(100);
  }
  while (!(MQTTClient.subscribe("CONFIG/QUERY"))) {
    MQTTClient.loop();
    Serial.println(" .. subscribing to CONFIG/QUERY");
    delay(100);
  }
  while (!(MQTTClient.subscribe("COMMAND/"))) {
    MQTTClient.loop();
    Serial.println(" .. subscribing to COMMAND/");
    delay(100);
  }
  while (!(MQTTClient.subscribe("EXCEPTION/"))) {
    MQTTClient.loop();
    Serial.println(" .. subscribing to EXCEPTION/");
    delay(100);
  }
  Serial.println("Setup Complete. Listening for topics ..");
}

void loop() {
  // Wait for a new event, publish topic
  // Fake this event with a push-button
  // Update button state
  // This needs to be called so that the Bouncer object can check if the button has been pressed
  bouncer.update();
  if (bouncer.rose()) {
    // Turn light on when button is pressed down
    // (i.e. if the state of the button rose from 0 to 1 (not pressed to pressed))
    digitalWrite(ledPin, LOW);
    Serial.println("Button pushed");
    sendMQTTStatus();
    sendAllMQTTAssets();
  }
  else if (bouncer.fell()) {
    // Turn light off when button is released
    // i.e. if state goes from high (1) to low (0) (pressed to not pressed)
    digitalWrite(ledPin, HIGH);
  }

  MQTTClient.loop();
  if (loopcount++ > 100) {
    if (digitalRead(ledPin) == HIGH) digitalWrite(ledPin, LOW);
    else digitalWrite(ledPin, HIGH);
    loopcount = 0;
    getNTPTime();
  }
  delay(100);
}


