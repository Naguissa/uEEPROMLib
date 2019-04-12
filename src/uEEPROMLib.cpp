/**
 * I2C EEPROM library. Split from uRTCLib https://github.com/Naguissa/uRTCLib
 *
 * This library controls any I2C EEPROM, independent ones or incorporated on DS1307 or DS3231 RTCs.
 *
 * @copyright Naguissa
 * @author Naguissa
 * @email naguissa@foroelectro.net
 * @url https://www.foroelectro.net/librerias-arduino-ide-f29/ueepromlib-arduino-libreria-simple-y-eficaz-para-e-t225.html
 * @url https://github.com/Naguissa/uEEPROMLib
 * @version 1.0.1
 * @created 2018-07-27
 */

#include <Arduino.h>
#include <Wire.h>
#include "uEEPROMLib.h"

/**
 * Constructor
 */
uEEPROMLib::uEEPROMLib() { }


/**
 * Constructor
 *
 * @param bool skipInit Set true to skip Wire.init (needed for STM32, SAM and Arduino, at least)
 */
uEEPROMLib::uEEPROMLib(bool skipInit) {
	init = skipInit;
}




/**
 * Constructor
 *
 * @param int ee_address I2C address of EEPROM
 */
uEEPROMLib::uEEPROMLib(const int ee_address) {
	_ee_address = ee_address;
}


/**
 * Constructor
 *
 * @param bool skipInit Set true to skip Wire.init (needed for STM32, SAM and Arduino, at least)
 * @param int ee_address I2C address of EEPROM
 */
uEEPROMLib::uEEPROMLib(bool skipInit, const int ee_address) {
	init = skipInit;
	_ee_address = ee_address;
}


/**
 * Sets EEPROM i2c addres
 *
 * @param int addr EEPROM i2C address
 */
void uEEPROMLib::set_address(const uint8_t addr) {
	_ee_address = addr;
}


/**
 * Read one byte
 *
 * @param unsigned int address Address inside EEPROM to read from
 * @return char read byte
 */
byte uEEPROMLib::_eeprom_read(const unsigned int address) {
	uEEPROMLIB_STM32_INIT_FIX()
	uEEPROMLIB_YIELD
	byte rdata = 0xFF;
	Wire.beginTransmission(_ee_address);
	Wire.write((int)(address >> 8)); // MSB
	Wire.write((int)(address & 0xFF)); // LSB
    delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
	if (Wire.endTransmission() == 0) {
		Wire.requestFrom(_ee_address, 1);
        delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
		if(Wire.available()) {
			rdata = (byte) Wire.read();
            delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
		}
	}
	uEEPROMLIB_YIELD
	return rdata;
}

/**
 * Read sequence of n bytes. Optionally from offset
 *
 * @param unsigned int address Address inside EEPROM to read from
 * @param byte* data Pointer to where read data to
 * @param uint8_t n number of bytes to read
 * @return  Bool    true if bytes read are the same as requested
 */
bool uEEPROMLib::eeprom_read(const unsigned int address, byte *data, const uint8_t n) {
    bool ret = false;
	uEEPROMLIB_STM32_INIT_FIX()
	uEEPROMLIB_YIELD
	Wire.beginTransmission(_ee_address);
	Wire.write((int)(address >> 8)); // MSB
	Wire.write((int)(address & 0xFF)); // LSB
    delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
	if (Wire.endTransmission() == 0) {
		Wire.requestFrom(_ee_address, (int) n);
        delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
		if(Wire.available()) {
			byte i = 0, j;
            for (; i < n && Wire.available(); i++) {
                *(data + i) = (byte) Wire.read();
 		        delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
            	uEEPROMLIB_YIELD
				// Added to wait if needed but cut after a failure (timeout)
            	for (j = 0; j < 255 && !Wire.available(); j++) {
	 		        delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
		        	uEEPROMLIB_YIELD
				}
            }
            ret = (i == n);
		}
	}
	uEEPROMLIB_YIELD
	return ret;
}


/**
 * Read a byte from EEPROM address
 *
 * @param unsigned int address Address inside EEPROM to read from
 * @return byte read data
 */
byte uEEPROMLib::eeprom_read(const unsigned int address) {
	return _eeprom_read(address);
}

/**
 * Write one byte to EEPROM
 *
 * @param unsigned int address Address inside EEPROM to write to
 * @param byte data byte to write
 * @return bool true if successful
 */
bool uEEPROMLib::_eeprom_write(const unsigned int address, const byte data) {
	uEEPROMLIB_YIELD
	Wire.beginTransmission(_ee_address);
	Wire.write((int)(address >> 8)); // MSB
	Wire.write((int)(address & 0xFF)); // LSB
	Wire.write(data);
	uEEPROMLIB_YIELD
	delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
	return Wire.endTransmission() == 0;
}


/**
 * Write sequence of n bytes
 *
 * @param address uint initial addesss to write to
 * @param data *byte pointer to data to write (without offset)
 * @param n uint8_t number of bytes to write
 * @return bool true if successful
 */
bool uEEPROMLib::eeprom_write(const unsigned int address, void *data, const uint8_t n) {
	bool r = true;
	uint8_t i;
	for (i = 0; i < n; i++) {
		r &= _eeprom_write(address + i, (byte) *(((byte *) data) + i));
	}
	return r;
}


/**
 * Write one byte to EEPROM address
 *
 * Template specialization come to cpp file instead h file
 *
 * @param unsigned int address Address inside EEPROM to write to
 * @param data char data to write
 */
bool uEEPROMLib::eeprom_write(const unsigned int address, char data) {
	return _eeprom_write(address, data);
}


/**
 * Write one byte to EEPROM address
 *
 * Template specialization come to cpp file instead h file
 *
 * @param unsigned int address Address inside EEPROM to write to
 * @param data unsigned char data to write
 */
bool uEEPROMLib::eeprom_write(const unsigned int address, unsigned char data) {
	return _eeprom_write(address, data);
}


