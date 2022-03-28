/*
  Titre      : DS18B20
  Auteur     : Crepin Vardin Fouelefack
  Date       : 28/03/2022
  Description: Envoie de l'adresse de chaque capteur et sa valeur ainsi que l'adresse Mac du uC
  Version    : 0.0.1
*/

/* Read multiple DS18B20 1-wire digital temperature sensors by address. More info: https://www.makerguides.com*/
// Include the required Arduino libraries:
#include <Arduino.h>
#include "WIFI_NINA_Connector.h"
#include "MQTTConnector.h"
#include <OneWire.h>
#include <DallasTemperature.h>

String Adresse = "";
String AdresseUc = "";
float Temp = 0.00;

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 0
#define TEMPERATURE_PRECISION 9 // Lower resolution

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found


DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

// function to print the temperature for a device
float printTemperature(DeviceAddress deviceAddress)
{
  // method 1 - slower
  //Serial.print("Temp C: ");
  //Serial.print(sensors.getTempC(deviceAddress));
  //Serial.print(" Temp F: ");
  //Serial.print(sensors.getTempF(deviceAddress)); // Makes a second call to getTempC and then converts to Fahrenheit

  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  return tempC;
}

// function to print a device address
 String printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("");
    Adresse = Adresse + String(deviceAddress[i], HEX);
  }
  return Adresse;
} 

/* String printMacAdd(byte mac[])
  {
   
    for (int i = 5; i >= 0; i--) {
      if (mac[i] < 16) {
        Serial.print("0");
      }
      Serial.print(mac[i], HEX);
      AdresseUc = String(mac[i], HEX);
      if (i > 0) {
        Serial.print(":");
      }
    }
    return AdresseUc;
    Serial.println();
}                                                 Pour essayer d'envoyer le Mac du uC              */

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  wifiConnect();                  //Branchement au réseau WIFI
  MQTTConnect();                  //Branchement au broker MQTT
  
  // Start up the library
  sensors.begin();
  
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  
  // locate devices on the bus
  Serial.print("Locating devices...");
  
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Loop through each device, print out address
 for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		Serial.print("Found device ");
		Serial.print(i, DEC);
		Serial.print(" with address: ");
		printAddress(tempDeviceAddress);
		Serial.println();
		
		Serial.print("Setting resolution to ");
		Serial.println(TEMPERATURE_PRECISION, DEC);
		
		// set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
		sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
		
		Serial.print("Resolution actually set to: ");
		Serial.print(sensors.getResolution(tempDeviceAddress), DEC); 
		Serial.println();
	}else{
          Serial.print("Found ghost device at ");
          Serial.print(i, DEC);
          Serial.print(" but could not detect address. Check power and cabling");
	    }
  } 
        //Envoie de l'adresse Mac du uC
 /* AdresseUc = printMacAdd(mac[]);
		Serial.print(AdresseUc);
    delay(5000);
  appendPayload(AdresseUc, 1010);  //Ajout de la valeur de la broche A3 au message MQTT  
  sendPayload();                                   //Envoie du message via le protocole MQTT  */

}

void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  //Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //Serial.println("DONE");
  
  
  // Loop through each device, print out temperature data
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
     {
          // Output the device ID
        Adresse = printAddress(tempDeviceAddress);
          Serial.print(" ");
          
          // It responds almost immediately. Let's print out the data
        Temp =	printTemperature(tempDeviceAddress); // Use a simple function to print out the data
          Serial.print("\n");

        appendPayload(Adresse, Temp);  //Ajout de la valeur de la broche A3 au message MQTT
        sendPayload();                                   //Envoie du message via le protocole MQTT
          delay(3000);
     } 
    Adresse = "";
      //else ghost device! Check your power requirements and cabling
    
  }
}

