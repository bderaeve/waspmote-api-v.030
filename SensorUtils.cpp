
#ifndef __WPROGRAM_H__
	#include "BjornClasses.h"
	#include "WaspClasses.h"
#endif

/*********************************************************************************************************
  *
  * CONSTRUCTOR / DEFAULT VALUES
  *
  *******************************************************************************************************/
StoreSensorData * Saver[5] = { &saveTemperature, &saveHumidity, &savePressure,
			&saveBattery, &saveCO2 };
#ifdef POWER_MODES			
ReadSensorSamplesFromEEPROM * EEPROM_Reader[5] = { &readTemperatureFromEEPROM, &readHumidityFromEEPROM,
			&readPressureFromEEPROM, &readBatteryFromEEPROM, &readCO2FromEEPROM };
#endif
			
SensorUtils::SensorUtils()
{
	
	reader[0] = &SensorUtils::measureTemperature;
	reader[1] = &SensorUtils::measureHumidity;
	reader[2] = &SensorUtils::measurePressure;
	reader[3] = &SensorUtils::measureBattery;
	reader[4] = &SensorUtils::measureCO2;
	#ifdef WEATHER_STATION
		reader[5] = &SensorUtils::measureAnemo;
		reader[6] = &SensorUtils::measureVane;
		reader[7] = &SensorUtils::getSummativeRainfall;
		reader[8] = &SensorUtils::measureLuminosity;
		reader[9] = &SensorUtils::measureSolarRadiation;
	#endif
	for(uint8_t i=0; i<NUM_SENSORS; i++)
		measuringInterval[i] = xbeeZB.defaultTime2WakeInt;
		
	for(uint8_t i=0; i<NUM_SENSORS; i++)
		savedPositions[i] = 0;	
		
	pluviometerCounter = 0;
	startedRaining = true;
	forceHighPerformance = false;
	
	acceptedSensorMask = 0;
	#ifdef WEATHER_STATION /// AGRICULTURE SENSOR BOARD V2
		acceptedSensorMask |= TEMPERATURE;
		acceptedSensorMask |= HUMIDITY;
		acceptedSensorMask |= PRESSURE;
		acceptedSensorMask |= BATTERY;
		acceptedSensorMask |= ANEMO;
		acceptedSensorMask |= VANE;
		acceptedSensorMask |= PLUVIO;
		acceptedSensorMask |= LUMINOSITY;
		acceptedSensorMask |= SOLAR_RADIATION;
	#else /// GASSES SENSOR BOARD V2
		acceptedSensorMask |= TEMPERATURE;
		acceptedSensorMask |= HUMIDITY;
		//acceptedSensorMask |= PRESSURE;  // atm we only have one sensor, used on weather station
		acceptedSensorMask |= BATTERY;
		acceptedSensorMask |= CO2;
	#endif
}

	
/*********************************************************************************************************
  *
  * MEASURING
  *
  *******************************************************************************************************/
void SensorUtils::measureTemperature()
{
		#ifdef SENS_DEBUG_V2
			USB.print(" T=");
		#endif
		
	temperature = 0;
	
	#ifndef WEATHER_STATION
		SensorGasv20.setSensorMode(SENS_ON, SENS_TEMPERATURE);
		delay(100);
		
		for (int i=0;i<NUM_MEASUREMENTS;i++)
		{
			// TEMPERATURE SENSOR:   RANGE: -40° -> +125° 
			temperature += SensorGasv20.readValue(SENS_TEMPERATURE);
		}
		temperature /= NUM_MEASUREMENTS;
		//USB.println(temperature);

			#ifdef SENS_DEBUG
				previous = millis();
				for (int i=0;i<NUM_MEASUREMENTS;i++)
				{
					temperature = SensorGasv20.readValue(SENS_TEMPERATURE);
					USB.println(temperature);
					delay(100);
				}	
				previous = millis() - previous;
				USB.print("timeWithDelay "); USB.println(previous);

				USB.println("no delay between measurements:");
				previous = millis();
				for (int i=0;i<NUM_MEASUREMENTS;i++)
				{
					temperature = SensorGasv20.readValue(SENS_TEMPERATURE);
					USB.println(temperature);
				}		
				previous = millis() - previous;
				USB.print("timeNoDelay "); USB.println(previous);			
			#endif
			
			#ifdef SENS_DEBUG_V2
				USB.print( temperature );
			#endif
			
		SensorGasv20.setSensorMode(SENS_OFF, SENS_TEMPERATURE);
		
	#else   /// WEATHER STATION
	
		SensorAgrV20.setSensorMode(SENS_ON, SENS_AGR_TEMPERATURE);
		delay(100);
		
		for (int i=0;i<NUM_MEASUREMENTS;i++)
		{
			/// TEMPERATURE SENSOR:   RANGE: -40° -> +125°
			temperature += SensorAgrV20.readValue(SENS_AGR_TEMPERATURE);
			delay(10);
		}
		SensorAgrV20.setSensorMode(SENS_OFF, SENS_AGR_TEMPERATURE);
		temperature /= NUM_MEASUREMENTS;
		temperature -= 1;
			#ifdef SENS_DEBUG_V2
				USB.print( temperature );
			#endif		
	#endif
}


void SensorUtils::measureHumidity()
{
		#ifdef SENS_DEBUG_V2
			USB.print(" H=");
		#endif
	
	humidity = 0;
	
	#ifndef WEATHER_STATION
	
		SensorGasv20.setSensorMode(SENS_ON, SENS_HUMIDITY);
		delay(100);
		
		for (int i=0;i<NUM_MEASUREMENTS;i++)
		{
			/* HUMIDITY SENSOR:   RANGE: 0 -> 100% */
			humidity += SensorGasv20.readValue(SENS_HUMIDITY);
		}
		humidity /= NUM_MEASUREMENTS;
		
			#ifdef SENS_DEBUG_V2
				USB.print( humidity );
			#endif		
		
			#ifdef SENS_DEBUG
				previous = millis();
				for (int i=0;i<NUM_MEASUREMENTS;i++)
				{
					humidity = SensorGasv20.readValue(SENS_HUMIDITY);
					USB.println(humidity);
					delay(100);
				}	
				previous = millis() - previous;
				USB.print("timeWithDelay "); USB.println(previous);
				
				USB.println("no delay between measurements:");
				previous = millis();
				for (int i=0;i<NUM_MEASUREMENTS;i++)
				{
					humidity = SensorGasv20.readValue(SENS_HUMIDITY);
					USB.println(humidity);
				}		
				previous = millis() - previous;
				USB.print("timeNoDelay "); USB.println(previous);			
			#endif
			
		SensorGasv20.setSensorMode(SENS_OFF, SENS_HUMIDITY);
	
	#else     /// WEATHER STATION
	
		SensorAgrV20.setSensorMode(SENS_ON, SENS_AGR_HUMIDITY);
		delay(100);

		for (int i=0;i<NUM_MEASUREMENTS;i++)
		{
			/* HUMIDITY SENSOR:   RANGE: 0 -> 100% */
			humidity += SensorAgrV20.readValue(SENS_AGR_HUMIDITY);
		}
		SensorAgrV20.setSensorMode(SENS_OFF, SENS_AGR_HUMIDITY);
		humidity /= NUM_MEASUREMENTS;	
			#ifdef SENS_DEBUG_V2
				USB.print( humidity );
			#endif			
		
	#endif
}


void SensorUtils::measurePressure()
{
		#ifdef SENS_DEBUG_V2
			USB.print(" P=");
		#endif
	
	pressure = 0;
	
	#ifndef WEATHER_STATION
	
		SensorGasv20.setSensorMode(SENS_ON, SENS_PRESSURE);
		delay(100);
		
		for (int i=0;i<NUM_MEASUREMENTS;i++)
		{
			/* ATMOSPHERIC PRESSURE SENSOR:   RANGE: 15 -> 115 kPa */
			pressure += SensorGasv20.readValue(SENS_PRESSURE);
		}
		pressure /= NUM_MEASUREMENTS;
			#ifdef SENS_DEBUG_V2
				USB.print( pressure );
			#endif	
					#ifdef SENS_DEBUG
						previous = millis();
						for (int i=0;i<NUM_MEASUREMENTS;i++)
						{
							pressure = SensorGasv20.readValue(SENS_PRESSURE);
							USB.println(pressure);
							delay(100);
						}	
						previous = millis() - previous;
						USB.print("timeWithDelay "); USB.println(previous);
					
						USB.println("no delay between measurements:");
						previous = millis();
						for (int i=0;i<NUM_MEASUREMENTS;i++)
						{
							pressure = SensorGasv20.readValue(SENS_PRESSURE);
							USB.println(pressure);
						}	
						previous = millis() - previous;
						USB.print("timeNoDelay "); USB.println(previous);				
					#endif	
		
		SensorGasv20.setSensorMode(SENS_OFF, SENS_PRESSURE);
	
	#else   /// WEATHER STATION
		
		SensorAgrV20.setSensorMode(SENS_ON, SENS_AGR_PRESSURE);
		delay(100);
	
		for (int i=0;i<NUM_MEASUREMENTS;i++)
		{
			/* ATMOSPHERIC PRESSURE SENSOR:   RANGE: 15 -> 115 kPa */
			pressure += SensorAgrV20.readValue(SENS_AGR_PRESSURE);
		}
		SensorAgrV20.setSensorMode(SENS_OFF, SENS_AGR_PRESSURE);
		pressure /= NUM_MEASUREMENTS;	
			#ifdef SENS_DEBUG_V2
				USB.print( pressure );
			#endif	
	
	#endif
}


void SensorUtils::measureBattery()
{
		#ifdef SENS_DEBUG_V2
			USB.print(" B=");
		#endif
	
	battery = 0;
	
	for (int i=0;i<NUM_MEASUREMENTS;i++)
	{
		battery += PWR.getBatteryLevel();
	}
	
	battery /= NUM_MEASUREMENTS;
		#ifdef SENS_DEBUG_V2
				USB.print( battery );
		#endif	
}


void SensorUtils::measureCO2()
{
	uint8_t er = 0;
	
	#ifndef WEATHER_STATION /// Cannot be measured on Agriculture board
	uint16_t time = 0;
		#ifdef SENS_DEBUG_V2
			USB.print(" CO2=(30 sec) ");
		#endif
		
	co2 = 0;
	long previous = 0;
		
	SensorGasv20.configureSensor(SENS_CO2, 1);
    SensorGasv20.setSensorMode(SENS_ON, SENS_CO2);
	  
	previous = millis();
	time = millis() - previous;
	
	//xbeeZB.setAlarmForRouter();
		
	while (time < 30000)
	{
		time = millis() - previous;
		/*
		if( intFlag & RTC_INT )
		{
			intFlag &= ~(RTC_INT); // Clear flag
				#ifdef FINAL_USB_DEBUG
					USB.println("\nCO2 INTERRUPTED");
				#endif
			er = measureSensors(xbeeZB.activeSensorMask);
				#ifdef FINAL_USB_DEBUG
					USB.print("\n ... RETURNED: "); USB.print(er);
				#endif
			er = COMM.checkNodeAssociation(LOOP);
				#ifdef FINAL_USB_DEBUG
					USB.print("\n ... RETURNED: "); USB.print(er);
				#endif
			if(er != 0) 
				COMM.retryJoining();
			else
				PackUtils.sendMeasuredSensors(xbeeZB.GATEWAY_MAC, xbeeZB.activeSensorMask);
				
			xbeeZB.setAlarmForRouter();
		}	
		*/		
		
	
		// dummy readings in order to warm the sensor
		SensorGasv20.readValue(SENS_CO2);    
	}
	
	for (int i=0;i<NUM_MEASUREMENTS;i++)
	{          
		/* CO2 SENSOR:   RANGE: 350 -> 10 000 ppm 
           normal outdoor level: 350 - 450 ppm; acceptable levels: < 600 ppm */
		co2 += SensorGasv20.readValue(SENS_CO2);
	}
	
	SensorGasv20.setSensorMode(SENS_OFF, SENS_CO2);
	co2 /= NUM_MEASUREMENTS;
	co2 *= 1000;
	
		#ifdef SENS_DEBUG_V2
				USB.print( co2 );
		#endif	
	#endif
}


#ifdef WEATHER_STATION
	void SensorUtils::measureAnemo()
	{
			#ifdef SENS_DEBUG_V2
				USB.print(" ANEMO=");
			#endif
		
		anemo = 0;
		
		// Turn on the sensor and wait for stabilization and response time
		SensorAgrV20.setSensorMode(SENS_ON, SENS_AGR_ANEMOMETER);
		delay(100);
		
		/* ANEMOMETER   RANGE: 0 -> 240 km/h */
		anemo = SensorAgrV20.readValue(SENS_AGR_ANEMOMETER);
		
		SensorAgrV20.setSensorMode(SENS_OFF, SENS_AGR_ANEMOMETER);
		
			#ifdef SENS_DEBUG_V2
				USB.print( anemo );
			#endif		
	}


	void SensorUtils::measureVane()
	{
			#ifdef SENS_DEBUG_V2
				USB.print(" VANE=");
			#endif
		
		vane = 0;
		
		// Turn on the sensor and wait for stabilization and response time
		SensorAgrV20.setSensorMode(SENS_ON, SENS_AGR_VANE);
		delay(100);

		for (int i=0;i<NUM_MEASUREMENTS;i++)
		{
			/* VANE   RANGE: 688 -> 120 OHM */
			vane += SensorAgrV20.readValue(SENS_AGR_VANE);
		}
		
		SensorAgrV20.setSensorMode(SENS_OFF, SENS_AGR_VANE);
		vane /= NUM_MEASUREMENTS;	
		
		convertVaneDirection();
	}


	void SensorUtils::measureCurrentRainfall()
	{
			#ifdef SENS_DEBUG_V2
				USB.println("measureCurrentPluvio");
			#endif
		
		for (int i=0;i<NUM_MEASUREMENTS;i++)
		{
			/* ANEMOMETER   RANGE: 0 -> 240 km/h */
			pluvio += SensorAgrV20.readValue(SENS_AGR_PLUVIOMETER);
		}
		
		pluvio /= NUM_MEASUREMENTS;		
	}
	
	
	void SensorUtils::rainfall_ISR()
	{
		pluviometerCounter == 65535 ? pluviometerCounter == 65535 : pluviometerCounter++;
		RTCUt.getTime();
		/*
		if(startedRaining)
		{
			startedRaining = false;
			startedRainingTime = RTCUt.RTCSecMinHourInt;
			COMM.sendWarning( (Errors) STARTED_RAINING);
		}
		else
		{
			if(RTCUt.RTCSecMinHourInt - startedRainingTime >= 720)
			{
				startedRaining = false;
				pluviometerCounter = 0;
				//COMM.sendWarning(RAIN_METER_HAS_BEEN_RESET);
			}
		}
		*/
	}
	
	
	void SensorUtils::getSummativeRainfall()
	{
			#ifdef SENS_DEBUG_V2
				USB.print(" PLUVIO=(sum_counter)");
			#endif	
		//summativeRainfallInMM = float (pluviometerCounter) * 0.2794; 
			#ifdef SENS_DEBUG_V2
				USB.print( (int) pluviometerCounter);
			#endif	
	}
	
	
	void SensorUtils::resetPluviometer()
	{
		pluviometerCounter = 0;
		COMM.sendWarning(RAIN_METER_HAS_BEEN_RESET);
	}
	
	
	void SensorUtils::measureLuminosity()
	{
			#ifdef SENS_DEBUG_V2
				USB.print(" LUM=");
			#endif
		luminosity = 0;
		// Turn on the sensor and wait for stabilization and response time
		SensorAgrV20.setSensorMode(SENS_ON, SENS_AGR_LDR);
		delay(100);
		
		for (int i=0;i<NUM_MEASUREMENTS;i++)
		{
			/*LUMINOSITY: RANGE: 0 -> 3.3 OHM */
			luminosity += SensorAgrV20.readValue(SENS_AGR_LDR);
		}
		SensorAgrV20.setSensorMode(SENS_OFF, SENS_AGR_LDR);
		luminosity /= NUM_MEASUREMENTS;		
		
			#ifdef SENS_DEBUG_V2
				USB.print(luminosity);
			#endif	
	}	
	
	void SensorUtils::measureSolarRadiation()
	{
			#ifdef SENS_DEBUG_V2
				USB.println(" RAD=");
			#endif	
		solar_radiation = 0;
		
		SensorAgrV20.setSensorMode(SENS_ON, SENS_AGR_RADIATION);
		delay(100);
		
		for (int i=0;i<NUM_MEASUREMENTS;i++)
		{
			/* */
			solar_radiation += SensorAgrV20.readValue(SENS_AGR_RADIATION);
		}
		SensorAgrV20.setSensorMode(SENS_OFF, SENS_AGR_RADIATION);
		solar_radiation /= NUM_MEASUREMENTS;
		
		// Conversion from voltage into umol·m-2·s-1
        solar_radiation /= 0.00015;		
			#ifdef SENS_DEBUG_V2
				USB.print(solar_radiation);
			#endif	
	}
	
	
	void SensorUtils::convertVaneDirection()
	{
		  switch(SensorAgrV20.vane_direction)
		  {
			case  SENS_AGR_VANE_N   :  USB.print("N");
									   vaneDirection = VANE_N;
									   break;
			case  SENS_AGR_VANE_NNE :  USB.print("NNE");
									   vaneDirection = VANE_NNE;
									   break;
			case  SENS_AGR_VANE_NE  :  USB.print("NE");
									   vaneDirection = VANE_NE;			
									   break;
			case  SENS_AGR_VANE_ENE :  USB.print("ENE");
									   vaneDirection = VANE_ENE;
									   break;
			case  SENS_AGR_VANE_E   :  USB.print("E");
									   vaneDirection = VANE_E;
									   break;
			case  SENS_AGR_VANE_ESE :  USB.print("ESE");
									   vaneDirection = VANE_ESE;
									   break;
			case  SENS_AGR_VANE_SE  :  USB.print("SE");
									   vaneDirection = VANE_SE;
									   break;
			case  SENS_AGR_VANE_SSE :  USB.print("SSE");
									   vaneDirection = VANE_SSE;
									   break;
			case  SENS_AGR_VANE_S   :  USB.print("S");
									   vaneDirection = VANE_S;
									   break;
			case  SENS_AGR_VANE_SSW :  USB.print("SSW");
									   vaneDirection = VANE_SSW;
									   break;
			case  SENS_AGR_VANE_SW  :  USB.print("SW");
									   vaneDirection = VANE_SW;
									   break;
			case  SENS_AGR_VANE_WSW :  USB.print("WSW");
									   vaneDirection = VANE_WSW;
									   break;
			case  SENS_AGR_VANE_W   :  USB.print("W");
									   vaneDirection = VANE_W;
									   break;
			case  SENS_AGR_VANE_WNW :  USB.print("WNW");
									   vaneDirection = VANE_WNW;
									   break;
			case  SENS_AGR_VANE_NW  :  USB.print("WN");
									   vaneDirection = VANE_NW;
									   break;
			case  SENS_AGR_VANE_NNW :  USB.print("NNW");
									   vaneDirection = VANE_NNW;
									   break;
		  }      	
	}
#endif

uint8_t SensorUtils::measureSensors(int count, ...)
{
	uint16_t mask = 0;

	va_list arguments;
	va_start (arguments, count);
	
	for(uint8_t x=0; x<count; x++)
	{
		mask += va_arg( arguments, int);
	}
	
	measureSensors(mask);
}


uint8_t SensorUtils::measureSensors(uint16_t mask)
{
	uint8_t error = 2;
	uint16_t indicator = 1;
	
		#ifdef FINAL_USB_DEBUG
			USB.print("\nMEASURING_SENSORS: ");
		#endif
		
		#ifdef SENS_DEBUG_V2
			USB.print("for mask "); USB.print( (int) mask );
			USB.print(" mask length "); USB.println( (int) xbeeZB.activeSensorMaskLength);
		#endif
	
	#ifndef WEATHER_STATION  /// INDOOR NODES - GASSES SENSOR BOARD /////////////////////////////////

	if( mask == 0 )
	{
		COMM.sendError(NODE_HAD_AN_ERROR_IN_MEASURE_SENSORS_RECEIVED_AN_EMPTY_MASK);
		return 1;
	}
	else if ( mask == BATTERY )
	{
		(this->*reader[3])();  	/// measure only battery,  don't turn on sensor board
		error = 0;				/// no time check since this is the only reason we came awake
	}
	else
	{
		error = 0;
		SensorGasv20.setBoardMode(SENS_ON);
			
		for(uint8_t i = 0; i < xbeeZB.activeSensorMaskLength; i++)
		{
			if(xbeeZB.defaultOperation)
			{
				if(indicator & mask)
				{
					(this->*reader[i])();  // read the corresponding sensor
				}
				indicator <<= 1;
			}
			else
			{
				if(xbeeZB.sleepMode == SLEEP || xbeeZB.sleepMode == DEEPSLEEP || 
				   xbeeZB.sleepMode == NONE)
				{			
					/// Use posInArray
					//USB.print("time: "); USB.println( (int) RTCUt.RTCSecMinHourInt );
					//USB.print("sensInt[i] "); USB.println( (int) measuringInterval[i] );
					if(indicator & mask && RTCUt.RTCSecMinHourInt % measuringInterval[i] == 0)
					{
						(this->*reader[i])();  // read the corresponding sensor
					}
				}
				else /// IF HIBERNATE:
				{
					if(indicator & mask && RTCUt.RTCSecMinHourInt % measuringInterval[i] == 0 )
					{
						(this->*reader[i])();  // read the corresponding sensor
					}
				}
				indicator <<= 1;
			}
		}
		SensorGasv20.OFF();
	}
	
	#else   ///  WEATHER_STATION - AGRICULTURE SENSOR BOARD /////////////////////////////////////////
	
	if( mask == 0 )
	{
		COMM.sendError(NODE_HAD_AN_ERROR_IN_MEASURE_SENSORS_RECEIVED_AN_EMPTY_MASK);
		return 1;
	}
	else if ( mask == BATTERY )
	{
		(this->*reader[3])();  	/// measure only battery,  don't turn on sensor board
		error = 0;				/// no time check since this is the only reason we came awake
	}
	else
	{
		error = 0;
		SensorAgrV20.setBoardMode(SENS_ON);
		RTC.ON();
		
		for(uint8_t i = 0; i < xbeeZB.activeSensorMaskLength; i++)
		{
			if(xbeeZB.defaultOperation)
			{
				if(indicator & mask)
				{
					(this->*reader[i])();  // read the corresponding sensor
				}
				indicator <<= 1;
			}
			else
			{
				if(xbeeZB.sleepMode == SLEEP || xbeeZB.sleepMode == DEEPSLEEP)
				{			
					/// Use posInArray
					if(indicator & mask && RTCUt.RTCSecMinHourInt % measuringInterval[i] == 0 )
					{
						(this->*reader[i])();  // read the corresponding sensor
					}
				}
				else /// IF HIBERNATE:
				{
					if(indicator & mask && RTCUt.RTCSecMinHourInt % measuringInterval[i] == 0 )
					{
						(this->*reader[i])();  // read the corresponding sensor
					}
				
				}
				indicator <<= 1;
			}
		}
		//SensorAgrV20.setBoardMode(SENS_OFF); 		// => disables rain interrupt
	}	
	#endif
	
	return error;

}




uint8_t SensorUtils::sensorValue2Chars(float value, SensorType type)
{
	uint8_t error = 2;
	//unsigned int i = 0;
	uint16_t i = 0;
	
	
	switch(type)
	{
		case TEMPERATURE:	
		{
			// TEMPERATURE SENSOR:   RANGE: -40° -> +125° 
			
			//USB.print("temp value = "); USB.println( value );
			value += 40;		// negative values start at 0
			value *= 100;		// accuracy: 2 decimals  (1 dec is ok but have 4 bits left anyway)
			/*
			i = (unsigned int) value;
			
			temp[0] = i/256;		// (i & 0xFF00)>>8;
			temp[1] = i%256;		// i & 0x00FF;
			*/
			temp[0] = MSByte(value);
			temp[1] = LSByte(value);

			 (unsigned int) value == ToMask(temp) ? error = 0 : error = 1;
		}
		break;
		
		case HUMIDITY:	
		{
			hum = (unsigned int) value;
			error = 0;
		}
		break;
		
		case PRESSURE:
		{
			value *= 100;		// accuracy: 2 decimals
			i = (unsigned int) value;
			
			pres[0] = MSByte(i);
			pres[1] = LSByte(i);

			(unsigned int) i == ToMask(pres) ? error = 0 : error = 1;
		}
		break;
		
		case BATTERY:
		{
			bat = (unsigned int) value;
			error = 0;
		}
		break;
		
		case CO2:
		{
			co_2[0] = MSByte(value);
			co_2[1] = LSByte(value);
			
			(unsigned int) value == ToMask(co_2) ? error = 0 :	error = 1;
		}
		break;
		
		case ANEMO:
		{
			an = (unsigned int) value;
			error = 0;		
		}
		
		case VANE:  // No conversion needed, value is sent in PAQUtils::InsertVane()
			 error = 0;
		break;
		
		case PLUVIO:
		{			
			rain_count[0] = MSByte(value);
			rain_count[1] = LSByte(value);
			
			(unsigned int) value == ToMask(rain_count) ? error = 0 : error = 1;		
		}
		break;
		
		case LUMINOSITY:
		{
			value *= 100;
			value /= MAX_LUMINOSITY; // To a percentage
			
			lum = (unsigned int) value;
			error = 0;
		}
		break;
		
		case SOLAR_RADIATION:
		{
			i = ( unsigned int ) value;
			radiation[0] = MSByte(i);
			radiation[1] = LSByte(i);
			error = 0;
		}
		break;
						
		default:
		{
			error = 1;
			COMM.sendError(NODE_HAD_AN_ERROR_IN_SENSOR_VALUE_2_CHARS);
		}
		break;	
	}	
	
	return error;
}

/*********************************************************************************************************
  *
  * DIFFERENT MEASURING INTERVALS
  *
  *******************************************************************************************************/
uint8_t SensorUtils::registerSensorMeasuringIntervalTime(SensorType type, uint16_t time)
{
		#ifdef NODE_DEBUG_V2
			USB.print("regSensTime = "); USB.println( (int) time );
		#endif

	uint8_t error = 2;
	switch(type)
	{	
		case TEMPERATURE:	
		{
			measuringInterval[0] = time;
			error = 0;
		}
			break;
		case HUMIDITY:	
		{
			measuringInterval[1] = time;
			error = 0;
		}
			break;		
		case PRESSURE:	
		{
			measuringInterval[2] = time;
			error = 0;
		}		
			break;
		case BATTERY:	
		{
			measuringInterval[3] = time;
			error = 0;
		}	
			break;		
		case CO2:	
		{
			measuringInterval[4] = time;
			error = 0;
		}		
			break;
		case ANEMO:	
		{
			measuringInterval[5] = time;
			error = 0;
		}		
			break;
		case VANE:	
		{
			measuringInterval[6] = time;
			error = 0;
		}
			break;
		case PLUVIO:	
		{
			measuringInterval[7] = time;
			error = 0;
		}		
			break;
		case LUMINOSITY:
		{
			measuringInterval[8] = time;
			error = 0;
		}
			break;
		case SOLAR_RADIATION:
		{
			measuringInterval[9] = time;
			error = 0;		
		}
			break;
		default:
			error = 1;
			COMM.sendError(NODE_HAD_AN_ERROR_IN_REGISTER_SENSOR_MEASURING_INTERVAL_TIME);
			break;
	}
	
	return error;
}  
  
  
void SensorUtils::saveSensorMeasuringIntervalTimes()
{
	uint16_t indicator = 1;
	uint16_t pos_eeprom = START_SENSOR_INTERVALS;
	uint8_t which_sensor = 0;
	
	for(pos_eeprom; pos_eeprom<=END_SENSOR_INTERVALS; pos_eeprom++)
	{
		if(indicator & xbeeZB.activeSensorMask)
		{
			//'EEPROM' => only overwrite necessary values
			xbeeZB.storeValue(pos_eeprom, measuringInterval[which_sensor]%256);
			pos_eeprom++;
			xbeeZB.storeValue(pos_eeprom, measuringInterval[which_sensor]/256);
		}
		else
		{
			pos_eeprom++;  //2nd ++ via for
		}
		
		which_sensor++;
		indicator <<= 1;
	}
}


void SensorUtils::readSensorMeasuringIntervalTimesFromEEPROM()
{
	uint16_t indicator = 1;
	uint16_t pos_eeprom = START_SENSOR_INTERVALS;
	uint8_t which_sensor = 0;
	
	for(pos_eeprom; pos_eeprom<=END_SENSOR_INTERVALS; pos_eeprom++)
	{
		if(indicator & xbeeZB.activeSensorMask)
		{
			measuringInterval[which_sensor] = ( (uint16_t) (Utils.readEEPROM(pos_eeprom++) + 
				Utils.readEEPROM(pos_eeprom)*256) );
		}
		else
		{
			measuringInterval[which_sensor] = 0;
			pos_eeprom++;  //2nd ++ via for
		}
		
		which_sensor++;
		indicator <<= 1;
	}
}



/*********************************************************************************************************
 * SAVE SENSOR VALUES IN CASE OF POWER SAVER MODE OR PARENT UNREACHABLE
 ********************************************************************************************************/
void SensorUtils::storeMeasuredSensorValues(uint16_t mask)
{
#ifdef POWER_MODES	
	uint16_t indicator = 1;
	uint16_t areFullMask = 0;

	
	if(xbeeZB.sleepMode == HIBERNATE)
	{
		for(uint8_t i=0; i<NUM_SENSORS_TO_SAVE; i++)
		{
			if(indicator & mask)
			{
				areFullMask += (*Saver[i])();
			}
			indicator <<= 1;
		}	
	}
	else
	{
		for(uint8_t i=0; i<NUM_SENSORS; i++)
		{
			if(indicator & mask)
			{
				areFullMask += keepSensorValueInMemory(indicator);
			}
		}
	}
	
	if(areFullMask > 0)
	{
		forceHighPerformance = true;
		xbeeZB.mustSendSavedSensorValues = false;
	}
#endif		
}

uint16_t SensorUtils::keepSensorValueInMemory(uint16_t mask)
{		
	uint16_t full = 0;
#ifdef POWER_MODES	
	switch(mask)
	{	
		case TEMPERATURE:	
		{
			sensorValue2Chars(SensUtils.temperature, TEMPERATURE);
			savedValues[0][savedPositions[0]++] = temp[0];
			savedValues[0][savedPositions[0]++] = temp[1];
			savedPositions[0] == 2 * MAX_NR_OF_SENSOR_SAVINGS ? full = 1 : full = 0;
		}
			break;
		case HUMIDITY:	
		{
			sensorValue2Chars(SensUtils.humidity, HUMIDITY);
			savedValues[1][savedPositions[1]++] = hum;
			savedPositions[1] == 2 * MAX_NR_OF_SENSOR_SAVINGS ? full = 1 : full = 0;
		}
			break;		
		case PRESSURE:	
		{
			sensorValue2Chars(SensUtils.pressure, PRESSURE);
			savedValues[2][savedPositions[2]++] = pres[0];
			savedValues[2][savedPositions[2]++] = pres[1];
			savedPositions[2] == 2 * MAX_NR_OF_SENSOR_SAVINGS ? full = 1 : full = 0;
		}		
			break;
		case BATTERY:	
		{
			sensorValue2Chars(SensUtils.battery, BATTERY);
			savedValues[3][savedPositions[3]++] = bat;
			savedPositions[3] == 2 * MAX_NR_OF_SENSOR_SAVINGS ? full = 1 : full = 0;
		}	
			break;		
		case CO2:	
		{
			sensorValue2Chars(SensUtils.co2, CO2);
			savedValues[4][savedPositions[4]++] = co_2[0];
			savedValues[4][savedPositions[4]++] = co_2[1];
			savedPositions[4] == 2 * MAX_NR_OF_SENSOR_SAVINGS ? full = 1 : full = 0;
		}		
			break;
		case ANEMO:	
		{
			sensorValue2Chars(SensUtils.anemo, ANEMO);	
			savedValues[5][savedPositions[5]++] = an;
			savedPositions[5] == 2 * MAX_NR_OF_SENSOR_SAVINGS ? full = 1 : full = 0;
		}		
			break;
		case VANE:	
		{
			savedValues[6][savedPositions[6]++] = (int) vaneDirection;
			savedPositions[6] == 2 * MAX_NR_OF_SENSOR_SAVINGS ? full = 1 : full = 0;
		}
			break;
		case PLUVIO:	
		{
			sensorValue2Chars(SensUtils.pluviometerCounter, PLUVIO);
			savedValues[7][savedPositions[7]++] = rain_count[0];
			savedValues[7][savedPositions[7]++] = rain_count[1];
			savedPositions[7] == 2 * MAX_NR_OF_SENSOR_SAVINGS ? full = 1 : full = 0;
		}		
			break;
		case LUMINOSITY:
		{
			sensorValue2Chars(SensUtils.luminosity, LUMINOSITY);
			savedValues[8][savedPositions[8]++] = lum;
			savedPositions[8] == 2 * MAX_NR_OF_SENSOR_SAVINGS ? full = 1 : full = 0;
		}
			break;
		case SOLAR_RADIATION:
		{
			sensorValue2Chars(SensUtils.solar_radiation, SOLAR_RADIATION);
			savedValues[9][savedPositions[9]++] = radiation[0];
			savedValues[9][savedPositions[9]++] = radiation[1];			
			savedPositions[9] == 2 * MAX_NR_OF_SENSOR_SAVINGS ? full = 1 : full = 0;	
		}
			break;
		default:
			COMM.sendError(NODE_HAD_AN_ERROR_IN_KEEP_SENSOR_VALUES_IN_MEMORY);
			break;
	}
#endif	
	return full;
	
}


	/***********************************************************
	 *	STATIC FUNCTION POINTERS TO SAVE SENSOR DATA IN EEPROM:
	 **********************************************************/
	uint16_t saveTemperature()
	{
		uint8_t nrSaved = Utils.readEEPROM(NR_OF_SAVED_TEMPERATURE_VALUES);
		
		if(nrSaved == MAX_NR_OF_SENSOR_SAVINGS)
			return TEMPERATURE;
			
		uint16_t eeprom_pos = FIRST_TEMPERATURE_VALUE + nrSaved;
		
		SensUtils.sensorValue2Chars(SensUtils.temperature, TEMPERATURE);
		
			#ifdef POWER_SAVER_DEBUG
				USB.print("stored temperature at"); USB.println( (int) eeprom_pos);
			#endif
		
		xbeeZB.storeValue(eeprom_pos++, SensUtils.temp[0]);
		xbeeZB.storeValue(eeprom_pos++, SensUtils.temp[1]);
		xbeeZB.storeValue(NR_OF_SAVED_TEMPERATURE_VALUES, nrSaved+1);

		return 0;
	}

	uint16_t saveHumidity()
	{
		uint8_t nrSaved = Utils.readEEPROM(NR_OF_SAVED_HUMIDITY_VALUES);
		
		if(nrSaved == MAX_NR_OF_SENSOR_SAVINGS)
			return HUMIDITY;
			
		uint16_t eeprom_pos = FIRST_HUMIDITY_VALUE + nrSaved;
		
		SensUtils.sensorValue2Chars(SensUtils.humidity, HUMIDITY);
		
			#ifdef POWER_SAVER_DEBUG
				USB.print("stored humidity at"); USB.println( (int) eeprom_pos);
			#endif
			
		xbeeZB.storeValue(eeprom_pos++, SensUtils.hum);
		xbeeZB.storeValue(NR_OF_SAVED_HUMIDITY_VALUES, nrSaved+1);
		
		return 0;
	}
	
	uint16_t savePressure()
	{
		uint8_t nrSaved = Utils.readEEPROM(NR_OF_SAVED_PRESSURE_VALUES);
		
		if(nrSaved == MAX_NR_OF_SENSOR_SAVINGS)
			return PRESSURE;
					
		uint16_t eeprom_pos = FIRST_PRESSURE_VALUE + nrSaved;

		SensUtils.sensorValue2Chars(SensUtils.pressure, PRESSURE);
		
			#ifdef POWER_SAVER_DEBUG
				USB.print("stored pressure at"); USB.println( (int) eeprom_pos);
			#endif
			
		xbeeZB.storeValue(eeprom_pos++, SensUtils.pres[0]);
		xbeeZB.storeValue(eeprom_pos++, SensUtils.pres[1]);
		xbeeZB.storeValue(NR_OF_SAVED_PRESSURE_VALUES, nrSaved+1);
		 
		return 0;
	}
	
	uint16_t saveBattery()
	{
		uint8_t nrSaved = Utils.readEEPROM(NR_OF_SAVED_BATTERY_VALUES);
		
		if(nrSaved == MAX_NR_OF_SENSOR_SAVINGS)
			return BATTERY;		
		
		uint16_t eeprom_pos = FIRST_BATTERY_VALUE + nrSaved;
	
		SensUtils.sensorValue2Chars(SensUtils.battery, BATTERY);

			#ifdef POWER_SAVER_DEBUG
				USB.print("stored battery at"); USB.println( (int) eeprom_pos);
			#endif
		
		xbeeZB.storeValue(eeprom_pos++, SensUtils.bat);
		xbeeZB.storeValue(NR_OF_SAVED_BATTERY_VALUES, nrSaved+1);
		
		return 0;		
	}
	
	uint16_t saveCO2()
	{
		uint8_t nrSaved = Utils.readEEPROM(NR_OF_SAVED_CO2_VALUES);
		
		if(nrSaved == MAX_NR_OF_SENSOR_SAVINGS)
			return CO2;
			
		uint16_t eeprom_pos = FIRST_CO2_VALUE + nrSaved;
		
		SensUtils.sensorValue2Chars(SensUtils.co2, CO2);
		
			#ifdef POWER_SAVER_DEBUG
				USB.print("stored CO2 at"); USB.println( (int) eeprom_pos);
			#endif
		
		xbeeZB.storeValue(eeprom_pos++, SensUtils.co_2[0]);
		xbeeZB.storeValue(eeprom_pos++, SensUtils.co_2[1]);
		xbeeZB.storeValue(NR_OF_SAVED_CO2_VALUES, nrSaved+1);
		
		return 0;	
	}

/*********************************************************************************************************
 * READ SENSOR VALUES FROM EEPROM IN CASE OF POWER SAVER MODE
 ********************************************************************************************************/	
#ifdef POWER_MODES
void SensorUtils::copyEEPROMValuesToMemory()
{
	for(uint8_t i=0; i<NUM_SENSORS_TO_SAVE; i++)
	{
		(*EEPROM_Reader[i])();
	}	
} 

	/***********************************************************
	 *	STATIC FUNCTION POINTERS TO READ SENSOR DATA IN EEPROM:
	 **********************************************************/
	extern void readTemperatureFromEEPROM()
	{
		uint8_t nrSaved = Utils.readEEPROM(NR_OF_SAVED_TEMPERATURE_VALUES);
		
		for(uint8_t i=0; i<nrSaved; i++)
			SensUtils.savedValues[0][SensUtils.savedPositions[0]++] = Utils.readEEPROM(FIRST_TEMPERATURE_VALUE + i);
		
		xbeeZB.storeValue(NR_OF_SAVED_TEMPERATURE_VALUES, 0);
	}
	
	extern void readHumidityFromEEPROM()
	{
		uint8_t nrSaved = Utils.readEEPROM(NR_OF_SAVED_HUMIDITY_VALUES);

		for(uint8_t i=0; i<nrSaved; i++)
			SensUtils.savedValues[1][SensUtils.savedPositions[1]++] = Utils.readEEPROM(FIRST_HUMIDITY_VALUE + i);
		
		xbeeZB.storeValue(NR_OF_SAVED_HUMIDITY_VALUES, 0);		
	}
	
	extern void readPressureFromEEPROM()
	{
		uint8_t nrSaved = Utils.readEEPROM(NR_OF_SAVED_PRESSURE_VALUES);

		for(uint8_t i=0; i<nrSaved; i++)
			SensUtils.savedValues[2][SensUtils.savedPositions[2]++] = Utils.readEEPROM(FIRST_PRESSURE_VALUE + i);
		
		xbeeZB.storeValue(NR_OF_SAVED_PRESSURE_VALUES, 0);		
	}
	
	extern void readBatteryFromEEPROM()
	{
		uint8_t nrSaved = Utils.readEEPROM(NR_OF_SAVED_BATTERY_VALUES);

		for(uint8_t i=0; i<nrSaved; i++)
			SensUtils.savedValues[3][SensUtils.savedPositions[3]++] = Utils.readEEPROM(FIRST_BATTERY_VALUE + i);
		
		xbeeZB.storeValue(NR_OF_SAVED_BATTERY_VALUES, 0);		
	}
	
	extern void readCO2FromEEPROM()
	{
		uint8_t nrSaved = Utils.readEEPROM(NR_OF_SAVED_CO2_VALUES);

		for(uint8_t i=0; i<nrSaved; i++)
			SensUtils.savedValues[4][SensUtils.savedPositions[4]++] = Utils.readEEPROM(FIRST_CO2_VALUE + i);
		
		xbeeZB.storeValue(NR_OF_SAVED_CO2_VALUES, 0);		
	}
#endif	

SensorUtils SensUtils = SensorUtils();
