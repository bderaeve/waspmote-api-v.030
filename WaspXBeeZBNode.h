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
#include "SensorUtils.h"
 
class WaspXBeeZB;


/******************************************************************************
 * Definitions & Declarations
 ******************************************************************************/
#define NODE_DEBUG
//#define NODE_TIME_DEBUG

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
	public:
		//! class constructor
		/*!
		  It saves the standard gateway address and defaultTime2Wake
		  \param void
		  \return void
		 */
		WaspXBeeZBNode(); 
		
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
		void setPhysicalSensorMask(uint16_t *);

		//! It allows to remotely set a sensor mask to a node
		/*! This sensor mask serves to indicate which installed sensors should be measured
		 *  \@post: activeSensorMaskLength will be set
		 */
		void setActiveSensorMask(uint16_t *);
		
		//! Stores the length of the nodes physical sensor mask
		void setPhysicalSensorMaskLength();
	
		//! Stores the length of the nodes active sensor mask
		void setActiveSensorMaskLength();	
		
		void printSensorMask(uint16_t);
		
		
		
		uint8_t setNewSleepTime(uint16_t);
		void convertTime2Wait2Char(uint16_t);
		
		
		//Addressing / Setup
		uint8_t panid[8]; 
		uint8_t GATEWAY_MAC[8]; 
		
		//Sensor
		uint16_t physicalSensorMask;
		uint8_t physicalSensorMaskLength;
		uint16_t activeSensorMask;
		uint8_t activeSensorMaskLength;
		
		
		//Deep sleep / Hibernate
		uint16_t defaultTime2Wake;   
		char time2wake[18];				//"dd:hh:mm:ss"
		
		
		
		
		
		
};

extern WaspXBeeZBNode	xbeeZB;

#endif /*WASPXBEEZBNODE_H*/
