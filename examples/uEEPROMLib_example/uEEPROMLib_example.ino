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
	Serial.begin(9600);
    Serial.println("Serial OK");

	delay(2500);
	Serial.println("Delay OK");

	#ifdef ARDUINO_ARCH_ESP8266
		Wire.begin(0, 2); // D3 and D4 on ESP8266
	#else
		Wire.begin();
	#endif


#ifdef ARDUINO_ARCH_AVR
	int inttmp = 32123;
#else
	// too logng for AVR 16 bits!
	int inttmp = 24543557;
#endif
	float floattmp = 3.1416;
	char chartmp = 'A';

    char string[17] = "ForoElectro.Net\0";


	// Testing template
	if (!eeprom.eeprom_write(0, inttmp)) {
		Serial.println("Failed to store INT");
	} else {
		Serial.println("INT correctly stored");
	}
	if (!eeprom.eeprom_write(4, floattmp)) {
		Serial.println("Failed to store FLOAT");
	} else {
		Serial.println("FLOAT correctly stored");
	}
	if (!eeprom.eeprom_write(8, chartmp)) {
		Serial.println("Failed to store CHAR");
	} else {
		Serial.println("CHAR correctly stored");
	}

	if (!eeprom.eeprom_write(9, (byte *) &string[0], 16)) {
		Serial.println("Failed to store STRING");
	} else {
		Serial.println("STRING correctly stored");
	}

	inttmp = 0;
	floattmp = 0;
	chartmp = 0;
    string[0] = string[1] = string[2] = string[3] = string[4] = 0;



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
	eeprom.eeprom_read(9, (byte *) &string[0], 16);
	Serial.println(string);

	Serial.println();


	for(pos = 26; pos < 1000; pos++) {
		eeprom.eeprom_write(pos, (unsigned char) (pos % 256));
	}

	pos = 0;
}

void loop() {
	Serial.print(pos);
	Serial.print(": ");
	Serial.print(eeprom.eeprom_read(pos));

	Serial.println();

	pos++;
	pos %= 1000;
	delay(1000);
}
