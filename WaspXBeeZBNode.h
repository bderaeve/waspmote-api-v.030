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

#define NODE_DEBUG_V2

//#define HIBERNATE_DEBUG
#define HIBERNATE_DEBUG_V2
#define HIBERNATE_DEBUG_V3
#define MATH_DEBUG
//#define SLEEP_DEBUG
//#define DEEPSLEEP_DEBUG
//#define TEST_DIFFICULT_DEBUG

//#define MATH_DEBUG_EXTENDED
//#define NODE_MEMORY_DEBUG
//#define NODE_TIME_DEBUG
//#define POWER_SAVER_DEBUG

#define ADD_NODE_REQ_DEBUG

//! Determines if all the different sleep times could be calculated and stored
/*! or wether the reserved memory is too small and the next values should be 
/*  calculated once the previous ones may be overwritten.
 */
#define EASY 0		/// Stored in EEPROM_READ_MODE
#define DIFFICULT 1

typedef enum{END_DEVICE, ROUTER, COORDINATOR}
	DeviceRole;

typedef enum {HIGHPERFORMANCE, POWERSAVER} 
	PowerPlan; 

typedef enum {SLEEP, DEEPSLEEP, HIBERNATE, NONE}
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

		//! It saves the device role in global 'deviceRole' 
		/*!   0=END_DEVICE,  1=ROUTER,  2=COORDINATOR
		 *    It is supposed the device type is correctly set via X-CTU (function set)
		 *    However, it gives the possibility to downgrade a Router to an End Device
		 */
		uint8_t getDeviceRole();
		
		//! It stores the device role in global 'deviceRole'
		/*!   Assumes that the device type is correctly set via X-CTU 
		 *		(Flash the XBee to different function set)
		 *    0=END_DEVICE,  1=ROUTER,  2=COORDINATOR
		 */
		uint8_t setDeviceRole(DeviceRole);
 
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
		/*! The first argument must be the number of arguments (corresponding to the sensor) followed by
		 *  The first sensor and its corresponding time
		 *  The second sensor and its corresponding time
		 *  ...
		 */
		uint8_t setActiveSensorMaskWithTimes(uint16_t, ...);
		
		
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
 
		//! Use this function if you want to set a time2sleep OFFSET via the Waspmote-IDE
		//! So this function ignores variable sleep times
		/*!	\param:	SleepMode: HIBERNATE or DEEPSLEEP
		 *  \param: time2sleep: 1 = 10 seconds
		 */
		void enterLowPowerMode(SleepMode, uint16_t);
		
		//! Node will go into HIBERNATE / (DEEP) SLEEP for the time specified. 
		/*! Function will check if node is in defaultOperatio or not.
		 *  \@pre: This time must have been set on beforehand!!!!
		 *  \@post: The next time2wake will be set in this function for SLEEP or DEEPSLEEP
		 *	\@post: The next time2wake should be set in the hibernateInterrupt()
		 */
		void enterLowPowerMode(SleepMode);
		
		//! Node will go into hibernate until the RTC alarm goes off. This alarm must have been set
		//! on beforehand!  (normally this is done immediately after waking up in the ifHibernateInterrupt)
		void hibernate();
		
		void findNextTime2Wake(SleepMode);

		
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
		uint8_t setNewDifferentSleepTimes(bool);
		
		void createAndSaveNewTime2SleepArray(uint16_t *);
		
		void calculateNextTimes();
		void recalculateTimesFromBeginning();
		
		uint16_t calculateMaxNrElementsForEEPROMArray(uint16_t *);
		
		//! In case of SLEEP / DEEPSLEEP this function is used to determine the correct
		//! position in 'time2wValuesArray'
		void updatePosInArray();

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
		uint8_t deviceRole;
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
		uint16_t lengthArray;	/// NR OF TIME2WAIT ENTRIES IN TIME2WVALUESARRAY
		uint16_t posInArray;  	/// USED BY SLEEP / DEEPSLEEP MODE INSTEAD OF EEROM
		uint16_t * sensMultiplier;
		uint16_t storedTime;
		uint16_t LCM;
		bool arrayReadMode;  	/// EASY / DIFFICULT
		bool lastArrayContainedLCM; 
		
		bool resetRTC;
		
		
		SleepMode sleepMode;	/// SLEEP / DEEPSLEEP / HIBERNATE 
		PowerPlan powerPlan;	/// MEASURE AND SEND OR ONLY MEASURE
		

		
		//! Defines if the node operates in default mode (one defaultTime2Wake) or if
		/*! individual sensors have different sleep time settings.
		 */
		bool defaultOperation;
		
		


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
