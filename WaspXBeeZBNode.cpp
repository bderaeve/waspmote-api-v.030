/* ==========================================================================
 *
 *			THESIS: Design of a Wireless Sensor Networking test-bed
 * 
 * ==========================================================================
 *
 *       Filename:  WaspXBeeZBNode.cpp
 *    Description:  Contains some extensions on the WaspXBeeZB class 
 *					regarding node setup and node parameters
 *
 * ======================================================================= */


#ifndef __WPROGRAM_H__
	#include "BjornClasses.h"
	#include "WaspClasses.h"
#endif

WaspXBeeZBNode::WaspXBeeZBNode()
{
	panid[0] = 0x00;
	panid[1] = 0x00;
	panid[2] = 0x00;
	panid[3] = 0x00;
	panid[4] = 0x00;
	panid[5] = 0x00;
	panid[6] = 0x00;
	panid[7] = 0xAA;

	GATEWAY_MAC[0] = 0x00;
	GATEWAY_MAC[1] = 0x13;
	GATEWAY_MAC[2] = 0xA2;
	GATEWAY_MAC[3] = 0x00;
	GATEWAY_MAC[4] = 0x40;
	GATEWAY_MAC[5] = 0x69;
	GATEWAY_MAC[6] = 0x73;
	GATEWAY_MAC[7] = 0x74;
	
	#ifdef NODE_DEBUG
		defaultTime2Wake = 1;   //"00:00:00:10"
	#else
		defaultTime2Wake = 90;	//"00:00:15:00";
	#endif
	convertTime2Wait2Char(defaultTime2Wake);	
}


void WaspXBeeZBNode::setGatewayMacAddress(uint8_t address[8])
{
	GATEWAY_MAC[0] = address[0];
	GATEWAY_MAC[1] = address[1];
	GATEWAY_MAC[2] = address[2];
	GATEWAY_MAC[3] = address[3];
	GATEWAY_MAC[4] = address[4];
	GATEWAY_MAC[5] = address[5];
	GATEWAY_MAC[6] = address[6];
	GATEWAY_MAC[7] = address[7];
}


uint8_t WaspXBeeZBNode::setNewSleepTime(uint16_t newTime)
{
	uint8_t error = 2;
	
	if( newTime > 0)
	{
		error = 0;
		defaultTime2Wake = newTime;
		convertTime2Wait2Char(newTime);
	}
	else
		error = 1;
	
	return error;
}


void WaspXBeeZBNode::setActiveSensorMask(int count, ...)
{
	va_list arguments;
	va_start (arguments, count);
	//int types;
	//uint16_t types = 0;
	activeSensorMask = 0;
	int sum = 0;
	
	for(int x = 0; x < count; x++)
	{
		activeSensorMask +=  va_arg( arguments, int );
		#ifdef NODE_DEBUG
			USB.print("sensorMask in for = ");
			USB.println( (int) activeSensorMask );
		#endif
	}
	
	setActiveSensorMaskLength();
	
	#ifdef NODE_DEBUG
	USB.print("sum sensorMask = ");
	USB.println( (int) activeSensorMask );
	#endif
}


void WaspXBeeZBNode::setPhysicalSensorMask(uint16_t * mask)
{
	physicalSensorMask = *mask;
	setPhysicalSensorMaskLength();
}


void WaspXBeeZBNode::setPhysicalSensorMaskLength()
{
	uint16_t indicator = 32768; // 2^15
	physicalSensorMaskLength = 16;
	bool stop = false;
	uint8_t i = 0;
	
	while( i<16 && !stop)
	{
		if(indicator & physicalSensorMask)
			stop = true;
		else
			physicalSensorMaskLength--;
		
		i++;
		indicator >>= 1;
	}
	
	#ifdef NODE_DEBUG
		USB.print("physicalSensorMaskLength = ");
		USB.println( (int) physicalSensorMaskLength);
	#endif
}


void WaspXBeeZBNode::setActiveSensorMaskLength()
{
	uint16_t indicator = 32768; // 2^15
	activeSensorMaskLength = 16;
	bool stop = false;
	uint8_t i = 0;
	
	while( i<16 && !stop)
	{
		if(indicator & activeSensorMask)
			stop = true;
		else
			activeSensorMaskLength--;
		
		i++;
		indicator >>= 1;
	}
	
	#ifdef NODE_DEBUG
		USB.print("activeSensorMaskLength = ");
		USB.println( (int) activeSensorMaskLength);
	#endif
}


void WaspXBeeZBNode::printSensorMask(uint16_t mask)
{
	USB.print("sensorMask = ");
	USB.println( (int) mask );
}


void WaspXBeeZBNode::convertTime2Wait2Char(uint16_t t2w)
{
	uint8_t numSecs = 0;
	uint8_t numMins = 0;
	uint8_t numHours = 0;
	uint8_t numDays = 0;
	uint16_t temp = 0;
	char  days[3] = {'0', '0', '\0'};// = (char *) calloc(2, sizeof(char));
	char hours[3] = {'0', '0', '\0'};// = (char *) calloc(2, sizeof(char));
	char  mins[3] = {'0', '0', '\0'};// = (char *) calloc(2, sizeof(char));
	char  secs[3] = {'0', '0', '\0'};// = (char *) calloc(2, sizeof(char));
	
	#ifdef NODE_TIME_DEBUG
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
		#ifdef NODE_TIME_DEBUG
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
		#ifdef NODE_TIME_DEBUG
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
		#ifdef NODE_TIME_DEBUG
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
			#ifdef NODE_TIME_DEBUG
				USB.print("numSecs = "); USB.println( (int) numSecs); 
				USB.print("* secs = "); USB.println(secs);
			#endif	
		}
	}
	
	sprintf(time2wake, "%s:%s:%s:%s", days, hours, mins, secs);	
	
	#ifdef NODE_TIME_DEBUG
		USB.print("*Days = "); USB.println(days);
		USB.print("*Hours = "); USB.println(hours);
		USB.print("*Minutes = "); USB.println(mins);
		USB.print("*Seconds = "); USB.println(secs);
			
		USB.print("time2wake = "); USB.println(time2wake);  //"dd:hh:mm:ss"
	#endif
}









WaspXBeeZBNode	xbeeZB = WaspXBeeZBNode();
