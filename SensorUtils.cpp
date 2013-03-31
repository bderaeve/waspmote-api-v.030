
#ifndef __WPROGRAM_H__
	#include "BjornClasses.h"
	#include "WaspClasses.h"
#endif

//#include <inttypes.h>

/*
 * Constructor
 */
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
		measuringInterval[i] = xbeeZB.defaultTime2Wake;
}

void SensorUtils::testPrint()
{
	USB.print("testSensUtils");
}
	


void SensorUtils::measureTemperature()
{
	#ifdef SENS_DEBUG
		USB.println("SensorUtils::measureTemperature()");
	#endif
	temperature = 0;
	SensorGasv20.setSensorMode(SENS_ON, SENS_TEMPERATURE);
	
	for (int i=0;i<10;i++)
	{
		/* TEMPERATURE SENSOR:   RANGE: -40° -> +125° */
		temperature += SensorGasv20.readValue(SENS_TEMPERATURE);
		delay(100);
	}
	
	SensorGasv20.setSensorMode(SENS_OFF, SENS_TEMPERATURE);
	temperature /= 10;
}


void SensorUtils::measureHumidity()
{
	#ifdef SENS_DEBUG
		USB.println("SensorUtils::measureHumidity()");
	#endif
	humidity = 0;
	SensorGasv20.setSensorMode(SENS_ON, SENS_HUMIDITY);
	
	for (int i=0;i<10;i++)
	{
		/* HUMIDITY SENSOR:   RANGE: 0 -> 100% */
		humidity += SensorGasv20.readValue(SENS_HUMIDITY);
		delay(100);
	}
	
	SensorGasv20.setSensorMode(SENS_OFF, SENS_HUMIDITY);
	humidity /= 10;
}


void SensorUtils::measurePressure()
{
	#ifdef SENS_DEBUG
		USB.println("SensorUtils::measurePressure()");
	#endif
	pressure = 0;
	SensorGasv20.setSensorMode(SENS_ON, SENS_PRESSURE);
	
	for (int i=0;i<10;i++)
	{
		/* ATMOSPHERIC PRESSURE SENSOR:   RANGE: 15 -> 115 kPa */
		pressure += SensorGasv20.readValue(SENS_PRESSURE);
		delay(100);
	}
	
	SensorGasv20.setSensorMode(SENS_OFF, SENS_PRESSURE);
}


void SensorUtils::measureBattery()
{
	#ifdef SENS_DEBUG
		USB.println("SensorUtils::measureBattery()");
	#endif
	battery = 0;
	for (int i=0;i<10;i++)
	{
		battery += PWR.getBatteryLevel();
		delay(100);
	}
	
	battery /= 10;
	//USB.print("measureBattery(): "); USB.println( (int) battery);
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
	
	for (int i=0;i<10;i++)
	{          
		/* CO2 SENSOR:   RANGE: 350 -> 10 000 ppm 
           normal outdoor level: 350 - 450 ppm; acceptable levels: < 600 ppm */
		co2 += SensorGasv20.readValue(SENS_CO2);
	}
	
	SensorGasv20.setSensorMode(SENS_OFF, SENS_CO2);
	co2 *= 100;
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
		(this->*reader[3])();  // measure only battery,  don't turn on sensor board
		error = 0;
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
			if(indicator & mask)
			{
				(this->*reader[i])();  // read the corresponding sensor
			}
			indicator <<= 1;
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
			char str[10];
			USB.print("pressure = "); USB.println(value);
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
			error = 1;
			USB.println("Went into default");
			break;
	}	
	
	return error;
}



SensorUtils SensUtils = SensorUtils();
