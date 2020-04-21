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
 * @version 1.1.0
 */
#include <Arduino.h>
#include <Wire.h>
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
	Wire.beginTransmission(_ee_address);
	Wire.write((int)(address >> 8)); // MSB
	Wire.write((int)(address & 0xFF)); // LSB
    delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside a for loop meses some values
	if (Wire.endTransmission() == 0) {
		Wire.requestFrom(_ee_address, 1);
        delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside a for loop meses some values
		if(Wire.available()) {
			rdata = (byte) Wire.read();
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
bool uEEPROMLib::eeprom_read(const unsigned int address, byte *data, const uint16_t n) {
	unsigned int _address = address;
	byte * _data = data;
	uint16_t remaining = n;
	bool ret = true;
	while (remaining > 0 && ret) {
		if (remaining < 32) {
			ret = _eeprom_read_sub(_address, (data + n - remaining), (uint8_t) remaining);
			remaining = 0;
		} else {
			ret = _eeprom_read_sub(_address, (data + n - remaining), 32);
			remaining -= 32;
			_address += 32;
			_data += 32;
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
bool uEEPROMLib::_eeprom_read_sub(const unsigned int address, byte *data, const uint8_t n) {
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
 		        delay(uEEPROMLIB_WIRE_SHORT_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
            	uEEPROMLIB_YIELD
				// Added to wait if needed but cut after a failure (timeout)
            	for (j = 0; j < 255 && !Wire.available(); j++) {
	 		        delay(uEEPROMLIB_WIRE_SHORT_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
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
	Wire.beginTransmission(_ee_address);
	Wire.write((int)(address >> 8)); // MSB
	Wire.write((int)(address & 0xFF)); // LSB
	Wire.write(data);
	uEEPROMLIB_YIELD
	delay(uEEPROMLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
	return Wire.endTransmission() == 0;
}

/**
 * \brief Write one block to EEPROM
 *
 * Internal INSECURE function to write up to 16 byte blocks (arduino has a 32 byte buffer but it includes 2-byte address
 *
 * @param address Address inside EEPROM to write to
 * @param data byte to write
 * @return true if successful
 */
bool uEEPROMLib::_eeprom_write_sub(const unsigned int address, byte *data, const uint8_t n) {
	uint8_t idx = 0;
	uEEPROMLIB_YIELD
	Wire.beginTransmission(_ee_address);
	Wire.write((int)(address >> 8)); // MSB
	Wire.write((int)(address & 0xFF)); // LSB
	for (; idx < n; idx++) {
		Wire.write(*(data + idx));
		uEEPROMLIB_YIELD
	}
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
bool uEEPROMLib::eeprom_write(const unsigned int address, void *data, const uint16_t n) {
	bool r = true;
	byte *dataptr;

	uint8_t sublen;
	uint16_t act;
	uint16_t i = 0;


	if (n == 0) {
		r = false;
	} else if (n == 1) {
		_eeprom_write(address, (byte) *((byte *) data));
	} else {
		dataptr = (byte *) data;
		// head part:
		if (address % 16 != 0) {
			r = _eeprom_write_sub(address, dataptr, (uint8_t) (address % 16));
			i = address % 16;
			dataptr += address % 16;
		}
		// 16 byte middle parts
		for (; r && i + 16 <= n; i += 16, dataptr += 16) {
			r &= _eeprom_write_sub(address + i, dataptr, 16);
		}
		// tail, if any
		if (r && i < n) {
			r &= _eeprom_write_sub(address, dataptr, (uint8_t) (n - i));
		}

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


