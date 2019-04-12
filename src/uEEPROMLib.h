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
#ifndef UEEPROMLIB
	#define UEEPROMLIB
	#include "Arduino.h"
	#include "Wire.h"

	#define UEEPROMLIB_ADDRESS 0x57

	// ESP8266 yield function
	#if ARDUINO_ARCH_ESP8266
		#define uEEPROMLIB_YIELD yield();
	#else
		#define uEEPROMLIB_YIELD
	#endif

	// Wire delay
	#define uEEPROMLIB_WIRE_DELAY 5



	class uEEPROMLib {
		public:
			// Constructors
			uEEPROMLib();
			uEEPROMLib(const int);
			uEEPROMLib(bool);
			uEEPROMLib(bool, const int);

			void set_address(const uint8_t);
			// EEPROM read functions
			bool eeprom_read(const unsigned int, byte *, const uint8_t);
			template <typename TR> void eeprom_read(const unsigned int, TR *);
			byte eeprom_read(const unsigned int);
			// EEPROM write functions
			bool eeprom_write(const unsigned int, void *, const uint8_t);
			bool eeprom_write(const unsigned int, char);
			bool eeprom_write(const unsigned int, unsigned char);
 			template <typename TW> bool eeprom_write(const unsigned int, const TW);
		private:
			// Adresses
			int _ee_address = UEEPROMLIB_ADDRESS;
			// EEPROM read and write private functions - works with bytes
			byte _eeprom_read(const unsigned int);
			bool _eeprom_write(const unsigned int, const byte);
			// Fix for 1st write error
			bool init = false;
			#define uEEPROMLIB_STM32_INIT_FIX() { if (!init) { init = true;  _eeprom_read(0); delay(10); } }
	};


	// Templates must be here because Arduino compiler incoptability to declare them on .cpp fil

	/**
	 * Write any datatype to EEPROM address
	 *
	 * @param unsigned int address Address inside EEPROM to write to
	 * @param data <typename> data to write
	 */
	template <typename TW> bool uEEPROMLib::eeprom_write(const unsigned int address, const TW data) {
		return eeprom_write(address, (void *) &data, sizeof(TW));
	}


	/**
	 * Read any datatype from EEPROM address
	 *
	 * @param unsigned int address Address inside EEPROM to read from
	 * @return <typename> read data
	 */
	template <typename TR> void uEEPROMLib::eeprom_read(const unsigned int address, TR *data) {
		eeprom_read(address, (byte *) data, sizeof(TR));
	}
#endif


