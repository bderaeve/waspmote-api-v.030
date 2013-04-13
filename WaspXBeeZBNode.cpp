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
	panid[7] = 0x0B;

	GATEWAY_MAC[0] = 0x00; //0x00;  //When GATEWAY == COORDINATOR use 0
	GATEWAY_MAC[1] = 0x00; //0x13;
	GATEWAY_MAC[2] = 0x00; //0xA2;
	GATEWAY_MAC[3] = 0x00; //0x00;
	GATEWAY_MAC[4] = 0x00; //0x40;
	GATEWAY_MAC[5] = 0x00; //0x69;
	GATEWAY_MAC[6] = 0x00; //0x73;
	GATEWAY_MAC[7] = 0x00; //0x74;
	
	nrOfPanics = 0;
	defaultOperation = true;
	powerPlan = HIGHPERFORMANCE;
	sleepMode = HIBERNATE;
	
	#ifdef HIBERNATE_DEBUG
		//defaultTime2WakeInt = 1;   //"00:00:00:10"
		defaultTime2WakeInt = 6;   //"00:00:01:00"
	#else
		defaultTime2WakeInt = 90;	//"00:00:15:00";
	#endif
	RTCUt.convertTime2Char(defaultTime2WakeInt, 0, defaultTime2WakeStr);	
	
	testVar = 0;
}


void WaspXBeeZBNode::testFunc()
{
	testVar++;

}

/**************************************************************************************
  *
  * ARCHITECTURE AND SYSTEM UTILITIES  +  DEFAULT SETTINGS
  *
  *************************************************************************************/
void WaspXBeeZBNode::hibernateInterrupt()
{
	///////////////////////////////////////
	// 1. Read time
	///////////////////////////////////////
	
	RTC.ON();
	RTCUt.getTime();
	RTC.setMode(RTC_OFF,RTC_NORMAL_MODE);  //this will save power
	
		#ifdef HIBERNATE_DEBUG_V2
			USB.print("\n\nAWAKE at: "); USB.println(RTCUt.getTime());
			#ifdef HIBERNATE_DEBUG_V1
				Utils.blinkLEDs(1000);
				Utils.blinkLEDs(1000);
			#endif
		#endif
		
		
	///////////////////////////////////////
	// 2. Clear interrupt flag
	///////////////////////////////////////	
	
    intFlag &= ~(HIB_INT);
	
	///////////////////////////////////////////////
	// 3. Read some necessary variables from eeprom
	///////////////////////////////////////////////
	
	readCoreVariablesFromEEPROM();
	
	///////////////////////////////////////////////
	// 4. Check if RTC is ok
	///////////////////////////////////////////////	
	
	/*
	if( resetRTC ) 
	{
		RTCUt.reinitialize();
		
		if( eepromReadMode == DIFFICULT )
			recalculateTimesFromBeginning();
	}		
	else if( mustCalculateNextTimes )
		calculateNextTimes();  //in this function: if max == LCM, lastArrayOfValues = true
	*/
	
	///////////////////////////////////////
	// 4. Set next time to wake up
	///////////////////////////////////////
	
	RTCUt.setNextTimeWhenToWakeUpViaOffset(3);
	/*
	if(defaultOperation)
		RTCUt.setNextTimeWhenToWakeUpViaOffset( ( (unsigned int) Utils.readEEPROM(DEFAULT_T2W_H) ) * 256
			+ Utils.readEEPROM(DEFAULT_T2W_L));
	else
	{	//find the next time to sleep:
		uint16_t storedTime = 0;
		uint8_t nrDays = 0;
		uint16_t lastValuePos = Utils.readEEPROM(NR_OF_TIMES_STORED_IN_EEPROM) + Utils.readEEPROM(START_T2W_VALUES);
		bool lastStoredValue = true;
		
		for(lastValuePos; lastValuePos > START_T2W_VALUES; lastValuePos--)
		{
			storedTime = (unsigned int) (Utils.readEEPROM(lastValuePos) ) * 256 + Utils.readEEPROM(--lastValuePos);
			
			if( storedTime >= ONE_DAY )
			{
				nrDays = storedTime / ONE_DAY;
				storedTime %= ONE_DAY;		
			}
			
			if( (storedTime % RTCUt.nextTime2WakeUpHoursMinsSecsInt == 0) && (nrDays % RTC.day == 0) )
			{
				RTCUt.setNextTimeWhenToWakeUpViaOffset( storedTime + nrDays * ONE_DAY );
				
				if(lastStoredValue)
				{
					//Depending on the mode, next time we awake, we must reset the RTC to zero or 
					//calculate the next times to save in EEPROM
					if( eepromReadMode == EASY )
						storeValue(MUST_RESET_RTC, true);
					else
					{
						if(lastArrayOfValues)
							storeValue(MUST_RESET_RTC, true);
						else
							storeValue(MUST_CALCULATE_NEXT_TIMES, true);
					}
				}
				
				break;
			}
			lastStoredValue = false;
		}

	}
	*/
	
	// PWR.HIBERNATE DOES NOT ACCEPT OWN LIBELIUM ALARM FUNCTIONS...
	
	//USB.print("setAlarm1 30s offset:");
	//RTC.setAlarm1(0,0,0,50,RTC_OFFSET,RTC_ALM1_MODE2);
	//USB.println(RTC.getAlarm1());
	//PWR.hibernate(RTC.getAlarm1(), RTC_ABSOLUTE, RTC_ALM1_MODE3);
}


uint8_t WaspXBeeZBNode::storeValue(int address, uint8_t value)
{
	uint8_t error = 2;
	
	if(address > 291 && address < 4096)
	{
		error = 0;
		//Utils.writeEEPROM(address, value);	
	}
	else
	{	
		error = 1;
	}	
}


void WaspXBeeZBNode::readCoreVariablesFromEEPROM()
{
	//SENSOR_MASKS:  in case of incoming requests
	physicalSensorMask = ( (unsigned int) Utils.readEEPROM(PHY_MASK_H) ) * 256
		+ Utils.readEEPROM(PHY_MASK_L);
	physicalSensorMaskLength = Utils.readEEPROM(PHY_MASK_LEN);
	activeSensorMask = ( (unsigned int) Utils.readEEPROM(ACT_MASK_H) ) * 256
		+ Utils.readEEPROM(ACT_MASK_L);	
	activeSensorMaskLength = Utils.readEEPROM(ACT_MASK_LEN);

	//SLEEP
	defaultOperation = Utils.readEEPROM(OPERATING_MODE);
	resetRTC = Utils.readEEPROM(MUST_RESET_RTC);
	eepromReadMode = Utils.readEEPROM(EEPROM_READ_MODE);
	powerPlan = (PowerPlan) Utils.readEEPROM(POWERPLAN);
	sleepMode = (SleepMode) Utils.readEEPROM(SLEEPMODE);	
	//defaultTime2WakeInt++;
	//	RTCUt.convertTime2Char(defaultTime2WakeInt, defaultTime2WakeStr);
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


uint8_t WaspXBeeZBNode::setActiveSensorTimes(uint16_t count, ...)
{
	va_list arguments;
	va_start (arguments, count);
	
	uint16_t * times = (uint16_t *) calloc(count, sizeof(uint16_t));
	
	for(uint8_t i = 0; i<count; i++)
		times[i] = va_arg( arguments, uint16_t );
	
	qsort(times, count, sizeof(uint16_t), compare);
	
	createAndSaveNewTime2SleepArray(times);
	
	free(times);
	times = NULL;
}


void WaspXBeeZBNode::setActiveSensorMask(uint8_t * mask)
{
	activeSensorMask = ( (unsigned int) mask[0]*256) + mask[1];
	
	storeValue(ACT_MASK_H, mask[0]);	
	storeValue(ACT_MASK_L, mask[1]);

	setActiveSensorMaskLength();
}


void WaspXBeeZBNode::setPhysicalSensorMask(uint8_t * mask)
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
	nrActivatedSensors = getNrActiveSensors(activeSensorMask);
	
	storeValue(ACT_MASK_LEN, activeSensorMaskLength);
	
	#ifdef NODE_DEBUG
		USB.print("activeSensorMaskLength = ");
		USB.println( (int) activeSensorMaskLength);
		USB.print("nrActiveSens ");
		USB.println( (int) nrActivatedSensors);
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

uint8_t WaspXBeeZBNode::getNrActiveSensors(uint16_t mask)
{
	uint8_t nr = 0;
	uint16_t indicator = 1;
	
	for(uint8_t i=0; i<16; i++)
	{
		if(indicator & mask)
			nr++;
			
		indicator <<= 1;
	}
	
	return nr;
}


uint8_t WaspXBeeZBNode::disableSensors(uint16_t * toDisableMask)
{
	uint8_t error = 2;
	//update mask:
	activeSensorMask &= (~(*toDisableMask));
	setActiveSensorMaskLength();
	
	//check if some sensor times are still different or if we can go back
	//to default mode:
	
	//A. find first value to compare with
	uint16_t indicator = 1;
	uint16_t value = 0;
	for(uint8_t i=0; i<activeSensorMaskLength; i++)
	{
		if(  indicator & activeSensorMask ) 
		{
			error = 0;
			value = SensUtils.measuringInterval[i];
			defaultOperation = true;
			break;
		}
		indicator <<= 1;
	}
	
	//B. compare first value with the others
	for(uint8_t i=0; i<activeSensorMaskLength; i++)
	{
		if(  indicator & activeSensorMask ) 
		{
			if( value != SensUtils.measuringInterval[i] )
			{
				defaultOperation = false;
				break;
			}
		}
		indicator <<= 1;
	}
	
	if(defaultOperation)  //store if the mode has changed
	{
		setNewDefaultTime2Sleep(value);
		storeValue(OPERATING_MODE, defaultOperation); 
	}
		
	//If no active sensors were found set time2sleep at 1 minute
	if(error == 2)
	{
		error = 1;
		setNewDefaultTime2Sleep(6);
	}
	

	
	return error;
}


void WaspXBeeZBNode::printSensorMask(uint16_t mask)
{
	USB.print("sensorMask = ");
	USB.println( (int) mask );
}


/**************************************************************************************
  *
  * DEEP SLEEP / HIBERNATE UTILITIES
  *
  *************************************************************************************/
void WaspXBeeZBNode::enterLowPowerMode(SleepMode sm, uint16_t howLong)
{
	if(sm == HIBERNATE)
	{
		RTCUt.getTime();
		RTCUt.setNextTimeWhenToWakeUpViaOffset(howLong);
		PWR.hibernate(RTCUt.nextTime2WakeUpChar, RTC_ABSOLUTE, RTC_ALM1_MODE3);
	}
	else
	{	
		for(uint16_t i=0; i<howLong*5; i++) 
			PWR.sleep(WTD_2S, ALL_OFF);
		
		if( intFlag & WTD_INT )  //"can be used as out of sleep interrupt"
        {
			intFlag &= ~(WTD_INT);
        }
      	USB.begin();
			#ifdef HIBERNATE_DEBUG_V2
				USB.print("out sleep");
			#endif
	}	
}
 
 
void WaspXBeeZBNode::hibernate()
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
		
		
	///////////////////////////////////////
	// 1. Store operating variables
	///////////////////////////////////////		
	
	defaultOperation == Utils.readEEPROM(OPERATING_MODE) ? : storeValue(OPERATING_MODE, defaultOperation);
	sleepMode == (SleepMode) Utils.readEEPROM(SLEEPMODE) ? : storeValue(SLEEPMODE, sleepMode);
	powerPlan == (PowerPlan) Utils.readEEPROM(POWERPLAN) ? : storeValue(POWERPLAN, powerPlan);
	
	
	if(defaultOperation)
	{

	}
	else
	{
		//store more: these values are stored only once via changeSensorFrequencies()
	}
	
	//PWR.hibernate(RTC.getAlarm1(),RTC_ABSOLUTE,RTC_ALM1_MODE3);
	
	//PWR.hibernate("04:15:00:30",RTC_ABSOLUTE,RTC_ALM1_MODE3);
	
	PWR.hibernate(RTCUt.nextTime2WakeUpChar, RTC_ABSOLUTE, RTC_ALM1_MODE3);
	
	//PWR.hibernate(xbeeZB.defaultTime2WakeStr, RTC_OFFSET, RTC_ALM1_MODE2);
}

  
/**************************************************************************************
  *
  * TIMING UTILITIES
  *
  *************************************************************************************/  
uint8_t WaspXBeeZBNode::setNewDefaultTime2Sleep(uint16_t newTime)
{
	uint8_t error = 2;
	
	if( newTime > 0)
	{
		error = 0;
		defaultOperation = true;
		
		defaultTime2WakeInt = newTime;  
		
		if(sleepMode == HIBERNATE)
		{
			storeValue(DEFAULT_T2W_H, defaultTime2WakeInt/256);	
			storeValue(DEFAULT_T2W_L, defaultTime2WakeInt%256);
		}
		
		RTCUt.setNextTimeWhenToWakeUpViaOffset(defaultTime2WakeInt);		
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
	//uint8_t maskLength = getMaskLength(sensorsToChangeMask);
	
	// i indicates which sensor
	// pos indicates where the sensor's time is in the array data, starting at 3
	for(uint8_t i=0; i<NUM_SENSORS; i++)
	{
		if(  indicator & sensorsToChangeMask ) 
		{
			error = 0;
			//calculate received time
			SensUtils.measuringInterval[i] = ( (unsigned int) array[pos++]*256) + array[pos];
			pos++;
			
			//if 0 set the sensor to be disabled in the active sensor mask.
			if(SensUtils.measuringInterval[i] == 0)
			{
				sensorsToDisableMask |= indicator;
			}	
			//if not 0, check if different than current default waking time
			else
			{
				//possibly activate more sensors
				activeSensorMask |= sensorsToChangeMask;
				
				//check if the added time will influence current sleeping times
				if(defaultOperation == true)
				{
					if(SensUtils.measuringInterval[i] != defaultTime2WakeInt)
					{
						defaultOperation = false;
					}
				}
			}
		}
		else
		{
			if( indicator & activeSensorMask )
				SensUtils.measuringInterval[i] = defaultTime2WakeInt;
			else
				SensUtils.measuringInterval[i] = 0;
		}
		indicator <<= 1;
	}
	
	if( error == 2 )
	{
		PackUtils.packetSize = 1;
		PackUtils.packetData[0] = NODE_RECEIVED_EMPTY_SENSOR_MASK_IN_CH_SENS_FREQ_REQ;
		char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
		PackUtils.escapeZerosInPacketData(contentToSend);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, SEND_ERROR, contentToSend);
		free(contentToSend);
		contentToSend = NULL;
		return error;
	} //error = 0 
	
	if( sensorsToDisableMask > 0 )
	{
		//disables sensors and checks if we can go back to defaultOperation mode
		error = disableSensors(&sensorsToDisableMask);
		
		if(error == 1)
		{
			PackUtils.packetSize = 1;
			PackUtils.packetData[0] = NODE_HAS_NO_LONGER_ACTIVE_SENSORS;
			char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
			PackUtils.escapeZerosInPacketData(contentToSend);
			
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, SEND_ERROR, contentToSend);
			free(contentToSend);
			contentToSend = NULL;
			return error;
		}
	} //error = 0
	
	if(! defaultOperation )
	{
		//finds and saves the next x hibernate times.
		//in case all sensors are set to the same interval defaultOperation mode is enabled again
		error = setNewDifferentSleepTimes();
		if(error)
		{
			PackUtils.packetSize = 1;
			PackUtils.packetData[0] = NODE_HAD_AN_ERROR_IN_SET_NEW_DIFFERENT_SLEEP_TIMES;
			char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
			PackUtils.escapeZerosInPacketData(contentToSend);
			
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, SEND_ERROR, contentToSend);
			free(contentToSend);
			contentToSend = NULL;
			return error;
		}
		
		//store the new operating mode
		storeValue(OPERATING_MODE, defaultOperation); 
	}
	
	return error; //if not zero send error message in PAQUtils, else send CH_SENS_FREQ_RES
}


/*static*/ int compare(const void *x, const void *y) 
{
	return  (*(int*)x - *(int*)y);
}
  

uint8_t WaspXBeeZBNode::setNewDifferentSleepTimes()
{
	
	//uint16_t max = 0, min = 0, pos = 0, i=1;
	uint8_t error = 2;
	uint16_t * sortedTimes = (uint16_t *) calloc(activeSensorMaskLength, sizeof(uint16_t));
	//uint16_t LCM = 0;

	//////////////////////////////////////////
	// SENSOR MEASURING INTERVALS
	//////////////////////////////////////////
	
	//create a temp array and fill it with the measuring intervals
	for(uint8_t i=0; i<NUM_SENSORS; i++)
	{
		if(SensUtils.measuringInterval[i] > 0)
			sortedTimes[i] = SensUtils.measuringInterval[i];
	}
	
	//sort the array:
	qsort(sortedTimes, activeSensorMaskLength, sizeof(uint16_t), compare);
	
	//estimate size of wake times array; factor will be used after x sleep cycles to calculate
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
		//factor = ( (max/min)/MAX_DATA + 1); 
		
		storeValue(MIN_SENS_TIME_L, SensUtils.minTime%256);	storeValue(MIN_SENS_TIME_H, SensUtils.minTime/256);
		storeValue(MAX_SENS_TIME_L, SensUtils.maxTime%256);	storeValue(MAX_SENS_TIME_H, SensUtils.maxTime/256);
		//storeValue(FACTOR_TO_INCREASE_BEFORE_REPEATING, factor);
		//storeValue(FACTOR_TO_MULTIPLY_L, i%256);
		//storeValue(FACTOR_TO_MULTIPLY_H, i/256);
	
		/// if mode == hibernate
		SensUtils.saveSensorMeasuringIntervalTimes();
		
		//This is the first (and possible last) time this function is executed:
		//Set sens_i[NUM_SENSORS] = 1
		createAndSaveNewTime2SleepArray(sortedTimes);
		
	}
	free(sortedTimes);
	sortedTimes = NULL;
}

	
void WaspXBeeZBNode::createAndSaveNewTime2SleepArray(uint16_t * sortedTimes)
{
	uint8_t sensorNr = 0, factor = 0, mode = EASY;
	/*uint16_t * */time2wValuesArray = (uint16_t *) calloc(MAX_DATA_PER_SENSOR*nrActivatedSensors, sizeof(uint16_t));
	lengthArray = 1;
	/*uint16_t * */sensMultiplier = (uint16_t *) calloc(nrActivatedSensors, sizeof(uint16_t));
	uint16_t maxInArray = 0,  LCM = 0, pos_eeprom = 0, eeprom_offset = 0;
	
	for(uint16_t i=0; i<nrActivatedSensors; i++)  //memset(sensMultiplier, 1, nrActivatedSensors);  doesn't work
		sensMultiplier[i] = 1;
	
	
	/////////////////////////////////
	// 1. CALCULATE LCM
	/////////////////////////////////
	LCM = calculateMaxNrElementsForEEPROMArray(sortedTimes);
		#ifdef MATH_DEBUG
			USB.print("LCM = "); USB.println( (int) LCM);
		#endif
	if( LCM/sortedTimes[0] < MAX_DATA_PER_SENSOR )
	{
		mode = EASY;
		factor = 1;
	}
	else
	{
		mode = DIFFICULT;
		factor = calculateMaxNrElementsForEEPROMArray(sortedTimes) / MAX_DATA_PER_SENSOR;
	}
	
		#ifdef MATH_DEBUG
			USB.print("\nsortedTimes: ");
			qsort(sortedTimes, nrActivatedSensors, sizeof(uint16_t), compare);
			for(uint16_t i=0; i<nrActivatedSensors; i++)
			{
				USB.print(sortedTimes[i]);
			}
			USB.print("\nsensMultiplier: ");
			for(uint16_t i=0; i<nrActivatedSensors; i++)
			{
				USB.print(sensMultiplier[i]);
			}
		#endif
	
	/////////////////////////////////
	// 2. CALCULATE VALUES
	/////////////////////////////////
	pos = 0;
		//FOR EACH ACTIVE SENSOR:
		for(sensorNr = 0; sensorNr<nrActivatedSensors; sensorNr++)
		{
			
			bool mayBeAdded = false;
			//FOR LOWEST TIME VALUES
			if( sensorNr == 0 )  
			{
				while( (pos < MAX_DATA_PER_SENSOR) && (sortedTimes[0] * sensMultiplier[0] <= LCM ) )//sortedTimes[nrActivatedSensors-1]) )
				{
					time2wValuesArray[pos] = sortedTimes[0] * sensMultiplier[0];
					pos++;  sensMultiplier[0]++;
					if( sortedTimes[0] * sensMultiplier[0] > LCM )//sortedTimes[nrActivatedSensors-1] )
					{
						maxInArray = sortedTimes[nrActivatedSensors-1];
						break;
					}
					else if( pos == MAX_DATA_PER_SENSOR)
					{
						maxInArray = time2wValuesArray[--pos];
					}
				}
				#ifdef MATH_DEBUG_EXTENDED
					USB.print("nrSensor=0: ");
					for(uint16_t i=0; i<MAX_DATA_PER_SENSOR;i++)
					{
						USB.print(time2wValuesArray[i]);
						USB.print(" ");
					}
					USB.print("\n\n");
				#endif
			}
			else
			{
				USB.print("pos "); USB.println( (int) pos);
				USB.print("sensNr "); USB.println( (int) sensorNr);
				while( (sortedTimes[sensorNr] * sensMultiplier[sensorNr] <= LCM ) )//maxInArray ) )
				{
				
					time2wValuesArray[pos] = sortedTimes[sensorNr] * sensMultiplier[sensorNr];
					pos++;  sensMultiplier[sensorNr]++;
					/*
					if( sortedTimes[sensorNr] * sensMultiplier[sensorNr] > sortedTimes[nrActivatedSensors-1] )
					{
						break;
					}
					*/
					#ifdef MATH_DEBUG_EXTENDED
						USB.print("nrSensor=0: ");
						for(uint16_t i=0; i<MAX_DATA_PER_SENSOR;i++)
						{
							USB.print(time2wValuesArray[i]);
							USB.print(" ");
						}
						USB.print("\n\n");
					#endif
				}
			}
		}

		#ifdef MATH_DEBUG_EXTENDED
			USB.print("\nFULLtime2wValuesArray: ");
			for(uint16_t i=0; i<pos; i++)
			{
				USB.print(time2wValuesArray[i]);
				USB.print(" ");
			}
		#endif
		
	////////////////////////////////////
	// 3. SORT & REMOVE DUPLICATE VALUES
	////////////////////////////////////
			
		// 1. Sort array
		qsort(time2wValuesArray, pos, sizeof(uint16_t), compare);
			#ifdef MATH_DEBUG
				USB.print("\nsorted ");
				for(uint16_t i=0; i<pos; i++)
					USB.print(time2wValuesArray[i]);
			#endif
		
		 //delete duplicate values and determine array length
		uint16_t newPos = 0; 
		for(uint8_t i=1; i<pos; i++)
		{
			if(time2wValuesArray[newPos] != time2wValuesArray[i]) 
			{
				time2wValuesArray[++newPos] = time2wValuesArray[i];	
				lengthArray++;
			}
		}
		
		// 2. Indicate end of array
		time2wValuesArray[lengthArray] = 0;
		
			#ifdef MATH_DEBUG
				USB.print("\nno duplicates ");
				for(uint16_t i=0; i<lengthArray; i++)
				{
					USB.print(time2wValuesArray[i]);
					USB.print(" ");
				}
			#endif
	
	/////////////////////////////////
	// 4. STORE VALUES
	/////////////////////////////////	
	/*
		if( sleepMode == HIBERNATE )
		{
			eeprom_offset = lengthArray;
	
			storeValue(NR_ACTIVATED_SENSORS, nrActivatedSensors);
			storeValue(EEPROM_READ_MODE, EASY);
			storeValue(NR_OF_TIMES_STORED_IN_EEPROM, lengthArray);
			
			pos_eeprom = START_T2W_VALUES;
			for(uint16_t i=0; i<lengthArray; i++)
			{
					storeValue(pos_eeprom++, time2wValuesArray[i]%256);
					storeValue(pos_eeprom++, time2wValuesArray[i]/256);
			}
			
			if( mode == DIFFICULT )
			{
				//Also save sensor multipliers
				storeValue(EEPROM_READ_MODE, DIFFICULT);
				
				pos_eeprom = FIRST_SENS_MULTIPLIER;
				for(sensorNr = 0; sensorNr<nrActivatedSensors; sensorNr++)
				{
					storeValue(pos_eeprom++, sensMultiplier[sensorNr]%256);
					storeValue(pos_eeprom++, sensMultiplier[sensorNr]/256);				
				}
			}
		}
	*/	
		/// else if( sleepmMode == DEEPSLEEP )
		//! use pointers from xbeeZB to search the next sleep time
}

void WaspXBeeZBNode::calculateNextTimes()
{

}

void WaspXBeeZBNode::recalculateTimesFromBeginning(){}
	
//! Function to determine if the multiples can fit within EEPROM_MAX_ARRAY_LENGTH 
/*! or if we need to split up (factor > 1)
 *!   THIS FUNCTION EXPECTS A SORTED ARRAY of length ACTIVESENSORMASKLENGTH
 *!	  HERE i !~ SENSORTYPE
 *!	\return the Least Common Multiple of all elements in the array
 */  
uint16_t WaspXBeeZBNode::calculateMaxNrElementsForEEPROMArray(uint16_t * sortedTimes)
{
	uint16_t indicator = 1;
	uint16_t LCM = 0;

	for(uint8_t i=0; i<nrActivatedSensors-1; i++)
	{
		#ifdef LCM_DEBUG
			USB.print("lcm of: "); 
			USB.print( (int) sortedTimes[i]); 
			USB.print(" ");
			USB.println( (int) sortedTimes[nrActivatedSensors-1] );
		#endif
		
		if( lcm( sortedTimes[i], sortedTimes[nrActivatedSensors-1] ) > LCM )
		{
			LCM = lcm( sortedTimes[i], sortedTimes[nrActivatedSensors-1] );
		}
	}
	
	#ifdef LCM_DEBUG
		USB.print("gcd: "); USB.println( (int) gcd(3,4));
		USB.print("gcd: "); USB.println( (int) gcd(2,4));
		USB.print("gcd: "); USB.println( (int) gcd(5,10));
		USB.print("gcd: "); USB.println( (int) gcd(6,60));
		USB.print("gcd: "); USB.println( (int) gcd(10,2));
		USB.print("gcd: "); USB.println( (int) gcd(60,6));
		USB.print("gcd: "); USB.println( (int) gcd(60,60));		
		
		USB.print("lcm: "); USB.println( (int) lcm(3,4));
		USB.print("lcm: "); USB.println( (int) lcm(2,4));
		USB.print("lcm: "); USB.println( (int) lcm(5,10));
		USB.print("lcm: "); USB.println( (int) lcm(6,60));
		USB.print("lcm: "); USB.println( (int) lcm(10,2));
		USB.print("lcm: "); USB.println( (int) lcm(60,6));
		USB.print("lcm: "); USB.println( (int) lcm(60,60));		
	#endif
	
	return LCM;
}	

/* Greatest Common Divisor */
/*static*/ int gcd( int a, int b )
{
  if ( a==0 ) 
  {	
  	//USB.print(b);
	return b;
  }
  return gcd ( b%a, a );
}



/* Least Common Multiple */
uint16_t lcm( uint16_t a, uint16_t b )
{
	return (a * b) / gcd( a, b );
}


uint16_t toUint16_t(char & high, char & low)
{
	#ifdef ADD_NODE_REQ_DEBUG
		USB.println("\ntoUint16");
		USB.println( (int) high );
		USB.println( (int) low );
	#endif
	
	return  ( (uint16_t) ((  high ) * 256 ) + ( low) );
}

	

uint16_t toUint16_t(char * ar)
{
	return  ( (uint16_t) ((  ar[0] ) * 256 ) + (ar[1]) );
}




WaspXBeeZBNode xbeeZB = WaspXBeeZBNode();
