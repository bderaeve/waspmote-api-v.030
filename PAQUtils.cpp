
#ifndef __WPROGRAM_H__
	#include "BjornClasses.h"
	#include "WaspClasses.h"
#endif


InsertData * Inserter[NUM_SENSORS] = {&InsertTemperature, &InsertHumidity, &InsertPressure,
				&InsertBattery, &InsertCO2, &InsertAnemo, &InsertVane, &InsertPluvio,
				&InsertLuminosity, &InsertSolarRadiation};		//WORKS

				
TreatData * myTreatPacket[NUM_APP_IDS] = {&NotInUse, &Add_Node_Request, &Add_Node_Response,
			&Mask_Request, &Mask_Response, &Change_Node_Frequency_Request,
			&Change_Node_Frequency_Response, &Change_Sensor_Frequency_Request,
			&Change_Sensor_Frequency_Response, &IO_Request, &IO_Data, 
			&Receive_Error,	&Send_Error, &Set_Power_Plan_Request, &Set_Power_plan_Response,
			&Set_Encryption_Request, &Set_Encryption_Response, &ResetPluvioMeter}; 
			
			
void PAQUtils::getPacketApplicationID(packetXBee * paq)
{
	applicationID = paq->packetID;
}


void PAQUtils::setPacketMask(uint16_t mask)
{
	packetData[0] = MSByte(mask);
	packetData[1] = LSByte(mask);
}


void PAQUtils::getPacketMask(packetXBee * paq)
{
	mask = ToMask(paq->data);
}


bool PAQUtils::maskMatchesNodesPhysicalSensorMask(uint16_t aMask)
{
	bool matches = 1;
	uint16_t indicator = 1;
	
	for(uint8_t i=0; i<xbeeZB.physicalSensorMaskLength; i++)
	{
		if(  !(indicator & xbeeZB.physicalSensorMask)  && (indicator & aMask) )
			matches = false;
		indicator <<= 1;
	}
	return matches;
}


void PAQUtils::getPacketOriginAddress(packetXBee * paq)
{
	uint8_t i = 0;
	while(i < 4)
	{
		originAddress[i] = paq->macSH[i++];
	}
	while(i < 8)
	{
		originAddress[i] = paq->macSL[i - 4];
		i++;
	}
}


bool PAQUtils::authorizeSender(packetXBee * receivedPaq)
{
	getPacketOriginAddress(receivedPaq);

	if( ! originAddress[7] == (xbeeZB.ACCEPTED_GATEWAY_BJORN[3] || xbeeZB.ACCEPTED_GATEWAY_ROEL[3]) )
		return false;
	if( ! originAddress[6] == (xbeeZB.ACCEPTED_GATEWAY_BJORN[2] || xbeeZB.ACCEPTED_GATEWAY_ROEL[2]) )
		return false;
	if( ! originAddress[5] == (xbeeZB.ACCEPTED_GATEWAY_BJORN[1] || xbeeZB.ACCEPTED_GATEWAY_ROEL[1]) )
		return false;
	if( ! originAddress[4] == (xbeeZB.ACCEPTED_GATEWAY_BJORN[0] || xbeeZB.ACCEPTED_GATEWAY_ROEL[0]) )
		return false;
	
	return true;
}	


bool PAQUtils::areSameMACAddresses(uint8_t * ad1, uint8_t * ad2)
{
	return ( ad1[4] == ad2[4] && ad1[5] == ad2[5] && ad1[6] == ad2[6] && ad1[7] == ad2[7] );
}


uint8_t PAQUtils::sendMask(uint8_t * destination, uint8_t type, uint16_t mask)
{
	uint8_t error = 2;
	packetSize = 2;
	
	setPacketMask(mask);
	
	char * content = (char *) calloc(packetSize*2 + 1, sizeof(char));
	escapeZerosInPacketData(content);
	
	error = COMM.sendMessage(destination, type, content);
	
	free(content);
	content = NULL;
	
	return error;
}


void PAQUtils::escapeZerosInPacketData(char * content)
{
	uint8_t pos = 0;
	for(uint8_t i=0; i<packetSize; i++)
	{
		if(packetData[i] == 0)
		{
			content[pos++] = 0xFF;	// FF = escape char
			content[pos++] = 0xFE;	// FE = 0
		}
		else if(packetData[i] == 0xFF)
		{
			content[pos++] = 0xFF;
			content[pos++] = 0xFD;
		}
		else
			content[pos++] = packetData[i];	
	}
	content[pos] = '\0';
}	


uint8_t PAQUtils::sendMeasuredSensors(uint8_t * destination, uint16_t mask)
{
	uint8_t error = 2;
	uint8_t pos = 2;	// Positions 0 and 1 are reserved for the mask
	packetSize = 2;  	// Need 2 bytes for the mask
	uint16_t indicator = 1;
	
	setPacketMask(mask);	
	
		#ifdef FINAL_USB_DEBUG
			USB.print("\nINSERTING: ");
		#endif

	for(int i=0; i<NUM_SENSORS; i++) 
	{
		// i:   indicates which sensor is handled
		// pos: determines the position in the data array it will get.
		if(indicator & mask)
		{
			if(xbeeZB.defaultOperation || RTCUt.RTCSecMinHourInt % SensUtils.measuringInterval[i] == 0)
			{
				error = (*Inserter[i])(&pos, packetData);	
				if(error != 0)
				{
					COMM.sendError(NODE_HAD_AN_ERROR_IN_SEND_MEASURED_SENSORS_IN_ONE_OF_THE_INSERTER_FUNPTRS);
					return 1;
				}
			}
		}
		indicator <<= 1;
	}
	
	if(error == 2)
	{
		COMM.sendError(NODE_HAD_AN_ERROR_IN_SEND_MEASURED_SENSORS_MASK_WAS_EMPTY);	
		return 3;
	}

	if(error == 0)
	{
		char * content = (char *) calloc(packetSize*2 + 1, sizeof(char));
		
		escapeZerosInPacketData(content);
		
		error = COMM.sendMessage(destination, IO_DATA, content);

		free(content);
		content = NULL;
		
		switch(error)
		{
			case 1 :	xbeeZB.storeError(SEND_MEASURED_SENSORS_RETURNED_1_NOT_SENT_DUE_TO_SLEEP);
						break;
			case 2 :	xbeeZB.storeError(SEND_MEASURED_SENSORS_RETURNED_2_NOT_SENT_OR_MESSAGE_LOST);
						break;
			default :	break;
		}
		
	}	
	return error;		
}


uint8_t PAQUtils::sendStoredErrors(uint8_t * destination)
{
	uint8_t error = 2;
	uint8_t nrStored = Utils.readEEPROM(NR_OF_STORED_ERRORS);
	
	if(nrStored <= 255) error = 0;
	else return 4;
	
	for(uint8_t i=1; i<nrStored; i++)
	{
		error = COMM.sendError( (Errors) Utils.readEEPROM(NR_OF_STORED_ERRORS + i ) );
	}
	
	if( error == 0 ) 
	{
		xbeeZB.number_of_stored_errors = 0;
		xbeeZB.storeValue(NR_OF_STORED_ERRORS, 0);
	}
	return error;
}


uint8_t PAQUtils::sendStoredSensors(uint8_t * destination)
{
	uint8_t error = 2;
	uint16_t nrSaved = 0;
	static bool mustReadEEPROM = true;
#ifdef POWER_MODES	
	for(int i=SEND_STORED_TEMPERATURES; i<SEND_STORED_RADIATIONS; i++)
	{
		if(SensUtils.savedPositions[i] != 0)
		{
			packetData[0] = SensUtils.savedPositions[i];
			for(int j=0; j<SensUtils.savedPositions[i]; j++)
			{
				packetData[j+1] = SensUtils.savedValues[i][j];
			}
			
			char * content = (char *) calloc(SensUtils.savedPositions[i]*2 + 1, sizeof(char));
			escapeZerosInPacketData(content);
			
			error = COMM.sendMessage(destination, i+NUM_APP_IDS, content);
					
			free(content);
			content = NULL;		
			
			if(error != 0)
				break;
				
			SensUtils.savedPositions[i] = 0;
		}
	}
	
	#ifndef WEATHER_STATION
	if(mustReadEEPROM)
		{
			nrSaved += Utils.readEEPROM(NR_OF_SAVED_TEMPERATURE_VALUES);
			nrSaved += Utils.readEEPROM(NR_OF_SAVED_HUMIDITY_VALUES);
			nrSaved += Utils.readEEPROM(NR_OF_SAVED_PRESSURE_VALUES);
			nrSaved += Utils.readEEPROM(NR_OF_SAVED_BATTERY_VALUES);
			nrSaved += Utils.readEEPROM(NR_OF_SAVED_CO2_VALUES);
			
			if(nrSaved > 0)
			{
				mustReadEEPROM = false;
				SensUtils.copyEEPROMValuesToMemory();
				sendStoredSensors(destination);
			}
		}
		else
		{
			mustReadEEPROM = true;
		}
	#endif
#endif	
	return error;
}


void PAQUtils::insertFrequenciesInPacketData(uint16_t mask)
{
	uint8_t pos = 2;	// Positions 0 and 1 are reserved for the mask
	packetSize = 2;  	// Need 2 bytes for the mask
	uint16_t indicator = 1;
	
	setPacketMask(mask);	
	
	for(int i=0; i<NUM_SENSORS; i++) 
	{
		// pos: determines the position in the data array it will get.
		if(indicator & mask)
		{
			packetData[pos++] = MSByte(SensUtils.measuringInterval[i]);
			packetData[pos++] = LSByte(SensUtils.measuringInterval[i]);
			packetSize += 2;
		}
		indicator <<= 1;
	}
}


/*********************************************************************************************************
 *	STATIC FUNCTION POINTERS TO EASILY INSERT THE SENSOR DATA:
 ********************************************************************************************************/
	uint8_t InsertTemperature(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
		
			#ifdef SENS_DEBUG_V2
				USB.print("T ");			
			#endif		
		
		error = SensUtils.sensorValue2Chars(SensUtils.temperature, TEMPERATURE);

		data[(*pos)++] = SensUtils.temp[0];
		data[(*pos)++] = SensUtils.temp[1];
		PackUtils.packetSize += 2;
		
		return error;
	}
	
	uint8_t InsertHumidity(uint8_t * pos, char * data)
	{
		uint8_t error = 2;

			#ifdef SENS_DEBUG_V2
				USB.print("H ");			
			#endif
			
		error = SensUtils.sensorValue2Chars(SensUtils.humidity, HUMIDITY);
		data[(*pos)++] = SensUtils.hum;
		PackUtils.packetSize++;
		
		return error;
	}
	
	uint8_t InsertPressure(uint8_t * pos, char * data)
	{
		uint8_t error = 2;

			#ifdef SENS_DEBUG_V2
				USB.print("P ");			
			#endif
			
		error = SensUtils.sensorValue2Chars(SensUtils.pressure, PRESSURE);
		
		data[(*pos)++] = SensUtils.pres[0];
		data[(*pos)++] = SensUtils.pres[1];	
		PackUtils.packetSize += 2;
		
		return error;
	}
	
	uint8_t InsertBattery(uint8_t * pos, char * data)
	{
		uint8_t error = 2;

			#ifdef SENS_DEBUG_V2
				USB.print("B ");			
			#endif
			
		error = SensUtils.sensorValue2Chars(SensUtils.battery, BATTERY);
		
		data[(*pos)++] = SensUtils.bat;
		PackUtils.packetSize++;
		
		return error;
	}
	
	uint8_t InsertCO2(uint8_t * pos, char * data)
	{
		uint8_t error = 2;

			#ifdef SENS_DEBUG_V2
				USB.print("CO2 ");			
			#endif
			
		error = SensUtils.sensorValue2Chars(SensUtils.co2, CO2);
		
		data[(*pos)++] = SensUtils.co_2[0];
		data[(*pos)++] = SensUtils.co_2[1];	
		PackUtils.packetSize += 2;
		
		return error;
	}
	
	
	uint8_t InsertAnemo(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
	
			#ifdef SENS_DEBUG_V2
				USB.print("ANEMO ");			
			#endif	
		
		error = SensUtils.sensorValue2Chars(SensUtils.anemo, ANEMO);	
		
		data[(*pos)++] = SensUtils.an;
		PackUtils.packetSize++;
		
		return error;		
	}
	
	uint8_t InsertVane(uint8_t * pos, char * data)
	{
		uint8_t error = 2;

			#ifdef SENS_DEBUG_V2
				USB.print("VANE ");			
			#endif
			
		data[(*pos)++] = (int) SensUtils.vaneDirection;
		PackUtils.packetSize++;
		
		return error = 0;	
	}
	
	
	uint8_t InsertPluvio(uint8_t * pos, char * data)
	{
		uint8_t error = 2;

			#ifdef SENS_DEBUG_V2
				USB.print("PLUVIO ");			
			#endif
			
		error = SensUtils.sensorValue2Chars(SensUtils.pluviometerCounter, PLUVIO);
		
		data[(*pos)++] = SensUtils.rain_count[0];
		data[(*pos)++] = SensUtils.rain_count[1];	
		PackUtils.packetSize += 2;
		
		#ifdef WEATHER_STATION
			SensUtils.resetPluviometer();
		#endif
		
		return error;		
	}
	
	uint8_t InsertLuminosity(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
		
			#ifdef SENS_DEBUG_V2
				USB.print("LUM ");			
			#endif
		
		error = SensUtils.sensorValue2Chars(SensUtils.luminosity, LUMINOSITY);
		
		data[(*pos)++] = SensUtils.lum;
		PackUtils.packetSize++;
		
		return error;
	}
	
	uint8_t InsertSolarRadiation(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
		
			#ifdef SENS_DEBUG_V2
				USB.print("RAD ");			
			#endif
			
		error = SensUtils.sensorValue2Chars(SensUtils.solar_radiation, SOLAR_RADIATION);
		
		data[(*pos)++] = SensUtils.radiation[0];
		data[(*pos)++] = SensUtils.radiation[1];	
		PackUtils.packetSize += 2;
		
		return error;	
	}

	
/*********************************************************************************************************
 *	STATIC FUNCTION POINTERS TO EASILY ANALYSE RECEIVED PACKAGES:
 ********************************************************************************************************/	
	uint8_t NotInUse(packetXBee * receivedPaq)  // APP_ID = 0 - Should never be received
	{
		COMM.sendError(NODE_RECEIVED_INVALID_PACKET_OF_TYPE_0_DONT_USE);
		return 1;
	}
	

	uint8_t Add_Node_Request(packetXBee * receivedPaq)  // APP_ID = 1
	{
		uint8_t error = 2;
			#ifdef ADD_NODE_REQ_DEBUG
				USB.println("ADD_NODE_REQ");
			#endif
			
		// 1. Check if sender is authorized 
			if( !PackUtils.authorizeSender(receivedPaq) )
			{
				COMM.sendError(NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_ADD_NODE_REQUEST);
				return 1;
			}
			else
			{
				#ifdef ADD_NODE_REQ_DEBUG
					USB.println("sender authorized");
				#endif		
			}	
		
		// 2. Set the physical mask
			error = xbeeZB.setPhysicalSensorMask( (uint8_t *) receivedPaq->data);
				#ifdef ADD_NODE_REQ_DEBUG
					USB.print("xbeeZB.phyMask = ");
					USB.println( (int) xbeeZB.physicalSensorMask );
				#endif
			if(error == 1)
			{
				COMM.sendError(NODE_RECEIVED_INVALID_MASK_IN_ADD_NODE_REQUEST);
				
				PackUtils.packetSize = 2;
				PackUtils.setPacketMask(xbeeZB.physicalSensorMask);
				char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
				PackUtils.escapeZerosInPacketData(contentToSend);
				
				error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, ADD_NODE_RES, contentToSend);				
				
				free(contentToSend);
				contentToSend = NULL;
				return 1;
			}
			else
			{
				#ifdef ADD_NODE_REQ_DEBUG
					USB.println("mask set ok");
				#endif		
			}				
		// 2. Set received mask as active sensor mask
			xbeeZB.setActiveSensorMask( (uint8_t *) receivedPaq->data );
				#ifdef ADD_NODE_REQ_DEBUG
					USB.print("xbeeZB.actMask = ");
					USB.print( (int) xbeeZB.activeSensorMask );
					USB.print("\n");
				#endif		
	
		// 3. Set Node Identifier  (not receiving this atm)
			//xbeeZB.setNodeIdentifier( itoa(receivedPaq->data[2], xbeeZB.nodeID, 10) );
		
		// 4. If succeeded an answer containing the mask will be sent back to the gateway
			PackUtils.packetSize = 2;
			PackUtils.setPacketMask(xbeeZB.physicalSensorMask);
			
			char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
			PackUtils.escapeZerosInPacketData(contentToSend);
			
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, ADD_NODE_RES, contentToSend);
				#ifdef ADD_NODE_REQ_DEBUG
					USB.print("\nNODE_RES send error = "); USB.println( (int) error );
				#endif
			free(contentToSend);
			contentToSend = NULL;
		
		// 5. Set in network = true
			xbeeZB.inNetwork = true;
		
		// 6. Measure and send the sensors of the received sensor mask
			error = SensUtils.measureSensors(xbeeZB.activeSensorMask);
			error = PackUtils.sendMeasuredSensors(xbeeZB.GATEWAY_MAC, xbeeZB.activeSensorMask);
		
		return error;
	}
	uint8_t Add_Node_Response(packetXBee * receivedPaq) // APP_ID = 2 - Should never be received
	{
		COMM.sendError(NODE_RECEIVED_INVALID_PACKET_OF_TYPE_2_ADD_NODE_RES);
		return 1;
	}
	
	
	uint8_t Mask_Request(packetXBee * receivedPaq)  // APP_ID = 3
	{
		uint8_t error = 2;
		
			#ifdef MASK_REQ_DEBUG
				USB.println("MASK_REQ");
			#endif
			
		// 1. Check if sender is authorized 
			if( !PackUtils.authorizeSender(receivedPaq) )
			{
				COMM.sendError(NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_MASK_REQUEST);
				return 1;
			}
			else
			{
				#ifdef MASK_REQ_DEBUG
					USB.println("sender authorized");
				#endif		
			}	
			
		// 2. Check if the node has an Ipsum ID: it checks if the node has been invited 
		//    to the network via an 'ADD_NODE_REQ' packet.
			if( !xbeeZB.inNetwork )
			{
				COMM.sendError(NODE_HAS_NOT_BEEN_INVITED_TO_THE_NETWORK_YET___USE_ADD_NODE_REQ);
				return 1;
			}
			
		// 3. If authorized an answer containing the mask will be sent back to the gateway
			PackUtils.packetSize = 2;
			PackUtils.setPacketMask(xbeeZB.physicalSensorMask);
			
			char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
			PackUtils.escapeZerosInPacketData(contentToSend);
			
				#ifdef MASK_REQ_DEBUG
					USB.print("contentToSend: ");
					USB.print( (int) contentToSend[0] );
					USB.print( (int) contentToSend[1] );
					USB.print( (int) contentToSend[2] );
				#endif
			
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, MASK_RES, contentToSend);
			
			free(contentToSend);
			contentToSend = NULL;		
			
			return error;
	}
	uint8_t Mask_Response(packetXBee * receivedPaq)  // APP_ID = 4 - Should never be received
	{
		COMM.sendError(NODE_RECEIVED_INVALID_PACKET_OF_TYPE_4_MASK_RES);
		return 1;		
	}
	
	
	uint8_t Change_Node_Frequency_Request(packetXBee * receivedPaq) // APP_ID = 5
	{
		uint8_t error = 2;
			#ifdef ADD_NODE_REQ_DEBUG
				USB.println("CH_NODE_FREQ_REQ");
			#endif	
			
		// 1. Check if sender is authorized 
			if( !PackUtils.authorizeSender(receivedPaq) )
			{
				COMM.sendError(NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_CH_NODE_FREQ_REQUEST);
				return 1;
			}
			else
			{
				#ifdef ADD_NODE_REQ_DEBUG_2
					USB.println("sender authorized");
				#endif		
			}	
			
		// 2. Check if the node has an Ipsum ID: it checks if the node has been invited 
		//    to the network via an 'ADD_NODE_REQ' packet.
			if( !xbeeZB.inNetwork )
			{
				COMM.sendError(NODE_HAS_NOT_BEEN_INVITED_TO_THE_NETWORK_YET___USE_ADD_NODE_REQ);
				return 1;
			}
			
		// 3. If authorized try to set the new frequency
		RTCUt.getTime();
		if( !xbeeZB.setNewDefaultTime2Sleep( ToMask(receivedPaq->data ) ) )
		{
			// 4A. If ok, send answer containing the new default frequency back
				PackUtils.packetSize = 2;  //needed in escapZeros function
				PackUtils.packetData[0] = MSByte(xbeeZB.defaultTime2WakeInt);
				PackUtils.packetData[1] = LSByte(xbeeZB.defaultTime2WakeInt);
				char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
				PackUtils.escapeZerosInPacketData(contentToSend);
				
				error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, CH_NODE_FREQ_RES, contentToSend); 	
				
				free(contentToSend);
				contentToSend = NULL;
				return 0;
		}
		else
		{
			// 4B. If invalid, send error 
				COMM.sendError(NODE_RECEIVED_INVALID_NEW_DEFAULT_SLEEP_TIME);
				return 1;
		}
		
	}
	uint8_t Change_Node_Frequency_Response(packetXBee * receivedPaq) // APP_ID = 6 - Should never be received
	{
		COMM.sendError(NODE_RECEIVED_INVALID_PACKET_OF_TYPE_6_CH_NODE_FREQ_RES);
		return 1;	
	}
	
	
	uint8_t Change_Sensor_Frequency_Request(packetXBee * receivedPaq)  // APP_ID = 7
	{
		uint8_t error = 2;
			#ifdef CH_SENS_FREQ_REQ_DEBUG
				USB.println("CH_SENS_FREQ_REQ");
			#endif	
			
		// 1. Check if sender is authorized 
			if( !PackUtils.authorizeSender(receivedPaq) )
			{
				COMM.sendError(NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_CH_SENS_FREQ_REQUEST);
				return 1;
			}
			else
			{
				#ifdef CH_SENS_FREQ_REQ_DEBUG
					USB.println("sender authorized");
				#endif		
			}	

		// 2. Check if the node has an Ipsum ID: it checks if the node has been invited 
		//    to the network via an 'ADD_NODE_REQ' packet.
			if( !xbeeZB.inNetwork )
			{
				COMM.sendError(NODE_HAS_NOT_BEEN_INVITED_TO_THE_NETWORK_YET___USE_ADD_NODE_REQ);
				return 1;
			}				
		
		//uint16_t receivedToChangeSensorsMask = ( (unsigned int) receivedPaq->data[0]*256) + receivedPaq->data[1];
		uint16_t receivedToChangeSensorsMask = ToMask(receivedPaq->data); //( (unsigned int) receivedPaq->data[0]*256) + receivedPaq->data[1];
		
			#ifdef CH_SENS_FREQ_REQ_DEBUG
				USB.print("to change mask: ");
				USB.println( (int) receivedToChangeSensorsMask);
			#endif
	
		// 3. SET INDIVIDUAL SENSOR TIMES AND NEW ACTIVE MASK
			error = xbeeZB.changeSensorFrequencies(receivedPaq->data);
			
			if(!error)
			{	
				PackUtils.insertFrequenciesInPacketData(receivedToChangeSensorsMask);
				
				char * contentToSend = (char *) calloc(PackUtils.packetSize*2 + 1, sizeof(char));
				PackUtils.escapeZerosInPacketData(contentToSend);
				
				error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, CH_SENS_FREQ_RES, contentToSend); 	
				
				free(contentToSend);
				contentToSend = NULL;
				return 0;	
			}
			else
			{
				// 3B. If error occured, send error 
				COMM.sendError(NODE_HAD_AN_ERROR_IN_XBEEZB_CHANGE_SENSOR_FREQUENCIES);	
				return 1;	
			}
	}
	uint8_t Change_Sensor_Frequency_Response(packetXBee * receivedPaq)  // APP_ID = 8 - Should never be received	
	{
		COMM.sendError(NODE_RECEIVED_INVALID_PACKET_OF_TYPE_8_CH_SENS_FREQ_RES);
		return 1;			
	}
	
	
	uint8_t IO_Request(packetXBee * receivedPaq)
	{
		uint8_t error = 2;
			
		// 1. Check if sender is authorized 
			if( !PackUtils.authorizeSender(receivedPaq) )
			{
				COMM.sendError(NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_IO_REQUEST);
				return 1;
			}
			else
			{
				#ifdef CH_SENS_FREQ_REQ_DEBUG
					USB.println("sender authorized");
				#endif		
			}		

		// 2. Check if the node has an Ipsum ID: it checks if the node has been invited 
		//    to the network via an 'ADD_NODE_REQ' packet.
			if( !xbeeZB.inNetwork )
			{
				COMM.sendError(NODE_HAS_NOT_BEEN_INVITED_TO_THE_NETWORK_YET___USE_ADD_NODE_REQ);
				return 1;
			}			
	
		PackUtils.getPacketMask(receivedPaq);
		if(!PackUtils.maskMatchesNodesPhysicalSensorMask(PackUtils.mask))
		{			
			// 3A. If mask is bigger than the physical mask, send error
			COMM.sendError(NODE_RECEIVED_A_MASK_THAT_DOESNT_MATCH_PHY_MASK_IN_IO_REQ);	
			return 1;
		}
		else
		{
			// 3B. If OK: MEASURE THE REQUESTED SENSORS AND SEND...
			if( ! SensUtils.measureSensors(PackUtils.mask) )
			{
				if( ! PackUtils.sendMeasuredSensors(PackUtils.originAddress, PackUtils.mask) )
				{
					return 0;
				}
				else
				{
					COMM.sendError(NODE_HAD_AN_ERROR_IN_PAQ_UTILS_IO_REQUEST_SENDING_SAMPLES);	
					return 1;
				}
			}
			else
			{
					COMM.sendError(NODE_HAD_AN_ERROR_IN_PAQ_UTILS_IO_REQUEST_MEASURE_SENSORS);	
					return 1;
			}		
		}
	}
	uint8_t IO_Data(packetXBee *)  // APP_ID = 10 - Should never be received
	{
		COMM.sendError(NODE_RECEIVED_INVALID_PACKET_OF_TYPE_10_IO_DATA);
		return 1;			
	}
	
	//In normal operation a libelium node should never receive any error messages!
	//However if this occurs the error message will be forwarded to the gateway
	uint8_t Receive_Error(packetXBee * receivedPaq)
	{
		// 1. Check if the node has an Ipsum ID: it checks if the node has been invited 
		//    to the network via an 'ADD_NODE_REQ' packet.
			if( !xbeeZB.inNetwork )
			{
				COMM.sendError(NODE_HAS_NOT_BEEN_INVITED_TO_THE_NETWORK_YET___USE_ADD_NODE_REQ);
				return 1;
			}

		// 2. Forward the received error to the gateway.
	
		uint8_t error = 2;
		char * content = (char *) calloc(MAX_DATA, sizeof(char));
		
		for(int i=0; i < receivedPaq->data_length; i++)
		{
			content[i] = receivedPaq->data[i];
		}
		
		memset(PackUtils.packetData, 0, MAX_DATA);
		sprintf(PackUtils.packetData, "Node ' %d ' received the following error message: %s", xbeeZB.nodeID, content);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, PackUtils.packetData);
		
		free(content);
		content = NULL;
	}
	
	uint8_t Send_Error(packetXBee * receivedPaq) // APP_ID = 12 - Should never be received
	{
		COMM.sendError(NODE_RECEIVED_INVALID_PACKET_OF_TYPE_12_SEND_ERROR);
		return 1;	
	}
	
	
	uint8_t Set_Power_Plan_Request(packetXBee * receivedPaq)
	{
		uint8_t error = 2;
		
		// 1. Check if sender is authorized 
		if( !PackUtils.authorizeSender(receivedPaq) )
		{
			COMM.sendError(NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_SET_POWER_PLAN_REQUEST);
			return 1;
		}	

		// 2. Check if the node has an Ipsum ID: it checks if the node has been invited 
		//    to the network via an 'ADD_NODE_REQ' packet.
		if( !xbeeZB.inNetwork )
		{
			COMM.sendError(NODE_HAS_NOT_BEEN_INVITED_TO_THE_NETWORK_YET___USE_ADD_NODE_REQ);
			return 1;
		}	
			
		// 3. Set the new power plan
		if( ! xbeeZB.setPowerPlan( (PowerPlan) receivedPaq->data[0] ) )
		{
			PackUtils.packetSize = 1;
			PackUtils.packetData[0] = receivedPaq->data[0];
			
			char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
			PackUtils.escapeZerosInPacketData(contentToSend);
			
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, SET_POWER_PLAN_RES, contentToSend);
				#ifdef ADD_NODE_REQ_DEBUG
					USB.print("\nNODE_RES send error = "); USB.println( (int) error );
				#endif
			free(contentToSend);
			contentToSend = NULL;	
		}
		else
		{
			error = COMM.sendError(INVALID_POWER_PLAN_RECEIVED_IN_SET_POWER_PLAN_REQ);
		}
			
		return error;	
	}
	
	uint8_t Set_Power_plan_Response(packetXBee * receivedPaq) // APP_ID = 14 - Should never be received
	{
		COMM.sendError(NODE_RECEIVED_INVALID_PACKET_OF_TYPE_14_SET_POWER_PLAN_RESPONSE);
		return 1;		
	}

	
	uint8_t Set_Encryption_Request(packetXBee * receivedPaq)
	{
		uint8_t error = 2;
		
		// 1. Check if sender is authorized 
		if( !PackUtils.authorizeSender(receivedPaq) )
		{
			COMM.sendError(NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_SET_ENCRYPTION_REQUEST);
			return 1;
		}	

		// 2. Check if the node has an Ipsum ID: it checks if the node has been invited 
		//    to the network via an 'ADD_NODE_REQ' packet.
		if( !xbeeZB.inNetwork )
		{
			COMM.sendError(NODE_HAS_NOT_BEEN_INVITED_TO_THE_NETWORK_YET___USE_ADD_NODE_REQ);
			return 1;
		}	
			
		// 3. Set the encryption mode
		if( ! xbeeZB.setEncryptionMode( (EncryptionMode) receivedPaq->data[0] ) )
		{
			PackUtils.packetSize = 1;
			PackUtils.packetData[0] = receivedPaq->data[0];
			
			char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
			PackUtils.escapeZerosInPacketData(contentToSend);
			
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, SET_ENCRYPTION_RES, contentToSend);

			free(contentToSend);
			contentToSend = NULL;	
		}
		else
		{
			error = COMM.sendError(INVALID_ENCRYPTION_MODE_RECEIVED_IN_SET_ENCRYPTION_REQ);
		}
			
		return error;	
	}
	
	uint8_t Set_Encryption_Response(packetXBee * receivedPaq) // APP_ID = 16 - Should never be received
	{
		COMM.sendError(NODE_RECEIVED_INVALID_PACKET_OF_TYPE_16_SET_ENCRYPTION_RESPONSE);
		return 1;		
	}


	uint8_t ResetPluvioMeter(packetXBee * receivedPaq) // APP_ID = 17
	{
		#ifdef WEATHER_STATION
			SensUtils.resetPluviometer();
			return 0;
		#endif
		
		return 1;
	}
	

PAQUtils PackUtils = PAQUtils();