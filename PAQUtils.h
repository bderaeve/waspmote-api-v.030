/* ==========================================================================
 *
 *			THESIS: Design of a Wireless Sensor Networking test-bed
 * 
 * ==========================================================================
 *
 *       Filename:  PAQUtils.h
 *    Description:  Contains additional functions to deal with packet data.
 *
 * ======================================================================= */

#ifndef PAQUTILS_H
#define PAQUTILS_H

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <inttypes.h>

/******************************************************************************
 * Definitions & Declarations
 ******************************************************************************/
//#define PACKET_DEBUG

//!
/*! Defines the packet types recognized by the Waspmote
 */		
typedef enum {DONT_USE, ADD_NODE_REQ, ADD_NODE_RES, MASK_REQ, MASK_RES, CH_NODE_FREQ_REQ,
	CH_NODE_FREQ_RES, CH_SENS_FREQ_REQ, CH_SENS_FREQ_RES, IO_REQUEST, IO_DATA,
	RECEIVE_ERROR, SEND_ERROR, CHANGE_MODE_REQ, CHANGE_MODE_RES, STARTED_RAINING}
	ApplicationIDs;

	
//!
/*! Contains the errors that can be notified to the gateway via an SEND_ERROR packet
 */			
typedef enum {	//INVALID PACKET RECEIVED
				NOT_IN_USE,
				NODE_RECEIVED_INVALID_PACKET_OF_TYPE_0_DONT_USE,
				NODE_RECEIVED_INVALID_PACKET_OF_TYPE_2_ADD_NODE_RES,
				NODE_RECEIVED_INVALID_PACKET_OF_TYPE_4_MASK_RES,
				NODE_RECEIVED_INVALID_PACKET_OF_TYPE_6_CH_NODE_FREQ_RES,
				NODE_RECEIVED_INVALID_PACKET_OF_TYPE_8_CH_SENS_FREQ_RES,
				NODE_RECEIVED_INVALID_PACKET_OF_TYPE_10_IO_DATA,
				NODE_RECEIVED_INVALID_PACKET_OF_TYPE_12_SEND_ERROR,
				//INVALID PACKET CONTENT RECEIVED
				NODE_RECEIVED_AN_UNKNOWN_PACKET_TYPE,
				NODE_RECEIVED_INVALID_MASK_IN_ADD_NODE_REQUEST,
				MASK_RECEIVED_IN_ADD_NODE_REQUEST_WAS_REFUSED_BY_THE_PHYSICAL_MASK,
				NODE_RECEIVED_INVALID_NEW_DEFAULT_SLEEP_TIME,
				NODE_RECEIVED_EMPTY_SENSOR_MASK_IN_CH_SENS_FREQ_REQ,
				NODE_HAS_NO_LONGER_ACTIVE_SENSORS,
				NODE_HAD_AN_ERROR_IN_SET_NEW_DIFFERENT_SLEEP_TIMES,
				//PROGRAM ERRORS
				NODE_HAD_AN_ERROR_IN_XBEEZB_CHANGE_SENSOR_FREQUENCIES,
				//UNAUTHORIZED REQUESTS
				NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_ADD_NODE_REQUEST
			}
	Errors;

	
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
	 *			the node will send it back to the gateway as confirmation.
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
 
class PAQUtils
{
	private:
		
	
	public:
		//! class constructor
		/*!
		  It does nothing
		  \param void
		  \return void
		*/
		PAQUtils(){};

		
		//! 
		/*! Retreive the type of the received packet, it stores in global 'applicationID'
		 */
		void getPacketApplicationID(packetXBee *);
		
		
		//!
		/*! 16-bit mask identifying which information (sensors) is sent in the packet
		 */
		void setPacketMask(uint16_t);
		
		
		//!
		/*! Retrieves the mask from a received packet, storing in global 'mask'
		 */
		void getPacketMask(packetXBee *);
		
		
		//!
		/*! Returns if the received mask is feasable according to this node's
		 *  physical sensor layout set via the ADD_NODE_REQ command
		 */
		bool maskMatchesNodesPhysicalSensorMask(uint16_t);
		
		
		void getPacketOriginAddress(packetXBee *);
		
		//void printReceivedPacketData(packetXBee *);
		
		//for reading in an error message
		//void readPacketData(packetXBee *);
		uint8_t sendMask(uint8_t *, uint8_t, uint16_t);
		uint8_t sendMeasuredSensors(uint8_t *, uint16_t);
		
		
		//!
		/*! It returns if the sender of a packet is an authorized sender
		 *  This means it is the gateway or one of the test XBees used,
		 *  stored in the constructor of 'WaspXBeeZBNode'
		 */
		bool authorizeSender(packetXBee *);
		
		
		//!
		/*! It removes the zeros from data to be sent
		 *  @pre: the data to send must be present in global 'packetData[MAX_DATA]'
		 *  @pre: the content param must be allocated
		 *  @param : the char *  to send, where the escaped data must be put in 
		 */
		void escapeZerosInPacketData(char *);
		
		
		//!
		/*! Returns if two MAC addresses are the same
		 */
		bool areSameMACAddresses(uint8_t *, uint8_t *);
	
		
		//!
		/*! Stores the packet type of a received packet / packet to be sent
		 */		
		uint8_t applicationID;
		

		//!
		/*! Stores the mask of a received packet / packet to be sent
		 */		
		uint16_t mask;
		
		
		//!
		/*! Stores the origin address of a received packet
		 */				
		uint8_t originAddress[8];
		
		
		//!
		/*! Stores the packet size of a received packet / packet to be sent
		 */				
		uint8_t packetSize;
		
		
		//!
		/*! Contains the packet data
		 */				
		char packetData[MAX_DATA];
 };
 
 extern PAQUtils PackUtils;


#endif /*PAQUTILS_H*/
