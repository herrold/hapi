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

#ifndef HAPIWiFi_H
#define HAPIWiFi_H

// ==============================================================
// Make sure to update this for your own WiFi and/or MQTT Broker!
// ==============================================================

#define MQTT_broker_default  "mqttbroker"    // Default hostname of mqtt broker
#define MQTT_broker_username "mqttuser"      // Required if broker security is enabled
#define MQTT_broker_password "mqttpass"
#define MQTT_port 1883

#define UDP_port 2390
#define NTP_port 123

#define HAPI_SSID "HAPInet"
#define HAPI_PWD  "HAPIconnect"

#endif //HAPIWiFi_H
