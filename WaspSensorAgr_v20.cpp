/*
 *  Copyright (C) 2012 Libelium Comunicaciones Distribuidas S.L.
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
 *  Version:		0.1
 *  Design:			David Gascón
 *  Implementation:	Alberto Bielsa, Manuel Calahorra, Yuri Carmona
 */


#ifndef __WPROGRAM_H__
#include "WaspClasses.h"
#endif

// Constructors ////////////////////////////////////////////////////////////////

WaspSensorAgr_v20::WaspSensorAgr_v20()
{
	pinMode(LOW_BAT_MON, INPUT);
	digitalWrite(LOW_BAT_MON, HIGH);
	
	pinMode(DIGITAL8,OUTPUT);
	pinMode(DIGITAL7,OUTPUT);
	pinMode(DIGITAL6,OUTPUT);
	pinMode(DIGITAL5,OUTPUT);
	pinMode(DIGITAL4,INPUT);
	pinMode(DIGITAL3,OUTPUT);
	pinMode(DIGITAL2,INPUT);	
	pinMode(DIGITAL1,OUTPUT);	
	pinMode(ANA0,OUTPUT);
	pinMode(SENS_PW_3V3,OUTPUT);
	pinMode(SENS_PW_5V,OUTPUT);
	pinMode(15, INPUT);
	pinMode(17, INPUT);
	pinMode(19, INPUT);
	
	digitalWrite(DIGITAL8,LOW);
	digitalWrite(DIGITAL7,LOW);
	digitalWrite(DIGITAL6,LOW);
	digitalWrite(DIGITAL5,LOW);
	digitalWrite(DIGITAL3,LOW);
	digitalWrite(DIGITAL1,LOW);
	digitalWrite(ANA0,LOW);
	digitalWrite(SENS_PW_3V3,LOW);
	digitalWrite(SENS_PW_5V,LOW);
}

// Public Methods //////////////////////////////////////////////////////////////

void	WaspSensorAgr_v20::setBoardMode(uint8_t mode)
{
//	delay(1000);
	
	switch( mode )
	{
		case	SENS_ON :	digitalWrite(SENS_PW_3V3,HIGH);
							digitalWrite(SENS_PW_5V,HIGH);
							// Sets RTC on to enable I2C
							if(!RTC.isON) RTC.setMode(RTC_ON, RTC_I2C_MODE);
							digitalWrite(DIGITAL5,HIGH);
							break;
		case	SENS_OFF:	digitalWrite(SENS_PW_3V3,LOW);
							digitalWrite(SENS_PW_5V,LOW);
							digitalWrite(DIGITAL5,LOW);
							break;
		default:;
	}
}


void	WaspSensorAgr_v20::setSensorMode(uint8_t mode, uint16_t sensor)
{	
	if( mode==SENS_ON )
	{
		switch( sensor )
		{
			case	SENS_AGR_PRESSURE	:	digitalWrite(SENS_SWITCH_1,HIGH);
											break;
			case	SENS_AGR_WATERMARK_1:	digitalWrite(SENS_SWITCH_2,HIGH);
											break;
			case	SENS_AGR_WATERMARK_2:	digitalWrite(SENS_SWITCH_2,HIGH);
											break;
			case	SENS_AGR_WATERMARK_3:	digitalWrite(SENS_SWITCH_2,HIGH);
											break;				
			case	SENS_AGR_ANEMOMETER	:	digitalWrite(SENS_SWITCH_3,HIGH);
											break;
			case	SENS_AGR_VANE		:	digitalWrite(SENS_SWITCH_3,HIGH);
											break;
			case	SENS_AGR_DENDROMETER:	digitalWrite(SENS_SWITCH_4,HIGH);
											break;
			case	SENS_AGR_PT1000		:	digitalWrite(SENS_SWITCH_4,HIGH);
											break;
			case	SENS_AGR_LEAF_WETNESS:	digitalWrite(SENS_SWITCH_4,HIGH);
											digitalWrite(SENS_MUX_SEL,HIGH);
											break;
			case	SENS_AGR_TEMPERATURE:	digitalWrite(SENS_SWITCH_4,HIGH);
											digitalWrite(SENS_MUX_SEL,HIGH);
											break;
			case	SENS_AGR_HUMIDITY	:	digitalWrite(SENS_SWITCH_4,HIGH);
											digitalWrite(SENS_MUX_SEL,HIGH);
											break;
			case	SENS_AGR_RADIATION	:	digitalWrite(SENS_SWITCH_4,HIGH);
											break;
			case	SENS_AGR_SENSIRION	:	digitalWrite(SENS_SWITCH_4,HIGH);
											break;
			case	SENS_AGR_LDR		:	digitalWrite(SENS_SWITCH_4,HIGH);
											break;
			default						:	;
		}
	}
	
	if( mode==SENS_OFF )
	{
		switch( sensor )
		{
			case	SENS_AGR_PRESSURE	:	digitalWrite(SENS_SWITCH_1,LOW);
											break;
			case	SENS_AGR_WATERMARK_1:	digitalWrite(SENS_SWITCH_2,LOW);
											break;
			case	SENS_AGR_WATERMARK_2:	digitalWrite(SENS_SWITCH_2,LOW);
											break;
			case	SENS_AGR_WATERMARK_3:	digitalWrite(SENS_SWITCH_2,LOW);
											break;				
			case	SENS_AGR_ANEMOMETER	:	digitalWrite(SENS_SWITCH_3,LOW);
											break;
			case	SENS_AGR_VANE		:	digitalWrite(SENS_SWITCH_3,LOW);
											break;
			case	SENS_AGR_DENDROMETER:	digitalWrite(SENS_SWITCH_4,LOW);
											break;
			case	SENS_AGR_PT1000		:	digitalWrite(SENS_SWITCH_4,LOW);
											break;
			case	SENS_AGR_LEAF_WETNESS:	digitalWrite(SENS_SWITCH_4,LOW);
											break;
			case	SENS_AGR_TEMPERATURE:	digitalWrite(SENS_SWITCH_4,LOW);
											break;
			case	SENS_AGR_HUMIDITY	:	digitalWrite(SENS_SWITCH_4,LOW);
											break;
			case	SENS_AGR_RADIATION:		digitalWrite(SENS_SWITCH_4,LOW);
											break;
			case	SENS_AGR_SENSIRION:		digitalWrite(SENS_SWITCH_4,LOW);
											break;
			case	SENS_AGR_LDR:			digitalWrite(SENS_SWITCH_4,LOW);
											break;
			default						:	;
		}
	}
}


float	WaspSensorAgr_v20::readValue(uint16_t sensor)
{
	return readValue(sensor,0);
}


float	WaspSensorAgr_v20::readValue(uint16_t sensor, uint8_t type)
{
	float aux=0;
	uint16_t aux2=0;
	
	switch( sensor )
	{
		case	SENS_AGR_PRESSURE:		aux2=analogRead(ANALOG3);
										aux=pressure_conversion(aux2); // KPa
										break;
		case	SENS_AGR_WATERMARK_1:	digitalWrite(SENS_MUX_SEL,LOW);
										aux=readWatermark(SENS_AGR_WATERMARK_1); // 50Hz - 200cbar || 10KHz - 0cbar
										digitalWrite(SENS_MUX_SEL,HIGH);
										break;
		case	SENS_AGR_WATERMARK_2:	digitalWrite(SENS_MUX_SEL,LOW);
										aux=readWatermark(SENS_AGR_WATERMARK_2); // 50Hz - 200cbar || 10KHz - 0cbar
										digitalWrite(SENS_MUX_SEL,HIGH);
										break;
		case	SENS_AGR_WATERMARK_3:	digitalWrite(SENS_MUX_SEL,LOW);
										aux=readWatermark(SENS_AGR_WATERMARK_3); // 50Hz - 200cbar || 10KHz - 0cbar
										digitalWrite(SENS_MUX_SEL,HIGH);
										break;
		case	SENS_AGR_ANEMOMETER:	aux=readAnemometer();
										break;
		case	SENS_AGR_VANE:			aux2=analogRead(ANALOG5);
										aux=(aux2*3.3)/1023; // Volts
										getVaneDirection(aux); // Direction
										break;
		case	SENS_AGR_DENDROMETER:	aux=readDendrometer(); //mm
										break;
		case	SENS_AGR_PT1000:		aux=readPT1000(); // º Celsius
										break;
		case	SENS_AGR_LEAF_WETNESS:	aux2=analogRead(ANALOG6);
										aux=(aux2*3.3)/1023;
										break;
		case	SENS_AGR_TEMPERATURE:	aux2=analogRead(ANALOG4);
										aux=mcp_conversion(aux2); // ºCelsius
										break;
		case	SENS_AGR_HUMIDITY:		aux2=analogRead(ANALOG2);
										aux=sencera_conversion(aux2); // %Hum
										break;
		case	SENS_AGR_RADIATION:		aux=readRadiation(); //Volts
										break;
		case	SENS_AGR_SENSIRION:		if( type==SENSIRION_TEMP ) aux=readSensirion(SENSIRION_TEMP); // ºCelsius
										else if( type==SENSIRION_HUM ) aux=readSensirion(SENSIRION_HUM);// %Hum
										break;
		case	SENS_AGR_PLUVIOMETER:	aux2=readPluviometer();
										aux=float(aux2) * 0.2794 * 20; // mm/min
										break;	
		case	SENS_AGR_LDR:			aux2=analogRead(ANALOG7);
										aux=float(aux2*3.3) / 1023;
										break;	
		default:;
	}
	return	aux;
}



/* attacInt() - attach interruption
 *
 * It attaches sensors board interruption, setting three-state
 */
void	WaspSensorAgr_v20::attachPluvioInt(void) 
{
		enableInterrupts(PLV_INT | HAI_INT);	
}


/* detachInt() - detaches interruptions
 *
 * It detaches sensor board interruption, unsetting three-state
 */
void	WaspSensorAgr_v20::detachPluvioInt(void) 
{
	disableInterrupts(PLV_INT | HAI_INT);
}

void	WaspSensorAgr_v20::sleepAgr(const char*	time2wake, uint8_t offset, uint8_t mode, uint8_t option)
{
	sleepAgr(time2wake, offset, mode, option, 0);
}

void	WaspSensorAgr_v20::sleepAgr(const char* time2wake, uint8_t offset, uint8_t mode, uint8_t option, uint8_t agr_interrupt)
{
	// Set RTC alarme to wake up from Sleep Power Down Mode
	RTC.setAlarm1(time2wake,offset,mode);
	RTC.close();
	digitalWrite(SENS_SWITCH_1, LOW);
	digitalWrite(SENS_SWITCH_2, LOW);
	digitalWrite(SENS_SWITCH_4, LOW);
	digitalWrite(SENS_MUX_SEL, LOW);
	digitalWrite(SENS_DATA, LOW);
	digitalWrite(SENS_SWITCH_3, LOW);

	PWR.switchesOFF(option);


	if (agr_interrupt & SENS_AGR_PLUVIOMETER)
	{
		enableInterrupts(PLV_INT | HAI_INT);
	}


	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_mode();
	sleep_disable();
	PWR.switchesON(option);
}


// Private Methods //////////////////////////////////////////////////////////////

float WaspSensorAgr_v20::readDendrometer()
{
	const byte address_ADC = B0010110;
	const byte dendro_channel = B10100001;
	byte data_dendro[3] = {0,0,0};
	float value_dendro = 0;
	
	if( !Wire.I2C_ON ) Wire.begin();
	delay(100);
	delay(100);
	
	delay(100);
  
	Wire.beginTransmission(B0010110);
	Wire.send(dendro_channel);
	Wire.send(B01010101);
	Wire.endTransmission();

	delay(300);
  
	Wire.requestFrom(B0010110, 3);
  
	int i=0;
	while(Wire.available())
	{
		data_dendro[i]=Wire.receive();
		i++;
	}
	
	if( Wire.I2C_ON && !ACC.isON && RTC.isON!=1){
		PWR.closeI2C();
		RTC.setMode(RTC_OFF, RTC_I2C_MODE);
	}
  
	return value_dendro = conversion(data_dendro,0);
}

float WaspSensorAgr_v20::readAnemometer()
{
	int reading_anemometer = 0;
	int previous_reading_anemometer = 0;
	int value_anemometer = 0;
	float wind_speed = 0;
	unsigned long start_anemometer=0;

	value_anemometer = 0;
	start_anemometer = millis();
	while((millis()-start_anemometer)<=3000)
	{
		previous_reading_anemometer = reading_anemometer;
		reading_anemometer = digitalRead(DIGITAL2);
    
		if((previous_reading_anemometer == 1)&&(reading_anemometer == 0))
		{
			value_anemometer++;
		}
		if( millis()-start_anemometer < 0 ) start_anemometer=millis(); //avoid millis overflow problem
	}
	delay(100);
	
	wind_speed = value_anemometer * 0.8;
  
	return value_anemometer;

}

float WaspSensorAgr_v20::readPT1000()
{
	const byte address_ADC = B0010110;
	const byte pt1000_channel = B10100000;
	byte data_pt1000[3] = {0,0,0};
	float value_pt1000 = 0;
	
	if( !Wire.I2C_ON ) Wire.begin();
	delay(100);
	delay(100);
	
	delay(100);

	Wire.beginTransmission(B0010110);
	Wire.send(pt1000_channel);
	Wire.send(B01010101);
	Wire.endTransmission();
  
	delay(300);
  
	Wire.requestFrom(B0010110, 3);

	int k=0;
	while(Wire.available())
	{
		data_pt1000[k]=Wire.receive();
		k++;
	}

	if( Wire.I2C_ON && !ACC.isON && RTC.isON!=1){
		PWR.closeI2C();
		RTC.setMode(RTC_OFF, RTC_I2C_MODE);
	}
	return value_pt1000 = conversion(data_pt1000,1);  
}

float WaspSensorAgr_v20::readRadiation()
{
	const byte address = B0010100;
	byte data_apogee[2] = {0,0};
	long val = 0;
	float val_def = 0;
	
	if( !Wire.I2C_ON ) Wire.begin();
  
	delay(100);
  
	delay(50);
	
	Wire.requestFrom(B0010100, 2);
  
	int i = 0;
	while(Wire.available())
	{
		data_apogee[i] = Wire.receive();
		i++;
	}
  
// FIN DE LA LECTURA
  	
	if( Wire.I2C_ON && !ACC.isON && RTC.isON!=1){
		PWR.closeI2C();
		RTC.setMode(RTC_OFF, RTC_I2C_MODE);
	}
// CONVERSIÓN A VALOR DE TENSIÓN
  
	val = long(data_apogee[1]) + long(data_apogee[0])*256;
    
	val_def = (val - 32769)*9;
	return val_def = val_def/65535;
}

float WaspSensorAgr_v20::readWatermark(uint8_t sens)
{
	uint8_t pin=0;
	int value_watermark = 0;
	int previous_value_watermark = 0;
	int counter_watermark = 0;
	unsigned long start = 0;
	float freq = 0;
	
	pinMode(15, INPUT);
	pinMode(17, INPUT);
	pinMode(19, INPUT);
	
	digitalWrite(SENS_MUX_SEL,LOW);
	
	switch( sens )
	{
		case SENS_AGR_WATERMARK_1:	pin=15;
						break;
		case SENS_AGR_WATERMARK_2:	pin=19;
						break;
		case SENS_AGR_WATERMARK_3:	pin=17;
						break;
	}
	
	counter_watermark = 0;
	start = millis();
	while(counter_watermark < 2000)
	{
		previous_value_watermark = value_watermark;
		value_watermark = digitalRead(pin);
    
		if((previous_value_watermark ==1)&&(value_watermark == 0))
		{
			counter_watermark++;
		}
	}
  
	return freq = 2000000 / float((millis() - start));
}

float WaspSensorAgr_v20::readSensirion(uint8_t parameter)
{
	int ack = 0;
	int val_read = 0;
	int ack2, ack3, i = 0;
	long a = 0;
	long b = 0;
	
	const byte HUMIDITY = B00000101;
	const byte TEMPERATURE = B00000011;
	
	

	if( parameter==SENSIRION_TEMP ) parameter=TEMPERATURE;
	else if( parameter==SENSIRION_HUM ) parameter=HUMIDITY;
		
  //************************************* 
  // PRIMER CICLO DE TRANSMISIÓN (START)
  
	pinMode(SENS_DATA,  OUTPUT);
	pinMode(SENS_CLK, OUTPUT);
	digitalWrite(SENS_DATA,  HIGH);
	digitalWrite(SENS_CLK, HIGH);
	delayMicroseconds(1);
	digitalWrite(SENS_DATA,  LOW);
	digitalWrite(SENS_CLK, LOW);
	delayMicroseconds(1);
	digitalWrite(SENS_CLK, HIGH);
	digitalWrite(SENS_DATA,  HIGH);
	delayMicroseconds(1);
	digitalWrite(SENS_CLK, LOW);

  //***************************************
  // escribir el comando (los 3 primeros bits deben ser la direccion (siempre 000), y los ultimos 5 bits son el comando)

	shiftOut(SENS_DATA, SENS_CLK, MSBFIRST, parameter);  //parameter: B00000011 para temperatura y B00000101 para Humedad
	digitalWrite(SENS_CLK, HIGH);
	pinMode(SENS_DATA, INPUT);

	a = millis();

	ack = digitalRead(SENS_DATA);
	while((ack == HIGH)&&((millis()-a)<10))
	{
		ack = digitalRead(SENS_DATA);
		if (millis() < a) a = millis();	//to avoid millis overflow
	}

	b = millis()-a;
	
	
 
	digitalWrite(SENS_CLK, LOW);

	a = millis();
	ack = digitalRead(SENS_DATA);
	while((ack == LOW)&&((millis()-a)<10))
	{
		ack = digitalRead(SENS_DATA);
		if (millis() < a) a = millis();	//to avoid millis overflow
	}
	b = millis()-a;
  
  //****************************************
  //Espera a que la conversión esté completa
  
	ack = digitalRead(SENS_DATA);
	a = millis();
	while((ack == HIGH)&&((millis()-a)<400))
	{
		ack = digitalRead(SENS_DATA);
	}
	b= millis()-a;
 
  //*****************************************
  //Obtención de los 8 bits más significativos

	a = millis();
	for(int i = 0; i < 8; i++)
	{
		digitalWrite(SENS_CLK, HIGH);
		val_read = (val_read * 2) + digitalRead(SENS_DATA);
		digitalWrite(SENS_CLK, LOW);
	}
	b = millis()-a;
  
	ack = digitalRead(SENS_DATA);
	a = millis();
	while((ack == LOW)&&((millis()-a)<10))
	{
		ack = digitalRead(SENS_DATA);
		if (millis() < a) a = millis();	//to avoid millis overflow
	}
	b = millis()-a;
  
  //****************************************
  //ACK del micro
	pinMode(SENS_DATA, OUTPUT);
	digitalWrite(SENS_DATA, LOW);
	delayMicroseconds(1);
	digitalWrite(SENS_CLK, HIGH);
	delayMicroseconds(400);
	digitalWrite(SENS_CLK, LOW);
	pinMode(SENS_DATA, INPUT);
   
  //***************************************
  //Lectura de los 8 bits menos significativos
	a = millis();
	for(int i = 0; i < 8; i++)
	{
		digitalWrite(SENS_CLK, HIGH);
		val_read = val_read * 2 + digitalRead(SENS_DATA);
		digitalWrite(SENS_CLK, LOW);
	}
	b = millis()-a;

  
  //**************************************
  //Prescindimos del byte CRC  
  
	pinMode(SENS_DATA, OUTPUT);
	digitalWrite(SENS_DATA, HIGH);
	digitalWrite(SENS_CLK, HIGH);
	delayMicroseconds(400);
	digitalWrite(SENS_CLK, LOW);
	
	digitalWrite(SENS_DATA, LOW);
  
	if( parameter==TEMPERATURE ) return temperature_conversion(val_read,SENS_PREC_HIGH);
	else if( parameter==HUMIDITY ) return humidity_conversion(val_read,SENS_PREC_HIGH);
} 

float WaspSensorAgr_v20::temperature_conversion(int readValue, int precision)
{
	float temperature = 0;
	float d1 = -39.7;
	float d2 = 0;
	
	float aux=0;
  
	switch(precision)
	{
		case SENS_PREC_HIGH:    d2 = 0.01;
					temperature = d1 + (d2 * float(readValue));
					break;    
		case SENS_PREC_LOW:     d2 = 0.04;
					temperature = d1 + (d2 * float(readValue));
					break;
	}
  
	return(temperature);
}


float WaspSensorAgr_v20::humidity_conversion(int readValue, int precision)
{
	float humidity = 0;
	float c1 = -2.0468;
	float c2 = 0;
	float c3 = 0;
  
	switch(precision)
	{
		case SENS_PREC_HIGH:    c2 = 0.0367;
					c3 = -1.5955e-6;
					humidity = c1 + (c2 * float(readValue)) + (c3 * float(readValue) * float(readValue));
					break;

		case SENS_PREC_LOW:     c2 = 0.5872;
					c3 = -4.0845e-4;
					humidity = c1 + (c2 * float(readValue)) + (c3 * float(readValue) * float(readValue));
					break;

	}
  
	//in case of saturation
	if( humidity > 99.0 )
	{
		humidity = 100.0;
	}
	
	return(humidity);
}

uint16_t WaspSensorAgr_v20::readPluviometer()
{
	int reading_pluviometer = 0;
	int previous_reading_pluviometer = 0;
	int value_pluviometer = 0;
	unsigned long start_pluviometer=0;

	value_pluviometer = 0;
	start_pluviometer = millis();
	while((millis()-start_pluviometer)<=3000)
	{
		previous_reading_pluviometer = reading_pluviometer;
		reading_pluviometer = digitalRead(DIGITAL4);
    
		if((previous_reading_pluviometer == 1)&&(reading_pluviometer == 0))
		{
			value_pluviometer++;
		}
		if( millis()-start_pluviometer < 0 ) start_pluviometer=millis(); //avoid millis overflow problem
	}
	delay(100);
  
	return value_pluviometer;
}


float WaspSensorAgr_v20::conversion(byte data_input[3], uint8_t type)
{
	long val = 0;
	float val_def = 0;
	int signo = 0;
	int overflow = 0;
	float Vcc = 3.3;
	float R1           = 1005.0; 
	float R2           = 1005.0;
	float R3           = 1005.0;
	float Rx           = 0.0;
	float tempPt       = 0.0;
	float equis        = 0.0;
  
	signo = int(data_input[0] >> 7);
	overflow = int((data_input[0] >> 6)&B00000001);

	val = long((data_input[2] >> 6)&B00000011) + long(data_input[1])*4 + long((data_input[0])&B00111111)*1024;

  
	if (signo == 1)
	{
		if (overflow == 1)
		{
      //OVERFLOW High
			val_def = 2;
		}
		else
		{
			val_def = val*1.5;
			val_def = val_def/65535;
		}
	}
	else
	{
		if (overflow == 0)
		{
      //OVERFLOW LOW
			val_def = -2;
		}
		else
		{
			val_def = -(65535 - val)*1.5;
			val_def = val_def/65535;
		}
	}

	if( type==1 ){
		Rx = (Vcc*R2*R3 + val_def*R3*(R1+R2)) / (Vcc*R1 - val_def*(R1+R2));
		equis = (Rx - 1001.52) / 1351.99;
		Rx = equis * 1494.46 + 996.04;
		tempPt = (Rx-1000)/3.9083;
		return(tempPt);
	}
	else if( type==0) return ( (val_def * 11000)/1000.0 );
}



float WaspSensorAgr_v20::pressure_conversion(int readValue)
{
	float pressure = 0;   
	
	pressure = (float(readValue) * 3300.0 / 1023.0)/0.6; //sensor output (mV)  
	 
	pressure = (((pressure + 270) / 5000) + 0.095 ) / 0.0088; // kPa
   
	return(pressure);
}


float WaspSensorAgr_v20::lws_conversion(int readValue)
{
	float lws = 0;
   
	lws = float(readValue) * 3.3 / 1023;
   
	return(lws);
   
}
 
 
void WaspSensorAgr_v20::getVaneDirection(float vane)
{
	if( vane<0.25 ) vane_direction=SENS_AGR_VANE_ESE;
	else if( vane>=0.25 && vane<0.28 ) vane_direction=SENS_AGR_VANE_ENE;
	else if( vane>=0.28 && vane<0.35 ) vane_direction=SENS_AGR_VANE_E;
	else if( vane>=0.35 && vane<0.5 ) vane_direction=SENS_AGR_VANE_SSE;
	else if( vane>=0.5 && vane<0.65 ) vane_direction=SENS_AGR_VANE_SE;
	else if( vane>=0.65 && vane<0.85 ) vane_direction=SENS_AGR_VANE_SSW;
	else if( vane>=0.85 && vane<1.1 ) vane_direction=SENS_AGR_VANE_S;
	else if( vane>=1.1 && vane<1.38 ) vane_direction=SENS_AGR_VANE_NNE;
	else if( vane>=1.38 && vane<1.6 ) vane_direction=SENS_AGR_VANE_NE;
	else if( vane>=1.6 && vane<1.96 ) vane_direction=SENS_AGR_VANE_WSW;
	else if( vane>=1.96 && vane<2.15 ) vane_direction=SENS_AGR_VANE_SW;
	else if( vane>=2.15 && vane<2.35 ) vane_direction=SENS_AGR_VANE_NNW;
	else if( vane>=2.35 && vane<2.6 ) vane_direction=SENS_AGR_VANE_N;
	else if( vane>=2.6 && vane<2.8 ) vane_direction=SENS_AGR_VANE_WNW;
	else if( vane>=2.8 && vane<3.1 ) vane_direction=SENS_AGR_VANE_W;
	else if( vane>=3.1 ) vane_direction=SENS_AGR_VANE_NW;
}

float WaspSensorAgr_v20::sencera_conversion(int readValue)
{
	float humidity = 0.0;   
	
    humidity = (float(readValue) * 3300.0 / 1023.0)/0.6; //sensor output (mV)
	humidity = (humidity - 800) / 31; // %hum
   
	return(humidity);
   
}
 
 float WaspSensorAgr_v20::mcp_conversion(int readValue)
{
	float temperature = 0;
   
	temperature = float(readValue) * 3300 / 1023; //sensor output (mV)
   
	temperature = (temperature - 500) / 10; 
   
	return(temperature);
   
}

WaspSensorAgr_v20 SensorAgrV20=WaspSensorAgr_v20();
