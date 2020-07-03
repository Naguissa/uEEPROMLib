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
 * @version 1.2.0
 */
#include "Arduino.h"
#ifndef URTCLIB_WIRE
  #if defined(ARDUINO_attiny) || defined(ARDUINO_AVR_ATTINYX4) || defined(ARDUINO_AVR_ATTINYX5) || defined(ARDUINO_AVR_ATTINYX7) || defined(ARDUINO_AVR_ATTINYX8) || defined(ARDUINO_AVR_ATTINYX61) || defined(ARDUINO_AVR_ATTINY43) || defined(ARDUINO_AVR_ATTINY828) || defined(ARDUINO_AVR_ATTINY1634) || defined(ARDUINO_AVR_ATTINYX313)
    #include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI
    #define WIRE TinyWireM
  #else
    #include <Wire.h>
    #define WIRE Wire
  #endif
#endif
#include "uEEPROMLib.h"


#include <SoftwareSerial.h>
const int rx=3;
const int tx=4;

SoftwareSerial SSerial(rx,tx);

// uEEPROMLib eeprom;
uEEPROMLib eeprom(0x57);



unsigned int pos;

void setup() {
delay (2000);
	SSerial.begin(115200);
    SSerial.println("Serial OK");

	delay(2500);
	SSerial.println("Delay OK");

	#ifdef ARDUINO_ARCH_ESP8266
		WIRE.begin(4, 5); // D3 and D4 on ESP8266
	#else
		WIRE.begin();
	#endif


#ifdef ARDUINO_ARCH_AVR
	int inttmp = 32123;
#else
	// too long for AVR 16 bits!
	int inttmp = 24543557;
#endif
	float floattmp = 3.1416;
	char chartmp = 'A';

  char c_string[35] = "abcdefghijklmnopqrstuvwxyz12345678";
  int string_length = strlen(c_string);

	// Write single char at address
	if (!eeprom.eeprom_write(8, chartmp)) {
	SSerial.println(F("Failed to store CHAR"));
	} else {
	SSerial.println(F("CHAR correctly stored"));
	}

	// Write a long string of chars FROM position 33 which isn't aligned to the 32 byte pages of the EEPROM
	if (!eeprom.eeprom_write(33, (byte *) c_string, strlen(c_string))) {
	SSerial.println(F("Failed to store STRING"));
	} else {
	SSerial.println(F("STRING correctly stored"));
	}

	// Write an int
	if (!eeprom.eeprom_write(0, inttmp)) {
		SSerial.println(F("Failed to store INT"));
	} else {
		SSerial.println(F("INT correctly stored"));
	}

	// write a float
	if (!eeprom.eeprom_write(4, floattmp)) {
		SSerial.println(F("Failed to store FLOAT"));
	} else {
		SSerial.println(F("FLOAT correctly stored"));
	}

	// Flush
	inttmp = floattmp = chartmp = 0;

	SSerial.print(F("C string length is: ")); SSerial.println(string_length, DEC);
	memset(c_string,0,sizeof(c_string));


	SSerial.print("int: ");
	eeprom.eeprom_read(0, &inttmp);
	SSerial.println(inttmp);

	SSerial.print("float: ");
	eeprom.eeprom_read(4, &floattmp);
	SSerial.println((float) floattmp);

	SSerial.print(F("char: "));
	eeprom.eeprom_read(8, &chartmp);
	SSerial.println(chartmp);

	SSerial.print(F("chararray: "));
	eeprom.eeprom_read(33, (byte *) c_string, string_length);
	SSerial.println(c_string);

	SSerial.println();
	SSerial.println(F("Printing value of each EEPROM address in HEX...."));


	pos = 0;

}

void loop() {

	SSerial.print(pos);
	SSerial.print(F(": 0x"));
	SSerial.print(eeprom.eeprom_read(pos), HEX);

	SSerial.println();

	pos++;
	delay(500);

}