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
	GATEWAY_MAC[0] = 0x00; //0x00;  //When GATEWAY == COORDINATOR use 0
	GATEWAY_MAC[1] = 0x00; //0x13;
	GATEWAY_MAC[2] = 0x00; //0xA2;
	GATEWAY_MAC[3] = 0x00; //0x00;
	GATEWAY_MAC[4] = 0x00; //0x40;
	GATEWAY_MAC[5] = 0x00; //0x69;
	GATEWAY_MAC[6] = 0x00; //0x73;
	GATEWAY_MAC[7] = 0x00; //0x74;
	
	ACCEPTED_GATEWAY_BJORN[0] = 0x40;  //Node A;
	ACCEPTED_GATEWAY_BJORN[1] = 0x69;
	ACCEPTED_GATEWAY_BJORN[2] = 0x73;
	ACCEPTED_GATEWAY_BJORN[3] = 0x7A;
	
	ACCEPTED_GATEWAY_ROEL[0] = 0x40;  //Gateway in use;
	ACCEPTED_GATEWAY_ROEL[1] = 0x69;
	ACCEPTED_GATEWAY_ROEL[2] = 0x73;
	ACCEPTED_GATEWAY_ROEL[3] = 0x74;
	
	powerPlan = HIGHPERFORMANCE;
	encryptionMode = ENCRYPTION_DISABLED;
	
	activeSensorMask = physicalSensorMask = 0x08;
	setPhysicalSensorMaskLength();
	setActiveSensorMaskLength();
	
	nrOfPanics = 0;
	defaultOperation = true;
	inNetwork = false;
	notInNetworkNrMinutesToSleep = 1;
	number_of_stored_errors = 0;
	mustSendSavedSensorValues = false;
	

	#ifdef HIBERNATE_DEBUG_V3
		defaultTime2WakeInt = 6;   //"00:00:00:10"
		//defaultTime2WakeInt = 6;   //"00:00:01:00"
	#else
		defaultTime2WakeInt = 6;	//"00:00:15:00";
	#endif
	/// The timeToWakeUp is set in COMM.setupXBee()
}


/**************************************************************************************
  *
  * ARCHITECTURE AND SYSTEM UTILITIES  +  DEFAULT SETTINGS
  *
  *************************************************************************************/
// PWR.HIBERNATE DOES NOT ACCEPT OWN LIBELIUM ALARM FUNCTIONS...

//USB.print("setAlarm1 30s offset:");
//RTC.setAlarm1(0,0,0,50,RTC_OFFSET,RTC_ALM1_MODE2);
//USB.println(RTC.getAlarm1());
//PWR.hibernate(RTC.getAlarm1(), RTC_ABSOLUTE, RTC_ALM1_MODE3);
void WaspXBeeZBNode::hibernateInterrupt()
{
	///////////////////////////////////////
	// 1. Read time
	///////////////////////////////////////
	
	RTC.ON();
	RTCUt.getTime();
	RTC.setMode(RTC_OFF,RTC_NORMAL_MODE);  //this will save power
	
		#ifdef FINAL_USB_DEBUG
			USB.print("\n\n\n-");  for(uint8_t i=0; i<60; i++){  USB.print("-"); }
		#endif 
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
	// 4. Find and set next time to wake
	///////////////////////////////////////////////	
	
	if(!inNetwork)
	{
		RTCUt.setNextTimeWhenToWakeUpViaOffset(defaultTime2WakeInt * notInNetworkNrMinutesToSleep);
	}
	else
	{
		if(defaultOperation)
			RTCUt.setNextTimeWhenToWakeUpViaOffset(defaultTime2WakeInt);
		else
		{
			findNextTime2Wake(HIBERNATE);
			xbeeZB.updatePosInArray();	
		}
			
		notInNetworkNrMinutesToSleep = 1;	
	}
	
	///////////////////////////////////////////////
	// 5. Enable XBee via a reduced setup ?
	///////////////////////////////////////////////		
	
	if( powerPlan == HIGHPERFORMANCE )
	{
          xbeeZB.init(ZIGBEE,FREQ2_4G,NORMAL);
          xbeeZB.ON();
          xbeeZB.wake(); 
	}		  
}


void WaspXBeeZBNode::setAlarmForRouter()
{
	if(defaultOperation)
	{
		RTCUt.setNextTimeWhenToMeasureInAlarm1(defaultTime2WakeInt);
		//USB.println(RTC.getAlarm1());
	}
	else
	{
		if(!posInArray) 
		{
			RTCUt.setNextTimeWhenToMeasureInAlarm1(time2wValuesArray[posInArray]);
			updatePosInArray();
		}
		else
		{
			RTCUt.setNextTimeWhenToMeasureInAlarm1(time2wValuesArray[posInArray] - time2wValuesArray[posInArray-1]);				
			updatePosInArray();
		}
	}
}


uint8_t WaspXBeeZBNode::storeValue(int address, uint8_t value)
{
	uint8_t error = 2;
	bool mustNotStore = false;
	
	mustNotStore = MUST_NOT_STORE_ON_EEPROM(address, value);
	if( mustNotStore ) return 0;
	
	if(address > 291 && address < 4090)
	{
		Utils.writeEEPROM(address, value);	
		return 0;
	}
	else
	{	
		return 1;
	}	
}


uint8_t WaspXBeeZBNode::storeValueADT(int address, uint8_t value)
{
	uint8_t error = 2;
	
	if(address > 291 && address < 4090)
	{
		Utils.writeEEPROM(address, value);	
		return 0;
	}
	else
	{	
		return 1;
	}	
}


void WaspXBeeZBNode::storeError(Errors e)
{
	if(number_of_stored_errors < 256)  number_of_stored_errors++;
	
	mustSendSavedSensorValues = true;
	storeValue(NR_OF_STORED_ERRORS, number_of_stored_errors);
	storeValue(NR_OF_STORED_ERRORS + number_of_stored_errors, (int) e);
	
	USB.print("\nSTORING ERROR MESSAGE: "); USB.print( (int) e ); USB.print(" TO EEPROM\n");
	
		#ifdef FINAL_USB_DEBUG
			uint8_t i=0;
			while(i++<10)		
			{
				Utils.setLED(LED0, LED_ON);
				delay(150);
				Utils.setLED(LED0, LED_OFF);
				delay(150);
			}
		#endif		
}


void WaspXBeeZBNode::printStoredErrors()
{
	number_of_stored_errors = Utils.readEEPROM(NR_OF_STORED_ERRORS);
	
	USB.print("\nRETRIEVED "); USB.print( (int) xbeeZB.number_of_stored_errors); USB.println(" STORED ERRORS: ");
	
	uint8_t count = 0;
	while(number_of_stored_errors > 0)
	{
		USB.println( (int) Utils.readEEPROM(FIRST_STORED_ERROR + count) );
		number_of_stored_errors--;
	}
}


void WaspXBeeZBNode::readCoreVariablesFromEEPROM()
{
	//SENSOR_MASKS:  in case of incoming requests
	inNetwork = Utils.readEEPROM(IN_NETWORK);
	if(!inNetwork)
	{
		notInNetworkNrMinutesToSleep = Utils.readEEPROM(NOT_IN_NETWORK_NR_MINUTES_TO_SLEEP);
			#ifndef HIBERNATE_DEBUG_V2
				if(notInNetworkNrMinutesToSleep < 256)  notInNetworkNrMinutesToSleep++;		
				USB.print("\nnrMins ");  USB.println( (int) notInNetworkNrMinutesToSleep );
			#endif
		defaultTime2WakeInt = ( (unsigned int) Utils.readEEPROM(DEFAULT_T2W_H)*256)
				+ Utils.readEEPROM(DEFAULT_T2W_L);				
	}
	else
	{
		physicalSensorMask = ( (unsigned int) Utils.readEEPROM(PHY_MASK_H) * 256)
			+ Utils.readEEPROM(PHY_MASK_L);
		physicalSensorMaskLength = Utils.readEEPROM(PHY_MASK_LEN);
		activeSensorMask = ( (unsigned int) Utils.readEEPROM(ACT_MASK_H) * 256)
			+ Utils.readEEPROM(ACT_MASK_L);	
		setActiveSensorMaskLength();	
		defaultOperation = Utils.readEEPROM(OPERATING_MODE);
			#ifdef HIBERNATE_DEBUG_V3
				USB.print("defaultOp "); USB.println( (int) defaultOperation );
			#endif
		
		if(defaultOperation)
		{
			defaultTime2WakeInt = ( (unsigned int) Utils.readEEPROM(DEFAULT_T2W_H)*256)
				+ Utils.readEEPROM(DEFAULT_T2W_L);
			#ifdef HIBERNATE_DEBUG_V3
				USB.print("defaultT2w "); USB.println( (int) defaultTime2WakeInt );
			#endif			
		}

		powerPlan = (PowerPlan) Utils.readEEPROM(POWERPLAN);
		sleepMode = (SleepMode) Utils.readEEPROM(SLEEPMODE);
	}
}


void WaspXBeeZBNode::storeProgramParametersToEEPROM()
{
	storeValue(IN_NETWORK, inNetwork);
	//notInNetworkNrMinutesToSleep == Utils.readEEPROM(NOT_IN_NETWORK_NR_MINUTES_TO_SLEEP) ? : storeValue(NOT_IN_NETWORK_NR_MINUTES_TO_SLEEP, notInNetworkNrMinutesToSleep);
	storeValue(NOT_IN_NETWORK_NR_MINUTES_TO_SLEEP, notInNetworkNrMinutesToSleep);
	//defaultTime2WakeInt/256 == Utils.readEEPROM(DEFAULT_T2W_H) ? : storeValue(DEFAULT_T2W_H, defaultTime2WakeInt/256);
	storeValue(DEFAULT_T2W_H, MSByte(defaultTime2WakeInt));
	//defaultTime2WakeInt%256 == Utils.readEEPROM(DEFAULT_T2W_L) ? : storeValue(DEFAULT_T2W_L, defaultTime2WakeInt%256);
	storeValue(DEFAULT_T2W_L, LSByte(defaultTime2WakeInt));
	//activeSensorMask/256 == Utils.readEEPROM(ACT_MASK_H) ? : storeValue(ACT_MASK_H, activeSensorMask/256);
	storeValue(ACT_MASK_H, MSByte(activeSensorMask));
	//activeSensorMask%256 == Utils.readEEPROM(ACT_MASK_L) ? : storeValue(ACT_MASK_L, activeSensorMask%256);
	storeValue(ACT_MASK_L, LSByte(activeSensorMask));
	//physicalSensorMask/256 == Utils.readEEPROM(PHY_MASK_H) ? : storeValue(PHY_MASK_H, physicalSensorMask/256);
	storeValue(PHY_MASK_H, MSByte(physicalSensorMask));
	//physicalSensorMask%256 == Utils.readEEPROM(PHY_MASK_L) ? : storeValue(PHY_MASK_L, physicalSensorMask%256);
	storeValue(PHY_MASK_L, LSByte(physicalSensorMask));
	//defaultOperation == Utils.readEEPROM(OPERATING_MODE) ? : storeValue(OPERATING_MODE, defaultOperation);
	storeValue(OPERATING_MODE, defaultOperation);

	
	sleepMode == (SleepMode) Utils.readEEPROM(SLEEPMODE) ? : storeValueADT(SLEEPMODE, sleepMode);
	powerPlan == (PowerPlan) Utils.readEEPROM(POWERPLAN) ? : storeValueADT(POWERPLAN, powerPlan);		
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



//!//////////////////////////////////////////////////////////////////////////////////////////////////
//!  NODE LAYOUT / PRESENT SENSORS 
//!//////////////////////////////////////////////////////////////////////////////////////////////////	
uint8_t WaspXBeeZBNode::getDeviceRole()
{
	uint8_t error = 2;
	
	error = getDeviceType();
	deviceRole = (DeviceRole) deviceType[3];
	
	return error;
}  


uint8_t WaspXBeeZBNode::setDeviceRole(DeviceRole role)
{
	uint8_t error = 2;
	uint8_t type;
	
	type = (int) role;
	
	error = setDeviceType( &type );
	getDeviceRole();
	
	return error;
}


uint8_t WaspXBeeZBNode::setPowerPlan(PowerPlan pp)
{
	switch(pp)
	{
		case HIGHPERFORMANCE : powerPlan = pp;
							   return 0;
			 break;
		case POWERSAVER 	 : powerPlan = pp;
							   return 0;
			 break;
		default				 : return 1;
	}
}


void WaspXBeeZBNode::verifyPowerPlan()
{
	if(SensUtils.forceHighPerformance || mustSendSavedSensorValues) 
	{
		powerPlan = POWERSAVER;
		SensUtils.forceHighPerformance = false;
		mustSendSavedSensorValues = true;
		
        xbeeZB.init(ZIGBEE,FREQ2_4G,NORMAL);
        xbeeZB.ON();
        xbeeZB.wake(); 		
	}
}


uint8_t WaspXBeeZBNode::setEncryptionMode(EncryptionMode em)
{
	switch(em)
	{
		case ENCRYPTION_DISABLED : encryptionMode = em;
							       return 0;
			 break;
		case ENCRYPTION_ENABLED	 : encryptionMode = em;
							       return 0;
			 break;
		default				 	 : return 1;
	}
}
	
uint8_t WaspXBeeZBNode::setPhysicalSensorMask(uint8_t * mask)
{
	uint8_t error = 2;
	uint16_t tempMask = 0;

	tempMask = ToMask(mask);
	
	if(tempMask &= ~SensUtils.acceptedSensorMask)
		return 1;	
	
	physicalSensorMask = ToMask(mask);
	
	storeValue(PHY_MASK_H, mask[0]);	
	storeValue(PHY_MASK_L, mask[1]);
	
	setPhysicalSensorMaskLength();
	
	return 0;
}


void WaspXBeeZBNode::setPhysicalSensorMaskLength()
{
	physicalSensorMaskLength = getMaskLength(physicalSensorMask);

	storeValue(PHY_MASK_LEN, physicalSensorMaskLength);
	
		#ifdef NODE_DEBUG
			USB.print("physicalSensorMaskLength = ");
			USB.println( (int) physicalSensorMaskLength);
		#endif
}	
void WaspXBeeZBNode::setActiveSensorMask(int count, ...)
{
	va_list arguments;
	uint8_t * actMask = (uint8_t *) calloc(2, sizeof(uint8_t));
	
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
	
	storeValue(ACT_MASK_H, MSByte(activeSensorMask));	
	storeValue(ACT_MASK_L, LSByte(activeSensorMask));

	setActiveSensorMaskLength();
	
	actMask[0] = MSByte(activeSensorMask);
	actMask[1] = LSByte(activeSensorMask);
	setPhysicalSensorMask(actMask);
	
	free(actMask);
	actMask = NULL;
	
	inNetwork = true;
	
		#ifdef NODE_DEBUG
			USB.print("sum sensorMask = ");
			USB.println( (int) activeSensorMask );
		#endif
}


uint8_t WaspXBeeZBNode::setActiveSensorMaskWithTimes(uint16_t count, ...)
{
	uint8_t error = 0;
	uint8_t * actMask = (uint8_t *) calloc(2, sizeof(uint8_t));
	va_list arguments;
	va_start (arguments, count);

	activeSensorMask = 0;
	int sum = 0;
	SensorType type;
	
	defaultOperation = false;
	memset(SensUtils.measuringInterval, 0, sizeof(SensUtils.measuringInterval));
	
	if(count % 2)
		return error = 1;   ///Invalid data entered
	
	for(int x = 0; x < count; x++)
	{
		type =  (SensorType) va_arg( arguments, uint16_t );
		activeSensorMask += type;
		x++;
		
		error = SensUtils.registerSensorMeasuringIntervalTime(type, va_arg( arguments, uint16_t ) );	
		
			#ifdef NODE_DEBUG_V2
				USB.print("sensorMask in for = ");
				USB.println( (int) activeSensorMask );
			#endif
			
		if(error)
			return error; //break;
	}

	storeValue(ACT_MASK_H, MSByte(activeSensorMask));	
	storeValue(ACT_MASK_L, LSByte(activeSensorMask));

	setActiveSensorMaskLength();
	
	actMask[0] = MSByte(activeSensorMask);
	actMask[1] = LSByte(activeSensorMask);
	setPhysicalSensorMask(actMask);
	
	free(actMask);
	actMask = NULL;
	
	inNetwork = true;
	
		#ifdef NODE_DEBUG_V2
			USB.print("\nsum sensorMask = ");
			USB.println( (int) activeSensorMask );
		#endif
		
	setNewDifferentSleepTimes(true);
	
	return error;
}


uint8_t WaspXBeeZBNode::setActiveSensorMask(uint8_t * mask)
{
	uint8_t error = 2;
	uint16_t tempMask = 0;

	tempMask = ToMask(mask);
	
	if(tempMask &= ~physicalSensorMask)
		return error = 1;
	
	activeSensorMask = ToMask(mask);
	
	storeValue(ACT_MASK_H, mask[0]);	
	storeValue(ACT_MASK_L, mask[1]);
	
	setActiveSensorMaskLength();
	
	return 0;
}


void WaspXBeeZBNode::setActiveSensorMaskLength()
{
	uint8_t temp;
	activeSensorMaskLength = getMaskLength(activeSensorMask);
	nrActivatedSensors = getNrActiveSensors(activeSensorMask);
	
	//USB.print("\nmasklength storing "); USB.println( (int) activeSensorMaskLength );
	storeValue(ACT_MASK_LEN, activeSensorMaskLength);
	
	
	storeValue(NR_ACT_SENS, nrActivatedSensors);
	
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
	
	if(length > NUM_SENSORS)
	{
		COMM.sendError(NODE_HAD_AN_ERROR_IN_GET_MASK_LENGTH);
		return 1;
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


void WaspXBeeZBNode::findNextTime2Wake(SleepMode sm)  
{
	switch(sm)
	{
		case DEEPSLEEP:
		{
			if(!posInArray) 
			{
				//RTCUt.reinitialize();
				RTCUt.setNextTimeWhenToWakeUpViaOffset(time2wValuesArray[posInArray]);
			}
			
			else
				RTCUt.setNextTimeWhenToWakeUpViaOffset(time2wValuesArray[posInArray] - time2wValuesArray[posInArray-1]);				
		}
		break;
		
		
		case HIBERNATE:
		{
			uint8_t nrDays = 0;
			uint16_t lastValuePos = ( (uint16_t) Utils.readEEPROM(NR_OF_TIMES_STORED_IN_EEPROM_H) * 256
										+ Utils.readEEPROM(NR_OF_TIMES_STORED_IN_EEPROM_L) ) * 2
									+ START_T2W_VALUES - 1;
			bool lastStoredValue = true;
			
				#ifdef HIBERNATE_DEBUG_V3
					USB.println("findNextT2W value.");
					USB.print("lastValPos = ");  USB.println( (int) lastValuePos );
					USB.print("RTCsecMinHourInt = "); USB.println( (int) RTCUt.RTCSecMinHourInt );
				#endif		
				
			while(lastValuePos > START_T2W_VALUES)
			{
				storedTime = (unsigned int) (Utils.readEEPROM(lastValuePos) ) * 256 + Utils.readEEPROM(--lastValuePos);
					#ifdef HIBERNATE_DEBUG_V3
						USB.print("storedTime:"); USB.println( (int) storedTime );
					#endif
				if( (RTCUt.RTCSecMinHourInt % storedTime == 0) )//&& (nrDays % RTC.day == 0) )
				{
						#ifdef HIBERNATE_DEBUG_V3
							USB.print("storedTime%=0 "); USB.println( (int) storedTime );
						#endif				
					//if(lastValuePos == START_T2W_VALUES)
					//	RTCUt.setNextTimeWhenToWakeUpViaOffset( storedTime );	
					//else
					//{
						uint16_t nextTime = (unsigned int) (Utils.readEEPROM(lastValuePos+1)  * 256 + Utils.readEEPROM(lastValuePos+2) );
							#ifdef HIBERNATE_DEBUG_V3
								USB.print("nextTime = "); USB.println( (int) nextTime );
							#endif				
						RTCUt.setNextTimeWhenToWakeUpViaOffset( nextTime - storedTime );
					//}
					
					if(lastStoredValue)
					{
						if( Utils.readEEPROM(ARRAY_READ_MODE) == DIFFICULT ) 
							setNewDifferentSleepTimes(false);
						else
						{
							uint16_t firstTime = (unsigned int) ( Utils.readEEPROM(START_T2W_VALUES) + Utils.readEEPROM(START_T2W_VALUES + 1) * 256 );
								#ifdef HIBERNATE_DEBUG_V3
									USB.print("firstTime = "); USB.println( (int) firstTime );
								#endif	
							RTCUt.reinitialize();								
							RTCUt.setNextTimeWhenToWakeUpViaOffset( firstTime );

						}
					}
					
					break;
				}
				lastStoredValue = false;
				lastValuePos--;
			}	
	
		}
		
		default:
			break;
	}	
} 

void WaspXBeeZBNode::updatePosInArray()
{
	if(arrayReadMode == EASY)
		posInArray == lengthArray-1 ? posInArray = 0 : posInArray+=1;
	else
		if(posInArray == lengthArray-1)
		{
			posInArray = 0;
			setNewDifferentSleepTimes(false);
		}
		else
			posInArray+=1;
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
			storeValue(DEFAULT_T2W_H, MSByte(defaultTime2WakeInt));	
			storeValue(DEFAULT_T2W_L, LSByte(defaultTime2WakeInt));
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
	uint8_t pos = 2;
	uint16_t sensorsToDisableMask = 0;
	uint16_t sensorsToChangeMask = ToMask(array);
	//uint8_t maskLength = getMaskLength(sensorsToChangeMask);
	
			#ifdef CH_SENS_FREQ_REQ_DEBUG
				USB.println( (int) sensorsToChangeMask);
			#endif
			
	// i indicates which sensor
	for(uint8_t i=0; i<NUM_SENSORS; i++)
	{
		if( indicator & sensorsToChangeMask  && (! (indicator & physicalSensorMask) ) )
		{
			COMM.sendError(INVALID_MASK_RECEIVED_IN_CH_SENS_FREQ_REQ__SENSOR_IS_NOT_IN_THE_NODES_PHY_MASK);
			return 1;
		}
	}
	
	// i indicates which sensor
	// pos indicates where the sensor's time is in the array data, starting at 3
	USB.print("\nReceived intervals:\n");
	for(uint8_t i=0; i<NUM_SENSORS; i++)
	{
		if(  indicator & sensorsToChangeMask ) 
		{
			error = 0;
			//calculate received time
			SensUtils.measuringInterval[i] = ( (unsigned int) array[pos]*256) + array[pos+1];
			USB.println( (int) SensUtils.measuringInterval[i]);
			pos+=2;
			
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
	
	setActiveSensorMaskLength();
	
	if( error == 2 )
	{
		COMM.sendError(NODE_RECEIVED_EMPTY_SENSOR_MASK_IN_CH_SENS_FREQ_REQ);
		return error;
	} //error = 0 
	
	if( sensorsToDisableMask > 0 )
	{
		//disables sensors and checks if we can go back to defaultOperation mode
		error = disableSensors(&sensorsToDisableMask);
		
		if(error == 1)
		{
			COMM.sendError(NODE_HAS_NO_LONGER_ACTIVE_SENSORS);
			return error;
		}
	} //error = 0
	
	if(! defaultOperation )
	{
		//finds and saves the next x hibernate times.
		//in case all sensors are set to the same interval defaultOperation mode is enabled again
		error = setNewDifferentSleepTimes(true);
		if(error)
		{
			COMM.sendError(NODE_HAD_AN_ERROR_IN_SET_NEW_DIFFERENT_SLEEP_TIMES);
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
  


uint8_t WaspXBeeZBNode::setNewDifferentSleepTimes(bool firstCall)
{
	uint8_t error = 2, j = 0;
	uint16_t * sortedTimes = (uint16_t *) calloc(activeSensorMaskLength, sizeof(uint16_t));
	uint16_t indicator = 1;
	
	if(firstCall || sleepMode == HIBERNATE)
	{
		time2wValuesArray = (uint16_t *) calloc(MAX_DATA_PER_SENSOR*nrActivatedSensors, sizeof(uint16_t));
		sensMultiplier = (uint16_t *) calloc(nrActivatedSensors, sizeof(uint16_t));
		lastArrayContainedLCM = true;
	} /// else the memory space is allocated and may be overwritten

	//////////////////////////////////////////
	// SENSOR MEASURING INTERVALS
	//////////////////////////////////////////
	
		if(!firstCall && sleepMode == HIBERNATE)
		{
			/// READ SENSOR MEASURINT INTERVALS FROM EEPROM
			SensUtils.readSensorMeasuringIntervalTimesFromEEPROM();	

			/// CHECK IF LAST ARRAY CONTAINED THE LCM
			lastArrayContainedLCM = Utils.readEEPROM(LAST_ARRAY_CONTAINED_LCM);
		}
	
	//create a temp array and fill it with the measuring intervals
		#ifdef NODE_DEBUG_V4
			USB.println("measuringIntervals");
		#endif
		
	for(uint8_t i=0; i<NUM_SENSORS; i++)
	{
		if(SensUtils.measuringInterval[i] > 0)
		{
			if(indicator & activeSensorMask)
			{
				sortedTimes[j] = SensUtils.measuringInterval[i];
					#ifdef NODE_DEBUG_V4
						USB.println( (int) sortedTimes[j]);
					#endif
				j++;
			}
		}
		indicator <<= 1;
	}
	
	//sort the temporary array:
	qsort(sortedTimes, j, sizeof(uint16_t), compare);
	
		#ifdef NODE_DEBUG_V4
			USB.print("sorted ");
			for(uint8_t i=0; i<j; i++)
				USB.println( (int) sortedTimes[i] );
		#endif
	
	SensUtils.minTime = sortedTimes[0];
	SensUtils.maxTime = sortedTimes[--j];
	
	//after playing with the intervals it can become possible they all have the same time again:
	if( SensUtils.minTime == SensUtils.maxTime )
	{
		setNewDefaultTime2Sleep(SensUtils.minTime);
		defaultOperation = true;
	}
	else
	{
		/////////////////////////////////
		// 1. SENS MULTIPLIERS = 1
		/////////////////////////////////
		if(firstCall || lastArrayContainedLCM)
		{
			for(uint16_t i=0; i<nrActivatedSensors; i++)  //memset(sensMultiplier, 1, nrActivatedSensors);  doesn't work
				sensMultiplier[i] = 1;
		}
	
		/////////////////////////////////////////////////////////////////////////////////////////////////
		// 2. CALCULATE LCM:
		//    Estimate size of wake times array; factor will be used after x sleep cycles to calculate
		//    the next x sleep cycles in case they don't fit within MAX_DATA
		/////////////////////////////////////////////////////////////////////////////////////////////////
		LCM = calculateMaxNrElementsForEEPROMArray(sortedTimes);
			#ifdef MATH_DEBUG_3
				USB.print("LCM = "); USB.println( (int) LCM);
			#endif
		
		if(firstCall)
		{
			if( LCM/sortedTimes[0] < MAX_DATA_PER_SENSOR )
			{
				arrayReadMode = EASY;
				//factor = 1;
			}
			else
			{
				arrayReadMode = DIFFICULT;
				//factor = calculateMaxNrElementsForEEPROMArray(sortedTimes) / MAX_DATA_PER_SENSOR;
			}
			
				#ifdef MATH_DEBUG
					USB.print("\nsortedTimes: ");
					//qsort(sortedTimes, nrActivatedSensors, sizeof(uint16_t), compare);
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
		}
		/// else if(sleepMode == HIBERNATE)
		///   readEEPROM(SLEEPMODE)  =>  already happenend
			
		///////////////////////////////////
		// 3. STORE TO EEPROM FOR HIBERNATE
		///////////////////////////////////
		if(firstCall && sleepMode == HIBERNATE)
		{
			//storeValue(MIN_SENS_TIME_L, SensUtils.minTime%256);	storeValue(MIN_SENS_TIME_H, SensUtils.minTime/256);
			//storeValue(MAX_SENS_TIME_L, SensUtils.maxTime%256);	storeValue(MAX_SENS_TIME_H, SensUtils.maxTime/256);

			SensUtils.saveSensorMeasuringIntervalTimes();
		}

		/////////////////////////////////
		// 4. CALCULATE VALUES
		/////////////////////////////////
		createAndSaveNewTime2SleepArray(sortedTimes);	

	}
	free(sortedTimes);
	sortedTimes = NULL;
	
	//////////////////////////////////////////
	// SET THE FIRST SLEEP TIME OFFSET
	//////////////////////////////////////////
	RTCUt.reinitialize();
	RTCUt.getTime();
	RTCUt.setNextTimeWhenToWakeUpViaOffset(time2wValuesArray[0]);
}

	
void WaspXBeeZBNode::createAndSaveNewTime2SleepArray(uint16_t * sortedTimes)
{
	uint8_t sensorNr = 0, factor = 0;
	lengthArray = 0;
	uint16_t maxInArray = 0,  pos_eeprom = 0, eeprom_offset = 0;
	uint16_t newPos = 0; 

		#ifdef TEST_DIFFICULT_DEBUG
			LCM = 15;
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
				//USB.print("pos "); USB.println( (int) pos);
				//USB.print("sensNr "); USB.println( (int) sensorNr);
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
		for(uint8_t i=1; i<=pos; i++)
		{
			if(time2wValuesArray[newPos] != time2wValuesArray[i]) 
			{
				time2wValuesArray[++newPos] = time2wValuesArray[i];	
				lengthArray++;
			}
		}
		
		// 2. Indicate end of array
		time2wValuesArray[lengthArray] = 0;
		
			#ifdef FINAL_USB_DEBUG
				USB.print("\nVariable sleep times: ");
				for(uint16_t i=0; i<lengthArray; i++)
				{
					USB.print(time2wValuesArray[i]);
					USB.print(" ");
				}
				USB.print("\n");
			#endif
		
		// 3. Check if last element == LCM
		if(arrayReadMode == DIFFICULT)
		{
			if(time2wValuesArray[lengthArray - 1] == LCM)
			{
				//Reset sensor multipliers
				for(uint16_t i=0; i<nrActivatedSensors; i++)  //memset(sensMultiplier, 1, nrActivatedSensors);  doesn't work
					sensMultiplier[i] = 1;
			}
		}
		
			#ifdef DEEPSLEEP_DEBUG
				USB.println("sensMultipliers: 0-nrActiveSensors");
				for(uint8_t i=0; i<nrActivatedSensors; i++)
					USB.println( (int) sensMultiplier[i]);
			#endif
	
	/////////////////////////////////
	// 4. STORE VALUES
	/////////////////////////////////	
	
		if( sleepMode == HIBERNATE )
		{
			pos_eeprom = START_T2W_VALUES;
			storeValue(NR_ACTIVATED_SENSORS, nrActivatedSensors);
			storeValue(NR_OF_TIMES_STORED_IN_EEPROM_H, lengthArray/256);
			storeValue(NR_OF_TIMES_STORED_IN_EEPROM_L, lengthArray%256);
			
			if( mode == EASY)
			{
				storeValue(ARRAY_READ_MODE, EASY);
			}
			
			/// SAVE TIMES TO WAKE
						#ifdef HIBERNATE_DEBUG_V3
							USB.println("\nSaving t2w values to..");
						#endif
			for(uint16_t i=0; i<lengthArray; i++)
			{
						#ifdef HIBERNATE_DEBUG_V3
							USB.println( (int) pos_eeprom );
							USB.println( (int) pos_eeprom+1 );
						#endif
					storeValue(pos_eeprom++, time2wValuesArray[i]%256);
					storeValue(pos_eeprom++, time2wValuesArray[i]/256);
			}
			
			/// IF DIFFICUTL, also save sensor multipliers
			if( mode == DIFFICULT )
			{
				storeValue(ARRAY_READ_MODE, DIFFICULT);
				pos_eeprom = FIRST_SENS_MULTIPLIER;
				
				for(sensorNr = 0; sensorNr<nrActivatedSensors; sensorNr++)
				{
					storeValue(pos_eeprom++, sensMultiplier[sensorNr]%256);
					storeValue(pos_eeprom++, sensMultiplier[sensorNr]/256);				
				}
			}
		}
		else /// sleepmMode == SLEEP || sleepMode == DEEPSLEEP )
		{
			//! use pointers from xbeeZB to search the next sleep time
			posInArray = 1;  /// Only for to start correctly, ignore
		}
}

/*
void WaspXBeeZBNode::calculateNextTimes()
{
	//posInArray = 0;
	
	if( sleepMode == HIBERNATE )
	{
		/// Read SensMultipliers from EEPROM
		//if( false,   //lastArray (LCM) ) RESET sensmultipliers
	}
	//if( == LCM) next Time must reset RTC
	
}
*/

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


void WaspXBeeZBNode::testMemory()
{
	char str[6];
	str[0] = 'a';
	str[1] = 'b';
	str[2] = 'c';
	str[3] = 'd';
	str[4] = 'e';
	str[5] = '\0';
	
	/*
	USB.println("start");
	USB.println(str);
	USB.println(mess);
	USB.println("abcd");
	USB.println(freeMemory());
	USB.println("kdfjmqioimeqfjqkfjekjfejfkedfjkfj");
	USB.println(freeMemory());
	USB.println("kdfjmqioimeqfjqkfjekjfejfkedfjkfa");
	USB.println(freeMemory());
	USB.println("kdfjmqioimeqfjqkfjekjfejfkedfjkfb");
	USB.println("kdfjmqioimeqfjqkfjekjfejfkedfjkfc");
	USB.println(freeMemory());
	USB.println("kdfjmqioimeqfjqkfjekjfejfkedfjkfd");
	USB.println("kdfjmqioimeqfjqkfjekjfejfkedfjkfe");
	USB.println("kdfjmqioimeqfjqkfjekjfejfkedfjkff");
	USB.println("kdfjmqioimeqfjqkfjekjfejfkedfjkfg");
	USB.println("end");
	USB.println(freeMemory());
	*/
}


void WaspXBeeZBNode::testMemory2()
{
	//char str[5] = {'t','e','s','t', '\0'};
	//char str[] = "test";
	USB.println("kkkk");
	//USB.println(str);
	USB.print("in: ");  USB.println(freeMemory());
	USB.println("boe");
	USB.println("bla");
	USB.println("blo");
	USB.println("kzd");
	USB.println("ezferif");
}


void WaspXBeeZBNode::testMemory3()
{
	//char * bla = (char *) calloc(20, sizeof(char));
	//bla = str(blablaxrtkks);
	//USB.println(bla);
	//USB.println(xstr
	//free(bla);
	//bla = NULL;
	

	//char buffer[8];

	//strcpy_P(buffer, (PGM_P)pgm_read_word(&(blabla)));

	//USB.println(buffer);
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
uint16_t toUint16_t(char & high, char & low)
{
	return  ( (uint16_t) ((  high ) * 256 ) + ( low) );
}
*/
	
/*
uint16_t toUint16_t(char * ar)
{
	return  ( (uint16_t) ((  ar[0] ) * 256 ) + (ar[1]) );
}
*/



WaspXBeeZBNode xbeeZB = WaspXBeeZBNode();
