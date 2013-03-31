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

//typedef enum {ERRORMESSAGE, IO_REQUEST, IO_DATA, CF_REQUEST, CF_RESPONSE, ADD_NODE_REQ, ADD_NODE_RES}
//	ApplicationIDs;

typedef enum {DONT_USE, ADD_NODE_REQ, ADD_NODE_RES, MASK_REQ, MASK_RES, CH_NODE_FREQ_REQ,
	CH_NODE_FREQ_RES, CH_SENS_FREQ_REQ, CH_SENS_FREQ_RES, IO_REQUEST, IO_DATA,
	RECEIVE_ERROR, SEND_ERROR}
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
 *	STATIC FUNCTION POINTERS FOR HANDLING RECEIVED MESSAGES:
 ********************************************************************************************************/
typedef uint8_t TreatData( packetXBee *);

	//! SHOULD NEVER BE RECEIVED: ApplicationID = 0 causes transmission errors. 
	/*!
	 *	\@post: will send an error message to the gateway
	 */
	extern uint8_t NotInUse(packetXBee *);  // APP_ID = 0

	
	//! This function is called by the receive function in 'CommUtils.h' if the
	//! received packet is a request to add a node to the network (Application_ID = 1).
	/*!
	 *	\param: a packetXBee which contains the physical sensormask
	 *	\@post:	the physical sensor mask will be stored in 'WaspXBeeZBNode.h' and
	 *			the node will send it back as confirmation.
	 */
	extern uint8_t Add_Node_Request(packetXBee *);  // APP_ID = 1
	//! SHOULD NEVER BE RECEIVED
	/*!
	 *	\@post: will send an error message to the gateway
	 */
	extern uint8_t Add_Node_Response(packetXBee *);   // APP_ID = 2
	
	
	//! This function is called by the receive function in 'CommUtils.h' if the
	//! received packet is a request for the nodes physical sensor layout.
	/*!
	 *	\param: empty packetXBee
	 *	\post: the physical sensor mask stored in 'WaspXBeeZBNode.h' will be send
	 */
	extern uint8_t Mask_Request(packetXBee *); // APP_ID = 3
	//! SHOULD NEVER BE RECEIVED
	/*!
	 *	\@post: will send an error message to the gateway
	 */	
	extern uint8_t Mask_Response(packetXBee *);  // APP_ID = 4
	
	
	//! This function is called by the receive function in 'CommUtils.h' if the
	//! received packet is a request to change or set a new frequency at which the
	//! node goes into hibernate.
	/*!
	 *	\param: packetXBee which contains the new sleep interval as data
	 *	\@post: the node will use this new sleep time. 
	 *	   !!!!!! All possible individual sensor sleep times will be erased !!!!!!!
	 */
	extern uint8_t Change_Node_Frequency_Request(packetXBee *);  // APP_ID = 5
	//! SHOULD NEVER BE RECEIVED
	/*!
	 *	\@post: will send an error message to the gateway
	 */		
	extern uint8_t Change_Node_Frequency_Response(packetXBee *);  // APP_ID = 6
	
	
	//! This function is called by the receive function in 'CommUtils.h' if the
	//! received packet is a request to change or set a new frequency for certain sensors
	/*!
	 *	\param: packetXBee which contains a sensor mask and corresponding frequencies
	 *	\@post: the sensors will get individual measure frequencies and the node will
	 *			try to find an efficient way to measure / sleep
	 */	
	extern uint8_t Change_Sensor_Frequency_Request(packetXBee *);  // APP_ID = 7
	//! SHOULD NEVER BE RECEIVED
	/*!
	 *	\@post: will send an error message to the gateway
	 */	
	extern uint8_t Change_Sensor_Frequency_Response(packetXBee *);  // APP_ID = 8	
	

	//! This function is called by the receive function in 'CommUtils.h' if the
	//! received packet is a request for sensor data.
	/*!
	 *  \@post: this function will send an answer with sensor data back if the mask is valid
	 *		else it will send an error message back.
	 *		The mask is valid if it contains the same or less sensors than present on 
	 *		the node (this mask is stored in WaspXBeeZBNode.h when a message of type
	 *		Add_Node_Request is received)
	 *  \param:	a packetXBee which contains the sensormask
	 */
	extern uint8_t IO_Request(packetXBee *);	// APP_ID = 9
	//! SHOULD NEVER BE RECEIVED
	/*!
	 *	\@post: will send an error message to the gateway
	 */		
	extern uint8_t IO_Data(packetXBee *);	// APP_ID = 10

	
	//! This function is called by the receive function in 'CommUtils.h' if the
	//! received packet is an error/message packet.
	//! !!!  In normal operation nodes should not receive such messages  !!!
	/*!
	 *	\@post:	the received error/message will be forwarded to the gateway
	 *	\param: a packetXBee which contains the error/message
	 */
	extern uint8_t Receive_Error(packetXBee *);  // APP_ID = 11
	//! SHOULD NEVER BE RECEIVED
	/*!
	 *	\@post: will send an error message to the gateway
	 */	
	extern uint8_t Send_Error(packetXBee *); // APP_ID = 12
	






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
		void testComm4(const char *, const char *);
		void testComm5(const char *, uint8_t, const char *);
		void testComm6();
		
 
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
		
		bool areSameMACAddresses(uint8_t *, uint8_t *);
		

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
