/* ==========================================================================
 *
 *			THESIS: Design of a Wireless Sensor Networking test-bed
 * 
 * ==========================================================================
 *
 *       Filename:  CommUtils.cpp
 *    Description:  Extra functionality for setting up the ZigBee network
 *					and sending packets.
 *
 * ======================================================================= */
 
#ifndef __WPROGRAM_H__
	#include "BjornClasses.h"
	#include "WaspClasses.h"
#endif

#include <inttypes.h>

CommUtils::CommUtils()
{
	retryJoin = false;
}

uint8_t CommUtils::setupXBee()
{
	uint8_t error = 2;
	uint8_t panID[8] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0B };
	
	///////////////////////////////
	// A. XBEE
	///////////////////////////////
	
	xbeeZB.init(ZIGBEE,FREQ2_4G,NORMAL);
	xbeeZB.ON();
	xbeeZB.wake();  // Must do this for end devices to work
	delay(1000);

	// 1.1 Set PANID: 0x0000000000001302 
		if(!xbeeZB.setPAN( panID ) )
		{
			error = 0;
			#ifdef COMM_DEBUG
				USB.println("setPAN ok");
			#endif			
		}
		else 
		{
			error = 1;
			#ifdef COMM_DEBUG
				USB.println("setPAN error");
			#endif 
			
			USB.print("\n\n\nXBEE NOT FOUND\n\n");
			
			while(1)		
			{
				Utils.setLED(LED0, LED_ON);
				delay(150);
				Utils.setLED(LED0, LED_OFF);
				delay(150);
			}
		}
	// 1.2 set all possible channels to scan, channels from 0x0B to 0x18 (0x19 and 0x1A are excluded)
		if(!xbeeZB.setScanningChannels(0x3F,0xFF))
		{
			error = 0;
			#ifdef COMM_DEBUG
				USB.println("setScanningChannels ok");
			#endif
		}
		else 
		{
			error = 1;
			#ifdef COMM_DEBUG
				USB.println("setScanningChannels error");
			#endif
		}
	// 1.3 It sets the time the Energy Scan will be performed
		if(!xbeeZB.setDurationEnergyChannels(3)) 
		{
			error = 0;
			#ifdef COMM_DEBUG
				USB.println("setDurationEnergyChannels ok");
			#endif
		}
		else
		{
			error = 1;
			#ifdef COMM_DEBUG
				USB.println("setDurationEnergyChannels error");
			#endif
		}
	// 1.4 Set channel verification JV=1 in order to make the XBee module to scan new coordinator
		if(!xbeeZB.setChannelVerification(1))
		{
			error = 0;
			#ifdef COMM_DEBUG
				USB.println("setChannelVerification ok");
			#endif
		}
		else
		{
			error = 1;
			#ifdef COMM_DEBUG
				USB.println("setChannelVerification error");
			#endif
		}
	
	// 1.5 Write values to XBEE memory	
		xbeeZB.writeValues();	
	
	// 1.6 Reboot the XBee module 
		xbeeZB.OFF(); 
		delay(3000); 
		xbeeZB.ON(); 
		xbeeZB.wake();  // For end devices: SM = 1!
		//delay(3000); 
	
	
	// 2. WAIT for association:
		if( !checkNodeAssociation(SETUP) )
		{
			error = 0;
			#ifdef ASSOCIATION_DEBUG
				USB.println("checkNodeAssociation() ok");
			#endif
		}
		else
		{
			error = 1;
			#ifdef ASSOCIATION_DEBUG
				USB.println("ERROR CHECKING NODE ASSOCIATION");  
			#endif
		}
		
	///////////////////////////////
	// B. RTC
	///////////////////////////////

	// Powers RTC up, init I2C bus and read initial values
	RTC.ON();	

	// Saving battery (!!!in combination with Hibernate, RTC may only be used to set
	// the wake! Other usage can cause internal collisions!!!)
	RTC.setMode(RTC_OFF,RTC_NORMAL_MODE); 
	
	if(!retryJoin)
	{
		RTCUt.setTime("13:04:04:05:00:00:00");
		RTCUt.setNextTimeWhenToWakeUpViaOffset( xbeeZB.defaultTime2WakeInt );
	}

	//////////////////////////////////////////////////
	// C. ENABLE WEATHER STATION (for rain interrupt)
	//////////////////////////////////////////////////	
	#ifdef WEATHER_STATION
		SensorAgrV20.setBoardMode(SENS_ON);
		delay(100);
	#endif	
	
	//////////////////////////////////////////////////
	// D. DISMISS ANY SAVED ERRORS
	//////////////////////////////////////////////////		
	xbeeZB.storeValue(NR_OF_STORED_ERRORS, 0);
	
	
	if(error != 0) 
	{
		xbeeZB.storeError(ERROR_SETTING_UP_XBEE_MODULE);
	}
		
	return error;
}


uint8_t CommUtils::setupXBee(uint8_t pan[8], DeviceRole role, uint8_t gateway[8], SleepMode sm, 
						char * nodeID, uint16_t defaultSleepingTime, PowerPlan pp)
{
	uint8_t error = 2;

	///////////////////////////////
	// A. XBEE
	///////////////////////////////
  
	xbeeZB.init(ZIGBEE,FREQ2_4G,NORMAL);
	xbeeZB.ON();
	xbeeZB.wake();  // Must do this for end devices to work
	delay(1000);

	// 1.1 Set PANID: 
		if(!xbeeZB.setPAN(pan))
		{
			error = 0;
			#ifdef COMM_DEBUG
				USB.println("setPAN ok");
			#endif
			
		}
		else 
		{
			error = 1;
			#ifdef COMM_DEBUG
				USB.println("setPAN error");
			#endif 
			
			long previous = millis();
			USB.print("\n\n\nXBEE NOT FOUND\n\n");
			
			#ifdef FINAL_USB_DEBUG
				while((millis()-previous) < 15000)			
				{
					Utils.setLED(LED0, LED_ON);   // If failed, blink red LED fast
					delay(150);
					Utils.setLED(LED0, LED_OFF);
					delay(150);
				}
			#endif

			setupXBee(pan, role, gateway, sm, nodeID, defaultSleepingTime, pp);
		}
	// 1.2 set all possible channels to scan, channels from 0x0B to 0x18 (0x19 and 0x1A are excluded)
		if(!xbeeZB.setScanningChannels(0x3F,0xFF))
		{
			error = 0;
			#ifdef COMM_DEBUG
				USB.println("setScanningChannels ok");
			#endif
		}
		else 
		{
			error = 1;
			#ifdef COMM_DEBUG
				USB.println("setScanningChannels error");
			#endif
		}
	// 1.3 It sets the time the Energy Scan will be performed
		if(!xbeeZB.setDurationEnergyChannels(3)) 
		{
			error = 0;
			#ifdef COMM_DEBUG
				USB.println("setDurationEnergyChannels ok");
			#endif
		}
		else
		{
			error = 1;
			#ifdef COMM_DEBUG
				USB.println("setDurationEnergyChannels error");
			#endif
		}
	// 1.4 Set channel verification JV=1 in order to make the XBee module to scan new coordinator
		if(!xbeeZB.setChannelVerification(1))
		{
			error = 0;
			#ifdef COMM_DEBUG
				USB.println("setChannelVerification ok");
			#endif
		}
		else
		{
			error = 1;
			#ifdef COMM_DEBUG
				USB.println("setChannelVerification error");
			#endif
		}
	
	// 1.5 Set node_ID
		xbeeZB.setNodeIdentifier(nodeID);
		
	// 1.6 Set deviceType
		xbeeZB.setDeviceRole(role);
		
	// 1.7 Set PowerPlan
		xbeeZB.setPowerPlan(pp);
	
	// 1.8 Write values to XBEE memory	
		xbeeZB.writeValues();	
	
	// 1.9 Reboot the XBee module 
		xbeeZB.OFF(); 
		delay(3000); 
		xbeeZB.ON(); 
		xbeeZB.wake();  // For end devices: SM = 1!

		
	// 2. WAIT for association:
		if( !checkNodeAssociation(SETUP) )
		{
			error = 0;
			#ifdef ASSOCIATION_DEBUG
				USB.println("checkNodeAssociation() ok");
			#endif
		}
		else
		{
			error = 1;
			#ifdef ASSOCIATION_DEBUG
				USB.println("ERROR CHECKING NODE ASSOCIATION");  
			#endif
		}
		
	///////////////////////////////
	// B. RTC
	///////////////////////////////
	
	// Powers RTC up, init I2C bus and read initial values
	RTC.ON();	
	
	// Saving battery (!!!in combination with Hibernate, RTC may only be used to set
	// the wake! Other usage can cause internal collisions!!!)
	RTC.setMode(RTC_OFF,RTC_NORMAL_MODE); 
	
	if(!retryJoin)
	{
		RTCUt.setTime("13:04:04:05:00:00:00");
		RTCUt.getTime();
	}	
	

	///////////////////////////////
	// C. STORE DEFAULT GATEWAY
	///////////////////////////////
	xbeeZB.setGatewayMacAddress(gateway);

	///////////////////////////////
	// D. STORE SLEEP MODE
	///////////////////////////////  
    if(role == END_DEVICE)
	{
		xbeeZB.sleepMode = sm;
		xbeeZB.defaultTime2WakeInt = defaultSleepingTime;
		
		RTCUt.setNextTimeWhenToWakeUpViaOffset( defaultSleepingTime );
	}
	else
	{
		xbeeZB.setAlarmForRouter();
	}

	//////////////////////////////////////////////////
	// E. ENABLE WEATHER STATION (for rain interrupt)
	//////////////////////////////////////////////////	
	#ifdef WEATHER_STATION
		SensorAgrV20.setBoardMode(SENS_ON);
		delay(100);
	#endif
	
	//////////////////////////////////////////////////
	// F. DISMISS ANY SAVED ERRORS
	//////////////////////////////////////////////////		
	xbeeZB.storeValue(NR_OF_STORED_ERRORS, 0);	
	
	if(error != 0) 
	{
		xbeeZB.storeError(ERROR_SETTING_UP_XBEE_MODULE);
	}
		
    return error;
}


uint8_t CommUtils::retryJoining()
{
	uint8_t error = 0;
	retryJoin = true;
	
	xbeeZB.getNodeIdentifier();
	xbeeZB.getPAN();
	
	error = setupXBee(xbeeZB.PAN_ID, (DeviceRole) xbeeZB.deviceRole, xbeeZB.GATEWAY_MAC,
				      (SleepMode) xbeeZB.sleepMode, xbeeZB.nodeID, xbeeZB.defaultTime2WakeInt,
					  xbeeZB.powerPlan);
	
	if( ! error )
	{
		xbeeZB.inNetwork = true;
		COMM.retryJoin = false;
		COMM.sendError(NODE_HAD_TO_RETRY_THE_JOINING_PROCESS__PROBABLY_LOW_RSSI);	
	}
	else
	{
		xbeeZB.storeError(FATAL_ERROR_NODE_FAILED_THE_RETRY_THE_JOINING_PROCESS_PARENT_UNREACHABLE);
	}
	
	return error;
}


uint8_t CommUtils::checkNodeAssociation(AssociationMode mode)
{
	uint8_t error = 2;
	
		#ifdef FINAL_USB_DEBUG
			USB.print("\n\nCHECK_ASSOCIATION: ");
		#endif
	
	long previous = millis();
	
	 xbeeZB.getAssociationIndication();
	//if(xbeeZB.getAssociationIndication()){
		while( xbeeZB.associationIndication != 0 && (millis()-previous) < 15000)
		{
			#ifdef ASSOCIATION_DEBUG
				USB.println("\n\n-----> not associated <----------");
				printCurrentNetworkParams();
				Utils.setLED(LED0, LED_ON);  
				delay(300);
				Utils.setLED(LED0, LED_OFF);
				delay(300);
				Utils.setLED(LED0, LED_ON);   
				delay(300);
				Utils.setLED(LED0, LED_OFF);
			#endif
			
			//! After a full setup the XBee will connect faster with this delay. 
			//! In normal operation mode this is just a waste of time.
			if( mode == SETUP )
				delay(6000);
				
			xbeeZB.getAssociationIndication();
			
			#ifdef ASSOCIATION_DEBUG_EXTENDED
				printAssociationState();	
			#endif
		}
		
			#ifdef ASSOCIATION_DEBUG
				printCurrentNetworkParams();
			#endif
		
		if(!xbeeZB.associationIndication) 
		{
			xbeeZB.getExtendedPAN();
			if( xbeeZB.extendedPAN[7] == 0 )
			{
				xbeeZB.storeError(CHECK_NODE_ASSOCIATION_RETURNED_1_NO_REACHABLE_PARENT_FOUND);
				return 1;
			}
			else
			{
				error =0;
			
				xbeeZB.setChannelVerification(0);
				xbeeZB.writeValues();
				
				#ifdef ASSOCIATION_DEBUG
					USB.println("\n\nSuccessfully joined a coordinator or router!"); 
					
					Utils.setLED(LED1, LED_ON);   // If joined, lighten green LED
					delay(2000);
					Utils.setLED(LED1, LED_OFF);
					
					printCurrentNetworkParams();
				#endif
			}
		}
		else
		{
			xbeeZB.storeError(CHECK_NODE_ASSOCIATION_RETURNED_2_XBEE_NOT_DETECTED_ON_WASPMOTE);
			
				#ifdef ASSOCIATION_DEBUG
					USB.println("Failed to join a network!");
					
					Utils.setLED(LED0, LED_ON);   // If failed, blink red LED twice fast
					delay(300);
					Utils.setLED(LED0, LED_OFF);
					delay(300);
					Utils.setLED(LED0, LED_ON);   
					delay(300);
					Utils.setLED(LED0, LED_OFF);
				#endif
			
			return 2;
		}
		
		#ifdef ASSOCIATION_DEBUG
		USB.print("\nerror: ");
		USB.println( (int) error);
		#endif
		
	return error;
	//}
	/*else
	{
		GENERATES AN ERROR ANYWAY, RETURN VALUE IS NOT CORRECT!
	
		error = 1;
		#ifdef COMM_DEBUG
			USB.println("getAssociationIndication error");
		#endif	
	}*/
}
	
	
#ifdef ASSOCIATION_DEBUG_EXTENDED
	void CommUtils::printAssociationState()
	{
	  switch(xbeeZB.associationIndication)
	  {
		case 0x00  :  USB.println("Successfully formed or joined a network");
					  break;
		case 0x21  :  USB.println("Scan found no PANs");
					  break;   
		case 0x22  :  USB.println("Scan found no valid PANs based on current SC and ID settings");
					  break;   
		case 0x23  :  USB.println("Valid Coordinator or Routers found, but they are not allowing joining (NJ expired)");
					  break;   
		case 0x24  :  USB.println("No joinable beacons were found");
					  break;   
		case 0x25  :  USB.println("Unexpected state, node should not be attempting to join at this time");
					  break;
		case 0x27  :  USB.println("Node Joining attempt failed");
					  break;
		case 0x2A  :  USB.println("Coordinator Start attempt failed");
					  break;
		case 0x2B  :  USB.println("Checking for an existing coordinator");
					  break;
		case 0x2C  :  USB.println("Attempt to leave the network failed");
					  break;
		case 0xAB  :  USB.println("Attempted to join a device that did not respond.");
					  break;
		case 0xAC  :  USB.println("Secure join error  :  network security key received unsecured");
					  break;
		case 0xAD  :  USB.println("Secure join error  :  network security key not received");
					  break;
		case 0xAF  :  USB.println("Secure join error  :  joining device does not have the right preconfigured link key");
					  break;
		case 0xFF  :  USB.println("Scanning for a ZigBee network (routers and end devices)");
					  break;
		default    :  USB.println("Unkown associationIndication");
					  break;
	  }
	}
#endif


#ifdef ASSOCIATION_DEBUG
void CommUtils::printCurrentNetworkParams()
{
      USB.print("operatingPAN: ");            /// get operating PAN ID 
      xbeeZB.getOperatingPAN();
      USB.print(xbeeZB.operatingPAN[0],HEX);
      USB.println(xbeeZB.operatingPAN[1],HEX);
    
      USB.print("extendedPAN: ");             /// get operating 64-b PAN ID 
      xbeeZB.getExtendedPAN();
      USB.print(xbeeZB.extendedPAN[0],HEX);
      USB.print(xbeeZB.extendedPAN[1],HEX);
      USB.print(xbeeZB.extendedPAN[2],HEX);
      USB.print(xbeeZB.extendedPAN[3],HEX);
      USB.print(xbeeZB.extendedPAN[4],HEX);
      USB.print(xbeeZB.extendedPAN[5],HEX);
      USB.print(xbeeZB.extendedPAN[6],HEX);
      USB.println(xbeeZB.extendedPAN[7],HEX);
    
      USB.print("channel: ");
      xbeeZB.getChannel();
      USB.println(xbeeZB.channel,HEX);  
}
#endif


#ifdef COMM_DEBUG
	void CommUtils::discoverNodes()
	{
		USB.println("\nScanning network. This will take up to 10 seconds.");
		xbeeZB.scanNetwork();    //Discovery nodes
		
		USB.print("\n\ntotalScannedBrothers: ");
		USB.println(xbeeZB.totalScannedBrothers,DEC);
		  
		printScannedNodesInformation();
	}


	void CommUtils::printScannedNodesInformation()
	{
		  for(char i=0; i<xbeeZB.totalScannedBrothers; ++i)
		  {
				USB.println("\n---------------------------------------");
				USB.print("Node ID: ");
				for(char j=0; j<20; j++)
				{      
					  USB.print(xbeeZB.scannedBrothers[i].NI[j]);		
				}
				USB.print("\nMAC: ");
				USB.print("0013A200");
				USB.print(xbeeZB.scannedBrothers[i].SL[0],HEX);
				USB.print(xbeeZB.scannedBrothers[i].SL[1],HEX);
				USB.print(xbeeZB.scannedBrothers[i].SL[2],HEX);
				USB.println(xbeeZB.scannedBrothers[i].SL[3],HEX);
				USB.print("Device Type: ");
				  switch(xbeeZB.scannedBrothers[i].DT)
				  {
					  case 0: 
						USB.println("Coordinator");
						break;
					  case 1: 
						USB.println("Router");
						break;
					  case 2: 
						USB.println("End Device");
						break;
				  }
				USB.print("16-bit Network Address: ");
				USB.print(xbeeZB.scannedBrothers[i].MY[0],HEX);
				USB.print(xbeeZB.scannedBrothers[i].MY[1],HEX);
				USB.print("\n16-bit Parent Network Address: ");
				USB.print(xbeeZB.scannedBrothers[i].PMY[0],HEX);
				USB.print(xbeeZB.scannedBrothers[i].PMY[1],HEX);
				USB.print("\nProfile ID (App layer addressing): ");
				USB.print(xbeeZB.scannedBrothers[i].PID[0],HEX);
				USB.print(xbeeZB.scannedBrothers[i].PID[1],HEX);
				USB.print("\nMANUFACTURER ID/ ");
				USB.print(xbeeZB.scannedBrothers[i].MID[0],HEX);
				USB.print(xbeeZB.scannedBrothers[i].MID[1],HEX);
				
				for(char j=0; j<20; j++)
				{      
					  xbeeZB.scannedBrothers[i].NI[j] = 0;		
				}
		  }
		  USB.println("\n---------------------------------------"); 
	}
#endif

uint8_t CommUtils::receiveMessages(DeviceRole role)
{
	uint8_t error = 2;
	bool stop = false;
	bool received = false;
	
	long previous = 0;
    previous=millis();
	
		#ifdef FINAL_USB_DEBUG
			USB.print("\n\nRECEIVE: ");
		#endif
	
	//Waiting ... sec on a message
	while( !stop )
	{
		  if( XBee.available() )
		  {
				error = 1;
				xbeeZB.treatData();
				
				if( !xbeeZB.error_RX )
				{
					//read ALL available packets (pos = 0 -> no packets available)
					while( xbeeZB.pos > 0 && !stop)
					{	
						error = 0;
						uint8_t count = 0;
						// Available information in 'xbeeZB.packet_finished' structure
						// HERE it should be introduced the User's packet treatment        
						// For example: show DATA field:
						
						if(xbeeZB.encryptionMode == ENCRYPTION_ENABLED && 
							isValidPacket(&(xbeeZB.packet_finished[xbeeZB.pos-1]->packetID)))
						{
							decryptIncomingMessages(xbeeZB.packet_finished[xbeeZB.pos-1]);
						}
						
						/// RECOVER ZEROS
						for(uint8_t pos = 0; pos<xbeeZB.packet_finished[xbeeZB.pos-1]->data_length; pos++)
						{
							if(xbeeZB.packet_finished[xbeeZB.pos-1]->data[pos] == -1 /*0xFF*/ &&
							   xbeeZB.packet_finished[xbeeZB.pos-1]->data[pos+1] == -2)
							{
								xbeeZB.packet_finished[xbeeZB.pos-1]->data[pos - count] = 0;
								pos++;  count++;
							}
							else
							{
								xbeeZB.packet_finished[xbeeZB.pos-1]->data[pos - count] = 
									xbeeZB.packet_finished[xbeeZB.pos-1]->data[pos];
							}
						}						
				
								#ifdef FINAL_USB_DEBUG
									USB.print("ID = "); USB.print( (int) xbeeZB.packet_finished[xbeeZB.pos-1]->packetID);
									USB.print(" Data: ");
									for(int f=0;f<xbeeZB.packet_finished[xbeeZB.pos-1]->data_length - count;f++)
									{
										receivedData[f] = xbeeZB.packet_finished[xbeeZB.pos-1]->data[f];
											//USB.print(xbeeZB.packet_finished[xbeeZB.pos-1]->data[f],BYTE);
											USB.print( (int) receivedData[f] ); USB.print("  ");
									}
									USB.print("\n");
								#endif  						
						
						/// HERE THE PACKETS ARE TREATED ///
						if(isValidPacket(&(xbeeZB.packet_finished[xbeeZB.pos-1]->packetID)))
						{
							received = true;
							error = (*myTreatPacket[xbeeZB.packet_finished[xbeeZB.pos-1]->packetID])
								(xbeeZB.packet_finished[xbeeZB.pos-1]);
							stop = true;
							if(error != 0) 
							{
								USB.print("\nPACKET TREATED BUT ERROR\n");
								error =  4;
							}
								#ifdef FINAL_USB_DEBUG
									USB.print("\nPACKET TREATED\n");
									USB.print("Free mem: "); USB.println(freeMemory());
								#endif
						}
						else
						{
							sendError(NODE_RECEIVED_AN_UNKNOWN_PACKET_TYPE);							
							stop = true;
								#ifdef RECEIVE_DEBUG
									USB.println("\nInvalid packetID");
								#endif
							error = 3;
						}

						free(xbeeZB.packet_finished[xbeeZB.pos-1]); 
						xbeeZB.packet_finished[xbeeZB.pos-1]=NULL; 
						xbeeZB.pos--;
						
						if(error == 3) return 3;
						else if(error == 4) return 4;
					}
				}
		  }
		  else
		  {
				error = 1;
				#ifdef RECEIVE_DEBUG_2
					USB.println("XBEE not available\r\n");
				#endif
		  }

		  if(role == ROUTER)
		  {
			  if( intFlag & RTC_INT )
			  {
			  	  intFlag &= ~(RTC_INT); // Clear flag
				  xbeeZB.setAlarmForRouter();
				  
				  ///USB.print("\nmeasure now...\n");
				  stop = true;
				  
				  error = 1;
			  }				
		  }
		  else /// role = END_DEVICE
		  {
				if( (millis()-previous) >= 3000 )
					stop = true;
		  }
	}

	if(error == 1 && received == true) 
		return 0;
	else
		return error;
}


void CommUtils::decryptIncomingMessages(packetXBee * receivedPaq)
{
	char * password = "1302AESJOR8";
    //1. Calculate size of encrypted message 
    uint8_t length = sizeof(receivedPaq->data); 
    
    //2. Declarete variable to store decrypted messsage 
    uint8_t decrypted_message[100]; 

    //3. Declarate variable to storre original size of  
    //   decrypted message. 
    uint16_t original_size[1]; 
    
    //4. Calculate decrypted message using AES algorithm 
    AES.decrypt(128, password,(uint8_t *) receivedPaq->data,length,decrypted_message,original_size,ECB,PKCS5); 
     
    //5. Save decrypted message back in the received packet data field 
    for (uint8_t i=0; i<original_size[0];i++)
	{ 
        receivedPaq->data[i] = (char) decrypted_message[i]; 
    } 	
}


bool CommUtils::isValidPacket(uint8_t * packetID)
{
	bool valid = false;
	for(int i=DONT_USE; i<RESET_PLUVIOMETER; i++)
	{
		if(i == *packetID)  valid = true;
	}
	return valid;
}


uint8_t CommUtils::sendMessage(uint8_t * destination, const char * message)
{
	uint8_t error = 2;
	packetXBee * paq_sent;
	paq_sent = (packetXBee*) calloc(1,sizeof(packetXBee)); 
	paq_sent->mode=UNICAST;
	paq_sent->MY_known=0;
	paq_sent->packetID = SEND_ERROR;  //APPLICATION_ID = 12
	paq_sent->opt=0; 
	xbeeZB.hops=0;
	  
	#ifndef SEND_MEMORY_LEAK_DEBUG
	/** 
	 *! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 *! When executing the xbeeZB.sendXBee(paq_sent) function like in the example given
	 *! on the Libelium website we observed there is a constant memory leak of 302 bytes
	 *! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 */
		USB.println("as example");
		xbeeZB.setOriginParams(paq_sent, "5678", MY_TYPE);
		xbeeZB.setDestinationParams(paq_sent, "0013A2004069737A", message, MAC_TYPE,DATA_ABSOLUTE);
	#else
		xbeeZB.setOriginParams(paq_sent, MY_TYPE);
		xbeeZB.setDestinationParams(paq_sent, destination, message, MAC_TYPE, DATA_ABSOLUTE);
	#endif
	xbeeZB.sendXBee(paq_sent);
	  
	if( !xbeeZB.error_TX )
	{
		#ifdef COMM_DEBUG
			USB.println("End device sends out a challenge ok");
			Utils.setLED(LED1, LED_ON);   // If transmission OK, blink green LED
			delay(500);
			Utils.setLED(LED1, LED_OFF);
		#endif			
		error = 0;
	}
	else
	{ 
		error = xbeeZB.error_TX;
		#ifdef COMM_DEBUG
			USB.println("\nchallenge transmission error\n");
			Utils.setLED(LED0, LED_ON);   // If transmission error, blink red LED
			delay(500);
			Utils.setLED(LED0, LED_OFF);
		#endif
	}

	free(paq_sent);
	paq_sent=NULL;
	return error;
}


uint8_t CommUtils::sendMessage(uint8_t * destination, uint8_t type, const char * message)
{
		#ifdef FINAL_USB_DEBUG
			USB.print("\nSENDING TYPE: ");  USB.print( (int) type );  USB.print(" DATA: "); 
			for(int j=0; j<PackUtils.packetSize+1; j++)
			{
				USB.print( (int) message[j]); USB.print("  ");
			}
		#endif

	uint8_t error = 2;
	packetXBee * paq_sent;
	paq_sent = (packetXBee*) calloc(1,sizeof(packetXBee)); 

	paq_sent->mode=UNICAST;
	paq_sent->MY_known=0;
	paq_sent->packetID = type;
	paq_sent->opt=0; 
	xbeeZB.hops=0;

	xbeeZB.setOriginParams(paq_sent, MY_TYPE);
	xbeeZB.setDestinationParams(paq_sent, destination, message, MAC_TYPE, DATA_ABSOLUTE);
				
	error = xbeeZB.sendXBee(paq_sent);
	  
	if( !xbeeZB.error_TX )
	{
		error = 0;	
			#ifdef FINAL_USB_DEBUG
				uint8_t i=0;
				while(i++<8)		
				{
					Utils.setLED(LED1, LED_ON);
					delay(150);
					Utils.setLED(LED1, LED_OFF);
					delay(150);
				}
			#endif			
	}
	else
	{ 
		error = xbeeZB.error_TX;
	}

	free(paq_sent);
	paq_sent=NULL;

	return error;
}


uint8_t CommUtils::sendError(Errors e)
{
	uint8_t error = 2;
	char er[2];
	packetXBee * paq_sent;

	er[0] = (unsigned int) e;
	er[1] = 0;
	
		#ifdef FINAL_USB_DEBUG
			USB.print("\nSENDING ERROR MESSAGE: "); USB.print( (int) er[0] ); USB.print(" TO GATEWAY\n");
		#endif	
		
	paq_sent = (packetXBee*) calloc(1,sizeof(packetXBee)); 
	
	paq_sent->mode=UNICAST;
	paq_sent->MY_known=0;
	paq_sent->packetID = SEND_ERROR;
	paq_sent->opt=0; 
	xbeeZB.hops=0;

	xbeeZB.setOriginParams(paq_sent, MY_TYPE);
	xbeeZB.setDestinationParams(paq_sent, xbeeZB.GATEWAY_MAC, er, MAC_TYPE, DATA_ABSOLUTE);

	error = xbeeZB.sendXBee(paq_sent);

	if( !xbeeZB.error_TX )
	{
		error = 0;
	}
	else
	{ 
		error = xbeeZB.error_TX;
	}

	free(paq_sent);
	paq_sent=NULL;

	return error;
}


uint8_t CommUtils::sendWarning(Errors e)
{
	sendError(e);
}


char * CommUtils::escapeZeros(char * content, uint8_t contentLength)
{
	uint8_t pos1 = 0, pos2 = 0;
	char * contentToSend = (char *) calloc(contentLength*2, sizeof(char));
		
	while(pos1 < contentLength)
	{
		if(content[pos1] == 0)
		{
			contentToSend[pos2++] = 0xFF;	// FF = escape char
			contentToSend[pos2++] = 0xFE; 	// FE = 0
			pos1++;
		}
		else if(content[pos1] = 0xFF)
		{
			contentToSend[pos2++] = 0xFF;	
			contentToSend[pos2++] = 0xFD; 
			pos1++;		
		}
		else
			contentToSend[pos2++] = content[pos1++];
	}
	contentToSend[pos2] = '\0';
	
	USB.print("contentToSend = ");
	for(pos1=0; pos1<pos2; pos1++)
		USB.print( (int) contentToSend[pos1]);
	USB.print("\n");
	return contentToSend;
}

CommUtils COMM = CommUtils();
