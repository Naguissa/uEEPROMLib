/**
 * \mainpage
 * \brief I2C EEPROM library. Split from uRTCLib.
 *
 * This library controls any I2C EEPROM, independent ones or incorporated on DS1307 or DS3231 RTCs.
 *
 * @see <a href="https://github.com/Naguissa/uEEPROMLib">https://github.com/Naguissa/uEEPROMLib</a>
 * @see <a href="https://www.foroelectro.net/librerias-arduino-ide-f29/ueepromlib-arduino-libreria-simple-y-eficaz-para-e-t225.html">https://www.foroelectro.net/librerias-arduino-ide-f29/ueepromlib-arduino-libreria-simple-y-eficaz-para-e-t225.html</a>
 * @see <a href="mailto:naguissa@foroelectro.net">naguissa@foroelectro.net</a>
 * @see <a href="https://github.com/Naguissa/uRTCLib">https://github.com/Naguissa/uRTCLib</a>
 * @version 1.2.1
 */
/** \file uEEPROMLib.h
 *   \brief uEEPROMLib header file
 */
#ifndef UEEPROMLIB
	/**
	 * \brief Prevent multiple inclussion
	 */
	#define UEEPROMLIB
	#include "Arduino.h"
	#ifndef UEEPROMLIB_WIRE
		#if defined(ARDUINO_attiny) || defined(ARDUINO_AVR_ATTINYX4) || defined(ARDUINO_AVR_ATTINYX5) || defined(ARDUINO_AVR_ATTINYX7) || defined(ARDUINO_AVR_ATTINYX8) || defined(ARDUINO_AVR_ATTINYX61) || defined(ARDUINO_AVR_ATTINY43) || defined(ARDUINO_AVR_ATTINY828) || defined(ARDUINO_AVR_ATTINY1634) || defined(ARDUINO_AVR_ATTINYX313)
			#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI
			#define UEEPROMLIB_WIRE TinyWireM
		#else
			#include <Wire.h>
			#define UEEPROMLIB_WIRE Wire
		#endif
	#endif

	/**
	 * \brief Default EEPROM address
	 *
	 * Usual address for EEPROMs on RTC modules is 0x57
	 */
	#define UEEPROMLIB_ADDRESS 0x57

	// ESP yield function (ESP32 has no need for that on dual core, but it has on single core version)
	#if ARDUINO_ARCH_ESP8266
		/**
		 * \brief ESP8266, yield to don't block ESP functionality.
		 *
		 * When this library is used in other MCUs this is simply removed by the preprocessor
		 */
		#define uEEPROMLIB_YIELD yield();
	#else
		#if ARDUINO_ARCH_ESP32
		/**
		 * \brief ESP32, yield to don't block ESP functionality.
		 *
		 * When this library is used in other MCUs this is simply removed by the preprocessor
		 */
			#define uEEPROMLIB_YIELD yield();
		#else
			#define uEEPROMLIB_YIELD
		#endif
	#endif


	/**
	 * \brief Wire delay - needed to give time to EEPROM to process Wire requests
	 */
	#define uEEPROMLIB_WIRE_DELAY 6

	/**
	 * \brief Wire very short delay - needed to give time to EEPROM to process Wire requests
	 */
	#define uEEPROMLIB_WIRE_SHORT_DELAY 1

	// BUFFER_LENGTH should come from Wire library
	 #ifdef BUFFER_LENGTH
	/**
	 * \brief Max buffer length, taken from Wire library if defined
	 */
		#define UEEPROMLIB_WIRE_MAX_RBUFFER (BUFFER_LENGTH - 2)
	#else
	/**
	 * \brief Max buffer length, failsafe if not defined in Wire library
	 */
		#define UEEPROMLIB_WIRE_MAX_RBUFFER 32
	#endif
		#define UEEPROMLIB_WIRE_MAX_WBUFFER (UEEPROMLIB_WIRE_MAX_RBUFFER - 2)


	class uEEPROMLib {
		public:
			// Constructors
			uEEPROMLib();
			uEEPROMLib(const int);
			uEEPROMLib(bool);
			uEEPROMLib(bool, const int);

			void set_address(const uint8_t);
			// EEPROM read functions
			bool eeprom_read(const unsigned int, byte *, unsigned int);
			template <typename TR> void eeprom_read(const unsigned int, TR *);
			byte eeprom_read(const unsigned int);

			// EEPROM write functions
			bool eeprom_write(const unsigned int, void *, const unsigned int);
			bool eeprom_write(const unsigned int, char);
			bool eeprom_write(const unsigned int, unsigned char);
 			template <typename TW> bool eeprom_write(const unsigned int, const TW);

			/**
			 * \brief: EEPROM page size control
			 */
			uint8_t page_size = 32; // common size, as AT24C32 (datasheet on extras)

		private:
			bool _eeprom_read_sub(const unsigned int, byte *, const uint8_t);
			bool _eeprom_write_sub(const unsigned int, byte *, const uint8_t);

			// Addresses
			int _ee_address = UEEPROMLIB_ADDRESS;

			// EEPROM read and write private functions - works with bytes
			byte _eeprom_read(const unsigned int);
			bool _eeprom_write(const unsigned int, const byte);
			// Fix for 1st write error
			bool init = false;
			#define uEEPROMLIB_STM32_INIT_FIX() { if (!init) { init = true;  _eeprom_read(0); delay(10); } }
	};


	// Templates must be here because Arduino compiler incompatibility to declare them on .cpp file

	/**
	 *
	 *
	 * @param unsigned int
	 * @param
	 */


	/**
	 * \brief Write any datatype to EEPROM address
	 *
	 * @param address Address inside EEPROM to write to
	 * @param data data to write (any type)
	 * @return false on error
	 */
	template <typename TW> bool uEEPROMLib::eeprom_write(const unsigned int address, const TW data) {
		return eeprom_write(address, (void *) &data, sizeof(TW));
	}


	/**
	 * \brief Read any datatype from EEPROM address
	 *
	 * @param address Address inside EEPROM to read from
	 * @return read data (any type)
	 */
	template <typename TR> void uEEPROMLib::eeprom_read(const unsigned int address, TR *data) {
		eeprom_read(address, (byte *) data, sizeof(TR));
	}
#endif


