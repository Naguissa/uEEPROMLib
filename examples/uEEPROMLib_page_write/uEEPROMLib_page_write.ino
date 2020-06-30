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
char longMessage[512] = "I2C EEPROM library example. Split from uRTCLib\n\nThis library controls any I2C EEPROM, independent ones or incorporated on DS1307 or DS3231 RTCs.\n\nhttps://github.com/Naguissa/uEEPROMLib\nhttps://www.foroelectro.net/librerias-arduino-ide-f29/ueepromlib-arduino-libreria-simple-y-eficaz-para-e-t225.html\0";

void setup() {  
  delay (2000);
	Serial.begin(115200);
  Serial.println("Serial OK");

 
	#ifdef ARDUINO_ARCH_ESP8266
		Wire.begin(4, 5); // D3 and D4 on ESP8266
	#else
		Wire.begin();
	#endif


	#ifdef ARDUINO_ARCH_ESP8266 || ARDUINO_ARCH_ESP32
	
	byte 	    random_numbers[1027]; // write 1027 bytes of random data
	uint16_t 	random_number = 0;
	uint16_t 	running_total = 0;
  int       eeprom_start_address = 123; // random
	Serial.println("Performing FULL EEPROM write test and verify with library - page writes (32 bytes at a time)");
	
	for (int i = 0; i < sizeof(random_numbers); i++)
	{
		random_number = rand() % 10 + 1; // range from 1 to 10
		random_numbers[i] = random_number;
		running_total += random_number;
	}

	Serial.print("Magic number: "); Serial.println(running_total, DEC);	
	
	Serial.println("Writing 1kB to EEPROM.");
	if (!eeprom.eeprom_write(eeprom_start_address, (byte *) random_numbers, sizeof(random_numbers))) {
		Serial.println("Failed to write to EEPROM!");
	} 
	
	delay(1000);
	random_number = 0;
	memset(random_numbers,0, sizeof(random_numbers));	// Flush back to zero		
	Serial.println("Reading 1kB from EEPROM....");
	eeprom.eeprom_read(eeprom_start_address, (byte *) random_numbers, sizeof(random_numbers));	
	
	for (int i = 0; i < sizeof(random_numbers); i++)
	{
    //Serial.print("Read from magic number array value: "); Serial.println (random_numbers[i], DEC);
		random_number += random_numbers[i];
	}	

  Serial.print("Calculated magic number: "); Serial.println(random_number, DEC);
	
	if ( random_number == running_total)
	{
		Serial.println("Magic numbers match. EEPROM read successful.");
	}
 else
 {
    Serial.println("==> VALIDATION ERROR <==");
 }
 
	
	#endif


	

	// Aligned String vs Unaligned
	if (!eeprom.eeprom_write(0, (byte *) longMessage, strlen(longMessage))) {
		Serial.println("Failed to store aligned STRING");
	} else {
		Serial.println("Aligned STRING correctly stored");
	}

	if (!eeprom.eeprom_write(500, (byte *) longMessage, strlen(longMessage))) {
		Serial.println("Failed to store unaligned STRING");
	} else {
		Serial.println("Unaligned STRING correctly stored");
	}

  char *readMessageA;
  char *readMessageB;
  readMessageA = (char * ) malloc(512);
  readMessageB = (char * ) malloc(512);
  Serial.println("-------------------------------------");
  Serial.println();

  Serial.println("Page Aligned string (offset: 0):");
  eeprom.eeprom_read(0, (byte *) readMessageA, strlen(longMessage));
  Serial.println(readMessageA);

  Serial.println();

  Serial.println("Page Unaligned string (offset: 500):");
  eeprom.eeprom_read(500, (byte *) readMessageB, strlen(longMessage));
  Serial.println(readMessageB);

  Serial.println();

  Serial.println("-------------------------------------");
   

}

void loop() {
	
}