/* ==========================================================================
 *
 *			THESIS: Design of a Wireless Sensor Networking test-bed
 * 
 * ==========================================================================
 *
 *       Filename:  RTCUtils.cpp
 *    Description:  Extra functionality for time settings
 *
 * ======================================================================= */
 
#ifndef __WPROGRAM_H__
	#include "BjornClasses.h"
	#include "WaspClasses.h"
#endif

#include <inttypes.h>

/**************************************************************************************
  *
  * DEEP SLEEP / HIBERNATE UTILITIES
  *
  *************************************************************************************/
  
/// ///////////////////////////////////////////////////////////////
///  INDOOR NODES - GASSES SENSOR BOARDS  
/// ///////////////////////////////////////////////////////////////
#ifndef WEATHER_STATION  
  
void PowerUtils::enterLowPowerMode(SleepMode sm, uint16_t howLong)
{
	if(sm == HIBERNATE)
	{
		RTCUt.getTime();
		RTCUt.setNextTimeWhenToWakeUpViaOffset(howLong);
		PWR.hibernate(RTCUt.nextTime2WakeUpChar, RTC_ABSOLUTE, RTC_ALM1_MODE3);
	}
	else
	{	
		for(uint16_t i=0; i<howLong; i++) 
		{
			PWR.sleep(WTD_8S, ALL_OFF);
			PWR.sleep(WTD_2S, ALL_OFF);
		}
		
		if( intFlag & WTD_INT )  //"can be used as out of sleep interrupt"
        {
			intFlag &= ~(WTD_INT);
        }
		
		xbeeZB.ON();
      	USB.begin();
			#ifdef HIBERNATE_DEBUG_V2
				USB.print("out sleep");
			#endif
	}	
}

 
//! This function REQUIRES that the correct time to sleep (absolute for hibernate and deep sleep)
//! or sleep time (offset for sleep) is known in either RTCUt.nextTime2WakeUpChar (hibernate) or in
//! time2wValuesArray[posArray] (sleep/deep). 
void PowerUtils::enterLowPowerMode(SleepMode sm, XBeeSleepMode xbs) ///AT END OF LOOP()
{
	if(sm == HIBERNATE)
	{
			#ifdef WASPMOTE_SLEEP_MODE_DEBUG
				USB.print("\nHIBERNATE\n");
			#endif
			#ifdef HIBERNATE_DEBUG_V2
				USB.print("\n\nEntering HIBERNATE at ");
				USB.println(RTC.getTime());
				USB.print("till ");
				USB.println(RTCUt.nextTime2WakeUpChar);
			#endif
		
		/// Store important parameters
		xbeeZB.storeProgramParametersToEEPROM();	
	
		PWR.hibernate(RTCUt.nextTime2WakeUpChar, RTC_ABSOLUTE, RTC_ALM1_MODE3);
		/// After hibernate we start in WaspXBeeZBNode::hibernateInterrupt()
	}
	else if(sm == DEEPSLEEP)  /// CANNOT BE COMBINED WITH HIBERNATE
	{
			#ifdef WASPMOTE_SLEEP_MODE_DEBUG
				USB.print("\nDEEPSLEEP\n");
			#endif	
			#ifdef DEEPSLEEP_DEBUG
				USB.print("\nentering deepsleep at "); 
				USB.print(RTC.getTime());
				USB.print("till "); 
				USB.print( RTCUt.nextTime2WakeUpChar );
			#endif
			
		if(xbs == NO_XBEE_SLEEP_MODE)
		{
			PWR.deepSleep(RTCUt.nextTime2WakeUpChar, RTC_ABSOLUTE, RTC_ALM1_MODE3, ALL_OFF);	
			xbeeZB.ON();
		}
		else
		{
			PWR.deepSleep(RTCUt.nextTime2WakeUpChar, RTC_OFFSET, RTC_ALM1_MODE2, SENS_OFF | UART1_OFF | BAT_OFF | RTC_OFF);
			xbeeZB.wake();
		}
		
		RTC.ON();
		RTCUt.getTime();
		RTC.setMode(RTC_OFF,RTC_NORMAL_MODE);  //this will save power
			
		USB.begin();
			#ifdef DEEPSLEEP_DEBUG
				USB.print("awake at "); USB.println(RTC.getTime());
			#endif
		if(xbeeZB.defaultOperation)
		{
			RTCUt.setNextTimeWhenToWakeUpViaOffset(xbeeZB.defaultTime2WakeInt);
		}
		else
		{
				#ifdef DEEPSLEEP_DEBUG
					USB.print("out of sleep, posInArray = ");
					USB.println( (int) posInArray );
					USB.print("next t2s = ");
					USB.println( (int) time2wValuesArray[posInArray]);
					USB.println( (int) time2wValuesArray[posInArray + 1]);
					USB.println( (int) (time2wValuesArray[posInArray + 1] - time2wValuesArray[posInArray]) );
				#endif
			xbeeZB.findNextTime2Wake(DEEPSLEEP);
			xbeeZB.updatePosInArray();	
		}
		/// GOTO "device enters loop"
	}
	else if(sm == SLEEP)
	{
			#ifdef WASPMOTE_SLEEP_MODE_DEBUG
				USB.print("\nSLEEP\n");
			#endif	
			#ifdef SLEEP_DEBUG
				USB.print("\nentering sleep at "); USB.println(RTC.getTime());
			#endif
		sleepTillNextTime2Wake(xbs);

		RTC.ON();
		RTCUt.getTime();
		RTC.setMode(RTC_OFF,RTC_NORMAL_MODE);  //this will save power
		RTCUt.setAwakeAtTime();
      	
		USB.begin();
			#ifdef SLEEP_DEBUG
				USB.print("awake at");  USB.println(RTC.getTime());
			#endif		
	
		if( intFlag & WTD_INT )  //"can be used as out of sleep interrupt"
        {
			intFlag &= ~(WTD_INT);
			if(!xbeeZB.defaultOperation) xbeeZB.updatePosInArray();
			
				#ifdef SLEEP_DEBUG
					USB.print("posInArray ");
					USB.println( (int) xbeeZB.posInArray );
				#endif
			
			/// GOTO "device enters loop"
        }
	}	
}


void PowerUtils::enterLowPowerMode(SleepMode sm)
{
	enterLowPowerMode(sm, NO_XBEE_SLEEP_MODE);
}


void PowerUtils::enterLowPowerMode() ///AT END OF LOOP()   AUTO SELECTION SLEEP MODE 
{
	if(RTCUt.nextTime2WakeUpHoursMinsSecsInt <= 2)
		enterLowPowerMode(SLEEP, XBEE_SLEEP_MODE);
	else
		enterLowPowerMode(HIBERNATE, NO_XBEE_SLEEP_MODE);
} 


void PowerUtils::hibernate()
{
		#ifdef HIBERNATE_DEBUG_V1
			USB.print("\n\nEntering HIBERNATE at ");
			USB.println(RTC.getTime());
			USB.print("for ");
			USB.println(xbeeZB.defaultTime2WakeStr);
		#endif
		
		#ifdef HIBERNATE_DEBUG_V2
			USB.print("\n\nEntering HIBERNATE at ");
			USB.println(RTC.getTime());
			USB.print("till ");
			USB.println(RTCUt.nextTime2WakeUpChar);
		#endif
			
	xbeeZB.storeProgramParametersToEEPROM();
	
	PWR.hibernate(RTCUt.nextTime2WakeUpChar, RTC_ABSOLUTE, RTC_ALM1_MODE3);
		/// PWR.hibernate(RTC.getAlarm1(),RTC_ABSOLUTE,RTC_ALM1_MODE3);					NOT WORKING
		/// PWR.hibernate(xbeeZB.defaultTime2WakeStr, RTC_OFFSET, RTC_ALM1_MODE2);		NOT WORKING
}


void PowerUtils::sleepTillNextTime2Wake(XBeeSleepMode xbs)
{
	uint8_t secondsBeenAwake;		/// Sleep will only be used for max 59 seconds
	uint8_t time2sleep;
	uint16_t time = 0;
		#ifdef SLEEP_DEBUG
			uint8_t count = 0;
		#endif
	RTC.getTime();
	if( RTC.second > RTCUt.RTCAwakeAtSeconds )
	{
		secondsBeenAwake = RTC.second - RTCUt.RTCAwakeAtSeconds;
			#ifdef SLEEP_DEBUG
				USB.print("\nsecAwake: "); USB.println( (int) secondsBeenAwake);
			#endif
	}
	else
	{
		secondsBeenAwake = RTC.second + (60 - RTCUt.RTCAwakeAtSeconds);
			#ifdef SLEEP_DEBUG
				USB.print("\nsecAwake: "); USB.println( (int) secondsBeenAwake);
			#endif
	}
	if(xbeeZB.defaultOperation) time = xbeeZB.defaultTime2WakeInt;
	else time = xbeeZB.time2wValuesArray[xbeeZB.posInArray+1] - xbeeZB.time2wValuesArray[xbeeZB.posInArray];
	
	time2sleep = time *	10 - secondsBeenAwake;
			#ifdef SLEEP_DEBUG
				USB.print("time2sleep: "); USB.println( (int) time2sleep );
			#endif
	if(xbs == NO_XBEE_SLEEP_MODE)
	{
		while(time2sleep > 10)
		{
			PWR.sleep(WTD_8S, ALL_OFF);
			PWR.sleep(WTD_2S, ALL_OFF);
			time2sleep -= 10;
				#ifdef SLEEP_DEBUG
					count+=10;
				#endif
		}
		
		while(time2sleep > 1)
		{
			RTC.ON();
			RTCUt.getTime();
			if(RTC.second%60 == 0)
				break;
			PWR.sleep(WTD_1S, ALL_OFF);
			time2sleep -= 1;	
				#ifdef SLEEP_DEBUG
					count+=1;
				#endif
		}
		xbeeZB.ON();
	}
	else	/// with ZigBee sleep
	{
		while(time2sleep > 10)
		{
			PWR.sleep(WTD_8S, SENS_OFF | UART1_OFF | BAT_OFF | RTC_OFF);
			PWR.sleep(WTD_2S, SENS_OFF | UART1_OFF | BAT_OFF | RTC_OFF);
			time2sleep -= 10;
				#ifdef SLEEP_DEBUG
					count+=10;
				#endif
		}
		
		while(time2sleep > 1)
		{
			RTC.ON();
			RTCUt.getTime();
			if(RTC.second%60 == 0)
				break;
			PWR.sleep(WTD_1S, SENS_OFF | UART1_OFF | BAT_OFF | RTC_OFF);
			time2sleep -= 1;
				#ifdef SLEEP_DEBUG
					count+=1;
				#endif
		}
		xbeeZB.wake();		
	}
		#ifdef SLEEP_DEBUG
			USB.begin();
			USB.print("\nslept for "); USB.println( (int) count );
		#endif
}


/// ///////////////////////////////////////////////////////////////
///  WEATHER STATION - AGRICULTURE SENSOR BOARD V2  
/// ///////////////////////////////////////////////////////////////
#else

//! This function REQUIRES that the correct time to sleep (absolute for hibernate and deep sleep)
//! or sleep time (offset for sleep) is known in either RTCUt.nextTime2WakeUpChar (hibernate) or in
//! time2wValuesArray[posArray] (sleep/deep). 
void PowerUtils::enterLowPowerModeWeatherStation(XBeeSleepMode xbs) ///AT END OF LOOP()
{
	/// SleepMode = DEEPSLEEP (from the moment we implement hibernate we can no longer re-enable pluviometer interrupts
	
			#ifdef WASPMOTE_SLEEP_MODE_DEBUG
				USB.print("\nDEEPSLEEP\n");
			#endif	
			#ifdef DEEPSLEEP_DEBUG
				USB.print("\nentering deepsleep at "); 
				USB.print(RTC.getTime());
				USB.print("till "); 
				USB.print( RTCUt.nextTime2WakeUpChar );
			#endif
		
	if(xbs == NO_XBEE_SLEEP_MODE)
	{
		//Put the mote to sleep with pluviometer interruptions enabled
		//PWR.deepSleep(RTCUt.nextTime2WakeUpChar, RTC_ABSOLUTE, RTC_ALM1_MODE3, ALL_OFF);
		SensorAgrV20.sleepAgr(RTCUt.nextTime2WakeUpChar, RTC_ABSOLUTE, RTC_ALM1_MODE3, UART0_OFF | UART1_OFF | BAT_OFF | RTC_OFF, SENS_AGR_PLUVIOMETER);
			
		//xbeeZB.ON();
	}
	else
	{
		//Put the mote to sleep with pluviometer interruptions enabled and ZigBee sleep
		SensorAgrV20.sleepAgr(RTCUt.nextTime2WakeUpChar, RTC_ABSOLUTE, RTC_ALM1_MODE3, UART1_OFF | BAT_OFF | RTC_OFF, SENS_AGR_PLUVIOMETER);
		//PWR.deepSleep(RTCUt.nextTime2WakeUpChar, RTC_OFFSET, RTC_ALM1_MODE2, SENS_OFF | UART1_OFF | BAT_OFF | RTC_OFF);
		
		//xbeeZB.wake();
	}
	
	//Detach pluviometer interruptions
	SensorAgrV20.detachPluvioInt();
	
	RTC.ON();
	RTCUt.getTime();
	RTC.setMode(RTC_OFF,RTC_NORMAL_MODE);  //this will save power


	//In case a pluviometer interruption arrived
	if(intFlag & PLV_INT)
	{
		SensUtils.rainfall_ISR();
		
		if(SensUtils.startedRaining)
		{
			COMM.sendMessage(xbeeZB.GATEWAY_MAC, STARTED_RAINING, "");
		}
		
		// Clearing the interruption flag before coming back to sleep
		clearIntFlag();
		enterLowPowerModeWeatherStation(xbs);
	}
	
	//In case an RTC interruption arrived
	else if(intFlag & RTC_INT)
	{
		USB.begin();
			#ifdef DEEPSLEEP_DEBUG
				USB.print("awake at "); USB.println(RTC.getTime());
			#endif			
			
		if(xbeeZB.defaultOperation)
		{
			RTCUt.setNextTimeWhenToWakeUpViaOffset(xbeeZB.defaultTime2WakeInt);
		}
		else
		{
				#ifdef DEEPSLEEP_DEBUG
					USB.print("out of sleep, posInArray = ");
					USB.println( (int) posInArray );
					USB.print("next t2s = ");
					USB.println( (int) time2wValuesArray[posInArray]);
					USB.println( (int) time2wValuesArray[posInArray + 1]);
					USB.println( (int) (time2wValuesArray[posInArray + 1] - time2wValuesArray[posInArray]) );
				#endif
			xbeeZB.findNextTime2Wake(DEEPSLEEP);
			xbeeZB.updatePosInArray();	
		}
		
		// Clearing the interruption flag before coming back to sleep
		clearIntFlag();
	/// GOTO "device enters loop"
	}
}
#endif




PowerUtils PWRUt = PowerUtils();



