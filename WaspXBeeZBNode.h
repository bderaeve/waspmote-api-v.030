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


#define xstr(s) str(s)
#define str(s) #s

//#define ToUint16(param) ( (uint16_t) ((  param[0] ) * 256 ) + (param[1]) )
	
#define MUST_NOT_STORE_ON_EEPROM(where, what) ( what == Utils.readEEPROM(where) )
	
	
/******************************************************************************
 * Includes
 ******************************************************************************/
#include "WaspXBeeZB.h" 
#include "PAQUtils.h"
 
/******************************************************************************
 * Definitions & Declarations
 ******************************************************************************/
//#define NODE_DEBUG

#define mess "test"
#define FreeMem "Free Memory: "


//#define NODE_DEBUG_V2
#define SLEEP_DEBUG
//#define DEEPSLEEP_DEBUG
//#define DEEPSLEEP_DEBUG_V2
//#define HIBERNATE_DEBUG
#define HIBERNATE_DEBUG_V2
//#define HIBERNATE_DEBUG_V3
#define WASPMOTE_SLEEP_MODE_DEBUG

#define FINAL_DEBUG
#define FINAL_USB_DEBUG

//#define MATH_DEBUG

//#define TEST_DIFFICULT_DEBUG

//#define MATH_DEBUG_EXTENDED
//#define NODE_MEMORY_DEBUG
//#define NODE_TIME_DEBUG
//#define POWER_SAVER_DEBUG

//#define ADD_NODE_REQ_DEBUG
//#define MASK_REQ_DEBUG
#define CH_SENS_FREQ_REQ_DEBUG



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

typedef enum {XBEE_SLEEP_ENABLED, XBEE_SLEEP_DISABLED}
	XBeeSleepMode;
	
typedef enum{ENCRYPTION_DISABLED, ENCRYPTION_ENABLED}
	EncryptionMode;

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
	
		//! It reads the necessary program parameters from EEPROM when a hibernate interrupt
		/*! is detected
		 */
		void readCoreVariablesFromEEPROM();
		
	
		//! It is called by 'setPhysicalSensorMask(uint8_t *)'
		/*! Stores the length of the nodes physical sensor mask into 'physicalSensorMaskLength'
		 */
		void setPhysicalSensorMaskLength();
		
		
		//! Stores the length of the nodes active sensor mask. It also stores how many
		/*! sensors of the 'activeSensorMask' are active
		 */
		void setActiveSensorMaskLength();
		
		
		//! It returns which bit is the MSB of a given sensor mask. So it gives the
		/*! length of a certain mask.
		 */
		uint8_t getMaskLength(uint16_t);
		
		
		//! It returns how many sensors of a given sensor mask are active.
		uint8_t getNrActiveSensors(uint16_t);
		
		
		//! It is called by 'changeSensorFrequencies(char *)'. 
		/*! The parameters contains the mask indicating which sensors must be disabled. 
		 *  @post : The function will update the 'activeSensorMask' and 'activeSensorMaskLength'
		 *        : If there are no longer sensors active with different measuring intervals 
		 *          'defaultOperation' will be re-enabled. Otherwise 'changeSensorFrequencies(char *)'
		 *          will calculate the necessary intervals.
		 */
		uint8_t disableSensors(uint16_t *);

		
	public:
		//! class constructor
		/*!
		  It saves the standard gateway address and defaultTime2Wake
		  \param void
		  \return void
		 */
		WaspXBeeZBNode(); 
		void printStoredErrors();
		
		void hibernateInterrupt();

		void setAlarmForRouter();
		
		void setGatewayMacAddress(uint8_t[8]);
		
		
		//void setPanID(uint8_t[8]);

/*********************************************************************************************************
 *	NODE SENSOR BOARD LAYOUT FUNCTIONALITY
 ********************************************************************************************************/			

		//! It saves the device role in global 'deviceRole' 
		/*!   0=END_DEVICE,  1=ROUTER,  2=COORDINATOR
		 *    It is supposed the device type is correctly set via X-CTU (function set)
		 *    However, it gives the possibility to downgrade a Router to an End Device remotely
		 */
		uint8_t getDeviceRole();
		
		
		//! It stores the device role in global 'deviceRole'
		/*!   Assumes that the device type is correctly set via X-CTU 
		 *		(Flash the XBee to different function set)
		 *    0=END_DEVICE,  1=ROUTER,  2=COORDINATOR
		 */
		uint8_t setDeviceRole(DeviceRole);
		
		
		//! It sets the node's powerplan to the received mode.
		/*! \return 0 : executed successfully
		 *  \return 1 : received invalid new power plan
		 */
		uint8_t setPowerPlan(PowerPlan);
		
		
		//! This function checks if the current HIGHPERFORMANCE mode is  the actual 
		/*! mode as set in the node or if it is the result of a request to send the saved
		 *  sensors from POWERSAVER mode. In the latter case it will make sure the next 
		 *  cycle will be executed again as POWERSAVER mode.
		 */
		void verifyPowerPlan();
		
		
		//! It sets the node's encryption mode to the received mode.
		/*! \return 0 : executed successfully
		 *  \return 1 : received invalid new encryption mode
		 */		
		uint8_t setEncryptionMode(EncryptionMode);
 
 
		//! It allows to REMOTELY set a sensor mask to a node
		/*! This sensor mask must correspond to the physical layout of the node and is
		 *  not supposed to change!
		 *  The function will check if the received mask is valid for this type of node
		 *  according to the settings of 'SensUtils.acceptedSensorMask' in order to avoid
		 *  program instabilities.
		 *  This function is invoked when an ADD_NODE_REQ command is received
		 *  \@post: physicalSensorMaskLength will be set
		 *  \return 2 : Function has not been executed
		 *			1 : Parameter was an invalid sensor mask
		 *			0 : Successfully executed
		 */
		uint8_t setPhysicalSensorMask(uint8_t *);
		
		
		//! It allows an installer to set a sensor mask of active sensors to a node VIA-LIBELIUM-IDE
		/*! The first argument must be the number of sensor types followed
		 *! The next arguments must be of type SensorType
		 *  \@post:	activeSensorMaskLength will be set
		 *  \@post: this mask will also be set as the node's physical sensor mask
		 */
		void setActiveSensorMask(int, ...);

		
		//! It allows to set sensors to be measured at different times VIA-LIBELIUM-IDE
		/*! The first argument must be the number of arguments (corresponding to the sensor) followed by
		 *  The first sensor and its corresponding time
		 *  The second sensor and its corresponding time
		 *  ...
		 *  \@post:	activeSensorMaskLength will be set
		 *  \@post: this mask will also be set as the node's physical sensor mask		 
		 */
		uint8_t setActiveSensorMaskWithTimes(uint16_t, ...);
		
		
		//! It allows to remotely set an active sensor mask to a node
		/*! This sensor mask serves to indicate which installed sensors should be measured
		 *  The function will check if the received mask is valid, comparing it to the node's
		 *  physical sensor layout
		 *  \@post: activeSensorMaskLength will be set
		 *  \return 2 : Function has not been executed
		 *			1 : Parameter was an invalid sensor mask
		 *			0 : Successfully executed		 
		 */
		uint8_t setActiveSensorMask(uint8_t *);
		


		void printSensorMask(uint16_t);
		
/*********************************************************************************************************
 *	DEEPSLEEP / HIBERNATE FUNCTIONALITY
 ********************************************************************************************************/
 
		void storeProgramParametersToEEPROM();
		
/*********************************************************************************************************
 *	VARIABLE TIMING FUNCTIONALITY
 ********************************************************************************************************/
 
 
 		void findNextTime2Wake(SleepMode);		
 
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
		void testMemory();
		void testMemory2();
		void testMemory3();
		
		//Store a value into the EEPROM memory
		/**
 *	!!!  EEPROM addresses from 0 to 291 are used by Waspmote to save important 
 *	!!!  data, so they must not be over-written. Thus, the available storage
 *	!!!	 addresses go from 292 to 4095. 
 */
		uint8_t storeValue(int, uint8_t);
		uint8_t storeValueADT(int, uint8_t);
		

		void storeError(Errors);
		
		
		//Addressing / Setup
		
		///  NODE SETUP - XBEE SETUP /////////////////////////////////////////////////////
		
		
		//uint8_t panid[8]; 
		
		//!
		/*! Contains the address of the gateway to send to
		 */				
		uint8_t GATEWAY_MAC[8]; 
		
		uint8_t ACCEPTED_GATEWAY_BJORN[4];
		uint8_t ACCEPTED_GATEWAY_ROEL[4];
		
		
		//!
		/*! Contains the node's device role - not necessary equal to the one
		 *  stored in the XBee / ZigBee profile, but for Waspmote sleep settings
		 */		
		DeviceRole deviceRole;
		
		
		uint8_t nrOfPanics;
		
		
		///  SENSORS ////////////////////////////////////////////////////////////////////
		
		//!
		/*! Contains the nodes physical sensor layout
		 */		
		uint16_t physicalSensorMask;
		
		
		//!
		/*! Contains the length of the node's physical sensor mask
		 */		
		uint8_t physicalSensorMaskLength;
		
		
		//!
		/*! Contains the nodes active sensors which can be measured at different times
		 */		
		uint16_t activeSensorMask;
		

		//!
		/*! Contains the length of the node's active sensor mask
		 */		
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
	
		//!
		/*! Stores the Waspmote's sleep mode
		 *  Options are: SLEEP, DEEPSLEEP, HIBERNATE, NONE
		 */			
		SleepMode sleepMode;
		
		
		//!
		/*! Stores the Waspmote's power plan
		 *	Options are: 
		 *    -> HIGHPERFORMANCE : the Waspmote wakes, measures, sends and checks for
		 *						   commands / requests
		 *    ->POWERSAVER : the Waspmote wakes, measures and goes back to sleep x times
		 *		 The x+1 th time the Waspmote wakes, measures, sends the x+1 samples
		 *		 and checks for commands / requests
		 *		 Samples are stored in RAM or EEPROM depending on sleepmode		
		 */
		PowerPlan powerPlan;
		
		
		//! Defines if the node operates in default mode (one defaultTime2Wake) or if
		/*! individual sensors have different sleep time settings.
		 */
		bool defaultOperation;
		
		
		//! Defines if the node has been invited to the network yet via an ADD_NODE_REQ
		/*! As long as the node is not invited its sleeping time will be increased
		 *  in order not to waste battery power
		 */
		bool inNetwork;
		
		
		//! 
		/*! Saves how long a node will sleep when it is not yet invited to the network
		 */
		uint8_t notInNetworkNrMinutesToSleep;

		bool mustSendSavedSensorValues;
		
		bool mustCalculateNextTimes;
		bool lastArrayOfValues;
		
		uint8_t nrSleepTimes;
		uint16_t * sleepTimes;


		uint16_t hibernateCycleNumber;
		
	
		uint8_t number_of_stored_errors;
		
		EncryptionMode encryptionMode;
};


//! The following functions should not be accessible from the libelium IDE 
//! so they can be static:

int compare(const void *, const void *);
int gcd(int, int);
uint16_t lcm(uint16_t, uint16_t);

//uint16_t toUint16_t(char &, char &);
//uint16_t toUint16_t(char *);

extern WaspXBeeZBNode	xbeeZB;

#endif /*WASPXBEEZBNODE_H*/
