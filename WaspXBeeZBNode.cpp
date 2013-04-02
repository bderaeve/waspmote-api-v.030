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
	panid[6] = 0x13;
	panid[7] = 0x02;

	GATEWAY_MAC[0] = 0x00;
	GATEWAY_MAC[1] = 0x13;
	GATEWAY_MAC[2] = 0xA2;
	GATEWAY_MAC[3] = 0x00;
	GATEWAY_MAC[4] = 0x40;
	GATEWAY_MAC[5] = 0x69;
	GATEWAY_MAC[6] = 0x73;
	GATEWAY_MAC[7] = 0x74;
	
	#ifdef HIBERNATE_DEBUG
		defaultTime2WakeInt = 1;   //"00:00:00:10"
	#else
		defaultTime2WakeInt = 90;	//"00:00:15:00";
	#endif
	convertTime2Wait2Char(defaultTime2WakeInt, defaultTime2WakeStr);	
}

/**************************************************************************************
  *
  * ARCHITECTURE AND SYSTEM UTILITIES  +  DEFAULT SETTINGS
  *
  *************************************************************************************/
void WaspXBeeZBNode::hibernateInterrupt()
{
	#ifdef NODE_DEBUG
		USB.println("Out of hibernate (in interrupt)");
		Utils.blinkLEDs(1000);
		Utils.blinkLEDs(1000);
	#endif
    intFlag &= ~(HIB_INT);
	
	readEEPROMVariables();
	
	RTC.ON();
    RTC.setMode(RTC_OFF,RTC_NORMAL_MODE); 
	
	
	#ifdef HIBERNATE_DEBUG
		USB.print("\n\nAWAKE at: "); USB.println(RTC.getTime());
	#endif
	
	
	
	
	// PWR.HIBERNATE DOES NOT ACCEPT LIBELIUM ALARM FUNCTIONS...
	
	//USB.print("setAlarm1 30s offset:");
	//RTC.setAlarm1(0,0,0,50,RTC_OFFSET,RTC_ALM1_MODE2);
	//USB.println(RTC.getAlarm1());
}


uint8_t WaspXBeeZBNode::storeValue(int address, uint8_t value)
{
	uint8_t error = 2;
	
	if(address > 291)
	{
		error = 0;
		Utils.writeEEPROM(address, value);	
	}
	else
	{	
		error = 1;
	}	
}


void WaspXBeeZBNode::readEEPROMVariables()
{
	//SENSOR_MASK
	physicalSensorMask = ( (unsigned int) Utils.readEEPROM(PHY_MASK_H) ) * 256
		+ Utils.readEEPROM(PHY_MASK_L);
	physicalSensorMaskLength = Utils.readEEPROM(PHY_MASK_LEN);
	activeSensorMask = ( (unsigned int) Utils.readEEPROM(ACT_MASK_H) ) * 256
		+ Utils.readEEPROM(ACT_MASK_L);	
	activeSensorMaskLength = Utils.readEEPROM(ACT_MASK_LEN);

	//SLEEP TIMES
	defaultTime2WakeInt = ( (unsigned int) Utils.readEEPROM(DEFAULT_T2W_H) ) * 256
		+ Utils.readEEPROM(DEFAULT_T2W_L);
		
	defaultTime2WakeInt++;
		convertTime2Wait2Char(defaultTime2WakeInt, defaultTime2WakeStr);
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


void WaspXBeeZBNode::testPrinting()
{
	USB.print("testXBeeZBNode\n");
}


/**************************************************************************************
  *
  * NODE LAYOUT / PRESENT SENSORS 
  *
  *************************************************************************************/
void WaspXBeeZBNode::setActiveSensorMask(int count, ...)
{
	va_list arguments;
	va_start (arguments, count);

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
	
	storeValue(ACT_MASK_H, activeSensorMask/256);	
	storeValue(ACT_MASK_L, activeSensorMask%256);

	setActiveSensorMaskLength();
	
	#ifdef NODE_DEBUG
	USB.print("sum sensorMask = ");
	USB.println( (int) activeSensorMask );
	#endif
}


void WaspXBeeZBNode::setActiveSensorMask(uint8_t mask[2])
{
	activeSensorMask = ( (unsigned int) mask[0]*256) + mask[1];
	
	storeValue(ACT_MASK_H, activeSensorMask/256);	
	storeValue(ACT_MASK_L, activeSensorMask%256);

	setActiveSensorMaskLength();
}


void WaspXBeeZBNode::setPhysicalSensorMask(uint8_t mask[2])
{
	physicalSensorMask = ( (unsigned int) mask[0]*256) + mask[1];
		
	storeValue(PHY_MASK_H, mask[0]);	
	storeValue(PHY_MASK_L, mask[1]);
	
	setPhysicalSensorMaskLength();
}



void WaspXBeeZBNode::setPhysicalSensorMaskLength()
{
	physicalSensorMaskLength = getMaskLength(physicalSensorMask);
/*
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
*/	
	storeValue(PHY_MASK_LEN, physicalSensorMaskLength);
	
	#ifdef NODE_DEBUG
		USB.print("physicalSensorMaskLength = ");
		USB.println( (int) physicalSensorMaskLength);
	#endif
}


void WaspXBeeZBNode::setActiveSensorMaskLength()
{
	activeSensorMaskLength = getMaskLength(activeSensorMask) ;
		
	storeValue(ACT_MASK_LEN, activeSensorMaskLength);
	
	#ifdef NODE_DEBUG
		USB.print("activeSensorMaskLength = ");
		USB.println( (int) activeSensorMaskLength);
	#endif
}

uint8_t WaspXBeeZBNode::getMaskLength(uint16_t mask)
{
	uint16_t indicator = 32768; // 2^15
	bool stop = false;
	uint8_t i = 0;
	uint8_t length = 16;
	
	while( i<16 && !stop)
	{
		if(indicator & mask)
			stop = true;
		else
			length--;
		
		i++;
		indicator >>= 1;
	}
	
	return length;
}


void WaspXBeeZBNode::disableSensors(uint16_t * toDisableMask)
{
	activeSensorMask &= (~(*toDisableMask));
	setActiveSensorMaskLength;
}


void WaspXBeeZBNode::printSensorMask(uint16_t mask)
{
	USB.print("sensorMask = ");
	USB.println( (int) mask );
}


/**************************************************************************************
  *
  * TIMING / HIBERNATE UTILITIES
  *
  *************************************************************************************/
void WaspXBeeZBNode::hibernate()
{
	#ifdef HIBERNATE_DEBUG
		USB.print("\n\nEntering HIBERNATE at ");
		USB.println(RTC.getTime());
		USB.print("for ");
		USB.println(xbeeZB.defaultTime2WakeStr);
	#endif
	
	if(defaultOperation)
	{
		storeValue(DEFAULT_T2W_H, defaultTime2WakeInt/256);	
		storeValue(DEFAULT_T2W_L, defaultTime2WakeInt%256);
	}
	else
	{
		//store more
	}
	
	PWR.hibernate(xbeeZB.defaultTime2WakeStr, RTC_OFFSET, RTC_ALM1_MODE2);
}
  
  
uint8_t WaspXBeeZBNode::setNewDefaultTime2Sleep(uint16_t newTime)
{
	uint8_t error = 2;
	
	if( newTime > 0)
	{
		error = 0;
		defaultTime2WakeInt = newTime;  //stored in hibernate()
		convertTime2Wait2Char(newTime, defaultTime2WakeStr);
		
		for(uint8_t i=0; i<NUM_SENSORS; i++)
		{
			defaultOperation = true;
			SensUtils.measuringInterval[i] = newTime;
		}
		
	}
	else
		error = 1;
	
	return error;
}


uint8_t WaspXBeeZBNode::changeSensorFrequencies(char * array)
{
	uint8_t error = 2;
	uint16_t indicator = 1;
	uint8_t pos = 3;
	uint16_t sensorsToDisableMask = 0;
	uint16_t sensorsToChangeMask = ( (unsigned int) array[0]*256) + array[1];
	uint8_t maskLength = getMaskLength(sensorsToChangeMask);
	
	// i indicates which sensor
	// pos indicates where the sensor's time is in the array data, starting at 3
	for(uint8_t i=0; i<maskLength; i++)
	{
		if(  indicator & sensorsToChangeMask ) 
		{
			error = 0;
			//calculate new time
			SensUtils.measuringInterval[i] = ( (unsigned int) array[pos++]*256) + array[pos];
			pos++;
			
			//if 0 disable the sensor in the active sensor mask.
			if(SensUtils.measuringInterval[i] == 0)
				sensorsToDisableMask |= indicator;
				
			//check if different
			else if(defaultOperation == true)
			{
				if(SensUtils.measuringInterval[i] != defaultTime2WakeInt)
				{
					defaultOperation = false;
				}
			}
		}
		else
			error = 1;
		
		indicator <<= 1;
	}
	
	if( sensorsToDisableMask > 0 )
		disableSensors(&sensorsToDisableMask);
	
	if(! defaultOperation )
		calculateNextTime2Sleep();
	
	return error;
}


/*static*/ int compare(const void *x, const void *y) 
{
	return  (*(int*)x - *(int*)y);
}
  
/* Greatest Common Divisor */
/*static*/ int gcd( int a, int b )
{
  if ( a==0 ) return b;
  return gcd ( b%a, a );
}






void WaspXBeeZBNode::calculateNextTime2Sleep()
{
	uint16_t max = 0, min = 0, pos = 0;
	uint8_t factor = 0;
	uint16_t * sortedTimes = (uint16_t *) calloc(activeSensorMaskLength, sizeof(uint16_t));

	//create a temp array and fill it with the measuring intervals
	for(uint8_t i=0; i<activeSensorMaskLength; i++)
	{
		if(SensUtils.measuringInterval[i] > 0)
			sortedTimes[i] = SensUtils.measuringInterval[i];
	}
	
	//sort the array:
	qsort(sortedTimes, activeSensorMaskLength, sizeof(uint16_t), compare);
	

		
	//estimate size of wake times array: factor will be used after x sleep cycles to calculate
	// the next x sleep cycles in case they don't fit within MAX_DATA
	SensUtils.minTime = sortedTimes[0];
	SensUtils.maxTime = sortedTimes[activeSensorMaskLength];
	
	//after playing with the intervals it can become possible they all have the same time again:
	if( SensUtils.minTime == SensUtils.maxTime )
	{
		setNewDefaultTime2Sleep(SensUtils.minTime);
		defaultOperation = true;
	}
	else
	{
		//save the array of individual measuring times and min/max/factor:
		//need this values to compare with the time of the big array of times. if %= 0 you need to
		//measure that sensor...
		SensorUtils.saveSensorMeasuringIntervalTimes();
		factor = ( (max/min)/MAX_DATA + 1); 
		
		storeValue(MIN_SENS_TIME_L, SensUtils.minTime%256);	storeValue(MIN_SENS_TIME_H, SensUtils.minTime/256);
		storeValue(MAX_SENS_TIME_L, SensUtils.maxTime%256);	storeValue(MAX_SENS_TIME_H, SensUtils.maxTime/256);
		storeValue(2005, factor);
	
	
	}
	free(sortedTimes);
	sortedTimes = NULL;
}



	void saveNewTime2SleepArray( )
	{
		uint16_t min = ( (unsigned int) array[pos++]*256) + array[pos];
	
		time2wValuesArray = (uint16_t *) calloc(MAX_DATA*activeSensorMaskLength, sizeof(uint16_t));
	
		//calculate all multiples of the sensor measuring times within the maximum measuring time:
		for(uint8_t i=1 ; i< MAX_DATA; i++)
		{
			for(uint8_t sens=0; i<= activeSensorMaskLength; sens++)
			{
				if(sortedTimes[sens] * i * factor <= max)
					time2wValuesArray[pos] = sortedTimes[sens] * i * factor;
			}
		}
		//store i;
		
		//sort to easily delete duplicate values
		qsort(time2wValuesArray, MAX_DATA*activeSensorMaskLength, sizeof(uint16_t), compare);
		
		 //delete duplicate values
		pos = 0; 
		for(uint8_t i=1; i<MAX_DATA*activeSensorMaskLength; i++)
		{
			if(time2wValuesArray[pos] != time2wValuesArray[i]) 
				time2wValuesArray[++pos] = time2wValuesArray[i];			
		}
		
		// save these values in EEPROM + save individual sensor times + save 'factor' and 'factor_max'
		if( ++pos < MAX_EEPROM_WRITE )
		{
			pos+=1000;
			while(pos >= 1000)
			{
				storeValue(pos, time2wValuesArray[pos]);
				pos--;
			}
		}
		else
		{
			error = 1;
			#ifdef HIBERNATE_DEBUG
				USB.println("size of time2wValues exceeds MAX_EEPROM_WRITE");
		}
	}
	
	
	
	
	
	/*
	if(nrSleepTimes == 1)
	{
		void defaultMode();
	}
	else
	{
		//fill array from the back:
		{
			uint8_t nrSleepTimes2 = nrSleepTimes;
			uint16_t times[nrSleepTimes2];
			uint8_t pos = NUM_SENSORS;  // = 8: needed values are at the back of array
			
			while(nrSleepTimes >= 0)
				times[--nrSleepTimes2] = timeCopy[--pos];
		}
		
		//fill time arrays:
		if( times[0] >= 60 && times[nrSleepTimes] <= 360)
			times[nrSleepTimes] *= 10;  // save EEPROM write cycles via this trick
		
		
		sleepTimes = (uint16 *) calloc( (times[nrSleepTimes]/times[0]), sizeof(uint16_t));
		for(uint8_t i=0; i<NUM_SENSORS; i++)
		{
			timeFlags[i] 
		
		}
		
	}



	#ifdef VIA_GCD
			uint8_t nrSleepTimes = 1;
			uint16_t timeCopy[activeSensorMaskLength];

			for(uint8_t i=0; i<activeSensorMaskLength; i++)
			{
				if(SensUtils.measuringInterval[i] > 0)
					timeCopy[i] = SensUtils.measuringInterval[i];
			}
			
			#ifdef HIBERNATE_DEBUG
				timeCopy[0] = 20; timeCopy[1] = 10; timeCopy[2] = 30;
				timeCopy[3] = 10; timeCopy[4] = timeCopy[5] = timeCopy[6] = timeCopy[7] = 0;
			#endif
	
	
			//qsort(timeCopy, NUM_SENSORS, sizeof(uint16_t), compare);
			#ifdef HIBERNATE_DEBUG
				USB.print("timeCopy[0] = "); USB.println( (int) timeCopy[0] );
				USB.print("timeCopy[1] = "); USB.println( (int) timeCopy[1] );
				USB.print("timeCopy[2] = "); USB.println( (int) timeCopy[2] );
				USB.print("timeCopy[3] = "); USB.println( (int) timeCopy[3] );
				USB.print("timeCopy[0] = "); USB.println( (int) timeCopy[4] );
				USB.print("timeCopy[0] = "); USB.println( (int) timeCopy[5] );
				USB.print("timeCopy[0] = "); USB.println( (int) timeCopy[6] );
				USB.print("timeCopy[0] = "); USB.println( (int) timeCopy[7] );
			#endif
			for(uint8_t i=0; i<NUM_SENSORS-1; i++)
			{
				if( timeCopy[i] != timeCopy[i+1] )
					nrSleepTimes++;
			}
			
			#ifdef HIBERNATE_DEBUG
				USB.print("nrSleepTimes = "); USB.println( (int) nrSleepTimes );
			#endif
	#endif
	

		
}

	
	//if(nrDifferentSleepTimes == 0) // do default operation
	

}*
*/

void WaspXBeeZBNode::convertTime2Wait2Char(uint16_t t2w, char * xbeeZBTime2Wake)
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
	
	sprintf(xbeeZBTime2Wake, "%s:%s:%s:%s", days, hours, mins, secs);	
	
	#ifdef NODE_TIME_DEBUG
		USB.print("*Days = "); USB.println(days);
		USB.print("*Hours = "); USB.println(hours);
		USB.print("*Minutes = "); USB.println(mins);
		USB.print("*Seconds = "); USB.println(secs);
			
		USB.print("time2wake = "); USB.println(xbeeZBTime2Wake);  //"dd:hh:mm:ss"
	#endif
}

/*
char * WaspXBeeZBNode::getTime2Wake()
{
	return time2wake;
}
*/











WaspXBeeZBNode xbeeZB = WaspXBeeZBNode();
