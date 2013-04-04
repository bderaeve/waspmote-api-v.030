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

#define PHY_MASK_L 300
#define	PHY_MASK_H 301
#define PHY_MASK_LEN 302
#define ACT_MASK_H 303
#define ACT_MASK_L 304
#define ACT_MASK_LEN 305

#define DEFAULT_T2W_L 306
#define DEFAULT_T2W_H 307


#define START_T2W_VALUES 1000

#define MIN_SENS_TIME_L 2001
#define MIN_SENS_TIME_H 2002
#define MAX_SENS_TIME_L 2003
#define MAX_SENS_TIME_H 2004
#define START_SENSOR_INTERVALS 2005
// 2005 -> 2005 += 2*MAX_SENSORS_2B -> 2037  = INDIVIDUAL SENSOR INTERVALS
#define END_SENSOR_INTERVALS 2037
#define FACTOR_TO_INCREASE_BEFORE_REPEATING 2038

#endif /*EEPROMUTILS_H*/
