
#ifndef __WPROGRAM_H__
	#include "BjornClasses.h"
	#include "WaspClasses.h"
#endif

#include <inttypes.h>


InsertData * Inserter[5] = {&InsertTemperature, &InsertHumidity, &InsertPressure,
				&InsertBattery, &InsertCO2};		//WORKS

TreatData * myTreatPacket[7] = {&ErrorMessage, &IO_Request, &IO_Data, &CF_Request,
				&CF_Response, &Add_Node_Request, &Add_Node_Response}; 
				

/*PacketUtils::PacketUtils() : inserter {&InsertTemperature, &InsertHumidity, &InsertPressure,
					&InsertBattery, &InsertCO2, &InsertAnemo, &InsertVane, &InsertPluvio}
			{}*/



//typedef void (PacketUtils::*my_pointer_to_function)(uint8_t *, char *); //= NULL;

/*
PacketUtils::PacketUtils()
{
	//pt2Member = &PacketUtils::InsertTemperature;  //WORKING
	
	//inserter[0] = &PacketUtils::InsertTemperature;  //WORKING for member funcs
	
	/*inserter = {&PacketUtils::InsertTemperature, &PacketUtils::InsertHumidity, &PacketUtils::InsertPressure,
					&PacketUtils::InsertBattery, &PacketUtils::InsertCO2, &PacketUtils::InsertAnemo, 
					&PacketUtils::InsertVane, &PacketUtils::InsertPluvio};  //NOT POSSIBLE FOR MEMBER FUNC
					
}
*/

/*
void PacketUtils::setPacketID(uint8_t id)
{
	packet[0] = id;
}

uint8_t PacketUtils::getPacketID()
{
	return packet[0];
}
*/



void PacketUtils::getPacketApplicationID(packetXBee * paq)
{
	applicationID = paq->packetID;
}

void PacketUtils::setPacketMask(uint16_t mask)
{
	char * str = "TEST";
	
	#ifdef PAQ_DEBUG
		USB.print("uint16_t packetMask: ");
		USB.println((int) mask);
		USB.println(str);
	#endif
	
	packetData[0] = mask/256;
	packetData[1] = mask%256;
	
	#ifdef PAQ_DEBUG
		USB.print("packetMask!!!!!: ");
		USB.println( (int) packetData[1]);
		USB.println( (char * ) packetData );
	#endif
}

void PacketUtils::getPacketMask(packetXBee * paq)
{
	mask =( (unsigned int) paq->data[0]*256) + paq->data[1];
}

bool PacketUtils::maskMatchesNodesActiveSensorMask(uint16_t aMask)
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


void PacketUtils::getPacketOriginAddress(packetXBee * paq)
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


void PacketUtils::printReceivedPacketData(packetXBee * paq)
{
	char receivedData[paq->data_length + 1];
	
	for(int i=0; i<paq->data_length; i++)
	{
		receivedData[i] = paq->data[i];
	}
	USB.print("PacketUtils::printReceivedPacketData(packetXBee *) :");
	USB.println(receivedData);
}

//only to read in error messages!
void PacketUtils::readPacketData(packetXBee * paq)
{
	for(int i=0; i<paq->data_length; i++)
	{
		packetData[i] = paq->data[i];
	}
}


void PacketUtils::testComm(uint8_t * destination, uint8_t type, const char * message)
{
		COMM.sendMessage(destination, type, message);

}

void PacketUtils::testComm2(uint8_t * destination, uint8_t type)
{
	COMM.sendMessage(destination, type, "TEST6");
	//USB.println("NEGERS");
}

void PacketUtils::testComm3(uint8_t * destination, uint8_t type, const char * message)
{
	const char * mes = "test7";
	
	COMM.sendMessage(destination, type, mes);

	USB.println(mes);
}

//uint8_t PacketUtils::setSensorData(uint16_t * mask)
uint8_t PacketUtils::sendMeasuredSensors(uint8_t * destination, uint16_t mask)
{
	uint8_t error = 2;
	//uint8_t pos = 2;	// Positions 0 and 1 are reserved for the mask
	//uint16_t indicator = 1;
	/*packetData = (char *) calloc(MAX_DATA, sizeof(char));
	
	
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
		#ifdef PAQ_DEBUG
			USB.println("MASK WAS EMPTY???");
		#endif
	}

	if(error == 0)
	{
		#ifdef PAQ_DEBUG
			USB.print("packetDataNew: ");
			for(int j=0; j<5; j++)
				USB.println( (int) packetData[j]);
		#endif
		//COMM.sendMessage(destination, IO_DATA, packetData);
		COMM.sendMessage(destination, IO_DATA, "TEST MESSAGE 5");
	}

	//free(packetData);
	//packetData = NULL;
	*/
	const char * mes = "TEST MESSAGE 5";
	COMM.sendMessage(destination, 2, mes);
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
	
	//In normal operation a libelium node should never receive any error messages!
	//However if this occurs it will be notified to the gateway
	uint8_t ErrorMessage(packetXBee * receivedPaq)
	{
		uint8_t error = 2;
		char aux[MAX_DATA];
		
		PAQ.packetData = (char *) calloc(1, sizeof(char) * MAX_DATA);
		
		PAQ.readPacketData(receivedPaq);
		sprintf(aux, "Node ' %d ' received the following error message: %s", xbeeZB.nodeID, PAQ.packetData);
		
		COMM.sendMessage(xbeeZB.GATEWAY_MAC, aux);
		
	}
	
	
	uint8_t IO_Request(packetXBee * receivedPaq)
	{
		uint8_t error = 2;
		//PAQ.packetData = (char *) calloc(1, sizeof(char) * MAX_DATA);
		
		PAQ.getPacketOriginAddress(receivedPaq);
	
		PAQ.getPacketMask(receivedPaq);
		if(!PAQ.maskMatchesNodesActiveSensorMask(PAQ.mask))
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
				*/
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
	
	
	
	uint8_t IO_Data(packetXBee *)
	{
		
	
	}
	uint8_t CF_Request(packetXBee *){}
	uint8_t CF_Response(packetXBee *){}
	
	
	uint8_t Add_Node_Request(packetXBee * receivedPaq)
	{
		uint8_t error = 2;
		uint16_t receivedPhysicalMask = 0;
		PAQ.packetData = (char *) calloc(2, sizeof(char));
		
		// Save the origin address
		PAQ.getPacketOriginAddress(receivedPaq);
	
		//Do the settings
		receivedPhysicalMask =( (unsigned int) receivedPaq->data[0]*256) + receivedPaq->data[1];
		xbeeZB.setPhysicalSensorMask(&receivedPhysicalMask);
		
		//Send answer back:
		PAQ.setPacketMask(xbeeZB.physicalSensorMask);
		COMM.sendMessage(PAQ.originAddress, ADD_NODE_RES, PAQ.packetData);
		
		free(PAQ.packetData);
		PAQ.packetData = NULL;
	}
	
	
	uint8_t Add_Node_Response(packetXBee *){}


PacketUtils PAQ = PacketUtils();
