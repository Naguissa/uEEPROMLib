/**
 * \class uEEPROMLib
 * \brief I2C EEPROM library. Split from uRTCLib.
 *
 * This library controls any I2C EEPROM, independent ones or incorporated on DS1307 or DS3231 RTCs.
 *
 * @see <a href="https://github.com/Naguissa/uEEPROMLib">https://github.com/Naguissa/uEEPROMLib</a>
 * @see <a href="https://www.foroelectro.net/librerias-arduino-ide-f29/ueepromlib-arduino-libreria-simple-y-eficaz-para-e-t225.html">https://www.foroelectro.net/librerias-arduino-ide-f29/ueepromlib-arduino-libreria-simple-y-eficaz-para-e-t225.html</a>
 * @see <a href="mailto:naguissa@foroelectro.net">naguissa@foroelectro.net</a>
 * @see <a href="https://github.com/Naguissa/uRTCLib">https://github.com/Naguissa/uRTCLib</a>
 * @version 1.2.0
 */
#include <Arduino.h>
	#ifndef UEEPROMLIB_WIRE
		#if defined(ARDUINO_attiny) || defined(ARDUINO_AVR_ATTINYX4) || defined(ARDUINO_AVR_ATTINYX5) || defined(ARDUINO_AVR_ATTINYX7) || defined(ARDUINO_AVR_ATTINYX8) || defined(ARDUINO_AVR_ATTINYX61) || defined(ARDUINO_AVR_ATTINY43) || defined(ARDUINO_AVR_ATTINY828) || defined(ARDUINO_AVR_ATTINY1634) || defined(ARDUINO_AVR_ATTINYX313)
			#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI
			#define UEEPROMLIB_WIRE TinyWireM
		#else
			#include <Wire.h>
			#define UEEPROMLIB_WIRE Wire
		#endif
	#endif
#include "uEEPROMLib.h"

/**
 * \brief Constructor
 */
uEEPROMLib::uEEPROMLib() { }


/**
 * \brief Constructor
 *
 * @param skipInit Set true to skip Wire.init (needed for STM32, SAM and Arduino, at least)
 */
uEEPROMLib::uEEPROMLib(bool skipInit) {
	init = skipInit;
}




/**
 * \brief Constructor
 *
 * @param addr I2C address of EEPROM
 */
uEEPROMLib::uEEPROMLib(const int addr) {
	_ee_address = addr;
}


/**
 * \brief Constructor
 *
 * @param skipInit Set true to skip Wire.init (needed for STM32, SAM and Arduino, at least)
 * @param addr I2C address of EEPROM
 */
uEEPROMLib::uEEPROMLib(bool skipInit, const int addr) {
	init = skipInit;
	_ee_address = addr;
}


/**
 * \brief Sets EEPROM i2c addres
 *
 * @param skipInit Set true to skip Wire.init (needed for STM32, SAM and Arduino, at least)
 * @param addr I2C address of EEPROM
 */
void uEEPROMLib::set_address(const uint8_t addr) {
	_ee_address = addr;
}


/**
 * \brief Read one byte
 *
 * @param address Address inside EEPROM to read from
 * @return read byte
 */
byte uEEPROMLib::_eeprom_read(const unsigned int address) {
	uEEPROMLIB_STM32_INIT_FIX()
	uEEPROMLIB_YIELD
	byte rdata = 0xFF;
	UEEPROMLIB_WIRE.beginTransmission(_ee_address);
	UEEPROMLIB_WIRE.write((int)(address >> 8)); // MSB
	UEEPROMLIB_WIRE.write((int)(address & 0xFF)); // LSB
    delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside a for loop meses some values
	if (UEEPROMLIB_WIRE.endTransmission() == 0) {
		UEEPROMLIB_WIRE.requestFrom(_ee_address, 1);
        delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside a for loop meses some values
		if(UEEPROMLIB_WIRE.available()) {
			rdata = (byte) UEEPROMLIB_WIRE.read();
            delay(uEEPROMLIB_WIRE_SHORT_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside a for loop meses some values
		}
	}
	uEEPROMLIB_YIELD
	return rdata;
}

/**
 * \brief Read sequence of n bytes.
 *
 * Public function, can read any arbitrary data length
 *
 * @param address Address inside EEPROM to read from
 * @param data Pointer to where read data to
 * @param number of bytes to read
 * @return true if bytes read are the same as requested
 */
bool uEEPROMLib::eeprom_read(const unsigned int address, byte *data, const unsigned int n) {
	unsigned int _address = address;
	byte * _data = data;
	unsigned int remaining = n;
	bool ret = true;
	while (remaining > 0 && ret) {
		if (remaining <= UEEPROMLIB_WIRE_MAX_RBUFFER) {
			ret = _eeprom_read_sub(_address, (data + n - remaining), remaining);
			remaining = 0;
		} else {
			ret = _eeprom_read_sub(_address, (data + n - remaining), UEEPROMLIB_WIRE_MAX_RBUFFER);
			remaining -= UEEPROMLIB_WIRE_MAX_RBUFFER;
			_address += UEEPROMLIB_WIRE_MAX_RBUFFER;
			_data += UEEPROMLIB_WIRE_MAX_RBUFFER;
		}
	}
	return ret;
}



/**
 * \brief Read sequence of n bytes.
 *
 * Private function to read chunks of 32 bytes maximum
 *
 * @param address Address inside EEPROM to read from
 * @param data Pointer to where read data to
 * @param number of bytes to read
 * @return true if bytes read are the same as requested
 */
bool uEEPROMLib::_eeprom_read_sub(const unsigned int address, byte *data, uint8_t n) {
    bool ret = false;
	byte temp = 0;
	uEEPROMLIB_STM32_INIT_FIX()
	uEEPROMLIB_YIELD
	UEEPROMLIB_WIRE.beginTransmission(_ee_address);
	UEEPROMLIB_WIRE.write((int)(address >> 8)); // MSB
	UEEPROMLIB_WIRE.write((int)(address & 0xFF)); // LSB
    delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
	if (UEEPROMLIB_WIRE.endTransmission() == 0) {
		UEEPROMLIB_WIRE.requestFrom(_ee_address, (int) n);
        delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
		if(UEEPROMLIB_WIRE.available()) {
			uint8_t i = 0, j;
            for (; i < n && UEEPROMLIB_WIRE.available(); i++) {
				temp = (byte) UEEPROMLIB_WIRE.read();
                *(data + i) = temp;
 		        delay(uEEPROMLIB_WIRE_SHORT_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
            	uEEPROMLIB_YIELD
				// Added to wait if needed but cut after a failure (timeout)
            	for (j = 0; j < 255 && !UEEPROMLIB_WIRE.available(); j++) {
	 		        delay(uEEPROMLIB_WIRE_SHORT_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for loop meses some values
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
 * \brief Read a byte from EEPROM address
 *
 * @param address Address inside EEPROM to read from
 * @return Read data
 */
byte uEEPROMLib::eeprom_read(const unsigned int address) {
	return _eeprom_read(address);
}

/**
 * \brief Write one byte to EEPROM
 *
 * @param address Address inside EEPROM to write to
 * @param data byte to write
 * @return true if successful
 */
bool uEEPROMLib::_eeprom_write(const unsigned int address, const byte data) {
	uEEPROMLIB_YIELD
	UEEPROMLIB_WIRE.beginTransmission(_ee_address);
	UEEPROMLIB_WIRE.write((int)(address >> 8)); // MSB
	UEEPROMLIB_WIRE.write((int)(address & 0xFF)); // LSB
	UEEPROMLIB_WIRE.write(data);
	uEEPROMLIB_YIELD
	delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
	return UEEPROMLIB_WIRE.endTransmission() == 0;
}

/**
 * \brief Write one block to EEPROM
 *
 * Internal INSECURE function to write up to n byte blocks (arduino has a 32 byte buffer but it includes 2-byte address
 *
 * @param address Address inside EEPROM to write to
 * @param data byte to write
 * @return true if successful
 */
bool uEEPROMLib::_eeprom_write_sub(const unsigned int address, byte *data, const uint8_t n) {
	uint8_t idx = 0;
	uEEPROMLIB_YIELD
	UEEPROMLIB_WIRE.beginTransmission(_ee_address);
	UEEPROMLIB_WIRE.write((int)(address >> 8)); // MSB
	UEEPROMLIB_WIRE.write((int)(address & 0xFF)); // LSB
	for (; idx < n; idx++) {
		UEEPROMLIB_WIRE.write(*(data + idx));
		uEEPROMLIB_YIELD
	}
	delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
	return UEEPROMLIB_WIRE.endTransmission() == 0;
}


/**
 * \brief Write sequence of n bytes
 *
 * @param address initial addesss to write to
 * @param data pointer to data to write (without offset)
 * @param n number of bytes to write
 * @return true if successful
 */
bool uEEPROMLib::eeprom_write(const unsigned int address, void *data, const unsigned int n = 0) {
	bool r = true;
	byte *dataptr;

	uint8_t len = 0;
	unsigned int temp_address = address;
	uint16_t bytes_not_written = n;
	uint16_t page_bytes_remaining = 0;

	if (n == 0) {
		r = false;
	} else if (n == 1) {
		r = _eeprom_write(address, (byte) *((byte *) data));
	} else {
		dataptr = (byte *) data;
		while (bytes_not_written > 0) {
			// Calculate remaining bytes in current page from the point of the current
			// address offset. Remember, each page is 32 bytes per the datasheet
			page_bytes_remaining = (page_size - (temp_address % page_size) );

			len = (page_bytes_remaining < bytes_not_written ? page_bytes_remaining : bytes_not_written);
			if (len > UEEPROMLIB_WIRE_MAX_WBUFFER) {
				len = UEEPROMLIB_WIRE_MAX_WBUFFER;
			}

//			Serial.print("Writing [");
//			Serial.print(len, DEC);
//			Serial.print("] bytes in address [");
//			Serial.print(temp_address, DEC);
//			Serial.println("].");

			// Write it
			r &= _eeprom_write_sub(temp_address, dataptr, len);
			bytes_not_written -= len;
			temp_address += len;
			dataptr += len;
		} // end while loop
	}

	return r;

} // end eeprom_write


/**
 * \brief Write one byte to EEPROM address
 *
 * @param address Address inside EEPROM to write to
 * @param data data to write
 * @return true if successful
 */
bool uEEPROMLib::eeprom_write(const unsigned int address, char data) {
	return _eeprom_write(address, data);
}


/**
 * \brief Write one byte to EEPROM address
 *
 * @param address Address inside EEPROM to write to
 * @param data data to write
 * @return true if successful
 */
bool uEEPROMLib::eeprom_write(const unsigned int address, unsigned char data) {
	return _eeprom_write(address, data);
}
