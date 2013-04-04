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
#ifndef WASPXBEEZBNODE_H
#define WASPXBEEZBNODE_H

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "WaspXBeeZB.h" 
 

/******************************************************************************
 * Definitions & Declarations
 ******************************************************************************/
#define NODE_DEBUG
#define HIBERNATE_DEBUG
#define MATH_DEBUG
//#define NODE_MEMORY_DEBUG
//#define NODE_TIME_DEBUG

#define EASY 0
#define DIFFICULT 1

/******************************************************************************
 * Class
 ******************************************************************************/
 

//! WaspXBeeZBNode Class
/*!
	WaspXBeeZBNode Class inherits from 'WaspXBeeZB' which defines the necessary
	variables and functions to manage XBee ZB modules.
	
	This class adds some extra functionality and facilitates the use of the functions
	in the libelium Waspmote IDE.
	
	For consistency with other API functions reasons the object 'xbeeZB' of the 
	parent class will be removed and introduced here.
 */
class WaspXBeeZBNode : public WaspXBeeZB
{
	private:
		//! Stores the length of the nodes physical sensor mask
		void setPhysicalSensorMaskLength();
		//! Stores the length of the nodes active sensor mask
		void setActiveSensorMaskLength();
		
		void readEEPROMVariables();
		
	public:
		//! class constructor
		/*!
		  It saves the standard gateway address and defaultTime2Wake
		  \param void
		  \return void
		 */
		WaspXBeeZBNode(); 
		
		
		void hibernateInterrupt();
		
	
		
		
		void setGatewayMacAddress(uint8_t[8]);
		
		//! It allows an installer to set a sensor mask of active sensors to a node via libelium-IDE
		/*! The first argument must be the number of sensor types followed
		 *! the next arguments must be of type SensorType
		 *  \@post:	activeSensorMaskLength will be set
		 */
		void setActiveSensorMask(int, ...);
		
		
		//! It allows to remotely set a sensor mask to a node
		/*! This sensor mask must correspond to the physical layout of the node and is
		 *! not supposed to change!
		 *  \@post: physicalSensorMaskLength will be set
		 */
		void setPhysicalSensorMask(uint8_t[2]);

		//! It allows to remotely set a sensor mask to a node
		/*! This sensor mask serves to indicate which installed sensors should be measured
		 *  \@post: activeSensorMaskLength will be set
		 */
		void setActiveSensorMask(uint8_t[2]);
		uint8_t getMaskLength(uint16_t);
		uint8_t getNrActiveSensors(uint16_t);

		uint8_t disableSensors(uint16_t *);
		void printSensorMask(uint16_t);
		
		
		
		//! Node will go into hibernate for 'xbeeZB.defaultTime2Wake'
		void hibernate();
		
		uint8_t setNewDefaultTime2Sleep(uint16_t);
		
		
///CHANGE SENSOR FREQUENCIES
		
		uint8_t changeSensorFrequencies(char *);
		
		uint8_t setNewDifferentSleepTimes();
		
		void createAndSaveNewTime2SleepArray(int, uint16_t *);
		
		uint16_t calculateMaxNrElementsForEEPROMArray(uint16_t *);
		
		void convertTime2Wait2Char(uint16_t, char *);
	/*	
		static int compare(const void *, const void *);
		static int gcd(int, int);
		static int lcm(int, int);
	*/	
		void testPrinting();
		
		//Store a value into the EEPROM memory
		/**
 *	!!!  EEPROM addresses from 0 to 291 are used by Waspmote to save important 
 *	!!!  data, so they must not be over-written. Thus, the available storage
 *	!!!	 addresses go from 292 to 4095. 
 */
		uint8_t storeValue(int, uint8_t);
		
		
		
		
		
		
		//Addressing / Setup
		uint8_t panid[8]; 
		uint8_t GATEWAY_MAC[8]; 
		
		//Sensor
		uint16_t physicalSensorMask;
		uint8_t physicalSensorMaskLength;
		uint16_t activeSensorMask;
		uint8_t activeSensorMaskLength;
		
				//!
		/*! Stores how much of the nodes sensors are activated
		 */	
		uint8_t nrActivatedSensors;
		
		
		//Deep sleep / Hibernate
		uint16_t defaultTime2WakeInt;   
		char defaultTime2WakeStr[18];		//"dd:hh:mm:ss"
		
		// Defines if the node operates in default mode (one time2wake) or if
		// individual sensors have different sleep time settings.
		bool defaultOperation;
		
		uint8_t nrSleepTimes;
		uint16_t * sleepTimes;
		
		//uint16_t * time2wValuesArray;
		//uint16_t factor;
		uint16_t hibernateCycleNumber;
			
		
};


//! The following functions should not be accessible from the libelium IDE 
//! so they can be static:

int compare(const void *, const void *);
int gcd(int, int);
uint16_t lcm(uint16_t, uint16_t);


extern WaspXBeeZBNode	xbeeZB;

#endif /*WASPXBEEZBNODE_H*/
