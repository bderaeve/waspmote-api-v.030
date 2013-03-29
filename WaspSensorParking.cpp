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
 *  Version:		0.9
 *  Design:		David Gascón
 *  Implementation:	Alberto Bielsa, Manuel Calahorra
 */
 

#ifndef __WPROGRAM_H__
//  #include <WProgram.h>
#include <WaspClasses.h>
#endif

// Constructors ////////////////////////////////////////////////////////////////

WaspSensorParking::WaspSensorParking()
{
	pinMode(DIGITAL7,OUTPUT);
	pinMode(DIGITAL6,OUTPUT);
	pinMode(DIGITAL5,OUTPUT);
	pinMode(DIGITAL4,OUTPUT);
	pinMode(DIGITAL3,OUTPUT);
	pinMode(DIGITAL2,OUTPUT);
	pinMode(DIGITAL1,OUTPUT);
	pinMode(16,OUTPUT);
	pinMode(17,OUTPUT);
	pinMode(19,OUTPUT);
	pinMode(SENS_PW_3V3,OUTPUT);
	pinMode(SENS_PW_5V,OUTPUT);
	
	digitalWrite(DIGITAL7,LOW);
	digitalWrite(DIGITAL6,LOW);
	digitalWrite(DIGITAL5,LOW);
	digitalWrite(DIGITAL4,LOW);
	digitalWrite(DIGITAL3,LOW);
	digitalWrite(DIGITAL2,LOW);
	digitalWrite(DIGITAL1,LOW);
	digitalWrite(16,LOW);
	digitalWrite(17,LOW);
	digitalWrite(19,LOW);
	digitalWrite(SENS_PW_3V3,LOW);
	digitalWrite(SENS_PW_5V,LOW);
}

// Public Methods //////////////////////////////////////////////////////////////

void	WaspSensorParking::setBoardMode(uint8_t mode)
{
	switch( mode )
	{
		case	SENS_ON:	digitalWrite(SENS_PW_5V,HIGH);
					break;
		case	SENS_OFF:	digitalWrite(SENS_PW_5V,LOW);
					break;
	}
}

void	WaspSensorParking::calibration(void)
{
	uint16_t val_x = 0;
	uint16_t val_y = 0;
	uint16_t val_z = 0;  
	int temp = 0;
	int temper = 0;
	float temp2 = 0.0;
	  
	for(int i=0; i<8; i++)
	{
		readParkingSetReset();
		val_x+=valueX;
		val_y+=valueY;
		val_z+=valueZ;  
		delay(500);  
	}
	  
	initialX = val_x/8;
	initialY = val_y/8;
	initialZ = val_z/8;

	temper = 0;  
	temp = 0;
	for(int i=0; i<8; i++)
	{
		temper=analogRead(SENS_PARKING_T);  
		delay(100);
		temp2 = ((float(temper) * 3300 / 1023) - 500) / 10;
		temper = int(temp2);
		temp = temp + temper;
	}

	temp2 = float(temp)/8;
	 
	temp = int(temp2);

	initialT = temp;
	  
	indexX = coefX2*temp*temp + coefX*temp + constX;
	indexY = coefY2*temp*temp + coefY*temp + constY;
	indexZ = coefZ2*temp*temp + coefZ*temp + constZ;

}

void	WaspSensorParking::loadReference(void)
{
	int address;

	address = PARKING_ADDRESS_COEFX2;
	coefX2 = eeprom_read_float((float*)(address));
	address = PARKING_ADDRESS_COEFY2;
	coefY2 = eeprom_read_float((float*)(address));
	address = PARKING_ADDRESS_COEFZ2;
	coefZ2 = eeprom_read_float((float*)(address));
	address = PARKING_ADDRESS_COEFX;
	coefX = eeprom_read_float((float*)(address));
	address = PARKING_ADDRESS_COEFY;
	coefY = eeprom_read_float((float*)(address));
	address = PARKING_ADDRESS_COEFZ;
	coefZ = eeprom_read_float((float*)(address));
	address = PARKING_ADDRESS_CONSTX;
	constX = eeprom_read_float((float*)(address));
	address = PARKING_ADDRESS_CONSTY;
	constY = eeprom_read_float((float*)(address));
	address = PARKING_ADDRESS_CONSTZ;
	constZ = eeprom_read_float((float*)(address));
}

void	WaspSensorParking::calculateReference(int temperature)
{
	referenceX = int((float(initialX) / indexX)*( coefX2*temperature*temperature + coefX*temperature + constX ));
	referenceY = int((float(initialY) / indexY)*( coefY2*temperature*temperature + coefY*temperature + constY ));
	referenceZ = int((float(initialZ) / indexZ)*( coefZ2*temperature*temperature + coefZ*temperature + constZ ));
}

int	WaspSensorParking::readTemperature(void)
{
	int temp = 0;
	int temper = 0;
	float temp2 = 0.0;
	  
	for(int i=0; i<8; i++)
	{
		temper=analogRead(SENS_PARKING_T);  
		delay(100);
		temp2 = ((float(temper) * 3300 / 1023) - 500) / 10;
		temper = int(temp2);
		temp = temp + temper;
	}

	temp2 = float(temp)/8;
	 
	temp = int(temp2);

	return temp;

}

void	WaspSensorParking::readParking(void)
{
	valueX = analogRead(SENS_PARKING_X);
	valueY = analogRead(SENS_PARKING_Y);
	valueZ = analogRead(SENS_PARKING_Z);
}

void	WaspSensorParking::readParkingSetReset(void)
{
	digitalWrite(PARKING_SET_RESET_Y,HIGH);
	digitalWrite(PARKING_SET_RESET_X,HIGH);
	delay(100);
	digitalWrite(PARKING_SET_RESET_Y,LOW);
	digitalWrite(PARKING_SET_RESET_X,LOW);
	delay(10);
	valueX = analogRead(SENS_PARKING_X);
	valueY = analogRead(SENS_PARKING_Y);
	delay(100);
	digitalWrite(PARKING_SET_RESET_Z,HIGH);
	delay(100);
	digitalWrite(PARKING_SET_RESET_Z,LOW);
	delay(10);
	valueZ = analogRead(SENS_PARKING_Z);
}

boolean	WaspSensorParking::estimateState(void)
{
	boolean status;

	if( (abs((valueX-referenceX))>=PARKING_THRESHOLD) || (abs((valueY-referenceY))>=PARKING_THRESHOLD) || (abs((valueZ-referenceZ))>=PARKING_THRESHOLD) ) status = PARKING_OCCUPIED;
	else status = PARKING_EMPTY;

	return status;
}


// Private Methods //////////////////////////////////////////////////////////////


WaspSensorParking SensorParking=WaspSensorParking();
