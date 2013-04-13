/* ==========================================================================
 *
 *			THESIS: Design of a Wireless Sensor Networking test-bed
 * 
 * ==========================================================================
 *
 *       Filename:  EEPROMUtils.h
 *    Description:  File to help with writing to waspmotes EEPROM.
 *
 *	!!!  EEPROM addresses from 0 to 291 are used by Waspmote to save important 
 *	!!!  data, so they must not be over-written. Thus, the available storage
 *	!!!	 addresses go from 292 to 4095. 
 *		 Another option is the use of the high capacity 2GB SD card but this
 *		 will consume more energy.
 *
 * ======================================================================= */
#ifndef EEPROMUTILS_H
#define EEPROMUTILS_H

#define MAX_DATA_PER_SENSOR 50
#define MAX_EEPROM_WRITE 500 //CAN CAUSE UP TO 1000 writes


/**
  * !!! POSITIONS 1000 - 2000 ARE RESERVED FOR STORING TIME2WAKE VALUES !!!
	!!! POSITIONS 2001 - MAX_SENSORS*2 + 6 ARE RESERVED FOR STORING INDIVIDUAL SENSOR TIMES !!!
			2001/2 = MIN_SENSOR_TIME_L/H
			2003/4 = MAX_SENSOR_TIME_L/H	@see: WaspXBeeZBNode::calculateNextTime2Sleep()
			2005 = FACTOR
			2006 = i
  */
  
#define IN_PANIC_ADDR 295  

#define MUST_CALCULATE_NEXT_TIMES 296
#define OPERATING_MODE 298		// BOOL defaultOperation
#define MUST_RESET_RTC 297
#define EEPROM_READ_MODE 299   // EASY / DIFFICULT


#define PHY_MASK_L 300
#define	PHY_MASK_H 301
#define PHY_MASK_LEN 302
#define ACT_MASK_H 303
#define ACT_MASK_L 304
#define ACT_MASK_LEN 305

#define DEFAULT_T2W_L 306
#define DEFAULT_T2W_H 307

#define NR_OF_TIMES_STORED_IN_EEPROM 308

#define POWERPLAN 309
#define SLEEPMODE 310

#define START_T2W_VALUES 1000

#define MIN_SENS_TIME_L 2001
#define MIN_SENS_TIME_H 2002
#define MAX_SENS_TIME_L 2003
#define MAX_SENS_TIME_H 2004
#define START_SENSOR_INTERVALS 2005
// 2005 -> 2005 += 2*MAX_SENSORS_2B -> 2037  = INDIVIDUAL SENSOR INTERVALS
#define END_SENSOR_INTERVALS 2037

// 2038 -> 2038 += 2*MAX_SENSORS_2B -> 2070  = SENSOR MULTIPLIERS
#define NR_ACTIVATED_SENSORS 2038
#define FIRST_SENS_MULTIPLIER 2040


//#define FACTOR_TO_INCREASE_BEFORE_REPEATING 2038



/*********************************************************************************************************
  *
  * SAVE SENSOR VALUES IN CASE OF POWER SAVER MODE
  *
  *******************************************************************************************************/

/**
  * !!! POSITIONS 3000 - ....... ARE RESERVED FOR STORING SENSOR VALUES FOR POWER SAVER MODE !!!
	!!! To allow for extensions the maximum number of values to store will be limited to 30.
  */
#define MUST_SEND_SAVED_SENSOR_VALUES 3000  
#define MUST_SEND_SAVED_SENSOR_VALUES_MASK_L 3001
#define MUST_SEND_SAVED_SENSOR_VALUES_MASK_H 3002

#define NR_OF_SAVED_TEMPERATURE_VALUES 3010
#define MAX_NR_OF_TEMPERATURE_SAVINGS 3011
#define NR_OF_SAVED_HUMIDITY_VALUES 3012
#define MAX_NR_OF_HUMIDITY_SAVINGS 3013
#define NR_OF_SAVED_PRESSURE_VALUES 3014
#define MAX_NR_OF_PRESSURE_SAVINGS 3015
#define NR_OF_SAVED_BATTERY_VALUES 3016
#define MAX_NR_OF_BATTERY_SAVINGS 3017
#define NR_OF_SAVED_CO2_VALUES 3018
#define MAX_NR_OF_CO2_SAVINGS 3019

#define FIRST_TEMPERATURE_VALUE 3020
#define LAST_TEMPERATURE_VALUE 3079

#define FIRST_HUMIDITY_VALUE 3080
#define LAST_HUMIDITY_VALUE 3139

#define FIRST_PRESSURE_VALUE 3140
#define LAST_PRESSURE_VALUE 3199

#define FIRST_BATTERY_VALUE 3200
#define LAST_BATTERY_VALUE 3259

#define FIRST_CO2_VALUE 3260
#define LAST_CO2_VALUE 3319





#endif /*EEPROMUTILS_H*/
