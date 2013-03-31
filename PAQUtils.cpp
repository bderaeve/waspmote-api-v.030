
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

void PAQUtils::setPacketMask(uint16_t a_mask)
{
	#ifdef PACKET_DEBUG
		// ENABLING THIS LINE(S) LEADS TO TRANSMISSION ERROR = 2!!!!!?????? (ZIE FILMPJE)
		//USB.print("uint16_t packetMask: ");
		//USB.println((int) a_mask);
	#endif
	
	packetData[0] = mask/256;
	packetData[1] = mask%256;
	
	#ifdef PACKET_DEBUG
		//USB.print("packetMask!!!!!: ");
		//USB.println( (int) packetData[1]);
		//USB.println( (char * ) packetData );
	#endif
}

void PAQUtils::getPacketMask(packetXBee * paq)
{
	mask =( (unsigned int) paq->data[0]*256) + paq->data[1];
}


bool PAQUtils::maskMatchesNodesActiveSensorMask(uint16_t aMask)
{
	bool matches = 1;
	uint16_t indicator = 1;
	
	for(uint8_t i=0; i<xbeeZB.activeSensorMaskLength; i++)
	{
		if(  !(indicator & xbeeZB.activeSensorMask)  && (indicator & aMask) )
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

/*
//only to read in error messages!
void PAQUtils::readPacketData(packetXBee * paq)
{
	for(int i=0; i<paq->data_length; i++)
	{
		packetData[i] = paq->data[i];
	}
}
*/

bool PAQUtils::areSameMACAddresses(uint8_t * ad1, uint8_t * ad2)
{
	return ( ad1[4] == ad2[4] && ad1[5] == ad2[5] && ad1[6] == ad2[6] && ad1[7] == ad2[7] );
}



//uint8_t PacketUtils::setSensorData(uint16_t * mask)
uint8_t PAQUtils::sendMeasuredSensors(uint8_t * destination, uint16_t mask)
{
	uint8_t error = 2;
	uint8_t pos = 2;	// Positions 0 and 1 are reserved for the mask
	uint16_t indicator = 1;
	packetData = (char *) calloc(MAX_DATA, sizeof(char));
	
	setPacketMask(mask);	
	
	for(int i=0; i<16; i++) 
	{
		// i:   indicates which sensor is handled
		// pos: determines the position in the data array it will get.
		if(indicator & mask)
		{
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
		#ifdef PACKET_DEBUG
			USB.print("packetData in sendMeasuredSensors: ");
			USB.println(packetData);
			for(int j=0; j<10; j++)
				USB.println( (int) packetData[j]);
		#endif
		COMM.sendMessage(destination, IO_DATA, packetData);
		//COMM.sendMessage(destination, IO_DATA, "TEST MESSAGE 5");
	}

	free(packetData);
	packetData = NULL;
	
	//const char * mes = "TESTMESSAGE";
	//COMM.sendMessage(destination, 2, mes);
	//USB.println("NEGERS");
	
	return error;		
}

	

/*********************************************************************************************************
 *	STATIC FUNCTION POINTERS TO EASILY INSERT THE SENSOR DATA:
 ********************************************************************************************************/
	uint8_t InsertTemperature(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
		
		error = SensUtils.sensorValue2Chars(SensUtils.temperature, TEMPERATURE);
			
		#ifdef PAQ_DEBUG
			USB.print("in void InsertTemperature: *pos = "); USB.println( (int) *pos);
			USB.print("SensUtils.temp[0] = "); USB.println( (int) SensUtils.temp[0] );
			USB.print("SensUtils.temp[1] = "); USB.println( (int) SensUtils.temp[1] );
		#endif
		data[(*pos)++] = SensUtils.temp[0];
		data[(*pos)++] = SensUtils.temp[1];
		
		return error;
	}
	
	uint8_t InsertHumidity(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
		
		error = SensUtils.sensorValue2Chars(SensUtils.humidity, HUMIDITY);
		data[(*pos)++] = SensUtils.hum;
		
		return error;
	}
	
	uint8_t InsertPressure(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
		
		error = SensUtils.sensorValue2Chars(SensUtils.pressure, PRESSURE);
		
		data[(*pos)++] = SensUtils.pres[0];
		data[(*pos)++] = SensUtils.pres[1];	
		
		return error;
	}
	
	uint8_t InsertBattery(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
		
		error = SensUtils.sensorValue2Chars(SensUtils.battery, BATTERY);
		
		data[(*pos)++] = SensUtils.bat;
		
		return error;
	}
	
	uint8_t InsertCO2(uint8_t * pos, char * data)
	{
		uint8_t error = 2;
		
		error = SensUtils.sensorValue2Chars(SensUtils.co2, CO2);
		
		data[(*pos)++] = SensUtils.co_2[0];
		data[(*pos)++] = SensUtils.co_2[1];		
		
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
	uint8_t NotInUse(packetXBee * receivedPaq)  // APP_ID = 0
	{
		uint8_t error = 2;
		char aux[MAX_DATA];
		
		sprintf(aux, "Node ' %d ' received an invalid packet of type 0: DONT_USE", xbeeZB.nodeID);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, aux);
		
		return error;
	}
	
	//WORKS WITHOUT CAUSING ERROR_TX = 2
	uint8_t Add_Node_Request(packetXBee * receivedPaq)  // APP_ID = 1
	{
		uint8_t error = 2;
		uint16_t receivedPhysicalMask = 0;
		PackUtils.packetData = (char *) calloc(2, sizeof(char));
		
		// Save the origin address
		PackUtils.getPacketOriginAddress(receivedPaq);
	
		//Do the settings
		receivedPhysicalMask =( (unsigned int) receivedPaq->data[0]*256) + receivedPaq->data[1];
		xbeeZB.setPhysicalSensorMask(&receivedPhysicalMask);
		//TODO: SET NODE ID
		
		//Send answer back:
		PackUtils.setPacketMask(xbeeZB.physicalSensorMask);
		error = COMM.sendMessage(PackUtils.originAddress, ADD_NODE_RES, PackUtils.packetData);
		
		free(PackUtils.packetData);
		PackUtils.packetData = NULL;
		
		return error;
	}
	
	/*
	uint8_t Add_Node_Response(packetXBee * receivedPaq) // APP_ID = 2
	{
		uint8_t error = 2;
		char aux[MAX_DATA];
		
		sprintf(aux, "Node ' %d ' received an invalid packet of type 2: ADD_NODE_RES", xbeeZB.nodeID);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, aux);
		
		return error;
	}
	
	
	uint8_t Mask_Request(packetXBee * receivedPaq)  // APP_ID = 3
	{
		uint8_t error = 2;
		char aux[MAX_DATA];
		
		//PAQ.getPacketOriginAddress(receivedPaq);
		
		// Only gateway is allowed to request a nodes physical mask
		//if( PAQ.areSameMACAddresses(PAQ.originAddress, xbeeZB.GATEWAY_MAC ) )
		{
			//Send back mask
		//	PAQ.setPacketMask(xbeeZB.physicalSensorMask);
		//	error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, MASK_RES, PAQ.packetData);
		}
		//else
		{
			//Send error
			sprintf(aux, "Node ' %d ' received an unauthorized request for his physical sensor mask", xbeeZB.nodeID);
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, aux);	
		}
		return error;
	}
	uint8_t Mask_Response(packetXBee * receivedPaq)  // APP_ID = 4
	{
		uint8_t error = 2;
		char aux[MAX_DATA];
		
		sprintf(aux, "Node ' %d ' received an invalid packet of type 4: MASK_RES", xbeeZB.nodeID);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, aux);
		
		return error;
	}
	
	/*
	uint8_t Change_Node_Frequency_Request(packetXBee * receivedPaq) // APP_ID = 5
	{
		uint8_t error = 2;
		uint16_t receivedTime = 0;
		char aux[MAX_DATA];
		
		receivedTime = ( (unsigned int) receivedPaq->data[0]*256) + receivedPaq->data[1];
		if( !xbeeZB.setNewSleepTime( receivedTime ) )
		{
			//PAQ.packetData = PAQ.packetData = (char *) calloc(2, sizeof(char));
			//PAQ.packetData[0] = xbeeZB.defaultTime2Wake/256;
			//PAQ.packetData[1] = xbeeZB.defaultTime2Wake%256;
			
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, CH_NODE_FREQ_RES, PAQ.packetData); //send answer	
			
			free(PAQ.packetData);
			PAQ.packetData = NULL;
		}
		else
		{
			sprintf(aux, "Node ' %d ' received an invalid new default sleep time", xbeeZB.nodeID);
			error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, aux);	
		}
		return error;	
	}
	uint8_t Change_Node_Frequency_Response(packetXBee * receivedPaq) // APP_ID = 6
	{
		uint8_t error = 2;
		char aux[MAX_DATA];
		
		sprintf(aux, "Node ' %d ' received an invalid packet of type 6: CH_NODE_FREQ_RES", xbeeZB.nodeID);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, aux);
		
		return error;	
	}
	
	
	uint8_t Change_Sensor_Frequency_Request(packetXBee * receivedPaq) {}  // APP_ID = 7
	
	uint8_t Change_Sensor_Frequency_Response(packetXBee * receivedPaq) {}  // APP_ID = 8	
	
	
	

	
	
	uint8_t IO_Request(packetXBee * receivedPaq)
	{
		uint8_t error = 2;
		//PAQ.packetData = (char *) calloc(1, sizeof(char) * MAX_DATA);
		
		//PAQ.getPacketOriginAddress(receivedPaq);
	
		//PAQ.getPacketMask(receivedPaq);
		//if(!PAQ.maskMatchesNodesActiveSensorMask(PAQ.mask))
		{
			error = 1;
			#ifdef PAQ_DEBUG
				USB.println("Error occurred in uint8_t IO_Request(packetXBee * receivedPaq): The received mask has more bits set than the mask stored in the node!");
			#endif
			
			//SEND ERROR MESSAGE;
			COMM.sendMessage(xbeeZB.GATEWAY_MAC, "receivedMask has more bits set than the mask stored in the node");	
		}
		else
		{
			//MEASURE THE REQUESTED SENSORS
			if( ! SensUtils.measureSensors(PAQ.mask) )
			{
				
				if( ! PAQ.sendMeasuredSensors(PAQ.originAddress, PAQ.mask) )
				{
					error = 0;
				}
				else
				{
					error = 1;
					#ifdef PAQ_DEBUG
						USB.println("Error occurred in uint8_t IO_Request(packetXBee * receivedPaq) in PAQ.sendMeasuredSensors(PAQ.originAddress, PAQ.mask)");
					#endif
				}
			}
				/*
				if( ! PAQ.setSensorData(&PAQ.mask) )
				{
					PAQ.setPacketMask(PAQ.mask);
					
					//Now send PAQ.packetData to PAQ.originAddress
					if( ! COMM.sendMessage( PAQ.originAddress, IO_DATA, PAQ.packetData ) )			
					{
						error = 0;
					}
					else
					{
						error = 1;
						#ifdef PAQ_DEBUG
							USB.println("Error occurred in uint8_t IO_Request(packetXBee * receivedPaq) in COMM.sendMessage( (PAQ.originAddress), (PAQ.packetData) )");
						#endif
					}
				}
				else
				{
					error = 1;
					#ifdef PAQ_DEBUG
						USB.println("Error occurred in uint8_t IO_Request(packetXBee * receivedPaq) in PAQ.setSensorData(&PAQ.mask)");
					#endif
				}
				
			else
			{
				error = 1;
				#ifdef PAQ_DEBUG
					USB.println("Error occurred in uint8_t IO_Request(packetXBee * receivedPaq) in SensUtils.measureSensors(&PAQ.mask)");
				#endif
			}
					
		}
		
		//free(PAQ.packetData);
		//PAQ.packetData = NULL;
		
		return error;	
	}
	
	
	
	uint8_t IO_Data(packetXBee *)  // APP_ID = 10
	{
		uint8_t error = 2;
		char aux[MAX_DATA];
		
		sprintf(aux, "Node ' %d ' received an invalid packet of type 10: IO_DATA", xbeeZB.nodeID);
		
		error = COMM.sendMessage(xbeeZB.GATEWAY_MAC, aux);
		
		return error;			
	}
	
		//In normal operation a libelium node should never receive any error messages!
	//However if this occurs it will be notified to the gateway
	uint8_t Receive_Error(packetXBee * receivedPaq)
	{
		uint8_t error = 2;
		char aux[MAX_DATA];
		
		PAQ.packetData = (char *) calloc(1, sizeof(char) * MAX_DATA);
		
		PAQ.readPacketData(receivedPaq);
		sprintf(aux, "Node ' %d ' received the following error message: %s", xbeeZB.nodeID, PAQ.packetData);
		
		COMM.sendMessage(xbeeZB.GATEWAY_MAC, aux);
		
	}
	
	uint8_t Send_Error(packetXBee * receivedPaq){}
*/










void PAQUtils::testComm(uint8_t * destination, uint8_t type, const char * message)
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