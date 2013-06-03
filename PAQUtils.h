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
#define NUM_APP_IDS 18 
typedef enum {DONT_USE, ADD_NODE_REQ, ADD_NODE_RES, 
			  MASK_REQ, MASK_RES, 
			  CH_NODE_FREQ_REQ, CH_NODE_FREQ_RES, 
			  CH_SENS_FREQ_REQ, CH_SENS_FREQ_RES, 
			  IO_REQUEST, IO_DATA,
			  RECEIVE_ERROR, SEND_ERROR,
			  SET_POWER_PLAN_REQ, SET_POWER_PLAN_RES, 
			  SET_ENCRYPTION_REQ, SET_ENCRYPTION_RES,
			  RESET_PLUVIOMETER}
	ApplicationIDs;

	
typedef enum {SEND_STORED_TEMPERATURES,
			  SEND_STORED_HUMIDITIES,
			  SEND_STORED_PRESSURES,
			  SEND_STORED_BATTERIES,
			  SEND_STORED_CO2S,
			  SEND_STORED_ANEMOS,
			  SEND_STORED_VANES,
			  SEND_STORED_PLUVIOS,
			  SEND_STORED_LUMINOSITIES,
			  SEND_STORED_RADIATIONS}
	SendStoredSampleApplicationIDs; /// enum value += NUM_APP_IDS
	
//!
/*! Contains the errors that can be notified to the gateway via an SEND_ERROR packet
 */			
typedef enum {	// INVALID PACKET RECEIVED
				/* 0:*/ NOT_IN_USE = 0,
				/* 1:*/ NODE_RECEIVED_INVALID_PACKET_OF_TYPE_0_DONT_USE,
				/* 2:*/ NODE_RECEIVED_INVALID_PACKET_OF_TYPE_2_ADD_NODE_RES,
				/* 3:*/ NODE_RECEIVED_INVALID_PACKET_OF_TYPE_4_MASK_RES,
				/* 4:*/ NODE_RECEIVED_INVALID_PACKET_OF_TYPE_6_CH_NODE_FREQ_RES,
				/* 5:*/ NODE_RECEIVED_INVALID_PACKET_OF_TYPE_8_CH_SENS_FREQ_RES,
				/* 6:*/ NODE_RECEIVED_INVALID_PACKET_OF_TYPE_10_IO_DATA,
				/* 7:*/ NODE_RECEIVED_INVALID_PACKET_OF_TYPE_12_SEND_ERROR,
				/* 8:*/	NODE_RECEIVED_INVALID_PACKET_OF_TYPE_14_SET_POWER_PLAN_RESPONSE,
				/* 9:*/	NODE_RECEIVED_INVALID_PACKET_OF_TYPE_16_SET_ENCRYPTION_RESPONSE,
				/*10:*/ NODE_RECEIVED_INVALID_PACKET_OF_TYPE_18_STARTED_RAINING,
				// INVALID PACKET CONTENT RECEIVED
				/*10:*/ NODE_RECEIVED_AN_UNKNOWN_PACKET_TYPE,
				/*11:*/ NODE_RECEIVED_INVALID_MASK_IN_ADD_NODE_REQUEST,
				/*12:*/ NODE_RECEIVED_INVALID_NEW_DEFAULT_SLEEP_TIME,
				/*13:*/ NODE_RECEIVED_EMPTY_SENSOR_MASK_IN_CH_SENS_FREQ_REQ,
				/*14:*/ NODE_HAS_NO_LONGER_ACTIVE_SENSORS,
				/*15:*/ NODE_HAD_AN_ERROR_IN_SET_NEW_DIFFERENT_SLEEP_TIMES,
				/*16:*/ NODE_RECEIVED_A_MASK_THAT_DOESNT_MATCH_PHY_MASK_IN_IO_REQ,
				/*17:*/	INVALID_MASK_RECEIVED_IN_CH_SENS_FREQ_REQ__SENSOR_IS_NOT_IN_THE_NODES_PHY_MASK,
				/*18:*/	INVALID_POWER_PLAN_RECEIVED_IN_SET_POWER_PLAN_REQ,
				/*19:*/	INVALID_ENCRYPTION_MODE_RECEIVED_IN_SET_ENCRYPTION_REQ,
				// PROGRAM ERRORS
				/*20:*/ NODE_HAD_AN_ERROR_IN_XBEEZB_CHANGE_SENSOR_FREQUENCIES,
				/*21:*/ SLEEP_TIME_EXCEEDED,
				/*22:*/	NODE_HAD_AN_ERROR_IN_SENSOR_VALUE_2_CHARS,
				/*23:*/ NODE_HAD_AN_ERROR_IN_PAQ_UTILS_IO_REQUEST_SENDING_SAMPLES,
				/*24:*/ NODE_HAD_AN_ERROR_IN_PAQ_UTILS_IO_REQUEST_MEASURE_SENSORS,
				/*25:*/	NODE_HAD_AN_ERROR_IN_SEND_MEASURED_SENSORS_IN_ONE_OF_THE_INSERTER_FUNPTRS,
				/*26:*/	NODE_HAD_AN_ERROR_IN_SEND_MEASURED_SENSORS_MASK_WAS_EMPTY,
				/*27:*/	NODE_HAD_AN_ERROR_IN_REGISTER_SENSOR_MEASURING_INTERVAL_TIME,
				/*28:*/	NODE_HAD_AN_ERROR_IN_KEEP_SENSOR_VALUES_IN_MEMORY,
				/*29:*/	NODE_HAD_AN_ERROR_IN_MEASURE_SENSORS_RECEIVED_AN_EMPTY_MASK,
				/*30:*/	NODE_HAD_AN_ERROR_IN_GET_MASK_LENGTH,
				//UNAUTHORIZED REQUESTS
				/*31:*/ NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_ADD_NODE_REQUEST,
				/*32:*/	NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_MASK_REQUEST,
				/*33:*/	NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_CH_NODE_FREQ_REQUEST,
				/*34:*/ NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_CH_SENS_FREQ_REQUEST,
				/*35:*/ NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_IO_REQUEST,
				/*36:*/	NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_SET_POWER_PLAN_REQUEST,
				/*37:*/	NODE_RECEIVED_AN_UNAUTHORIZED_REQUEST_OF_SET_ENCRYPTION_REQUEST,
				/*38:*/	NODE_HAS_NOT_BEEN_INVITED_TO_THE_NETWORK_YET___USE_ADD_NODE_REQ,
				//COMMUNICATION ERRORS (saved in EEPROM)
				/*39:*/	ERROR_SETTING_UP_XBEE_MODULE,
				/*40:*/	CHECK_NODE_ASSOCIATION_RETURNED_1_NO_REACHABLE_PARENT_FOUND,
				/*41:*/	CHECK_NODE_ASSOCIATION_RETURNED_2_XBEE_NOT_DETECTED_ON_WASPMOTE,
				/*42:*/	SEND_MEASURED_SENSORS_RETURNED_1_NOT_SENT_DUE_TO_SLEEP,
				/*43:*/	SEND_MEASURED_SENSORS_RETURNED_2_NOT_SENT_OR_MESSAGE_LOST,
				/*44:*/	FATAL_ERROR_NODE_FAILED_THE_RETRY_THE_JOINING_PROCESS_PARENT_UNREACHABLE,
				/*45:*/	NODE_FAILED_TO_SEND_THE_MEASURED_SENSORS_AFTER_A_SUCCESSFULL_RETRY_JOINING,
				//WARNINGS
				/*46:*/	NODE_HAD_TO_RETRY_THE_JOINING_PROCESS__PROBABLY_LOW_RSSI,
				/*47:*/	RAIN_METER_HAS_BEEN_RESET
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
extern uint8_t InsertAnemo(uint8_t *, char *);
extern uint8_t InsertVane(uint8_t *, char *);
extern uint8_t InsertPluvio(uint8_t *, char *);
extern uint8_t InsertLuminosity(uint8_t *, char *);
extern uint8_t InsertSolarRadiation(uint8_t *, char *);

//typedef uint8_t SendStoredSamples( void );




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
	
	
	//! This function is called by the receive function in 'CommUtils.h' if the
	//! received packet is a request to change the node's power plan.
	/*!
	 *  \@post: this function will send an answer with the new power plan back if the 
	 *      received plan was valid else it will send an error message back.
	 *  \param:	a packetXBee which contains the power plan
	 */	
	extern uint8_t Set_Power_Plan_Request(packetXBee *); // APP_ID = 13
	//! SHOULD NEVER BE RECEIVED
	/*!
	 *	\@post: will send an error message to the gateway
	 */		
	extern uint8_t Set_Power_plan_Response(packetXBee *); // APP_ID = 14
	
	
	//! This function is called by the receive function in 'CommUtils.h' if the
	//! received packet is a request to change or set a new frequency for certain sensors
	/*!
	 *	\param: packetXBee which contains a sensor mask and corresponding frequencies
	 *	\@post: the sensors will get individual measure frequencies and the node will
	 *			try to find an efficient way to measure / sleep
	 */		
	extern uint8_t Set_Encryption_Request(packetXBee *);  // APP_ID = 15
	extern uint8_t Set_Encryption_Response(packetXBee *); // APP_ID = 16
	
	
	
	extern uint8_t ResetPluvioMeter(packetXBee *); // APP_ID = 17

	


	


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
		
		//! It sends the measured sensors
		/*! @pre: 'SensUtils.measureSensors(xbeeZB.activeSensorMask)'
		 *  \param 1: The destination address
		 *	\param 2: The mask containing the sensors to be sent
		 *	\return error=3 --> The command has not been executed, mask was empty?
					error=2 --> The sensors could not be sent or the message is lost
					error=1 --> *Without XBee Sleep: The message could not be sent
								*With XBee Sleep: The message hangs in parent buffer and will be 
								 delivered when the end device wakes up
					error=0 --> The command has been executed with no errors
		 */
		uint8_t sendMeasuredSensors(uint8_t *, uint16_t);
		
		//! It sends the sensors 
		uint8_t sendStoredSensors(uint8_t *);
		
		uint8_t sendStoredErrors(uint8_t *);
		
		//! It is called by 'CH_SENS_FREQ_REQ'.
		/*! It inserts the measuring intervals of the sensors found in the mask (param1)
		 *  into the 'packetData' variable.
		 *  It also sets the packet mask.
		 */
		void insertFrequenciesInPacketData(uint16_t mask);
		
		
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
