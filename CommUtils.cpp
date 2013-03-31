
#ifndef __WPROGRAM_H__
	#include "BjornClasses.h"
	//#include "CommUtils.h"
	//#include "WaspXBeeZBNode.h"
	#include "WaspClasses.h"
#endif

#include <inttypes.h>

void CommUtils::testenPrinten()
{
	USB.print("testCommUtils");
}

uint8_t CommUtils::setupXBee()
{
	uint8_t error = 2;
	xbeeZB.init(ZIGBEE,FREQ2_4G,NORMAL);
	
	xbeeZB.ON();
	xbeeZB.wake();  // Must do this for end devices to work
	delay(1000);

	// 1.1 Set PANID: 0x00000000000000AA 
		if(!xbeeZB.setPAN(xbeeZB.panid))
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
		delay(3000); 
	
	
	// 2. WAIT for association:
		if( !checkNodeAssociation() )
		{
			error = 0;
			#ifdef COMM_DEBUG
				USB.println("checkNodeAssociation() ok");
			#endif
		}
		else
		{
			error = 1;
			#ifdef COMM_DEBUG
				USB.println("ERROR CHECKING NODE ASSOCIATION");  
			#endif
		}
	
	return error;
}


uint8_t CommUtils::checkNodeAssociation()
{
	uint8_t error = 2;
	long previous = millis();
	
	 xbeeZB.getAssociationIndication();
	//if(xbeeZB.getAssociationIndication()){
		while( xbeeZB.associationIndication != 0 && (millis()-previous) < 120000)
		{
			USB.println("\n\n-----> not associated <----------");
			printCurrentNetworkParams();
			
			delay(6000);
			
			xbeeZB.getAssociationIndication();
			printAssociationState();	
		}
		
		if(!xbeeZB.associationIndication) 
		{
			error = 0;
			
			xbeeZB.setChannelVerification(0);
			xbeeZB.writeValues();
			
			#ifdef COMM_DEBUG
				USB.println("\n\nSuccessfully joined a coordinator or router!"); 
				
				Utils.setLED(LED1, LED_ON);   // If joined, lighten green LED
				delay(2000);
				Utils.setLED(LED1, LED_OFF);
				
				printCurrentNetworkParams();
			#endif

		}
		else
		{
			error = 1;
			#ifdef COMM_DEBUG
				USB.println("Failed to join a network!");
				
				Utils.setLED(LED0, LED_ON);   // If failed, blink red LED twice fast
				delay(300);
				Utils.setLED(LED0, LED_OFF);
				delay(300);
				Utils.setLED(LED0, LED_ON);   
				delay(300);
				Utils.setLED(LED0, LED_OFF);
			#endif
		}
	//}
	/*else
	{
	
		GENERATES AN ERROR ANYWAY, RETURN VALUE IS NOT CORRECT!
	
		error = 1;
		#ifdef COMM_DEBUG
			USB.println("getAssociationIndication error");
		#endif	
	}*/
	
	return error;
}
	

void CommUtils::printCurrentNetworkParams()
{
      USB.print("operatingPAN: ");            // get operating PAN ID 
      xbeeZB.getOperatingPAN();
      USB.print(xbeeZB.operatingPAN[0],HEX);
      USB.println(xbeeZB.operatingPAN[1],HEX);
    
      USB.print("extendedPAN: ");              // get operating 64-b PAN ID 
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


uint8_t CommUtils::receiveMessages()
{
	uint8_t error = 2;
	long previous = 0;
    previous=millis();
	
    while( (millis()-previous) < 20000 )
    {
          if( XBee.available() )
          {
				error = 1;
                xbeeZB.treatData();
				
				#ifdef COMM_DEBUG
					USB.print("start printing xbeeZB.error_RX:");
					USB.println(xbeeZB.error_RX);
				#endif
				
                if( !xbeeZB.error_RX )
                {
					//read ALL available packets (pos = 0 -> no packets available)
					while( xbeeZB.pos > 0 )
					{	
						// Available information in 'xbeeZB.packet_finished' structure
						// HERE it should be introduced the User's packet treatment        
						// For example: show DATA field:
						error = 0;
						
						#ifdef COMM_DEBUG
							USB.print("Data: ");
							
							for(int f=1;f<xbeeZB.packet_finished[xbeeZB.pos-1]->data_length;f++)
							{
								receivedData[f] = xbeeZB.packet_finished[xbeeZB.pos-1]->data[f];
									USB.print(xbeeZB.packet_finished[xbeeZB.pos-1]->data[f],BYTE);
							}
						#endif  
						
						
						//(*myTreatPacket[xbeeZB.packet_finished[xbeeZB.pos-1]->packetID])
						//	(xbeeZB.packet_finished[xbeeZB.pos-1]);
						
					
						#ifdef COMM_DEBUG
							USB.print("receivedData(MAX_DATA) = ");
							USB.println(receivedData);
						#endif
						
						//sendAnswerBack();
						
						//Forward the data
						//sendMessage(receivedData);
						
						//previous=millis();
						
						// AFTER THE DATA IS TREATED BY OWN FUNCTIONS:
						//free memory
						free(xbeeZB.packet_finished[xbeeZB.pos-1]); 
						
						//free pointer
						xbeeZB.packet_finished[xbeeZB.pos-1]=NULL; 
						
						xbeeZB.pos--;
					}
                }
          }
          else
          {
				error = 1;
				#ifdef COMM_DEBUG
					USB.println("XBEE not available\r\n");
				#endif
          }  
    }
	return error;
}

bool CommUtils::sendMessageLocalWorking(const char * message, const char * destination)
{
      bool error = false;
	  
	  packetXBee * paq_sent;
      paq_sent=(packetXBee*) calloc(1,sizeof(packetXBee)); 
      paq_sent->mode=UNICAST;
      paq_sent->MY_known=0;
      paq_sent->packetID=IO_DATA;//0x02;
      paq_sent->opt=0; 
      xbeeZB.hops=0;
      xbeeZB.setOriginParams(paq_sent, MY_TYPE);
      xbeeZB.setDestinationParams(paq_sent, destination, message, MAC_TYPE, DATA_ABSOLUTE);
      xbeeZB.sendXBee(paq_sent);
      USB.print("start printing xbeeZB.error_TX:");
      USB.println(xbeeZB.error_TX);// print xbeeZB.error_TX
      if( !xbeeZB.error_TX )
      {
          //XBee.println("ok");
          USB.println("End device sends out a challenge ok");
          Utils.setLED(LED1, LED_ON);   // Ok, blink green LED
          delay(500);
          Utils.setLED(LED1, LED_OFF);
          error = false;
      }
      else
      { 
          USB.println("challenge transmission error\n\n");
          Utils.setLED(LED0, LED_ON);   // Error, blink red LED
          delay(500);
          Utils.setLED(LED0, LED_OFF);
          error = true;
      }
      free(paq_sent);
      paq_sent=NULL;
      return error;
} 

bool CommUtils::sendMessageLocalWorkingWithType(const char * message, uint8_t type, const char * destination)
{
      bool error = false;
	  
	  packetXBee * paq_sent;
      paq_sent=(packetXBee*) calloc(1,sizeof(packetXBee)); 
      paq_sent->mode=UNICAST;
      paq_sent->MY_known=0;
      paq_sent->packetID=type; //IO_DATA;//0x02;
      paq_sent->opt=0; 
      xbeeZB.hops=0;
      xbeeZB.setOriginParams(paq_sent, MY_TYPE);
      xbeeZB.setDestinationParams(paq_sent, destination, message, MAC_TYPE, DATA_ABSOLUTE);
      xbeeZB.sendXBee(paq_sent);
      USB.print("start printing xbeeZB.error_TX:");
      USB.println(xbeeZB.error_TX);// print xbeeZB.error_TX
      if( !xbeeZB.error_TX )
      {
          //XBee.println("ok");
          USB.println("End device sends out a challenge ok");
          Utils.setLED(LED1, LED_ON);   // Ok, blink green LED
          delay(500);
          Utils.setLED(LED1, LED_OFF);
          error = false;
      }
      else
      { 
          USB.println("challenge transmission error\n\n");
          Utils.setLED(LED0, LED_ON);   // Error, blink red LED
          delay(500);
          Utils.setLED(LED0, LED_OFF);
          error = true;
      }
      free(paq_sent);
      paq_sent=NULL;
      return error;
} 

uint8_t CommUtils::sendMessage(uint8_t * destination, const char * message)
{
      uint8_t error = 2;
	  packetXBee * paq_sent;
      paq_sent = (packetXBee*) calloc(1,sizeof(packetXBee)); 
      paq_sent->mode=UNICAST;
      paq_sent->MY_known=0;
      paq_sent->packetID = 12;// = ERRORMESSAGE;  //= APPLICATION_ID
      paq_sent->opt=0; 
      xbeeZB.hops=0;
	  
      xbeeZB.setOriginParams(paq_sent, MY_TYPE);
      xbeeZB.setDestinationParams(paq_sent, destination, message, MAC_TYPE, DATA_ABSOLUTE);
      xbeeZB.sendXBee(paq_sent);
	  
	  #ifdef COMM_DEBUG
		  USB.print("start printing xbeeZB.error_TX:");
		  USB.println(xbeeZB.error_TX);
	  #endif
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
			error = 1;
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
	#ifdef COMM_DEBUG
		USB.println("sendMessage: data in const char * message = "); 
		//for(int j=0; j<10; j++)
		//	USB.println( (int) message[j]);
		USB.print("Message: ");
		USB.println( message );
		USB.print("dest = "); USB.println( (int) destination[1] );
		USB.print("type = "); USB.println( (int) type );	
	#endif COMM_DEBUG
	
		

      uint8_t error = 2;
	  packetXBee * paq_sent;
      paq_sent = (packetXBee*) calloc(1,sizeof(packetXBee)); 
      paq_sent->mode=UNICAST;
      paq_sent->MY_known=0;
      paq_sent->packetID = 0x52; //type;
	  //paq_sent->packetID = 0x52;
      paq_sent->opt=0; 
      xbeeZB.hops=0;
	  
      //xbeeZB.setOriginParams(paq_sent, "5678", MY_TYPE);
	  xbeeZB.setOriginParams(paq_sent, MY_TYPE);
      xbeeZB.setDestinationParams(paq_sent, destination, message, MAC_TYPE, DATA_ABSOLUTE);
	  //xbeeZB.setDestinationParams(paq_sent, "0013A2004069737A", message, MAC_TYPE, DATA_ABSOLUTE);
      xbeeZB.sendXBee(paq_sent);
	 
	  #ifdef COMM_DEBUG
	 delay(500);	  USB.print("start printing xbeeZB.error_TX:");
		  USB.println(xbeeZB.error_TX);
	  #endif
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
			error = 1;
	  		#ifdef COMM_DEBUG
		        USB.println("challenge transmission error\n\n");
				Utils.setLED(LED0, LED_ON);   // If transmission error, blink red LED
				delay(500);
				Utils.setLED(LED0, LED_OFF);
			#endif
      }
	  
      free(paq_sent);
      paq_sent=NULL;
      return error;
}

CommUtils COMM = CommUtils();
