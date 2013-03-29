/* ==========================================================================
 *
 *			THESIS: Design of a Wireless Sensor Networking test-bed
 * 
 * ==========================================================================
 *
 *       Filename:  PacketData.h
 *    Description:  Contains the general structure of the data we send in
 *					packetXBee (WaspXBeeCore.h) structures.
 *
 * ======================================================================= */
#ifndef PACKETUTILS_H
#define PACKETUTILS_H

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <inttypes.h>
#include "WaspXBeeCore.h"

#ifndef __WASPXBEECONSTANTS_H__
  #include "WaspXBeeConstants.h"
#endif

/******************************************************************************
 * Definitions & Declarations
 ******************************************************************************/
#define PAQ_DEBUG

typedef enum {ERRORMESSAGE, IO_REQUEST, IO_DATA, CF_REQUEST, CF_RESPONSE, ADD_NODE_REQ, ADD_NODE_RES}
	ApplicationIDs;


//! Function pointers for the insertion of sensor data
/*! Since the size of the data to insert is constant there's no reason to
/*! make these functions non-static / member functions.
 */
typedef uint8_t InsertData(uint8_t *, char *);

extern uint8_t InsertTemperature(uint8_t *, char *);
extern uint8_t InsertHumidity(uint8_t *, char *);
extern uint8_t InsertPressure(uint8_t *, char *);
extern uint8_t InsertBattery(uint8_t *, char *);
extern uint8_t InsertCO2(uint8_t *, char *);
/*uint8_t InsertAnemo(uint8_t *, char *);
uint8_t InsertVane(uint8_t *, char *);
uint8_t InsertPluvio(uint8_t *, char *);*/


/*********************************************************************************************************
 *	STATIC FUNCTION POINTERS FOR HANDLING MESSAGES:
 ********************************************************************************************************/
typedef uint8_t TreatData( packetXBee *);

	//! This function is called by the receive function in 'CommUtils.h' if the
	//! received packet is an error/message packet (Application_ID = 0).
	//! !!!  In normal operation nodes should not receive such messages  !!!
	/*!
	 *	\@post:	the received error/message will be forwarded to the gateway
	 *	\param: a packetXBee which contains the error/message
	 */
	extern uint8_t ErrorMessage(packetXBee *);

	
	//! This function is called by the receive function in 'CommUtils.h' if the
	//! received packet is a request for sensor data (Application_ID = 1).
	/*!
	 *  \@post: this function will send an answer back if the mask is valid
	 *		else it will send an error message back.
	 *		The mask is valid if it contains the same or less sensors than present on 
	 *		the node (this mask is stored in WaspXBeeZBNode.h when a message of type
	 *		Add_Node_Request is received)
	 *  \param:	a packetXBee which contains the sensormask
	 */
	extern uint8_t IO_Request(packetXBee *);
	extern uint8_t IO_Data(packetXBee *);
	extern uint8_t CF_Request(packetXBee *);
	extern uint8_t CF_Response(packetXBee *);
	extern uint8_t Add_Node_Request(packetXBee *);
	extern uint8_t Add_Node_Response(packetXBee *);





/******************************************************************************
 * Class
 ******************************************************************************/
class PacketUtils
{
	private:
			
		//PacketData data;
		
		//InsertData * inserter[8];// = {&InsertTemperature, &InsertHumidity, &InsertPressure,
					//&InsertBattery, &InsertCO2, &InsertAnemo, &InsertVane, &InsertPluvio};
		//void (PacketUtils::*pt2Member)(uint8_t *, char *);// = NULL; 
		
		//void (PacketUtils::*inserter[8])(uint8_t *, char *);
//static InsertData * Inserter[5] = {&InsertTemperature, &InsertHumidity, &InsertPressure,
	//			&InsertBattery, &InsertCO2};
	public:
		//! class constructor
		/*!
		  It does nothing
		  \param void
		  \return void
		 */
		PacketUtils(){};
			
		//InsertData * Inserter[5] = {&InsertTemperature, &InsertHumidity, &InsertPressure,
		//		&InsertBattery, &InsertCO2};
		 /*: inserter{&InsertTemperature, &InsertHumidity, &InsertPressure,
					&InsertBattery, &InsertCO2, &InsertAnemo, &InsertVane, &InsertPluvio}*/
			

		//! Integer value corresponding to the nodeID (char nodeID[20])
		//void setPacketID(uint8_t);
		//uint8_t getPacketID();
		
		//! Type of the data which is sent
		void getPacketApplicationID(packetXBee *);
		
		
		//! 16-bit mask identifying which information (sensors) is sent
		void setPacketMask(uint16_t);
		void getPacketMask(packetXBee *);
		
		bool maskMatchesNodesActiveSensorMask(uint16_t);
		
		void getPacketOriginAddress(packetXBee *);
		
		void printReceivedPacketData(packetXBee *);
		
		//for reading in an error message
		void readPacketData(packetXBee *);
		
		
		
		
		void testComm(uint8_t * , uint8_t, const char *);
		void testComm2(uint8_t *, uint8_t);
		void testComm3(uint8_t * , uint8_t, const char *);
		
 
 		//! It enters the nodes sensor values into the packetData data field.
		/*! 
		  \pre	read new sensor values	
		  \return  	error=2 --> The command has not been executed
					error=1 --> There has been an error while executing the command
					error=0 --> The command has been executed with no errors
		*/ 
		//uint8_t setSensorData(uint16_t *);
		//uint8_t send_As_IO_Data(uint8_t *, const char *);
		uint8_t sendMeasuredSensors(uint8_t *, uint16_t);
		

		//! Structure Variable : Data. All the data here, even when it is > payload
		/*! For a full mask the sequence of sensor data is the following:  
		/*! DATA = |TYPE(1B)|ID(1B)|MASK(2B)|PLUVIO(?)|VANE(?)|ANEMO(?)|CO2(2B)|BAT(1B)|PRES(2B)|HUM(1B)|TEMP(2B)|
		 */		
		 
		//packetXBee newPacket;
		
		uint8_t applicationID;
		uint16_t mask;
		uint8_t originAddress[8];
		char * packetData; //[MAX_DATA];
		
		
	
};	

 
extern PacketUtils PAQ;


#endif /*PACKETUTILS*/
