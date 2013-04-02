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

#define PHY_MASK_L 300
#define	PHY_MASK_H 301
#define PHY_MASK_LEN 302
#define ACT_MASK_H 303
#define ACT_MASK_L 304
#define ACT_MASK_LEN 305




#endif /*EEPROMUTILS_H*/
