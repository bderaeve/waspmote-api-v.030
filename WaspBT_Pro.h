/* 
    brief Library for managing bluetooth modules
    
    Copyright (C) 2009 Libelium Comunicaciones Distribuidas S.L.
    http://www.libelium.com
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2.1 of the License, or
    (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
  
    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
  
    Version:		0.032

    Design:		David Gascón

    Implementation:	Javier Siscart

 */
 
#ifndef WaspBT_Pro_h
#define WaspBT_Pro_h

/******************************************************************************
 * Includes
 ******************************************************************************/
 
#include <inttypes.h>

/******************************************************************************
 * Definitions & Declarations
 ******************************************************************************/

/***********OPERATION MODES************/
// Uncomment to enable debug mode
//#define DEBUG_MODE

// uncomment to enable date and time in each device
#define ENABLE_DATE_AND_TIME

// uncomment to enable friendly name features
#define ENABLE_FRIENDLY_NAME

// General constants
#define BT_BLUEGIGA_RATE	38400		// Default baudrate
#define PORT_USED   1				// UART used
#define DEFAULT_MAX_INQUIRY_RESULTS	250	
#define RX_BUFFER	64			
#define BLOCK_SIZE 	32			// Bytes per block used reading UART
#define BLOCK_MAC_SIZE	17			// Bytes per block used to parse Macs
#define COMMAND_SIZE	40
#define BT_ON		0
//#define BT_SLEEP	1
#define BT_OFF		2
#define BT_PW   42				// Bluetooth pin
#define TOTAL			"Total: "
#define ERRORSD1		"errSD1"	// Error writting.

// File names definitions
#define INQFILE 		"Devices.txt"
#define INQFILEHEAD     	"Discovered devices"
#define HANDSFREEFILE		"Handsfree.txt"
#define HANDSFREEFILEHEAD	"Discovered handsfree"
#define MOBILEFILE		"Mobile.txt"
#define MOBILEFILEHEAD		"Discovered mobile phones"
#define OTHERSFILE		"Others.txt"
#define OTHERSFILEHEAD		"unclassified devices"
#define ENDSTRING		"---"


//TX power levels allowed for WT12
#define TX_POWER_0	-27		//-27dBm; minimum value
#define TX_POWER_1	-22	
#define TX_POWER_2	-17
#define TX_POWER_3	-12
#define TX_POWER_4	-7
#define TX_POWER_5	-2
#define TX_POWER_6	3
#define TX_POWER_DEFAULT_WT12	3	//default factory value			
#define TX_POWER_MAX_WT12	3	// maximum value


/******************************************************************************
 * Class
 ******************************************************************************/
 
//! WaspBT_Pro Class
/*!
	WaspBT_Pro Class defines all the variables and functions used for managing the Bluetooth Board
 */
class WaspBT_Pro
{
	private:
	
	//! Variable : integer used as counter
    	/*!
	 */
	uint16_t i;

	//! Variable : integer used to store TX power
    	/*! seven defined TX power leves. txPower = 0 for minimum power and txPower = 6 for maximum
	 */
	int8_t txPower;

	//! Variable : MAC address
    	/*!
	 */
	char mac_address[BLOCK_MAC_SIZE+1];
	
	//! Variable : class of device
    	/*!
	 */
	char CoD[7];
	
	//! Variable : RSSI
    	/*!
	 */
	char RSSI[4];	

	//! Variable : array for storing the data received from the module
    	/*!
	 */
	uint8_t commandAnswer[RX_BUFFER];

	//! Variable : array for storing command to be sent to the module.
    	/*! 
	 */
	char theCommand[COMMAND_SIZE];

	//! Variable : baud rate used to communicate with the Bluetooth module
    	/*!
		By default, it is 38400bps
	 */
	uint16_t _baudRate;

	//! Variable : UART used by the Bluetooth module
    	/*!
		Bluetooth module is connected to the UART1
	 */
	uint8_t	_uart;
	
	//! Variable : power mode
    	/*!
		Possible values are: BT_ON, BT_SLEEP, BT_OFF	
	 */
	uint8_t _pwrMode;	

	//! Variable : stores identifier address.
    	/*!
	*/
	int  mem_addr;

	//! Variable : stores waspmote identifier (8 char length).
    	/*!
	 */
	char identifier[9];

	//! Variable : Stores number of lines of devices file before inquiry
    	/*!
	 */
	uint32_t numLinesBefore;

	//! Variable : Stores number of lines of devices file after inquiry.
    	/*!
	 */
	uint32_t numLinesAfter;

	//! Variable : Stores number of lines of mobile file before inquiry
    	/*!
	 */
	uint32_t mobNumLinesBefore;

	//! Variable : Stores number of lines of mobile file after inquiry.
    	/*!
	 */
	uint32_t mobNumLinesAfter;

	//! Variable : Stores number of lines of handsfree file before inquiry
    	/*!
	 */
	uint32_t handsNumLinesBefore;

	//! Variable : Stores number of lines of handsfree file after inquiry.
    	/*!
	 */
	uint32_t handsNumLinesAfter;
	
	//! Set power saving mode of bluetooth device.  
    	/*!
	 */
	uint8_t setMode(uint8_t pwrMode);

	//! It looks for specific text in module answer.
    	/*!
	 */
	uint16_t lookForAnswer(uint8_t* data, const char* expectedAnswer);

	//! It looks for specific text in module answer.
    	/*!
	 */
	uint16_t lookForAnswer(char* data, const char* expectedAnswer);

	//! Sends a command to bluetooth module. 
    	/*!
	 */
	void sendCommand(const char* theText);

	//! Reads command answer and saves it. 
    	/*!
	 */
	void readCommandAnswer();
	
	//! Change Inquiry TX radio power.
    	/*!
	 */
	void changeInquiryPower(int8_t power);
#ifdef ENABLE_FRIENDLY_NAME
	//! Looks for friendly names and stores in device array. 
    	/*!
	 */
	void parseNames();
#endif

	//! Saves Mac, CoD and RSSI of a discovered device.
    	/*!
	 */
	uint8_t parseBlock(char* block);

	//! Reads UART while inquiry is being answered.
    	/*!
	 */
	void waitInquiryAnswer(long inquiryTime, uint16_t MAX_DEVICES, bool name, bool limited); 

#ifdef ENABLE_DATE_AND_TIME
	//! Read nodeID from EEPROM and Date from RTC. Store them for each discovered device.
    	/*!
	 */
	void getSetDateID();
#endif

	//! Parses module answer trying to find specified MAC, stops inquiry when mac is found. if dewvice is found, saved on position 0.
    	/*!
	 */
	bool  waitScanDeviceAnswer(long inquiryTime, char* mac);

	//! Sets time to be waiting for inquiry answer.
    	/*!
	 */
	uint16_t setInquiryTime(uint16_t time);

	//! It cancels currently inquiry.
    	/*!
	 */
	void scanNetworkCancel();

	//! Erase corresponding files on SD.
    	/*!
	 */
	void eraseSDFiles();

	//! Create necessary files on SD.
    	/*!
	 */
	uint8_t createSDFiles();


#ifdef DEBUG_MODE
	//! Test purposes only.
    	/*!
	 */
	void printBuffer();

	//! Test purposes only.
    	/*!
	 */
	void printBuffer2();

#endif

	public:

	//! class constructor
  	/*!
	  It does nothing
	  \param void
	  \return void
	 */
	WaspBT_Pro();
	
	//! Variable : Stores number of discovered devices during an inquiry
    	/*!
	 */
	uint16_t numberOfDevices;

	//! Variable : Stores number of discovered handsfree during an inquiry
    	/*!
	 */
	uint8_t handsCounter;

	//! Variable : Stores number of discovered mobile phones during an inquiry
    	/*!
	 */
	uint8_t mobileCounter;

	//! Variable : Stores number of unclassified discovered devices during an inquiry
    	/*!
	 */
	uint8_t othersCounter;
	
	//! Variable : Stores temperature measured by bluetooth module 
    	/*!
	 */
	char temperature[4];

	//! It opens the UART and powers the module
    	/*!
	 */
	void ON();
	
	//! It closes the UART and powers off the module
    	/*!
	 */
	void OFF();

	//! Initializes some parameters of bluetooth module
    	/*!
	 */
	void init(char * ID);
	
	//! It resets the module
    	/*!
	 */
	uint8_t reset();
	
	//! It scans looking for devices. 
    	/*!
	 */
	int8_t scanNetwork(uint8_t time, int8_t power);

#ifdef ENABLE_FRIENDLY_NAME
	//! It scans looking for devices, including friendly name. 
    	/*!
	 */
	int8_t scanNetworkName(uint8_t time, int8_t power);
#endif	

	//! It scans network and stops when finds "MAX_DEVICES".
    	/*!
	 */
	int8_t scanNetworkLimited(uint16_t MAX_DEVICES, int8_t power);

	//! It scans looking for devices showing RSSI
    	/*!
	 */
	int16_t scanDevice(char* Mac, uint8_t maxTime, int8_t power);

	//! Reads temperature from internal module sensor
    	/*!
	 */
	long getTemp();

	//! Prints data of last inquiry
    	/*!
	 */
	uint8_t printInquiry();

	//! Prints mobiles of last inquiry
    	/*!
	 */
	uint8_t printMobiles();

	//! Prints handsfree of last inquiry
    	/*!
	 */
	uint8_t printHandsFree();

	//! Classify devices of last inquiry according CoD and save clasified devices on corresponding SD file.
    	/*!
	 */
	uint8_t classifyDevices();
				
};

extern WaspBT_Pro BT_Pro;

#endif

