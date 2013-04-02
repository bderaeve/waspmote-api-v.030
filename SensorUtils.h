#ifndef SENSORUTILS_H
#define SENSORUTILS_H

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <inttypes.h>

/******************************************************************************
 * Definitions & Declarations
 ******************************************************************************/
//#define SENS_DEBUG

typedef enum {TEMPERATURE = 0x0001, HUMIDITY = 0x0002, PRESSURE = 0x0004, 
	BATTERY = 0x0008, CO2 = 0x0010, ANEMO = 0x0020, VANE = 0x040, PLUVIO = 0x080} 
	SensorType;
	
#define NUM_SENSORS 8  //MUST BE <=16 FOR EEPROM CONFIGURATION

/******************************************************************************
 * Class
 ******************************************************************************/
 
class SensorUtils
{
	private:
		void (SensorUtils::*reader[8])(void);
	
	public:
		//! class constructor
		/*!
		  It does nothing
		  \param void
		  \return void
		*/
		SensorUtils();
		
		void testPrinting();
		
		
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
		
		
		void measureAnemo();
		void measureVane();
		void measurePluvio();
		
		  
		//! Measures all sensors found as arguments
		//!!!!! The first argument must be the number of sensor types followed
		/*!!!!! This function takes care of all turning ON/OFF board and sensor requirements !!!!!
		  \return  	error=2 --> The command has not been executed
					error=1 --> The MASK was 0, no sensors measured
					error=0 --> The command has been executed with no errors
		*/
		uint8_t measureSensors(int, ...);
		
		  
		//! Measures all sensors found in the mask argument
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
		
		
		//static int compare(const void *x, const void *y); 
		//static int gcd(int, int);
		
		//! It is called when sensors get individual sensor times
		saveSensorMeasuringIntervalTimes();
		
		
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
		unsigned char temp[2];

		//! Variable : the averaged humidity value
		/*!
		 */
		float humidity;
		
		//! Variable : the humidity value in bytes
		/*! HUMIDITY SENSOR:   RANGE: 0 -> 100%
		 */
		unsigned char hum;
		
		//! Variable : the averaged atm pressure value
		/*!
		 */
		float pressure;
		
		//! Variable : the pressure value in bytes, 
		/*! PRESSURE SENSOR:   RANGE: 15 - 115kPa
		/*!   force in 2 bytes:  115000 - 2^16 = 50000  --> RANGE: 50kPa - 115kPa
		/*!	  offset -= 50000
		 */
		unsigned char pres[2];
		
		//! Variable : the averaged battery level
		/*!
		 */		
		uint16_t battery;
		
		//! Variable : the battery level in bytes
		/*! BATTERY LEVEL:   RANGE: 0 -> 100%
		 */
		unsigned char bat;
		
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
		unsigned char co_2[2];		
		
		

		//!
		/*! Stores the individual measuring interval times of the sensors
		 */
		uint16_t measuringInterval[8];
		
		//!
		/*! Stores the biggest measuring interval time of all the active sensors
		 */
		uint16_t maxTime;
		
		//!
		/*! Stores the smallest measuring interval times of all the active sensors
		 */
		uint16_t minTime;
	
 };
 
 extern SensorUtils SensUtils;


#endif /*SENSORUTILS_H*/
