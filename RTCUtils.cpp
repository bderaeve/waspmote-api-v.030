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


void RTCUtils::reinitialize()
{
	RTC.setTime("00:00:00:00:00:00:00");
	getTime();
		#ifdef FINAL_DEBUG_NODE
			COMM.sendMessage(xbeeZB.GATEWAY_MAC, "RTC has been reset");	
		#endif
}

char * RTCUtils::getTime()
{
	RTC.getTime();
	convertRTCToInt();
	
	#ifdef RTC_UTILS_DEBUG_V2
		USB.print("RTC = "); USB.println(RTC.getTime());
		USB.print("RTCint = "); USB.println( (int) RTCSecMinHourInt );
	#endif
	
	return RTC.getTime();
}


uint8_t RTCUtils::convertTime2Char(uint16_t t2w, uint8_t numDaysIn, char * xbeeZBTime2Wake)
{
	uint8_t error = 2;
	
	if( t2w < 0 )
		return error;
		
	else
	
	error = 0;
	uint8_t numSecs = 0;
	uint8_t numMins = 0;
	uint8_t numHours = 0;
	uint8_t numDays = numDaysIn;
	uint16_t temp = 0;
	char  days[3] = {'0', '0', '\0'};// = (char *) calloc(2, sizeof(char));
	char hours[3] = {'0', '0', '\0'};// = (char *) calloc(2, sizeof(char));
	char  mins[3] = {'0', '0', '\0'};// = (char *) calloc(2, sizeof(char));
	char  secs[3] = {'0', '0', '\0'};// = (char *) calloc(2, sizeof(char));
	
	#ifdef RTC_UTILS_DEBUG_V1
		USB.print("uint16_t t2w = "); USB.println( (int) t2w);
		USB.print("*Days = "); USB.println(days);
		USB.print("*Hours = "); USB.println(hours);
		USB.print("*Minutes = "); USB.println(mins);
		USB.print("*Seconds = "); USB.println(secs);
	#endif

	// > 1 DAY:
	if(t2w >= 8640)
	{
		numDays = t2w / 8640;
		t2w %= 8640;
		if(numDays > 9) sprintf(days, "%d", numDays);
		else sprintf(days, "0%d", numDays);
		#ifdef RTC_UTILS_DEBUG_V1
			USB.print("numDays = "); USB.println(numDays); 
			USB.print("* days = "); USB.println(days);
		#endif		
	}	
	// > 1 HOUR:
	if(t2w >= 360)
	{
		numHours = t2w / 360;
		t2w %= 360;
		if(numHours > 9) sprintf(hours, "%d", numHours);
		else sprintf(hours, "0%d", numHours);
		#ifdef RTC_UTILS_DEBUG_V1
			USB.print("numHours = "); USB.println( (int) numHours); 
			USB.print("* hours = "); USB.println(hours);
		#endif	
	}
	// > 1 MIN:
	if(t2w >= 6)
	{	
		numMins = t2w / 6;
		t2w %= 6;
		if(numMins > 9) sprintf(mins, "%d", numMins);
		else sprintf(mins, "0%d", numMins);
		#ifdef RTC_UTILS_DEBUG_V1
			USB.print("numMins = "); USB.println( (int) numMins); 
			USB.print("* mins = "); USB.println(mins);
		#endif	
	}
	
	// < 1 MIN
	if(t2w < 6) 
	{
		if(t2w > 0)
		{
			numSecs = t2w * 10;
			sprintf(secs, "%d", numSecs);
			#ifdef RTC_UTILS_DEBUG_V1
				USB.print("numSecs = "); USB.println( (int) numSecs); 
				USB.print("* secs = "); USB.println(secs);
			#endif	
		}
	}
	
	sprintf(xbeeZBTime2Wake, "%s:%s:%s:%s", days, hours, mins, secs);	
	
	#ifdef RTC_UTILS_DEBUG_V1
		USB.print("*Days = "); USB.println(days);
		USB.print("*Hours = "); USB.println(hours);
		USB.print("*Minutes = "); USB.println(mins);
		USB.print("*Seconds = "); USB.println(secs);
			
		USB.print("time2wake = "); USB.println(xbeeZBTime2Wake);  //"dd:hh:mm:ss"
	#endif
	
	return error;
}


uint8_t RTCUtils::setNextTimeWhenToWakeUpViaOffset(uint16_t offset)
{
	uint8_t error = 2;
	
	if( offset < 0 )
		return error;
	else
		error = 0;
		if( offset >= ONE_DAY )
		{
			nextDay2WakeUpInt = offset / ONE_DAY;
			offset -= nextDay2WakeUpInt * ONE_DAY;
			nextDay2WakeUpInt += RTC.date;
		}
		else
		{
			nextDay2WakeUpInt = RTC.date;
			#ifdef RTC_UTILS_DEBUG_V2;
				USB.print("RTC.date = "); USB.println( (int) RTC.date);
				//USB.print("RTC.day = "); USB.println( (int) RTC.day); //not this one!
			#endif
		}
		
		nextTime2WakeUpHoursMinsSecsInt = offset + RTCSecMinHourInt;
		convertTime2Char(nextTime2WakeUpHoursMinsSecsInt, nextDay2WakeUpInt, nextTime2WakeUpChar);
			#ifdef RTC_UTILS_DEBUG_V2
				USB.print("nextTime2WakeUpHoursMinsSecsInt = "); USB.println( (int) nextTime2WakeUpHoursMinsSecsInt );
				USB.print("as char: "); USB.println(nextTime2WakeUpChar);
			#endif
			
		return error;
}


void RTCUtils::convertAlarm1ToInt()
{
	Alarm1int = 0;
	Alarm1int += RTC.second_alarm1 / 10;
	Alarm1int += RTC.minute_alarm1 * 6;
	Alarm1int += RTC.hour_alarm1 * 360;
	//Alarm1int += RTC.day_alarm1 * 8640;
}


void RTCUtils::convertRTCToInt()
{
	#ifdef RTC_UTILS_DEBUG_V2
		USB.print("RTCsecs "); USB.println( (int) RTC.second );
		USB.print("RTCmins "); USB.println( (int) RTC.minute );
		USB.print("RTChours "); USB.println( (int) RTC.hour );
		//USB.print("RTCdays "); USB.println( (int) RTC.day );
	#endif

	RTCSecMinHourInt = 0;
	RTCSecMinHourInt += RTC.second / 10;
	RTCSecMinHourInt += RTC.minute * 6;
	RTCSecMinHourInt += RTC.hour * 360;
	//RTCint += RTC.day * 8640;
}


RTCUtils RTCUt = RTCUtils();


