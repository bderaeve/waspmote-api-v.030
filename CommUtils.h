/* ==============================================================================
 *
 *			THESIS: Design of a Wireless Sensor Networking test-bed
 * 
 * ==============================================================================
 *
 *       Filename:  CommUtils.h
 *    Description:  Extension to the communication methods in 
 *						WaspXBeeCore.h
 *						WaspXBeeZB.h
 *					This class facilitates setting up the XBee from the IDE,
 *				    checking association and sending different types of 
 *					messages, defined in the ApplicationIDs enum in 'PAQUtils.h'
 *
 * =========================================================================== */
#ifndef COMMUTILS_H
#define COMMUTILS_H

#ifndef __WASPXBEECONSTANTS_H__
  #include "WaspXBeeConstants.h"
#endif

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <inttypes.h>
#include "PAQUtils.h"

/******************************************************************************
 * Definitions & Declarations
 ******************************************************************************/
//#define COMM_DEBUG
#define RECEIVE_DEBUG
#define SEND_DEBUG
//#define ASSOCIATION_DEBUG
//#define TIME_DEBUG
#define SEND_MEMORY_LEAK_DEBUG

extern TreatData * myTreatPacket[];  //Declared in 'PacketUtils.h'

//! From our tests it appeared that the delay provided in the
/*! 'checkNodeAssociation(AssociationMode mode)' function influences the time a 
 *  node needs to join a parent.  After setting up a node, a longer delay is 
 *  beneficial. Sometimes it happened the node runned through the 15 or 20 seconds 
 *  provided(with only 100 ms delay in between association checks) without getting 
 *  connected. When providing a longer delay (6000 ms eg) makes it more easy to 
 *  connect after setup. The node would then either connect after the first 6s
 *  or after 12s,  but never run through the whole 15 or 20 seconds maximum time.
 *  When a node has already a parent this longer delay is just a waste of time,
 *  in this case a delay of 100ms is used until either the node connects or the
 *  15/20 seconds time is up.
 */
typedef enum {SETUP, LOOP} 
	AssociationMode;
	
/******************************************************************************
 * Class
 ******************************************************************************/
 
class CommUtils
{
	private:
		#ifdef COMM_DEBUG
			//! It is called by 'CommUtils::discoverNodes()';
			void printScannedNodesInformation();
		#endif
		
		//! It is called by 'CommUtils::receiveMessages(DeviceRole)' 
		/*! It returns if the received data is a valid packet for the program to process
		/*  The types accepted are stored in the ApplicationIDs enum in 'PAQUtils.h'
		 */
		bool isValidPacket(uint8_t *);		
		
		//! It can be called by 'CommUtils::receiveMessages(DeviceRole)'
		/*! In case xbeeZB.encryptionMode == ENCRYPTION_ENABLED it will replace the
		 *  original chars in packetXBee->data with the decrypted chars.
		 */
		void decryptIncomingMessages(packetXBee *);
		
		
	public:
		//! class constructor
		/*!
		 *  It does nothing
		 *  \param void
		 *  \return void
		 */
		CommUtils();
	
	//!//////////////////////////////////////////////////////////////////////////////////////////////////
	//!  NETWORKING 
	//!//////////////////////////////////////////////////////////////////////////////////////////////////
	
		//! This setup function will initialize the XBEE and program with the default
		/*! values described for Group T in 2012-2013. See paper DESIGN OF A WIRELESS SENSOR
		/*	NETWORKING TEST-BED by Bjorn Deraeve and Roel Storms
		 *  \@post : defaultTime2Wake will be set
		 *         : if this call is not the result of a 'retryJoining()' also the RTC time will be set 
		 *  \@post : in case the no parent was found the ERROR_SETTING_UP_XBEE_MODULE error will be
		 *			 saved to EEPROM.		 
		 *  \return: 0 : successfully joined and associated
		 *			 1 : either XBee not present or no coordinator found, use 
		 *				 #define ASSOCIATION_DEBUG
		 */
		uint8_t setupXBee();
		
		
		//! This setup function will initialize the program with the settings given in parameters:
		/*!	\param 0: PAN ID
		 *	\param 1: DEVICE ROLE: END_DEVICE, ROUTER, or COORDINATOR
		 *	\param 2: DEFAULT GATEWAY MAC ADDRESS
		 *	\param 3: SLEEPMODE
		 *	\param 4: NODE IDENTIFIER (the NI must be a 20 character max string)
		 *  \param 5: DEFAULT SLEEPING TIME 
		 *  \param 6: POWER PLAN
		 *  \@post :  If this call is not the result of a 'retryJoining()' also the RTC time will be set 
		 *  \@post :  In case the no parent was found the ERROR_SETTING_UP_XBEE_MODULE error will be
		 *			  saved to EEPROM.
		 *  \return:  0 : successfully joined and associated
		 *			  1 : either XBee not present or no coordinator found, use #define ASSOCIATION_DEBUG
		 */
		uint8_t setupXBee(uint8_t[8], DeviceRole, uint8_t[8], SleepMode, char *, uint16_t, PowerPlan);
		
		
		//! It is called when the node detects it is no longer connected to the network. This 
		/*! is possibly only detected when sending a message fails.
         *	! The result of 'COMM.checkNodeAssociation(AssociationMode)' is not reliable when 
		 *    network parameters are read from XBee memory.
		 *  \return: 0 : successfully joined and associated
		 *			 1 : either XBee not present or no coordinator found, use 
		 *				 #define ASSOCIATION_DEBUG
		 */
		uint8_t retryJoining();		
		
		
		//! It gets and prints the current node association state.
		/*! If necessary the function will wait for 20s until the XBee module has joined
		 *	a network.
		 *  \return error=2 --> XBee not detected on Waspmote
		 * 			error=1 --> No joinable networks found, try doing setupXBee() and check those 
		 *						results (verify if correct PAN ID and CHANNEL!) 
		 *  		error=0 --> The command has been executed with no errors, successfully joined
		 */ 
		uint8_t checkNodeAssociation(AssociationMode);
		
		
			//! It prints the current association state stored in xbeeZB.associationIndication
			#ifdef ASSOCIATION_DEBUG_EXTENDED
				void printAssociationState();
			#endif
		
			//! It prints the current 16-bit operating PAN ID, 64-bit operating PAN ID 
			/*! and operating channel
			 */
			#ifdef ASSOCIATION_DEBUG
				void printCurrentNetworkParams();
			#endif
			
		
			#ifdef COMM_DEBUG
			//! It discovers and reports all the modules found in its current operating PAN ID and CHANNEL
			/*! The results are printed via USB and stored in different fields in 'WaspXBeeCore.h'
			 *	@see: Waspmote ZigBee Networking Guide section 8.5
			 */
			void discoverNodes();	
			#endif

	//!//////////////////////////////////////////////////////////////////////////////////////////////////
	//!  SEND AND RECEIVE PACKETS 
	//!//////////////////////////////////////////////////////////////////////////////////////////////////		
		
		//! It checks for any received messages 
		/*! In debug mode it also stores possible messages in 'receivedData[MAX_DATA]' and prints the content
		/*! If necessary the function will wait for 20 sec until the XBee module has received
		/*!	something.
		/*!!!! IT WILL ALSO CALL THE CORRESPONDING FUNCTION TO DEAL WITH THE INCOMING TYPE OF MESSAGES !!!!!
		  !!!! SO THIS FUNCTION CAN ALSO BE RESPONSIBLE FOR SENDING / ANSWERRING MESSAGES !!!!!!!!!!!!!!!!!!
		  \return  	error=4 --> Error occured and sent to gateway while treating the packet data
					error=3 --> Received invalid packet type
					error=2 --> The command has not been executed
					error=1 --> No data received
					error=0 --> Message received and treated successfully
		*/ 		
		uint8_t receiveMessages(DeviceRole);
		
		
		// error = 2: nothing received
		// error = 3: read error
		//uint8_t receiveTest();
				
		
		//! It sends a packet of type (applicationID) 0: 'ERRORMESSAGE' to destination
		/*!
		  \param destination: the destination MAC address
		  \param message: the message to be sent
		  \return   error=2 --> The command has not been executed / The message could not be sent or is lost
					error=1 --> *Without XBee Sleep: The message could not be sent
								*With XBee Sleep: The message hangs in parent buffer and will be 
								 delivered when the end device wakes up
					error=0 --> The command has been executed with no errors
		*/
		uint8_t sendMessage(uint8_t * destination, const char * message);
		
		
		//! It sends a packet of a certain type (applicationID) to destination
		/*!
		  \param destination: the destination MAC address
		  \param applicationID: the type of the packet to send
		  \param message: the message to be sent
		  \return   error=2 --> The command has not been executed / The message could not be sent or is lost
					error=1 --> *Without XBee Sleep: The message could not be sent
								*With XBee Sleep: The message hangs in parent buffer and will be 
								 delivered when the end device wakes up
					error=0 --> The command has been executed with no errors
		*/
		uint8_t sendMessage(uint8_t * destination, uint8_t, const char * message);		
		
		
		//! It sends a packet of type 12: 'SEND_ERROR' to the default gateway.
		/*! The errors to be sent are stored in the Errors enum in 'PAQUtils.h'
		 */
		uint8_t sendError(Errors);
		uint8_t sendWarning(Errors);
		
		
		//! It replaces the zeros in data by 0xFFFE so it can be sent correctly
		/*! 
		  \param data    : the data where zeros must be replaced
		  \param length  : the length of the original data stream
		  \return char * : the new data with escaped zeros
		 */
		char * escapeZeros(char * data , uint8_t length);
		
		bool retryJoin;
		
		#ifdef RECEIVE_DEBUG
			//! It stores the received data for debugging reasons.
			char receivedData[MAX_DATA];
		#endif
};
 
extern CommUtils COMM;


#endif /*COMMUTILS_H*/
