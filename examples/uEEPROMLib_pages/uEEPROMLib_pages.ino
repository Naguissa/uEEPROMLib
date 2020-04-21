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

// 305 bytes
char longMessage[310] = "I2C EEPROM library example. Split from uRTCLib\n\nThis library controls any I2C EEPROM, independent ones or incorporated on DS1307 or DS3231 RTCs.\n\nhttps://github.com/Naguissa/uEEPROMLib\nhttps://www.foroelectro.net/librerias-arduino-ide-f29/ueepromlib-arduino-libreria-simple-y-eficaz-para-e-t225.html\0";

void setup() {
delay (2000);
	Serial.begin(9600);
    Serial.println("Serial OK");

	delay(2500);
	Serial.println("Delay OK");

	#ifdef ARDUINO_ARCH_ESP8266
		Wire.begin(0, 2); // D3 and D4 on ESP8266
	#else
		Wire.begin();
	#endif

	// Alligned
	if (!eeprom.eeprom_write(0, (byte *) &longMessage[0], 305)) {
		Serial.println("Failed to store aligned STRING");
	} else {
		Serial.println("Aligned STRING correctly stored");
	}

	if (!eeprom.eeprom_write(311, (byte *) &longMessage[0], 305)) {
		Serial.println("Failed to store unaligned STRING");
	} else {
		Serial.println("Unaligned STRING correctly stored");
	}

}

void loop() {
	char *readMessageA;
	char *readMessageB;
	readMessageA = (char * ) malloc(210);
	readMessageB = (char * ) malloc(210);
	Serial.println("-------------------------------------");
	Serial.println();

	Serial.println("Algined string:");
	eeprom.eeprom_read(0, (byte *) readMessageA, 305);
	Serial.println(readMessageA);

	Serial.println();

	Serial.println("Unalgined string:");
	eeprom.eeprom_read(0, (byte *) readMessageB, 305);
	Serial.println(longMessage);

	Serial.println();
}
