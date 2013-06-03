#ifndef SENSORUTILS_H
#define SENSORUTILS_H

/************************************************************************************************************
 * Includes
 ***********************************************************************************************************/
#include <inttypes.h>


/************************************************************************************************************
 * Definitions & Declarations
 ***********************************************************************************************************/
#define MSByte(param) (( (unsigned int) param) / 256 )  //(i & 0xFF00)>>8;
#define LSByte(param) (( (unsigned int) param) % 256 )  // i & 0x00FF;  assume compiler is smart enough

#define ToMask(param) ( (uint16_t) ((  param[0] ) * 256 ) + (param[1]) )

//#define SENS_DEBUG
#define SENS_DEBUG_V2

#define NUM_MEASUREMENTS 10
#define NUM_SENSORS 10  //MUST BE <=16 FOR CURRENT EEPROM CONFIGURATION
#define NUM_SENSORS_TO_SAVE 5
#define MAX_LUMINOSITY 4
#define MAX_NR_OF_SENSOR_SAVINGS 30

typedef unsigned char byte;  ///Other types like uint8_t can be found in 'stdint.h'

typedef enum {TEMPERATURE = 0x0001, HUMIDITY = 0x0002, PRESSURE = 0x0004, 
	BATTERY = 0x0008, CO2 = 0x0010, ANEMO = 0x0020, VANE = 0x0040, PLUVIO = 0x0080,
	LUMINOSITY = 0x0100, SOLAR_RADIATION = 0x0200} 
	SensorType;
	
typedef enum {VANE_N, VANE_NNE, VANE_NE, VANE_ENE, 
			  VANE_E, VANE_ESE, VANE_SE, VANE_SSE, 
			  VANE_S, VANE_SSW, VANE_SW, VANE_WSW, 
			  VANE_W, VANE_WNW, VANE_NW, VANE_NNW} 
	VaneDirection;

//! Function pointers to save sensor data
/*! Since the size of the data to insert is constant there's no reason to
/*! make these functions non-static / member functions.
 *	\return	error = SensorType 	--> EEPROM FULL, request to send the values
 *			error = 0			--> The command has been executed with no errors
 */	
typedef uint16_t StoreSensorData();
	extern uint16_t saveTemperature();
	extern uint16_t saveHumidity();
	extern uint16_t savePressure();
	extern uint16_t saveBattery();
	extern uint16_t saveCO2();

//! Function pointers to easily read the sensor data stored in EEPROM.	
#ifdef POWER_MODES
	typedef void ReadSensorSamplesFromEEPROM();
		extern void readTemperatureFromEEPROM();
		extern void readHumidityFromEEPROM();
		extern void readPressureFromEEPROM();
		extern void readBatteryFromEEPROM();
		extern void readCO2FromEEPROM();
#endif	


/************************************************************************************************************
 * Class
 ***********************************************************************************************************/
 
class SensorUtils
{
	private:
		void (SensorUtils::*reader[NUM_SENSORS])(void);
		void convertVaneDirection();
		
	public:
		//! class constructor
		/*!
		  It does nothing
		  \param void
		  \return void
		*/
		SensorUtils();
		
		
		//! It gets the current SENS_TEMPERATURE value
		/*!
		It stores in global variable 'temperature' the currently measured temperature.
		This value is actually the average of 10 values read from the sensor.
		*/
		void measureTemperature();
		
		
		//! It gets the current SENS_HUMIDITY value
		/*!
		It stores in global 'humidity' variable the currently measured humidity.
		This value is actually the average of 10 values read from the sensor.
		*/
		void measureHumidity();
		
		
		//! It gets the current SENS_PRESSURE value
		/*!
		It stores in global variable 'pressure' the currently measured atm pressure.
		This value is actually the average of 10 values read from the sensor.
		!!!!! Also had to do a "callibration" in WaspSensorGas_v20::pressureConversion(int readValue)
		!!!!! According to libelium-dev this is the "correct" way to obtain an acceptable result:
		!!!!!  "you may find some offset because we adjusted the formula to fit the output value of the 
		!!!!!  sensors we used to test the board and had to correct a small error, and yours may differ 
		!!!!!  a little bit, so adjust this value (in the formula in the API is a 270) may help to have 
		!!!!!  a more accurate value."
		!!!!!		=> Changed +270 into -130 in the formula
		*/
		void measurePressure();
		
		
		//! It gets the current BATTERY level
		/*!
		It stores in global variable 'battery' the currently measured battery level.
		This value is actually the average of 10 values read from the sensor.
		*/
		void measureBattery();
		
		
		//! It gets the current SENS_CO2 value
		/*!
		It stores in global variable 'co2' the currently measured CO2 level.
		The sensor will be warming up (30 sec) first. 
		*/		  
		void measureCO2();  
		
		/// WEATHER_STATION /////////////////////////////////////////////////////////////////////////////////
		#ifdef WEATHER_STATION
			//! It gets the current wind speed and stores in global 'anemo'. 
			void measureAnemo();
			
			
			//! It gets the current wind direction and stores in global 'vane'.
			/*!
			It calls 'convertVaneDirection()' which converts the float into a VaneDirection
			*/
			void measureVane();
			
			
			//! It gets the current SENS_AGR_PLUVIOMETER value
			/*!
			It stores in global variable 'current_rainfall' the currently measured 
			rainfall in mm/min.  (not very accurate, please measure via ISR / pluviometerCounter )
			*/				
			void measureCurrentRainfall();
			
			
			//! Interrupt Service Routine called when a PluvioInt has been generated
			/*!
			It stores the summative rainfall since the previous reset, in integer.
			To get the value in mm: summativeRainfallInMM = float (pluviometerCounter) * 0.2794
			(this will be done at the gateway / webinterface)
			*/
			void rainfall_ISR();
			
			
			//! It stores the summative rainfall since the previous resetPluviometer()
			void getSummativeRainfall();
			
			//! It resets the pluviometerCounter. Intended to manually empty the meter via an 
			/*! 'RESET_PLUVIO_METER' packet. Otherwise, by default the counter will be reset
			    each two hours. To go back to default just sent the same packet again.
			*/
			void resetPluviometer();
			
			//! It gets the current luminosity and stores in float 'luminosity'.
			void measureLuminosity();
			
			//! It gets the current luminosity and stores in float 'solar_radiation'.
			void measureSolarRadiation();
			
			
		#endif /// WEATHER_STATION //////////////////////////////////////////////////////////////////////////	
		
		  
		//! Measures all sensors found as arguments,  meant to do measurements via the IDE
		//!!!!! The first argument must be the number of sensor types followed
		/*!!!!! This function takes care of all turning ON/OFF board and sensor requirements !!!!!
		  \return  	error=2 --> The command has not been executed
					error=1 --> The MASK was 0, no sensors measured
					error=0 --> The command has been executed with no errors
		*/
		uint8_t measureSensors(int, ...);
		
		  
		//! Measures all sensors found in the mask argument,  meant to use remotely.
		/*!!!!! This function takes care of all turning ON/OFF board and sensor requirements !!!!!
		  \return  	error=2 --> The command has not been executed
					error=1 --> The MASK was 0, no sensors measured
					error=0 --> The command has been executed with no errors
		*/
		uint8_t measureSensors(uint16_t);
		
		  
		//! Converts a float sensor value to its most compact binary value according
		/*! to the corresponding decimals per sensor type.
		  \param float: the sensor value to convert
		  \param SensorType: the SensorType
		  \return  	error=2 --> The command has not been executed
					error=1 --> There has been an error while executing the command
					error=0 --> The command has been executed with no errors
		*/ 
		uint8_t sensorValue2Chars(float, SensorType);
		
		
		//! Stores all the measured sensors found in the mask argument (for HIBERNATE mode)
		/*! \return	error=3 --> EEPROM FULL, values must be sent
					error=2 --> The command has not been executed
					error=1 --> The MASK was 0, no sensors measured
					error=0 --> The command has been executed with no errors
		*/		
		void storeMeasuredSensorValues(uint16_t);
		
		
		//! Stores all the measured sensors found in the mask in RAM (for SLEEP modes)
		/*! \return  mask containing the sensors that have used all the reserved space 
		             and that must be send to the gateway.
		*/
		uint16_t keepSensorValueInMemory(uint16_t);
		
		
		//! It enters the sensor's measuring interval time to the correct position in 
		//! the measuringInterval[NUM_SENSORS] array
		uint8_t registerSensorMeasuringIntervalTime(SensorType, uint16_t);
		
		
		//! It saves the values in measuringInterval[NUM_SENSORS] to the correct positions
		//! in EEPROM
		void saveSensorMeasuringIntervalTimes();
		
		
		//! It reads the values of the currently active sensors from EEPROM and stores them
		//! into 'uint16_t measuringInterval[NUM_SENSORS]'
		void readSensorMeasuringIntervalTimesFromEEPROM();
		
		
		//! It reads sensor values from EEPROM in preparation to send them.
		void copyEEPROMValuesToMemory();
		
		
		
		
		
		//! Variable : the averaged temperature value
		/*!
		 */
		float temperature;
		
		//! Variable : the temperature value in bytes
		/*! TEMPERATURE SENSOR:   RANGE: -40° -> +125° 
		/*!  offset += 40° for negative values,
		/*!  *= 100 for 2 decimals accuracy
		/*!	 --> fits in 2 bytes
		 */		
		byte temp[2];

		
		//! Variable : the averaged humidity value
		/*!
		 */
		float humidity;
		
		//! Variable : the humidity value in bytes
		/*! HUMIDITY SENSOR:   RANGE: 0 -> 100%
		 */
		byte hum;

		
		//! Variable : the averaged atm pressure value
		/*!
		 */
		float pressure;
		
		//! Variable : the pressure value in bytes, 
		/*! PRESSURE SENSOR:   RANGE: 15 - 115kPa
		/*!   force in 2 bytes:  115000 - 2^16 = 50000  --> RANGE: 50kPa - 115kPa
		/*!	  offset -= 50000
		 */
		byte pres[2];
		
		
		//! Variable : the averaged battery level
		/*!
		 */		
		uint16_t battery;
		
		//! Variable : the battery level in bytes
		/*! BATTERY LEVEL:   RANGE: 0 -> 100%
		 */
		byte bat;

		
		//! Variable : the CO2 value
		/*!
		 */
		float co2;
		
		//! Variable : the CO2 value in bytes, 
		/*! CO2 SENSOR:   RANGE: 350 - 10000 ppm
		/*!   normal outdoor level: 350 - 450 ppm; acceptable levels: < 1500 ppm (UK standard for 
		/*!   teaching and learning spaces) 
		/*!	  --> 2B
		 */
		byte co_2[2];

		
		//! Variable : the anemo value
		/*! RANGE: 0 - 240 km/h
		 */			
		float anemo;
		
		//! Variable : the anemo value in bytes
		/*! 
		 */		
		byte an;

		
		//! Variable : the vane value
		/*!
		 */			
		float vane;
		
		//! Variable : the vane value expressed as VaneDirection
		/*!
		 */
		VaneDirection vaneDirection;
		
		//! Variable : the pluvio value
		/*!
		 */	
		float pluvio;
		
		//! Variable : the pluvio value in bytes
		/*!
		 */
		byte rain_count[2];
		
		//! Variable : the summative pluvio value ( incremented via the rainfall_ISR() )
		/*!
		 */			
		uint16_t pluviometerCounter;

		
		bool startedRaining;

		uint16_t startedRainingTime;
		
		float luminosity;
		
		byte lum;

		byte savedLuminosities[MAX_NR_OF_SENSOR_SAVINGS];
	
		float solar_radiation;
		byte radiation[2];
		
		//byte savedRadiations[ 2 * MAX_NR_OF_SENSOR_SAVINGS ];
		
		uint8_t savedPositions[NUM_SENSORS];
		
		#ifdef POWER_MODES	
			byte savedValues[NUM_SENSORS][2 * MAX_NR_OF_SENSOR_SAVINGS];
		#endif

		//!
		/*! Stores the individual measuring interval times of the sensors
		 */
		uint16_t measuringInterval[NUM_SENSORS];
		
		//!
		/*! Stores the biggest measuring interval time of all the active sensors
		 */
		uint16_t maxTime;
		
		//!
		/*! Stores the smallest measuring interval times of all the active sensors
		 */
		uint16_t minTime;
		
		uint16_t acceptedSensorMask;
		
		long previous;
		
		bool forceHighPerformance;
	
 };
 
 extern SensorUtils SensUtils;


#endif /*SENSORUTILS_H*/
