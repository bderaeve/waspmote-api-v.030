
#ifndef __WPROGRAM_H__
	#include "BjornClasses.h"
	#include "WaspClasses.h"
#endif

//#include <inttypes.h>

/*********************************************************************************************************
  *
  * CONSTRUCTOR / DEFAULT VALUES
  *
  *******************************************************************************************************/
StoreSensorData * Saver[5] = { &saveTemperature, &saveHumidity, &savePressure,
			&saveBattery, &saveCO2};

			
SensorUtils::SensorUtils()
{
	reader[0] = &SensorUtils::measureTemperature;
	reader[1] = &SensorUtils::measureHumidity;
	reader[2] = &SensorUtils::measurePressure;
	reader[3] = &SensorUtils::measureBattery;
	reader[4] = &SensorUtils::measureCO2;
	reader[5] = &SensorUtils::measureAnemo;
	reader[6] = &SensorUtils::measureVane;
	reader[7] = &SensorUtils::measurePluvio;
	
	for(uint8_t i=0; i<8; i++)
		measuringInterval[i] = xbeeZB.defaultTime2WakeInt;
}

	
/*********************************************************************************************************
  *
  * MEASURING
  *
  *******************************************************************************************************/
void SensorUtils::measureTemperature()
{
		#ifdef SENS_DEBUG
			USB.println("SensorUtils::measureTemperature()");
			previous = millis();
		#endif
		
	temperature = 0;
	SensorGasv20.setSensorMode(SENS_ON, SENS_TEMPERATURE);
	
	#ifndef SENS_DEBUG
	for (int i=0;i<NUM_MEASUREMENTS;i++)
	{
		/* TEMPERATURE SENSOR:   RANGE: -40° -> +125° */
		temperature += SensorGasv20.readValue(SENS_TEMPERATURE);
	}
	temperature /= NUM_MEASUREMENTS;
	#endif
	
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
		
	SensorGasv20.setSensorMode(SENS_OFF, SENS_TEMPERATURE);
	
	#ifdef SENS_DEBUG
		previous = millis() - previous;
		USB.print("SensorUtils::measureTemperature() took  ms"); USB.println(previous);
	#endif
}


void SensorUtils::measureHumidity()
{
	#ifdef SENS_DEBUG
		USB.println("SensorUtils::measureHumidity()");
		previous = millis();
	#endif
	
	humidity = 0;
	SensorGasv20.setSensorMode(SENS_ON, SENS_HUMIDITY);
	
	#ifndef SENS_DEBUG
	for (int i=0;i<NUM_MEASUREMENTS;i++)
	{
		/* HUMIDITY SENSOR:   RANGE: 0 -> 100% */
		humidity += SensorGasv20.readValue(SENS_HUMIDITY);
	}
	humidity /= NUM_MEASUREMENTS;
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
	
	#ifdef SENS_DEBUG
		previous = millis() - previous;
		USB.print("SensorUtils::measureHumidity() took  ms"); USB.println(previous);
	#endif
}


void SensorUtils::measurePressure()
{
	#ifdef SENS_DEBUG
		USB.println("SensorUtils::measurePressure()");
		previous = millis();
	#endif
	
	pressure = 0;
	SensorGasv20.setSensorMode(SENS_ON, SENS_PRESSURE);
	
	#ifndef SENS_DEBUG
	for (int i=0;i<NUM_MEASUREMENTS;i++)
	{
		/* ATMOSPHERIC PRESSURE SENSOR:   RANGE: 15 -> 115 kPa */
		pressure += SensorGasv20.readValue(SENS_PRESSURE);
	}
	pressure /= NUM_MEASUREMENTS;
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
	
	#ifdef SENS_DEBUG
		previous = millis() - previous;
		USB.print("SensorUtils::measurePressure() took  ms"); USB.println(previous);
	#endif
}


void SensorUtils::measureBattery()
{
	#ifdef SENS_DEBUG
		USB.println("SensorUtils::measureBattery()");
		previous = millis();
	#endif
	
	battery = 0;
	
	#ifndef SENS_DEBUG
	for (int i=0;i<NUM_MEASUREMENTS;i++)
	{
		battery += PWR.getBatteryLevel();
	}
	
	battery /= NUM_MEASUREMENTS;
	#endif
	
		#ifdef SENS_DEBUG
			previous = millis();
			for (int i=0;i<NUM_MEASUREMENTS;i++)
			{
				battery = PWR.getBatteryLevel();
				USB.println( (int) battery);
				delay(100);
			}
			previous = millis() - previous;
			USB.print("timeWithDelay "); USB.println(previous);			
		
			USB.println("no delay between measurements:");
			previous = millis();
			for (int i=0;i<NUM_MEASUREMENTS;i++)
			{
				battery = PWR.getBatteryLevel();
				USB.println( (int) battery);
			}	
			previous = millis() - previous;
			USB.print("timeNoDelay "); USB.println(previous);				
		#endif		
		
	#ifdef SENS_DEBUG
		previous = millis() - previous;
		USB.print("SensorUtils::measureBattery() took  ms"); USB.println(previous);
	#endif
}


void SensorUtils::measureCO2()
{
	#ifdef SENS_DEBUG
		USB.println("SensorUtils::measureCO2()");
	#endif
	co2 = 0;
	long previous = 0;
	USB.println("Measuring CO2: 30 sec");
	
	SensorGasv20.configureSensor(SENS_CO2, 1);
    SensorGasv20.setSensorMode(SENS_ON, SENS_CO2);
	  
	previous = millis();
	while (millis() - previous < 30000)
	{
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
}


void SensorUtils::measureAnemo()
{
	#ifdef SENS_DEBUG
		USB.println("SensorUtils::measureAnemo()");
	#endif
}
void SensorUtils::measureVane()
{
	#ifdef SENS_DEBUG
		USB.println("SensorUtils::measureVane()");
	#endif
}
void SensorUtils::measurePluvio()
{
	#ifdef SENS_DEBUG
		USB.println("SensorUtils::measurePluvio()");
	#endif
}


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

	if( mask == 0 )
	{
		error = 1;
		#ifdef SENS_DEBUG
			USB.println("*mask = 0 in uint8_t SensorUtils::measureSensors(uint16_t mask)");
		#endif
	}
	else if ( mask == BATTERY )
	{
		(this->*reader[3])();  	/// measure only battery,  don't turn on sensor board
		error = 0;				/// no time check since this is the only reason we came awake
		#ifdef SENS_DEBUG
			USB.println("*mask = BATTERY in uint8_t SensorUtils::measureSensors(uint16_t * mask)");
		#endif
	}
	else
	{
		#ifdef SENS_DEBUG
			USB.print("In SensorUtils::measureSensors(uint16_t mask): mask = ");
			USB.println( (int) mask );
			USB.println("Turning on sensorBoard in uint8_t SensorUtils::measureSensors(uint16_t mask)");
		#endif
		SensorGasv20.ON();
		SensorGasv20.setBoardMode(SENS_ON);
		
		//Stabilization delay after turning on sensor board
		delay(100);
	
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
					if(indicator & mask && RTCUt.RTCSecMinHourInt % xbeeZB.time2wValuesArray[xbeeZB.posInArray-1] == 0 )
					{
						(this->*reader[i])();  // read the corresponding sensor
					}
				}
				else /// IF HIBERNATE:
				{
					if(indicator & mask && RTCUt.RTCSecMinHourInt % xbeeZB.storedTime == 0 )
					{
						(this->*reader[i])();  // read the corresponding sensor
					}
				
				}
				indicator <<= 1;
			}
		}
		
		SensorGasv20.OFF();
		error = 0;
	}
	
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
			value += 40;		// negative values start at 0
			value *= 100;		// accuracy: 2 decimals  (1 dec is ok but have 4 bits left anyway)
			
			i = (unsigned int) value;
			
			temp[0] = i/256;		// (i & 0xFF00)>>8;
			temp[1] = i%256;		// i & 0x00FF;
			
			#ifdef SENS_DEBUG
				USB.print("i = "); 			USB.println( (int) i );
				USB.print("temp[0] = ");	USB.println( (int) temp[0] );
				USB.print("temp[1] = ");	USB.println( (int) temp[1] );
			#endif
			
			if( i == ( ((unsigned int) temp[0]*256) + temp[1] )  )
				error = 0;
			else
				error = 1;
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
			//char str[10];
			//USB.print("pressure = "); USB.println(value);
			value *= 100;		// accuracy: 2 decimals
			value -= 50000;
			
			i = (unsigned int) value;
			//USB.print("i = "); USB.println(( long int ) i);
			
			pres[0] = i/256;
			pres[1] = i%256;
			
			//USB.print("pres[0] = "); USB.println((int) pres[0]);
			//USB.print("pres[1] = "); USB.println((int) pres[1]);
			
			if( i == ( ((unsigned int) pres[0]*256) + pres[1] )  )	
				//USB.print("i == "); USB.println( (long int) ( ((unsigned int) pres[0]*256) + pres[1] ) );
				error = 0;
			else
				error = 1;
				
			//USB.print("Error = "); USB.println((int)error);
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
			i = (unsigned int) value;
			
			co_2[0] = i/256;
			co_2[1] = i%256;
			
			if( i == ( ((unsigned int) co_2[0]*256) + co_2[1] )  )
				error = 0;
			else
				error = 1;
		}
		break;
		
		default:
		{
			error = 1;
			USB.println("Went into default");
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
		default:
			error = 1;
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
  *
  * SAVE SENSOR VALUES IN CASE OF POWER SAVER MODE
  *
  *******************************************************************************************************/
uint8_t SensorUtils::measureAndstoreSensorValues(uint16_t mask)
{
	uint8_t error = 2;
	uint16_t indicator = 1;
	uint16_t toMeasureMask = 0;
		#ifdef POWER_SAVER_DEBUG
			USB.println("storing values");
		#endif
	error = measureSensors(mask);
	if( error ) return error;
	
	for(uint8_t i=0; i<NUM_SENSORS; i++)
	{
		if(indicator & mask)
		{
			toMeasureMask += (*Saver[i])();
			error = 0;
		}
		indicator <<= 1;
	}
	
	if( toMeasureMask > 0 )
	{
		//This bool should be read when waking up
		xbeeZB.storeValue(MUST_SEND_SAVED_SENSOR_VALUES, true);
		xbeeZB.storeValue(MUST_SEND_SAVED_SENSOR_VALUES_MASK_H ,toMeasureMask/256);
		xbeeZB.storeValue(MUST_SEND_SAVED_SENSOR_VALUES_MASK_L ,toMeasureMask%256);
	}
	
	return error;
}

/*********************************************************************************************************
 *	STATIC FUNCTION POINTERS TO SAVE SENSOR DATA:
 ********************************************************************************************************/
	uint16_t saveTemperature()
	{
		uint16_t error = 2;
		uint16_t eeprom_pos = FIRST_TEMPERATURE_VALUE + Utils.readEEPROM(NR_OF_SAVED_TEMPERATURE_VALUES);
		uint8_t maxAllowed = Utils.readEEPROM(MAX_NR_OF_TEMPERATURE_SAVINGS);
		//uint8_t nrPresentValues = Utils.readEEPROM(NR_OF_SAVED_TEMPERATURE_VALUES) avoid eeprom writes
		
		SensUtils.sensorValue2Chars(SensUtils.temperature, TEMPERATURE);
		
			#ifdef POWER_SAVER_DEBUG
				USB.print("stored temperature at"); USB.println( (int) eeprom_pos);
			#endif
		
		xbeeZB.storeValue(eeprom_pos++, SensUtils.temp[0]);
		xbeeZB.storeValue(eeprom_pos++, SensUtils.temp[1]);
		//storeValue(nrPresentValues + 1, NR_OF_SAVED_TEMPERATURE_VALUES);
		
		if(eeprom_pos == LAST_TEMPERATURE_VALUE || eeprom_pos == MAX_NR_OF_TEMPERATURE_SAVINGS)
			return error = TEMPERATURE;
		else 
			return error = 0;
	}

	uint16_t saveHumidity()
	{
		uint16_t error = 2;
		uint16_t eeprom_pos = FIRST_HUMIDITY_VALUE + Utils.readEEPROM(NR_OF_SAVED_HUMIDITY_VALUES);
		uint8_t maxAllowed = Utils.readEEPROM(MAX_NR_OF_HUMIDITY_SAVINGS);
		
		SensUtils.sensorValue2Chars(SensUtils.humidity, HUMIDITY);
		
			#ifdef POWER_SAVER_DEBUG
				USB.print("stored humidity at"); USB.println( (int) eeprom_pos);
			#endif
			
		xbeeZB.storeValue(eeprom_pos++, SensUtils.hum);
		
		if(eeprom_pos == LAST_HUMIDITY_VALUE || eeprom_pos == MAX_NR_OF_HUMIDITY_SAVINGS)
			return error = HUMIDITY;
		else 
			return error = 0;
	}
	
	uint16_t savePressure()
	{
		uint16_t error = 2;
		uint16_t eeprom_pos = FIRST_PRESSURE_VALUE + Utils.readEEPROM(NR_OF_SAVED_PRESSURE_VALUES);
		uint8_t maxAllowed = Utils.readEEPROM(MAX_NR_OF_PRESSURE_SAVINGS);
		
		SensUtils.sensorValue2Chars(SensUtils.pressure, PRESSURE);
		
			#ifdef POWER_SAVER_DEBUG
				USB.print("stored pressure at"); USB.println( (int) eeprom_pos);
			#endif
			
		xbeeZB.storeValue(eeprom_pos++, SensUtils.pres[0]);
		xbeeZB.storeValue(eeprom_pos++, SensUtils.pres[1]);
		
		if(eeprom_pos == LAST_PRESSURE_VALUE || eeprom_pos == MAX_NR_OF_PRESSURE_SAVINGS)
			return error = PRESSURE;
		else 
			return error = 0;
	}
	
	uint16_t saveBattery()
	{
		uint16_t error = 2;
		uint16_t eeprom_pos = FIRST_BATTERY_VALUE + Utils.readEEPROM(NR_OF_SAVED_BATTERY_VALUES);
		uint8_t maxAllowed = Utils.readEEPROM(MAX_NR_OF_BATTERY_SAVINGS);	

		SensUtils.sensorValue2Chars(SensUtils.battery, BATTERY);

			#ifdef POWER_SAVER_DEBUG
				USB.print("stored battery at"); USB.println( (int) eeprom_pos);
			#endif
		
		xbeeZB.storeValue(eeprom_pos++, SensUtils.bat);
		
		if(eeprom_pos == LAST_BATTERY_VALUE || eeprom_pos == MAX_NR_OF_BATTERY_SAVINGS)
			return error = BATTERY;
		else 
			return error = 0;		
	}
	
	uint16_t saveCO2()
	{
		uint16_t error = 2;
		uint16_t eeprom_pos = FIRST_CO2_VALUE + Utils.readEEPROM(NR_OF_SAVED_CO2_VALUES);
		uint8_t maxAllowed = Utils.readEEPROM(MAX_NR_OF_CO2_SAVINGS);
		
		SensUtils.sensorValue2Chars(SensUtils.co2, CO2);
		
			#ifdef POWER_SAVER_DEBUG
				USB.print("stored CO2 at"); USB.println( (int) eeprom_pos);
			#endif
		
		xbeeZB.storeValue(eeprom_pos++, SensUtils.co_2[0]);
		xbeeZB.storeValue(eeprom_pos++, SensUtils.co_2[1]);
		
		if(eeprom_pos == LAST_CO2_VALUE || eeprom_pos == MAX_NR_OF_CO2_SAVINGS)
			return error = CO2;
		else 
			return error = 0;	
	}

SensorUtils SensUtils = SensorUtils();
