/*
 *  Copyright (C) 2009 Libelium Comunicaciones Distribuidas S.L.
 *  http://www.libelium.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 2.1 of the License, or
 *  (at your option) any later version.
   
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
  
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Version:		0.8
 *  Design:		David Gascón
 *  Implementation:	Alberto Bielsa, David Cuartielles
 */
 

#ifndef __WPROGRAM_H__
  #include "WaspClasses.h"
#endif

// Constructors ////////////////////////////////////////////////////////////////

WaspSensorSmart_v20::WaspSensorSmart_v20()
{
	pinMode(DIGITAL1,INPUT);
	pinMode(DIGITAL2,INPUT);
	pinMode(DIGITAL3,OUTPUT);
	pinMode(DIGITAL4,OUTPUT);
	pinMode(DIGITAL5,OUTPUT);
	pinMode(DIGITAL6,OUTPUT);
	pinMode(DIGITAL7,OUTPUT);
	pinMode(DIGITAL8,OUTPUT);
	pinMode(SENS_PW_3V3,OUTPUT);
	pinMode(SENS_PW_5V,OUTPUT);

	digitalWrite(DIGITAL3,LOW);
	digitalWrite(DIGITAL4,LOW);
	digitalWrite(DIGITAL5,LOW);
	digitalWrite(DIGITAL6,LOW);
	digitalWrite(DIGITAL7,LOW);
	digitalWrite(DIGITAL8,LOW);
	digitalWrite(SENS_PW_3V3,LOW);
	digitalWrite(SENS_PW_5V,LOW);
}

// Public Methods //////////////////////////////////////////////////////////////

void	WaspSensorSmart_v20::setBoardMode(uint8_t mode)
{
	switch( mode )
	{
		case	SENS_ON :	digitalWrite(SENS_PW_3V3,HIGH);
					digitalWrite(SENS_PW_5V,HIGH);
					// Sets RTC on to enable I2C
					if(!RTC.isON) RTC.setMode(RTC_ON, RTC_I2C_MODE);
					break;
		case	SENS_OFF:	digitalWrite(SENS_PW_3V3,LOW);
					digitalWrite(SENS_PW_5V,LOW);
					break;
	}
}



void	WaspSensorSmart_v20::setSensorMode(uint8_t mode, uint16_t sensor)
{
	if( mode==SENS_ON )
	{
		switch( sensor )
		{
			case	SENS_SMART_LDR		:	digitalWrite(DIGITAL8,HIGH);
								break;
			case	SENS_SMART_FLOW_5V	:	digitalWrite(DIGITAL6,HIGH);
								break;
			case	SENS_SMART_LCELLS	:	if(SENS_SMART_LCELL_IN_USE==0) digitalWrite(DIGITAL4,LOW);
								if(SENS_SMART_LCELL_IN_USE==1) digitalWrite(DIGITAL3,LOW);
								break;
			case	SENS_SMART_EFERGY	:	digitalWrite(DIGITAL5,HIGH);
								break;
			case	SENS_SMART_TEMPERATURE	:	digitalWrite(DIGITAL8,HIGH);
								break;
			case	SENS_SMART_HUMIDITY	:	digitalWrite(DIGITAL8,HIGH);
								break;
			case	SENS_SMART_US_3V3	:	digitalWrite(DIGITAL7,HIGH);
								break;
			case	SENS_SMART_FLOW_3V3	:	digitalWrite(DIGITAL6,HIGH);
								break;
			case	SENS_SMART_US_5V	:	digitalWrite(DIGITAL7,HIGH);
								break;
		}
	}
	
	if( mode==SENS_OFF )
	{
		switch( sensor )
		{
			case	SENS_SMART_LDR		:	digitalWrite(DIGITAL8,LOW);
								break;
			case	SENS_SMART_FLOW_5V	:	digitalWrite(DIGITAL6,LOW);
								break;
			case	SENS_SMART_LCELLS	:	if(SENS_SMART_LCELL_IN_USE==0) digitalWrite(DIGITAL4,HIGH);
								if(SENS_SMART_LCELL_IN_USE==1) digitalWrite(DIGITAL3,HIGH);
								break;
			case	SENS_SMART_EFERGY	:	digitalWrite(DIGITAL5,LOW);
								break;
			case	SENS_SMART_TEMPERATURE	:	digitalWrite(DIGITAL8,LOW);
								break;
			case	SENS_SMART_HUMIDITY	:	digitalWrite(DIGITAL8,LOW);
								break;
			case	SENS_SMART_US_3V3	:	digitalWrite(DIGITAL7,LOW);
								break;
			case	SENS_SMART_DFS_3V3	:	digitalWrite(DIGITAL7,LOW);
								break;
			case	SENS_SMART_FLOW_3V3	:	digitalWrite(DIGITAL6,LOW);
								break;
			case	SENS_SMART_US_5V	:	digitalWrite(DIGITAL7,LOW);
								break;
			case	SENS_SMART_DFS_5V	:	digitalWrite(DIGITAL7,LOW);
								break;
		}
	}
}



float	WaspSensorSmart_v20::readValue(uint16_t sensor)
{
	return readValue(sensor, 0);
}


float	WaspSensorSmart_v20::readValue(uint16_t sensor, uint8_t type)
{
	int aux=0;
	float value = 0.0;
	
	switch( sensor )
	{
		case	SENS_SMART_LDR			:	aux = analogRead(ANALOG5);
											value = ldr_conversion(aux);
											break;
		case	SENS_SMART_FLOW_5V		:	value = flow_reading(DIGITAL1, type);
											break;
		case	SENS_SMART_LCELLS		:	aux = analogRead(ANALOG6);
											value = lcell_conversion(aux);
											break;
		case	SENS_SMART_EFERGY		:	aux = analogRead(ANALOG1);
											value = efergy_conversion(aux);
											break;
		case	SENS_SMART_TEMPERATURE	:	aux = analogRead(ANALOG4);
											value = temperature_conversion(aux);
											break;
		case	SENS_SMART_HUMIDITY		:	aux = analogRead(ANALOG7);
											value = humidity_conversion(aux);
											break;
		case	SENS_SMART_US_3V3		:	value = ultrasound_conversion(ANALOG3, type);
											break;
		case	SENS_SMART_DFS_3V3		:	value = ld_reading(ANALOG3);
											break;
		case	SENS_SMART_FLOW_3V3		:	value = flow_reading(DIGITAL2, type);
											break;
		case	SENS_SMART_US_5V		:	value = ultrasound_conversion(ANALOG2, type);
											break;
		case	SENS_SMART_DFS_5V		:	value = ld_reading(ANALOG2);
											break;
		default:;
	}
	

	return value;
}



// Private Methods //////////////////////////////////////////////////////////////

 float WaspSensorSmart_v20::ldr_conversion(int readValue)
{
	float ldr = 0;
   
	ldr = float(readValue) * 3.3 / 1023;
      
	return(ldr);
}

 float WaspSensorSmart_v20::ld_reading(uint16_t socket)
{
	int aux = 0;
	float ld = 0;
   
	aux = analogRead(socket);
   
	if (socket == ANALOG3)
	{
		ld = float(aux) * 3.3 / 1023;
	}else if (socket == ANALOG2)
			{
				ld = float(aux) * 5.5 / 1023;
			}
      
	return(ld);
}


 float WaspSensorSmart_v20::lcell_conversion(int readValue)
{
	float lcell = 0;
   
	lcell = float(readValue) * 3.3 / 1023;
      
	return(lcell);
}

 float WaspSensorSmart_v20::efergy_conversion(int readValue)
{
	float efergy = 0;
   
	efergy = float(readValue) * 3.3 / 1023;

	efergy = efergy * 26.78;
      
	return(efergy);
}

 float WaspSensorSmart_v20::temperature_conversion(int readValue)
{
	float temperature = 0;
   
	temperature = float(readValue) * 3300 / 1023;
   
	temperature = (temperature - 500) / 10;
   
	return(temperature);   
}

 float WaspSensorSmart_v20::humidity_conversion(int readValue)
{
	float humidity = 0;
   
	humidity = float(readValue) * 5500 / 1023;
   
	humidity = (humidity - 800) / 31;
   
	return(humidity);
   
}

 float WaspSensorSmart_v20::ultrasound_conversion(uint16_t socket, uint8_t model)
{
	float distance = 0;
   
	distance = float(analogRead(socket)) * 3300 / 1023;

	USB.println(distance);

	switch( model )
	{
		case	SENS_US_WRA1	:	if ( socket == ANALOG3 )
						{
							distance = distance / 3.2;
						} else
						{
							distance = distance / 2.94;
						}

						break;

		case	SENS_US_EZ0	:	if ( socket == ANALOG3 )
						{
							distance = distance / 2.5;
						} else
						{
							distance = distance / 2.28;
						}
						break;

		default			:	distance = -1.0;
						break;
	}

  
	return(distance);
   
}


 float WaspSensorSmart_v20::flow_reading(int socket, uint8_t model)
{
	float flow = 0.0;
	int value = 0;
	long start = 0;
	int previous_reading = 0;
	int reading = 0;
	start = millis();
	while((millis()-start)<=1000)
	{
	  previous_reading = reading;
	  reading = digitalRead(socket);
	  
	  if((previous_reading == 1)&&(reading == 0))
	  {
	    value++;
	  }
	}
	delay(100);


	switch(model)
	{
		case SENS_FLOW_FS100	:	flow = float(value) / 65;
						break;

		case SENS_FLOW_FS200	:	flow = float(value) / 7.5;
						break;

		case SENS_FLOW_FS400	:	flow = float(value) / 6.5;
						break;

		default 		:	flow = -1.0;
						break;
	}

	return flow;

}



WaspSensorSmart_v20 SensorSmartv20=WaspSensorSmart_v20();
