#include <Arduino.h>

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

HAPI Remote Terminal Unit Firmware Code V3.1.1
Authors: Tyler Reed, Mark Miller
ESP Modification: John Archbold

Sketch Date: June 29th, 2017
Sketch Version: V3.1.1
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

void setupControls(void){
  int i;
  // Initialize
}

void checkControls(void) {
  CFuncDef c;
  currentTime = now();            // Update currentTime and ..
                                  //  check all the control functions
  for (int device=0;device<ArrayLength(HapicFunctions);device++) { // For each device
    c = HapicFunctions[device];                //  initialize access structure
    c.oPtr(device);                            //  call the check function
  }
}

void control_thing(int i) {
  CFuncDef c;
  ControlData d;
  c = HapicFunctions[i];
  d = HapicData[i];

  if (d.hc_active) {                  // Is the thing on?
    if (d.hc_end > currentTime) {     // Yes, should it be turned off?
      d.hc_active = false;
      digitalWrite(d.hc_controlpin, !d.hc_polarity);
      if (d.hc_repeat != 0) {   // Is repeat active?
        d.hc_start += d.hc_repeat;
        d.hc_end += d.hc_repeat;
      }
    }
    if (c.iPtr(i) < d.hcs_offValue) { // Is the TurnOff value exceeded?
      d.hc_active = false;
      digitalWrite(d.hc_controlpin, !d.hc_polarity);
    }
  } else if (d.hc_start >= currentTime || c.iPtr(i) > d.hcs_onValue) { // Is the turnOn value exceeded?
    d.hc_active = true;        // Turn it On, thing is now on
    digitalWrite(d.hc_controlpin, d.hc_polarity);
  }
}

float controlPumps(int i) {
  control_thing(i);
}

float controlLamps(int i) {
  control_thing(i);
}

