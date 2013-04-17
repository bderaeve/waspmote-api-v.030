
#ifndef __WPROGRAM_H__
	#include "BjornClasses.h"
	#include "WaspClasses.h"
#endif

//#include <inttypes.h>

InsertData * Inserter[5] = {&InsertTemperature, &InsertHumidity, &InsertPressure,
				&InsertBattery, &InsertCO2};		//WORKS

TreatData * myTreatPacket[13] = {&NotInUse, &Add_Node_Request, &Add_Node_Response,
			&Mask_Request, &Mask_Response, &Change_Node_Frequency_Request,
			&Change_Node_Frequency_Response, &Change_Sensor_Frequency_Request,
			&Change_Sensor_Frequency_Response, &IO_Request, &IO_Data, 
			&Receive_Error,	&Send_Error}; 



void PAQUtils::testPrinting()
{
	USB.print("testPAQUtils\n");
	USB.println("testPAQUtilsss");
}



void PAQUtils::getPacketApplicationID(packetXBee * paq)
{
	applicationID = paq->packetID;
}

void PAQUtils::setPacketMask(uint16_t mask)
{
	#ifdef PACKET_DEBUG
		//USB.print("uint16_t packetMask: ");
		//USB.println((int) a_mask);
	#endif
	
	packetData[0] = mask/256;
	packetData[1] = mask%256;
	
	#ifdef PACKET_DEBUG
		//USB.print("packetMask!!!!!: ");
		//USB.println( (int) packetData[1]);
	#endif
}

void PAQUtils::getPacketMask(packetXBee * paq)
{
	mask =( (unsigned int) paq->data[0]*256) + paq->data[1];
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

/* 	CAUSES CHALLENGE TRANSMISSION ERROR = 2
void PAQUtils::printReceivedPacketData(packetXBee * paq)
{
	char receivedData[paq->data_length + 1];
	
	for(int i=0; i<paq->data_length; i++)
	{
		receivedData[i] = paq->data[i];
	}
	USB.print("PAQUtils::printReceivedPacketData(packetXBee *) :");
	USB.println(receivedData);
}*/




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



uint8_t PAQUtils::sendMeasuredSensors(uint8_t * destination, uint16_t mask)
{
	uint8_t error = 2;
	uint8_t pos = 2;	// Positions 0 and 1 are reserved for the mask
	packetSize = 2;  	// Need 2 bytes for the mask
	uint16_t indicator = 1;
	
	setPacketMask(mask);	
	
	for(int i=0; i<16; i++) 
	{
		// i:   indicates which sensor is handled
		// pos: determines the position in the data array it will get.
		if(indicator & mask)
		{
			//keeping track of packetSize so memset on packetData is not necessary here
			(*Inserter[i])(&pos, packetData);	
			error = 0;
		}
		indicator <<= 1;
	}
	
	if(error == 2)
	{
		error = 1;
		#ifdef PACKET_DEBUG
			USB.println("MASK WAS EMPTY???");
		#endif
	}

	if(error == 0)
	{
		char * content = (char *) calloc(packetSize*2 + 1, sizeof(char));

		escapeZerosInPacketData(content);
		
		#ifdef PACKET_DEBUG
			USB.print("strlen "); USB.print(strlen(content));
			USB.print("\n");
			
			for(int j=0; j<packetSize; j++)
				USB.println( (int) packetData[j]);
			for(int j=0; j<packetSize*2; j++)
				USB.println( (int) content[j]);
				
			USB.print("packetSize in sendMeasuredSensors: ");
			USB.println( (int) packetSize);
			USB.print("packetData in sendMeasuredSensors: ");
			USB.println(content);
		#endif
		
		error = COMM.sendMessage(destination, IO_DATA, content);
		//COMM.sendMessage(destination, IO_DATA, "TEST");
		
		free(content);
		content = NULL;
	}	
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
		else
			content[pos++] = packetData[i];	
	}
	content[pos] = '\0';
}	

/*********************************************************************************************************
 *	STATIC FUNCTION POINTERS TO EASILY INSERT THE SENSOR DATA:
 ********************************************************************************************************/
	uint8_t InsertTemperature(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
		
		error = SensUtils.sensorValue2Chars(SensUtils.temperature, TEMPERATURE);
			
		#ifdef PACKET_DEBUG
			USB.print("in void InsertTemperature: *pos = "); USB.println( (int) *pos);
			USB.print("SensUtils.temp[0] = "); USB.println( (int) SensUtils.temp[0] );
			USB.print("SensUtils.temp[1] = "); USB.println( (int) SensUtils.temp[1] );
		#endif
		data[(*pos)++] = SensUtils.temp[0];
		data[(*pos)++] = SensUtils.temp[1];
		PackUtils.packetSize += 2;
		
		return error;
	}
	
	uint8_t InsertHumidity(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
		
		error = SensUtils.sensorValue2Chars(SensUtils.humidity, HUMIDITY);
		data[(*pos)++] = SensUtils.hum;
		PackUtils.packetSize++;
		
		return error;
	}
	
	uint8_t InsertPressure(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
		
		error = SensUtils.sensorValue2Chars(SensUtils.pressure, PRESSURE);
		
		data[(*pos)++] = SensUtils.pres[0];
		data[(*pos)++] = SensUtils.pres[1];	
		PackUtils.packetSize += 2;
		
		return error;
	}
	
	uint8_t InsertBattery(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
		
		error = SensUtils.sensorValue2Chars(SensUtils.battery, BATTERY);
		
		data[(*pos)++] = SensUtils.bat;
		PackUtils.packetSize++;
		
		return error;
	}
	
	uint8_t InsertCO2(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
		
		error = SensUtils.sensorValue2Chars(SensUtils.co2, CO2);
		
		data[(*pos)++] = SensUtils.co_2[0];
		data[(*pos)++] = SensUtils.co_2[1];	
		PackUtils.packetSize += 2;
		
		return error;
	}
	
	/*
	uint8_t InsertAnemo(uint8_t * pos, char * data);
	uint8_t InsertVane(uint8_t * pos, char * data);
	uint8_t InsertPluvio(uint8_t * pos, char * data);
	*/

	
/*********************************************************************************************************
 *	STATIC FUNCTION POINTERS TO EASILY ANALYSE RECEIVED PACKAGES:
 ********************************************************************************************************/	
	//WORKS WITHOUT CAUSING ERROR_TX = 2
	uint8_t NotInUse(packetXBee * receivedPaq)  // APP_ID = 0 - Should never be received
	{
		uint8_t error = 2;
		PackUtils.packetSize = 1;
		PackUtils.packetData[0] = NODE_RECEIVED_INVALID_PACKET_OF_TYPE_0_DONT_USE;
		char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
		PackUtils.escapeZerosInPacketData(contentToSend);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, SEND_ERROR, contentToSend);
		free(contentToSend);
		contentToSend = NULL;
		return error;
	}
	

	uint8_t Add_Node_Request(packetXBee * receivedPaq)  // APP_ID = 1
	{
		uint8_t error = 2;
			#ifdef ADD_NODE_REQ_DEBUG
				USB.println("ADD_NODE_REQ");
			#endif
	
		// 1. Set the physical mask
		xbeeZB.setPhysicalSensorMask( (uint8_t *) receivedPaq->data);
			#ifdef ADD_NODE_REQ_DEBUG
				USB.print("xbeeZB.phyMask = ");
				USB.print( (int) xbeeZB.physicalSensorMask );
				USB.print("\n");
			#endif
		
		// 2. Set received mask as active sensor mask
		xbeeZB.setActiveSensorMask( (uint8_t *) receivedPaq->data );
			#ifdef ADD_NODE_REQ_DEBUG
				USB.print("xbeeZB.actMask = ");
				USB.print( (int) xbeeZB.activeSensorMask );
				USB.print("\n");
			#endif		
		
		// 3. Set Node Identifier  (not receiving this atm)
		//xbeeZB.setNodeIdentifier( itoa(receivedPaq->data[2], xbeeZB.nodeID, 10) );
		
		// 4. Send answer back:
		PackUtils.packetSize = 2;
		PackUtils.setPacketMask(xbeeZB.physicalSensorMask);
		
		char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
		PackUtils.escapeZerosInPacketData(contentToSend);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, ADD_NODE_RES, contentToSend);
		
		free(contentToSend);
		contentToSend = NULL;
		
		return error;
	}
	uint8_t Add_Node_Response(packetXBee * receivedPaq) // APP_ID = 2 - Should never be received
	{
		uint8_t error = 2;
		PackUtils.packetSize = 1;
		PackUtils.packetData[0] = NODE_RECEIVED_INVALID_PACKET_OF_TYPE_2_ADD_NODE_RES;
		char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
		PackUtils.escapeZerosInPacketData(contentToSend);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, SEND_ERROR, contentToSend);
		free(contentToSend);
		contentToSend = NULL;
		return error;
	}
	
	
	uint8_t Mask_Request(packetXBee * receivedPaq)  // APP_ID = 3
	{
		uint8_t error = 2;
		
			#ifdef MASK_REQ_DEBUG
				USB.println("MASK_REQ");
			#endif
		
		/* NOT POSSIBLE IF 0 IS USED TO ADDRESS THE GATEWAY 
		PackUtils.getPacketOriginAddress(receivedPaq);
		
		// Only gateway is allowed to request a nodes physical mask
		if( PackUtils.areSameMACAddresses(PackUtils.originAddress, xbeeZB.GATEWAY_MAC ) )
		{
			//Send back mask
			PackUtils.setPacketMask(xbeeZB.physicalSensorMask);
			error = PackUtils.sendMask(PackUtils.originAddress, MASK_RES, xbeeZB.physicalSensorMask);
		}
		else
		{
			//Send error
			memset(PackUtils.packetData, 0, MAX_DATA);
			sprintf(PackUtils.packetData, "Node ' %d ' received an unauthorized request for his physical sensor mask", xbeeZB.nodeID);
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, PackUtils.packetData);	
		}
		*/
		PackUtils.packetSize = 2;
		PackUtils.setPacketMask(xbeeZB.physicalSensorMask);
		
		char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
		PackUtils.escapeZerosInPacketData(contentToSend);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, MASK_RES, contentToSend);
		
		free(contentToSend);
		contentToSend = NULL;		
		
		return error;
	}
	uint8_t Mask_Response(packetXBee * receivedPaq)  // APP_ID = 4 - Should never be received
	{
		uint8_t error = 2;
		PackUtils.packetSize = 1;
		PackUtils.packetData[0] = NODE_RECEIVED_INVALID_PACKET_OF_TYPE_4_MASK_RES;
		char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
		PackUtils.escapeZerosInPacketData(contentToSend);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, SEND_ERROR, contentToSend);
		free(contentToSend);
		contentToSend = NULL;
		return error;
	}
	
	
	uint8_t Change_Node_Frequency_Request(packetXBee * receivedPaq) // APP_ID = 5
	{
		uint8_t error = 2;
		
		if( !xbeeZB.setNewDefaultTime2Sleep( toUint16_t(receivedPaq->data ) ) )
		{
			//send answer
			PackUtils.packetData[0] = xbeeZB.defaultTime2WakeInt/256;
			PackUtils.packetData[1] = xbeeZB.defaultTime2WakeInt%256;
			PackUtils.packetSize = 2;  //needed in escapZeros function
			char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
			PackUtils.escapeZerosInPacketData(contentToSend);
			
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, CH_NODE_FREQ_RES, contentToSend); 	
			
			free(contentToSend);
			contentToSend = NULL;
		}
		else
		{
			//send error
			PackUtils.packetSize = 1;
			PackUtils.packetData[0] = NODE_RECEIVED_INVALID_NEW_DEFAULT_SLEEP_TIME;
			char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
			PackUtils.escapeZerosInPacketData(contentToSend);
			
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, SEND_ERROR, contentToSend);
			
			free(contentToSend);
			contentToSend = NULL;
		}
		return error;	
	}
	uint8_t Change_Node_Frequency_Response(packetXBee * receivedPaq) // APP_ID = 6 - Should never be received
	{
		uint8_t error = 2;
		PackUtils.packetSize = 1;
		PackUtils.packetData[0] = NODE_RECEIVED_INVALID_PACKET_OF_TYPE_6_CH_NODE_FREQ_RES;
		char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
		PackUtils.escapeZerosInPacketData(contentToSend);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, SEND_ERROR, contentToSend);
		free(contentToSend);
		contentToSend = NULL;
		return error;
	}
	
	
	uint8_t Change_Sensor_Frequency_Request(packetXBee * receivedPaq)  // APP_ID = 7
	{
		uint8_t error = 2;
		uint16_t receivedToChangeSensorsMask = ( (unsigned int) receivedPaq->data[0]*256) + receivedPaq->data[1];
		
		// Save the origin address
		PackUtils.getPacketOriginAddress(receivedPaq);
	
		// SET INDIVIDUAL SENSOR TIMES AND NEW ACTIVE MASK
		error = xbeeZB.changeSensorFrequencies(receivedPaq->data);
		
		if(error)
		{	
			memset(PackUtils.packetData, 0, MAX_DATA);
			sprintf(PackUtils.packetData, "Node ' %s ' had an error in xbeeZB::changeSensorFrequencies ");
		}
		else
		{
			PackUtils.sendMask(PackUtils.originAddress, CH_SENS_FREQ_RES, receivedToChangeSensorsMask);
		}
	}
	uint8_t Change_Sensor_Frequency_Response(packetXBee * receivedPaq)  // APP_ID = 8 - Should never be received	
	{
		uint8_t error = 2;
		PackUtils.packetSize = 1;
		PackUtils.packetData[0] = NODE_RECEIVED_INVALID_PACKET_OF_TYPE_8_CH_SENS_FREQ_RES;
		char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
		PackUtils.escapeZerosInPacketData(contentToSend);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, SEND_ERROR, contentToSend);
		free(contentToSend);
		contentToSend = NULL;
		return error;
	}
	
	
	uint8_t IO_Request(packetXBee * receivedPaq)
	{
		uint8_t error = 2;
				
		PackUtils.getPacketOriginAddress(receivedPaq);
	
		PackUtils.getPacketMask(receivedPaq);
		if(!PackUtils.maskMatchesNodesPhysicalSensorMask(PackUtils.mask))
		{
			error = 1;
			#ifdef PACKET_DEBUG
				USB.println("Error occurred in uint8_t IO_Request(packetXBee * receivedPaq): The received mask has more bits set than the mask stored in the node!");
			#endif
			
			//SEND ERROR MESSAGE;
			COMM.sendMessage(xbeeZB.GATEWAY_MAC, "receivedMask has more bits set than the mask stored in the node");	
		}
		else
		{
			//MEASURE THE REQUESTED SENSORS AND SEND
			if( ! SensUtils.measureSensors(PackUtils.mask) )
			{
				if( ! PackUtils.sendMeasuredSensors(PackUtils.originAddress, PackUtils.mask) )
				{
					error = 0;
				}
				else
				{
					#ifdef PACKET_DEBUG
						USB.println("Error occurred in uint8_t IO_Request(packetXBee * receivedPaq) in PAQ.sendMeasuredSensors(PAQ.originAddress, PAQ.mask)");
					#endif
					memset(PackUtils.packetData, 0, MAX_DATA);
					sprintf(PackUtils.packetData, "Node ' %d ' could SEND the requested sensors", xbeeZB.nodeID);
		
					error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, PackUtils.packetData);	
				}
			}
			else
			{
				#ifdef PACKET_DEBUG
					USB.println("Error occurred in uint8_t IO_Request(packetXBee * receivedPaq) in SensUtils.measureSensors(&PAQ.mask)");
				#endif
				memset(PackUtils.packetData, 0, MAX_DATA);
				sprintf(PackUtils.packetData, "Node ' %d ' could not MEASURE the requested sensors", xbeeZB.nodeID);
		
				error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, PackUtils.packetData);
			}
					
		}
		return error;	
	}
	uint8_t IO_Data(packetXBee *)  // APP_ID = 10 - Should never be received
	{
		uint8_t error = 2;
		PackUtils.packetSize = 1;
		PackUtils.packetData[0] = NODE_RECEIVED_INVALID_PACKET_OF_TYPE_10_IO_DATA;
		char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
		PackUtils.escapeZerosInPacketData(contentToSend);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, SEND_ERROR, contentToSend);
		free(contentToSend);
		contentToSend = NULL;
		return error;		
	}
	
	//In normal operation a libelium node should never receive any error messages!
	//However if this occurs the error message will be notified to the gateway
	uint8_t Receive_Error(packetXBee * receivedPaq)
	{
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
		uint8_t error = 2;
		PackUtils.packetSize = 1;
		PackUtils.packetData[0] = NODE_RECEIVED_INVALID_PACKET_OF_TYPE_12_SEND_ERROR;
		char * contentToSend = (char *) calloc(PackUtils.packetSize + 1, sizeof(char));
		PackUtils.escapeZerosInPacketData(contentToSend);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, SEND_ERROR, contentToSend);
		free(contentToSend);
		contentToSend = NULL;
		return error;		
	}











void PAQUtils::testComm(uint8_t * destination, uint8_t type, char * message)
{
		COMM.sendMessage(destination, type, message);

}

//THIS FUNCTION CAUSES TRANSMISSION ERROR = 2
/*
void PAQUtils::testComm2(uint8_t * destination, uint8_t type)
{
	COMM.sendMessage(destination, type, "TEST6");
	USB.println("PRINT SOMETHING");
}
*/
/*
void PAQUtils::testComm3(uint8_t * destination, uint8_t type, const char * message)
{
	const char * mes = "test7";
	
	COMM.sendMessage(destination, type, message);

	USB.println(mes);
}

void PAQUtils::testComm4(const char * mess, const char * desti)
{
	COMM.sendMessageLocalWorking(mess, desti);
}

void PAQUtils::testComm5(const char * mess, uint8_t type, const char * desti)
{
	COMM.sendMessageLocalWorkingWithType(mess, type, desti);
	//USB.print("TEST");
}

void PAQUtils::testComm6()
{
	char * to = "0013A2004069737A";
	COMM.sendMessageLocalWorking("TEST MESSAGE", to);
}

void PAQUtils::testComm7()
{
	//const char * adres = "0013A2004069737A";
	//COMM.sendMessageLocalWorking("TEST MESSAGE", adres);
	//USB.print("test");
}
*/

PAQUtils PackUtils = PAQUtils();