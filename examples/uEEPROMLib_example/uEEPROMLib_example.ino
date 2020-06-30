/**
 * I2C EEPROM library example. Split from uRTCLib https://github.com/Naguissa/uRTCLib
 *
 * This library controls any I2C EEPROM, independent ones or incorporated on DS1307 or DS3231 RTCs.
 *
 * @copyright Naguissa
 * @author Naguissa
 * @email naguissa@foroelectro.net
 * @url https://www.foroelectro.net/librerias-arduino-ide-f29/ueepromlib-arduino-libreria-simple-y-eficaz-para-e-t225.html
 * @url https://github.com/Naguissa/uEEPROMLib
 * @version 1.0.0
 * @created 2018-07-27
 */
#include "Arduino.h"
#include "Wire.h"
#include "uEEPROMLib.h"


// uEEPROMLib eeprom;
uEEPROMLib eeprom(0x57);



unsigned int pos;

void setup() {
delay (2000);
	Serial.begin(115200);
    Serial.println("Serial OK");

	delay(2500);
	Serial.println("Delay OK");

	#ifdef ARDUINO_ARCH_ESP8266
		Wire.begin(4, 5); // D3 and D4 on ESP8266
	#else
		Wire.begin();
	#endif


#ifdef ARDUINO_ARCH_AVR
	int inttmp = 32123;
#else
	// too long for AVR 16 bits!
	int inttmp = 24543557;
#endif
	float floattmp = 3.1416;
	char chartmp = 'A';

  char c_string[128] = "abcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()ABCDEFGHIJKLMNOPQ";
  int string_length = strlen(c_string);
  
	// Write single char at address 
	if (!eeprom.eeprom_write(8, chartmp)) {
	Serial.println("Failed to store CHAR");
	} else {
	Serial.println("CHAR correctly stored");
	}

	// Write a long string of chars FROM position 33 which isn't aligned to the 32 byte pages of the EEPROM
	if (!eeprom.eeprom_write(33, (byte *) c_string, strlen(c_string))) {
	Serial.println("Failed to store STRING");
	} else {
	Serial.println("STRING correctly stored");
	}

	// Write an int
	if (!eeprom.eeprom_write(0, inttmp)) {
		Serial.println("Failed to store INT");
	} else {
		Serial.println("INT correctly stored");
	}

	// write a float
	if (!eeprom.eeprom_write(4, floattmp)) {
		Serial.println("Failed to store FLOAT");
	} else {
		Serial.println("FLOAT correctly stored");
	}

	// Flush
	inttmp = floattmp = chartmp = 0;

	Serial.print("C string length is: "); Serial.println(string_length, DEC);
	memset(c_string,0,sizeof(c_string));

	
	Serial.print("int: ");
	eeprom.eeprom_read(0, &inttmp);
	Serial.println(inttmp);

	Serial.print("float: ");
	eeprom.eeprom_read(4, &floattmp);
	Serial.println((float) floattmp);

	Serial.print("char: ");
	eeprom.eeprom_read(8, &chartmp);
	Serial.println(chartmp);

	Serial.print("chararray: ");
	eeprom.eeprom_read(33, (byte *) c_string, string_length);
	Serial.println(c_string);

	Serial.println();
	Serial.println("Printing value of each EEPROM address in HEX....");


	pos = 0;
 
}

void loop() {
  
	Serial.print(pos);
	Serial.print(": 0x");
	Serial.print(eeprom.eeprom_read(pos), HEX);

	Serial.println();

	pos++;
	delay(500);

}