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
	
	if(defaultOperation)
		setNewDefaultTime2Sleep(value);
		
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
		//store more: these values are stored only once via changeSensorFrequencies()
	}
	
	PWR.hibernate(xbeeZB.defaultTime2WakeStr, RTC_OFFSET, RTC_ALM1_MODE2);
}
  
  
uint8_t WaspXBeeZBNode::setNewDefaultTime2Sleep(uint16_t newTime)
{
	uint8_t error = 2;
	
	if( newTime > 0)
	{
		error = 0;
		uint16_t indicator = 1;
		
		defaultOperation = true;
		
		defaultTime2WakeInt = newTime;  //stored in hibernate()
		convertTime2Wait2Char(newTime, defaultTime2WakeStr);
		
		for(uint8_t i=0; i<NUM_SENSORS; i++)
		{
			if(indicator & activeSensorMask)
				SensUtils.measuringInterval[i] = newTime;
				
			indicator <<= 1;
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
		indicator <<= 1;
	}
	
	if( error == 2 )
	{
		memset(PackUtils.packetData, 0, MAX_DATA);
		sprintf(PackUtils.packetData, "Node ' %s ' received empty sensor mask", xbeeZB.nodeID);
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, PackUtils.packetData);	
	} //error = 0
	
	if( sensorsToDisableMask > 0 )
	{
		//disables sensors and checks if we can go back to defaultOperation mode
		error = disableSensors(&sensorsToDisableMask);
		
		if(error == 1)
		{
			memset(PackUtils.packetData, 0, MAX_DATA);
			sprintf(PackUtils.packetData, "Node ' %s ' has no more active sensors, sleeptime = 1 min", xbeeZB.nodeID);
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, PackUtils.packetData);
		}
	} //error = 0
	
	if(! defaultOperation )
	{
		//finds and saves the next x hibernate times.
		//in case all sensors are set to the same interval defaultOperation mode is enabled again
		error = setNewDifferentSleepTimes();
		if(error)
		{
			memset(PackUtils.packetData, 0, MAX_DATA);
			sprintf(PackUtils.packetData, "Node ' %s ' had an error in ", xbeeZB.nodeID);
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, PackUtils.packetData);
		}
	}
	
	return error; //if not zero send error message in PAQUtils, else send CH_SENS_FREQ_RES
}


/*static*/ int compare(const void *x, const void *y) 
{
	return  (*(int*)x - *(int*)y);
}
  



uint8_t WaspXBeeZBNode::setNewDifferentSleepTimes()
{
	
	uint16_t max = 0, min = 0, pos = 0, i=1;
	uint8_t factor = 0, error = 2, mode = 0;
	uint16_t * sortedTimes = (uint16_t *) calloc(activeSensorMaskLength, sizeof(uint16_t));
	uint16_t LCM = 0;

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
	
		SensUtils.saveSensorMeasuringIntervalTimes();
		
		//This is the first (and possible last) time this function is executed:
		//Set sens_i[NUM_SENSORS] = 1
		createAndSaveNewTime2SleepArray(mode, sortedTimes);
		
	}
	free(sortedTimes);
	sortedTimes = NULL;
}



	
	
void WaspXBeeZBNode::createAndSaveNewTime2SleepArray(int mode, uint16_t * sortedTimes)
{
	uint8_t sensorNr = 0, factor = 0;
	uint8_t EEPROM_OFFSET = 0;
	uint16_t * time2wValuesArray = (uint16_t *) calloc(MAX_DATA_PER_SENSOR*nrActivatedSensors, sizeof(uint16_t));
	uint16_t * sensMultiplier = (uint16_t *) calloc(nrActivatedSensors, sizeof(uint16_t));
	uint16_t maxInArray = 0, lengthArray = 1, LCM = 0;
	for(uint16_t i=0; i<nrActivatedSensors; i++)  //memset(sensMultiplier, 1, nrActivatedSensors);  doesn't work
		sensMultiplier[i] = 1;
	
	
	/////////////////////////////////
	// CALCULATE LCM
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
	
		#ifdef HIBERNATE_DEBUG
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
	// CALCULATE VALUES
	/////////////////////////////////
	pos = 0;
		//FOR EACH ACTIVE SENSOR:
		for(sensorNr = 0; sensorNr<nrActivatedSensors; sensorNr++)
		{
			
			bool mayBeAdded = false;
			//FOR LOWEST TIME VALUES
			if( sensorNr == 0 )  
			{
				while( (pos < MAX_DATA_PER_SENSOR) && (sortedTimes[0] * sensMultiplier[0] <= sortedTimes[nrActivatedSensors-1]) )
				{
					time2wValuesArray[pos] = sortedTimes[0] * sensMultiplier[0];
					pos++;  sensMultiplier[0]++;
					if( sortedTimes[0] * sensMultiplier[0] > sortedTimes[nrActivatedSensors-1] )
					{
						maxInArray = sortedTimes[nrActivatedSensors-1];
						break;
					}
					else if( pos == MAX_DATA_PER_SENSOR)
					{
						maxInArray = time2wValuesArray[--pos];
					}
				}
				#ifdef HIBERNATE_DEBUG
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
				while( (sortedTimes[sensorNr] * sensMultiplier[sensorNr] <= maxInArray ) )
				{
				
					time2wValuesArray[pos] = sortedTimes[sensorNr] * sensMultiplier[sensorNr];
					pos++;  sensMultiplier[sensorNr]++;
					/*
					if( sortedTimes[sensorNr] * sensMultiplier[sensorNr] > sortedTimes[nrActivatedSensors-1] )
					{
						break;
					}
					*/
					#ifdef HIBERNATE_DEBUG
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

	#ifdef HIBERNATE_DEBUG
		USB.print("\nFULLtime2wValuesArray: ");
		for(uint16_t i=0; i<pos; i++)
		{
			USB.print(time2wValuesArray[i]);
			USB.print(" ");
		}
	#endif
		
	/////////////////////////////////
	// STORE VALUES
	/////////////////////////////////
			
		//first: sort array and remove duplicates
		qsort(time2wValuesArray, pos, sizeof(uint16_t), compare);
			#ifdef HIBERNATE_DEBUG_2
				USB.print("\nsorted ");
				for(uint16_t i=0; i<pos; i++)
					USB.print(time2wValuesArray[i]);
			#endif
		
		 //delete duplicate values
		uint16_t newPos = 0; 
		for(uint8_t i=1; i<pos; i++)
		{
			if(time2wValuesArray[newPos] != time2wValuesArray[i]) 
			{
				time2wValuesArray[++newPos] = time2wValuesArray[i];	
				lengthArray++;
			}
		}
		
		time2wValuesArray[lengthArray] = 0;
		
			#ifdef HIBERNATE_DEBUG
				USB.print("\nno duplicates ");
				for(uint16_t i=0; i<lengthArray; i++)
				{
					USB.print(time2wValuesArray[i]);
					USB.print(" ");
				}
			#endif
		
		//take differences
		/*
		if(stop)
		{
			//store differences
			uint16_t pos_eeprom = START_T2W_VALUES+EEPROM_OFFSET;
			for(uint16_t i=0; i<ARRAY_LENGTH; i++)
			{
					storeValue(pos_eeprom, time2wValuesArray[i]%256);
					pos_eeprom++;
					storeValue(START_T2W_VALUES+EEPROM_OFFSET+i+1, time2wValuesArray[i]/256);
					pos_eeprom++;
			}
			EEPROM_OFFSET = 0;
			
			//if we have non-integer multiples of MAX then we can repeat the process to reduce EEPROM write cycles
			//else we can just restart reading from the beginning.
			
		}
		else
		{
		
		//ALSO STORE VALUES!!! SO WE CAN RE-USE THE MALLOC
		//if not easy: must store multipliers and timevalues so you can calculate the next values once hibernate has gone through all values

		
				/*
				//FOR MIDDLE TIME VALUES
				else if( sensorNr < activeSensorMaskLength ) 
				{
					while(pos < MAX_DATA_PER_SENSOR * sensorNr && sortedTimes[sensorNr] * sensMultiplier[sensorNr] <= sortedTimes[activeSensorMaskLength])
					{
						//if this value is smaller then all other values present in the array, it may be added, else MAYBE..:
						value = sortedTimes[sensorNr] * sensMultiplier[sensorNr];
						mayBeAdded = false;
						for(uint8_t i=0; i<sensorNr; i++)
						{
							if( value <= sortedTimes[i] * sensMultiplier[i])
								mayBeAdded = true;
							else
								mayBeAdded = false;
						}
						
						if(mayBeAdded)
						{
							time2wValuesArray[pos] = value;
							pos++; sensMultiplier[sensorNr]++;
						}
						//but: if it is bigger than all previous max values AND still smaller or equal than sortedTimes[activeSensorMaskLength], than it 
						//may also be added, BUT, only once so you (the sensMultiplier) don't take lead on the other values
						else
						{						
							if( value <= sortedTimes[activeSensorMaskLength] )
							{
								time2wValuesArray[pos] = value;
								pos++; sensMultiplier[sensorNr]++;
								break;  //only add once: get out of while
							}
						}
					}
				}
				//FOR MAX TIME VALUE
				else if( sensorNr == activeSensorMaskLength ) 
				{
					//-if THIS ONE HERE is added than you may stop repeating....
					//-may only be added once to the array...  but it can already be in it by integer multiples of lower values
					//-biggest value may only be added if for each smaller value, the sensMultiplier+1 leads to > this biggest value OR if the biggest
					// multiple of the smaller value is equal to this biggest value
					value = sortedTimes[sensorNr] * 1;
					mayBeAdded = false; 
					for(uint8_t i=0; i<activeSensorMaskLength - 1; i++)
					{
						if(sortedTimes[i] * sensMultiplier[i + 1] > value)
							mayBeAdded = true;
						else
							mayBeAdded = false;
					}
					
					if(mayBeAdded)
					{
						stop = true;  //ARRAY WAS LONG ENOUGH, STOP??????? TO SAVE EEPROM, STOP = FALSE and calculate bigger values also..
									  //BUT FIRST WRITE THEM TO EEPROM!!
						time2wValuesArray[pos] = value;
					}
				}//end if	
			}//end for
			*/
			
			

	
	
	
	
}
	
	
	
	
	
	
	
	
/*	
	}

	//calculate all multiples of the sensor measuring times within the maximum measuring time:
	
	
	indicator = 1;

	//store i and factor;
	storeValue(FACTOR_TO_INCREASE_BEFORE_REPEATING, factor);
	storeValue(FACTOR_TO_MULTIPLY_L, i%256);
	storeValue(FACTOR_TO_MULTIPLY_H, i/256);
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
*/
	
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
		#ifdef MATH_DEBUG
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
	
	#ifdef MATH_DEBUG
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




WaspXBeeZBNode xbeeZB = WaspXBeeZBNode();
