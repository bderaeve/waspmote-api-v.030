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
//#define NODE_DEBUG
//#define HIBERNATE_DEBUG
#define HIBERNATE_DEBUG_V2
#define MATH_DEBUG
//#define MATH_DEBUG_EXTENDED
//#define NODE_MEMORY_DEBUG
//#define NODE_TIME_DEBUG
//#define POWER_SAVER_DEBUG
//#define ADD_NODE_REQ_DEBUG
//! Determines if all the different sleep times could be calculated and stored
/*! or wether the reserved memory is too small and the next values should be 
/*  calculated once the previous ones may be overwritten.
 */
#define EASY 0		/// Stored in EEPROM_READ_MODE
#define DIFFICULT 1

typedef enum {HIGHPERFORMANCE, POWERSAVER} 
	PowerPlan; 

typedef enum {SLEEP, HIBERNATE}
	SleepMode;



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
		
		void readCoreVariablesFromEEPROM();
		
	public:
		//! class constructor
		/*!
		  It saves the standard gateway address and defaultTime2Wake
		  \param void
		  \return void
		 */
		WaspXBeeZBNode(); 
		
		
		void hibernateInterrupt();
		
		uint8_t testVar;
		void testFunc();
		
		
		void setGatewayMacAddress(uint8_t[8]);

/*********************************************************************************************************
 *	NODE SENSOR BOARD LAYOUT FUNCTIONALITY
 ********************************************************************************************************/			

		//! It allows to remotely set a sensor mask to a node
		/*! This sensor mask must correspond to the physical layout of the node and is
		 *! not supposed to change!
		 *  \@post: physicalSensorMaskLength will be set
		 */
		void setPhysicalSensorMask(uint8_t *);
		
		
		//! It allows an installer to set a sensor mask of active sensors to a node via libelium-IDE
		/*! The first argument must be the number of sensor types followed
		 *! the next arguments must be of type SensorType
		 *  \@post:	activeSensorMaskLength will be set
		 */
		void setActiveSensorMask(int, ...);

		
		//! It allows to set the active sensors to be measured at different times and to do this
		//! setings via the libelium IDE
		/*! The first argument must be the number of times (corresponding to the sensor) followed
		 *! The next arguments must be of type int
		 *  \pre: setActiveSensorMask() must be executed with the corresponding sensors
		 */
		uint8_t setActiveSensorTimes(uint16_t, ...);
		
		
		//! It allows to remotely set a sensor mask to a node
		/*! This sensor mask serves to indicate which installed sensors should be measured
		 *  \@post: activeSensorMaskLength will be set
		 */
		void setActiveSensorMask(uint8_t *);
			
		uint8_t getMaskLength(uint16_t);
		uint8_t getNrActiveSensors(uint16_t);

		uint8_t disableSensors(uint16_t *);
		void printSensorMask(uint16_t);
		
/*********************************************************************************************************
 *	DEEPSLEEP / HIBERNATE FUNCTIONALITY
 ********************************************************************************************************/
 
		//! Use this function if you want to set a time2sleep offset via the Waspmote-IDE
		/*!	\param:	SleepMode: HIBERNATE or DEEPSLEEP
		 *  \param: time2sleep: 1 = 10 seconds
		 */
		void enterLowPowerMode(SleepMode, uint16_t);
		
		//! Node will go into hibernate until the RTC alarm goes off. This alarm must have been set
		//! on beforehand!  (normally this is done immediately after waking up in the ifHibernateInterrupt)
		void hibernate();

		
/*********************************************************************************************************
 *	VARIABLE TIMING FUNCTIONALITY
 ********************************************************************************************************/
 
		//! Set a new standard time2sleep for the node
		/*! \@post: all sensors in the active sensor mask will be measured and send
		 *			at this sleep interval
		 *	\@post: defaultOperation = true
		 */		
		uint8_t setNewDefaultTime2Sleep(uint16_t);
		
		//! Change one of more sensor's measuring interval. If the received sensor interval time
		/*! is zero the sensor will be removed from the active sensor mask
		 *	\@post: if not all sensors operate at the same measuring interval defaultOperation = false
		 *  \@post: if !defaultOperation setNewDifferentSleepTimes() is called
		 */
		uint8_t changeSensorFrequencies(char *);
		
		//! This function prepares the values for the algorithm which determines the next times 2 sleep
		//! and then calls that function.  
		uint8_t setNewDifferentSleepTimes();
		
		void createAndSaveNewTime2SleepArray(uint16_t *);
		
		void calculateNextTimes();
		void recalculateTimesFromBeginning();
		
		uint16_t calculateMaxNrElementsForEEPROMArray(uint16_t *);
		
		

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
		uint8_t nrOfPanics;
		
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
		
		uint16_t * time2wValuesArray;
		uint16_t lengthArray;
		uint16_t * sensMultiplier;
		
		SleepMode sleepMode;
		PowerPlan powerPlan;
		

		
		//! Defines if the node operates in default mode (one defaultTime2Wake) or if
		/*! individual sensors have different sleep time settings.
		 */
		bool defaultOperation;
		
		
		bool resetRTC;
		bool eepromReadMode;  // Easy / Difficult
		bool mustCalculateNextTimes;
		bool lastArrayOfValues;
		
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

uint16_t toUint16_t(char &, char &);
uint16_t toUint16_t(char *);

extern WaspXBeeZBNode	xbeeZB;

#endif /*WASPXBEEZBNODE_H*/
