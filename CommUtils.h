/* ==========================================================================
 *
 *			THESIS: Design of a Wireless Sensor Networking test-bed
 * 
 * ==========================================================================
 *
 *       Filename:  CommUtils.h
 *    Description:  Extension to the communication methods in 
 *						WaspXBeeCore.h
 *						WaspXBeeZB.h
 *
 * ======================================================================= */
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
//#define ASSOCIATION_DEBUG
//#define TIME_DEBUG

extern TreatData * myTreatPacket[];  //Declared in 'PacketUtils.h'

typedef enum {SETUP, LOOP} 
	AssociationMode;


/******************************************************************************
 * Class
 ******************************************************************************/
 
class CommUtils
{
	private:
		
	public:
		//! class constructor
		/*!
		  It does nothing
		  \param void
		  \return void
		 */
		CommUtils(){};
		
		#ifdef COMM_DEBUG
			void testPrinting();
		#endif
		
		//! This setup function will initialize the XBEE and program with the default
		/*! values described for Group T in 2012-2013. See paper DESIGN OF A WIRELESS SENSOR
		/*	NETWORKING TEST-BED by Bjorn Deraeve and Roel Storms
		 *  \@post : defaultTime2Wake will be set
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
		 *  \@post : defaultTime2Wake will be set
		 *  \return: 0 : successfully joined and associated
		 *			 1 : either XBee not present or no coordinator found, use 
		 *				 #define ASSOCIATION_DEBUG
		 */
		uint8_t setupXBee(uint8_t[8], DeviceRole, uint8_t[8], SleepMode, char *);
		
		
		//! It gets and prints the current node association state.
		/*! If necessary the function will wait for 20s until the XBee module has joined
		/*!	a network.
		  \return  	error=2 --> XBee not detected on Waspmote
					error=1 --> No joinable networks found, try doing setupXBee() and check those 
								results (verify if correct PAN ID and CHANNEL!) 
					error=0 --> The command has been executed with no errors, successfully joined
		*/ 
		uint8_t checkNodeAssociation(AssociationMode);
		
		
		//! It prints the current association state stored in xbeeZB.associationIndication
		#ifdef ASSOCIATION_DEBUG
			void printAssociationState();
		#endif
		
		
		//! It prints the current 16-bit operating PAN ID, 64-bit operating PAN ID 
		//! and operating channel
		void printCurrentNetworkParams();
		
		
		//! It checks for any received messages 
		/*! In debug mode it also stores possible messages in 'receivedData[MAX_DATA]' and prints the content
		/*! If necessary the function will wait for 20 sec until the XBee module has received
		/*!	something.
		/*!!!! IT WILL ALSO CALL THE CORRESPONDING FUNCTION TO DEAL WITH THE INCOMING TYPE OF MESSAGES !!!!!
		  !!!! SO THIS FUNCTION CAN ALSO BE RESPONSIBLE FOR SENDING / ANSWERRING MESSAGES !!!!!!!!!!!!!!!!!!
		  \return  	error=2 --> The command has not been executed
					error=1 --> No data received
					error=0 --> Message received successfully
		*/ 		
		uint8_t receiveMessages();
		
		uint8_t receiveTest();
		
		//! It sends a packet of type (applicationID) 0: 'ERRORMESSAGE' to destination
		/*!
		  \param destination: the destination MAC address
		  \param message: the message to be sent
		  \return   error=2 --> The command has not been executed
					error=1 --> The message could not be sent
					error=0 --> The command has been executed with no errors
		*/
		uint8_t sendMessage(uint8_t * destination, const char * message);
		
		
		//! It sends a packet of a certain type (applicationID) to destination
		/*!
		  \param destination: the destination MAC address
		  \param applicationID: the type of the packet to send
		  \param message: the message to be sent
		  \return   error=2 --> The command has not been executed
					error=1 --> The message could not be sent
					error=0 --> The command has been executed with no errors
		*/
		uint8_t sendMessage(uint8_t * destination, uint8_t, /*const*/ char * message);		
		
		char * escapeZeros(char *, uint8_t);
		
		#ifdef RECEIVE_DEBUG
			//! It stores the received data for debugging reasons.
			char receivedData[MAX_DATA];
		#endif
};
 
extern CommUtils COMM;


#endif /*COMMUTILS_H*/
