/* ==========================================================================
 *
 *			THESIS: Design of a Wireless Sensor Networking test-bed
 * 
 * ==========================================================================
 *
 *       Filename:  RTC.h
 *    Description:  Extension to the libeliums RTC functions in WaspRTC.h
 *					This because the hibernate function can not deal with times
 *					received from RTC.getTime() or RTC.getAlarm() but only
 *					with char * in the form of "dd:hh:mm:ss"
 *
 *					The uint16_t times stored in this class are of the type 
 *					compatible with the Gateway software, 
 *						e.g. 1 = 10 seconds
 *							 360 = 1 hour
 *					!! The MAXIMUM allowed time to sleep is 1 WEEK !!
 *
 * ======================================================================= */
#ifndef RTCUTILS_H
#define RTCUTILS_H

#ifndef __WASPXBEECONSTANTS_H__
  #include "WaspXBeeConstants.h"
#endif

/******************************************************************************
 * Includes
 ******************************************************************************/

//#include <inttypes.h>


/******************************************************************************
 * Definitions & Declarations
 ******************************************************************************/
//#define RTC_UTILS_DEBUG_V2

#define ONE_DAY 8640

/******************************************************************************
 * Class
 ******************************************************************************/
 
class RTCUtils
{
	private:

		
	public:
		//! class constructor
		/*!
		  It does nothing
		  \param void
		  \return void
		 */
		RTCUtils();
		
		
		void reinitialize();
		
		//! It sets the time received in parameter and updates the RTCUtils time variables
		/*! \param: the time to set: "13:04:04:05:00:00:00"  (see 'RTC.h')
		 */
		void setTime(const char *);
		
		//! It gets from the RTC the date and time, storing them in the corresponding variables
		//! in WaspRTC:RTC and stores the converted value in this object in 'RTCInt'
		char * getTime();
		
		//! It sets the time when the Waspmote woke up
		/*!
		 *	\pre: RTCUt.getTime()
		 */
		void setAwakeAtTime();
		
		//! converts an integer time2wait to a WaspRTC compactibel char *
		/*!
		 *  \param:	the int to convert
		 *	\param: the char * to store in
		 *	\return: 2 if function has not been executed: time2wait > 1 WEEK
		 *			 0 if ok
		 */
		uint8_t convertTime2Char(uint16_t, uint8_t, char *);
		
		
		//! Adds a given offset (1 = 10 sec) time to the SAVED RTC value and stores it in
		//! 'NextTime2Wake' as int and char *
		/*!
		 *   !!!! USE THIS FUNCTION TO SET VARIABLE HIBERNATE TIMES !!!!
		 *
		 *  \param:	the uint16_t offset time to add to the SAVED RTC value
		 *	\return: 2 if function has not been executed: invalid offset > 1 WEEK
		 *			 0 if ok
		 */		
		uint8_t setNextTimeWhenToWakeUpViaOffset(uint16_t);
		
		uint8_t setNextTimeWhenToMeasureInAlarm1(uint16_t);
		
		//! converts the value returned from WaspRTC:RTC to an int value compactible
		//! with our timing system
		void convertRTCToInt();
		
		
		//! converts the value returned from WaspRTC:Alarm1 to an int value compactible
		//! with our timing system
		void convertAlarm1ToInt();

		
	
		//!
		/*! Stores the RTC sec, min, hour value as uint16_t as long as its < 65536
		 */	
		uint16_t RTCSecMinHourInt;
		
		
		//!
		/*! Stores the time when the Waspmote woke up
		 */
		uint16_t RTCAwakeAtSeconds;
		
		
		//!
		/*! Stores the Alarm1 value as uint16_t as long as its < 65536
		 */			
		uint16_t Alarm1int;
		
		
		//!
		/*! Stores the next time to wake up(hours, mins and seconds) absolute time as int
		 */
		uint16_t nextTime2WakeUpHoursMinsSecsInt;
		
		
		//!
		/*! Stores the next time to wake up(day) absolute time as int
		 */
		uint8_t nextDay2WakeUpInt;
		
		
		//!
		/*! Stores the next time to wake up offset as WaspRTC:hibernate compactible
		 *! char *
		 */		
		char nextTime2WakeUpChar[12];	
};
 
extern RTCUtils RTCUt;


#endif /*RTCUTILS_H*/
