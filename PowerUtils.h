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
#ifndef POWERUTILS_H
#define POWERUTILS_H

#ifndef __WASPXBEECONSTANTS_H__
  #include "WaspXBeeConstants.h"
#endif

/******************************************************************************
 * Includes
 ******************************************************************************/

/******************************************************************************
 * Definitions & Declarations
 ******************************************************************************/
#define WASPMOTE_WEATHER_SLEEP_DEBUG	
/******************************************************************************
 * Class
 ******************************************************************************/
 
class PowerUtils
{
	private:

		
	public:
		//! class constructor
		/*!
		  It does nothing
		  \param void
		  \return void
		 */
		PowerUtils(){};
		
/// ///////////////////////////////////////////////////////////////
///  INDOOR NODES - GASSES SENSOR BOARDS  
/// ///////////////////////////////////////////////////////////////		
		#ifndef WEATHER_STATION
		
		//! Use this function if you want to set a time2sleep OFFSET via the Waspmote-IDE
		//! So this function ignores variable sleep times
		/*!	\param:	SleepMode: HIBERNATE or DEEPSLEEP
		 *  \param: time2sleep: 1 = 10 seconds
		 */
		void enterLowPowerMode(SleepMode, uint16_t);
		
		
		//! Node will go into HIBERNATE / (DEEP) SLEEP for the time specified. 
		/*! Function will check if node is in defaultOperatio or not.
		 *  \@pre: This time must have been set on beforehand!!!!
		 *  \@post: The next time2wake will be set in this function for SLEEP or DEEPSLEEP
		 *	\@post: The next time2wake should be set in the hibernateInterrupt()
		 *  \param : The Waspmote's sleep mode: SLEEP, DEEPSLEEP, HIBERNATE, NONE
		 *  \param : The XBee's / ZigBee sleep mode: XBEE_SLEEP_MODE or NO_XBEE_SLEEP_MODE
		 *  @note: XBee sleep mode can only be used in combination with SLEEP or DEEPSLEEP as
		 *         Waspmote sleep mode
		 */
		void enterLowPowerMode(SleepMode, XBeeSleepMode);
		
		
		//!
		/*! Utiliy function: will call enterLowPowerMode(SleepMode, XBeeSleepMode) with
		 *	NO_XBEE_SLEEP_MODE as second parameter
		 */
		void enterLowPowerMode(SleepMode);
		
		
		//!
		/*! The node will automatically select one of the Waspmote's sleep modes depending 
		 *  on the time to sleep, limiting user control.
		 *	@note: depending on the chosen mode, XBee / ZigBee sleep will be used
		 *         short sleep times -> SLEEP / DEEPSLEEP + XBEE_SLEEP
		 *		   longer sleep time -> HIBERNATE + NO_XBEE_SLEEP
		 */
		void enterLowPowerMode();
		
		
		//! 
		/*! Node will go into hibernate until the RTC alarm goes off. This alarm must have been set
		 *  on beforehand!  (normally this is done immediately after waking up in the ifHibernateInterrupt)
		 */
		void hibernate();
		
		
		//!
		/*! Function to enable Waspmote SLEEP mode in combination with HIBERNATE mode
		 *  From the moment Waspmote HIBERNATE is implemented all other RTC functionalities
		 *  are lost, so DEEPSLEEP cannot be used. This function uses the internal Watchdog
		 *  in combination with Waspmote SLEEP mode in order to make this possible
		 */
		void sleepTillNextTime2Wake(XBeeSleepMode);		

		
/// ///////////////////////////////////////////////////////////////
///  WEATHER STATION - AGRICULTURE SENSOR BOARD V2  
/// ///////////////////////////////////////////////////////////////		
		#else
		
		void enterLowPowerModeWeatherStation(XBeeSleepMode);
		
		#endif
	

		//!
		/*! Stores the next time to wake up offset as WaspRTC:hibernate compactible
		 *! char *
		 */		
		char nextTime2WakeUpChar[12];	
};
 
extern PowerUtils PWRUt;


#endif /*POWERUTILS_H*/
