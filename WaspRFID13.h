/*! \file WaspRFDI.h

    \brief Library for managing the RFID module PN532
    Copyright (C) 2012 Libelium Comunicaciones Distribuidas S.L.
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

    Version:		0.1
    Design:			David Gascón
    Implementation:	Ahmad Saad, Javier Solobera
*/
 
/*! \def WaspACC_h
    \brief The library flag
    
 */
#ifndef WaspRFDI_h
#define WaspRFDI_h

/******************************************************************************
 * Definitions & Declarations
 ******************************************************************************/
#define PREAMBLE 0x00
#define STARTCODE1 0x00
#define STARTCODE2 0xFF
#define POSTAMBLE 0x00
#define HOSTTOPN532 0xD4
#define FIRMWAREVERSION 0x02
#define GETGENERALSTATUS 0x04
#define SAMCONFIGURATION  0x14
#define INLISTPASSIVETARGET 0x4A
#define INDATAEXCHANGE 0x40
#define MIFARE_READ 0x30
#define MIFARE_WRITE 0xA0
#define AUTH_WITH_KEYA 0x60

#define SOCKET0		0
#define SOCKET1		1

//! PN532 Commands
#define GETFIRMWAREVERSION (0x02)
#define GETGENERALSTATUS (0x04)
#define READREGISTER (0x06)
#define WRITEREGISTER (0x08)
#define READGPIO (0x0C)
#define WRITEGPIO (0x0E)
#define SETSERIALBAUDRATE (0x10)
#define SETPARAMETERS (0x12)
#define SAMCONFIGURATION (0x14)
#define POWERDOWN (0x16)
#define RFCONFIGURATION (0x32)
#define RFREGULATIONTEST (0x58)

#define INLISTPASSIVETARGET (0x4A)
#define INATR (0x50)
#define INPSL (0x4E)
#define INDATAEXCHANGE (0x40)
#define INCOMMUNICATETHRU (0x42)
#define INDESELECT (0x44)
#define INRELEASE (0x52)
#define INSELECT (0x54)
#define INAUTOPOLL (0x60)
#define TGINITASTARGET (0x8C)
#define TGSETGENERALBYTES (0x92)
#define PN532_WAKEUP (0x55)

#define PN532_MIFARE_ISO14443A (0x00)
// Mifare Commands
#define MIFARE_CMD_AUTH_A (0x60)
#define MIFARE_CMD_AUTH_B (0x61)
#define MIFARE_CMD_READ (0x30)
#define MIFARE_CMD_WRITE (0xA0)
#define MIFARE_CMD_TRANSFER (0xB0)
#define MIFARE_CMD_DECREMENT (0xC0)
#define MIFARE_CMD_INCREMENT (0xC1)
#define MIFARE_CMD_STORE (0xC2)

#define RFID_RATE 38400
#define _delay 1

#define A 0x00 //125 Khz module
#define B 0x01 //13.56 Mhz module


typedef uint8_t CardIdentifier [4]; 
 
typedef uint8_t KeyAccess[6]; 
 
typedef uint8_t blockData[16];


//! WaspRFID13 Class
/*!
	WaspRFID13 Class defines all the variables and functions used for managing 
	the 13MHz RFID module
 */
class WaspRFID13{
 
public:
 
 //*************************************************************
 // Constructor of the class							       *
 //*************************************************************
	//!Void constructor
	WaspRFID13();  
	
 
 //*************************************************************
 // Public Methods										       *
 //*************************************************************
	
	//! Switches ON the module and assigns the UART
	/*!
    \param uint8_t socket: The uart por used
	\return void  
    */ 	
    void ON(uint8_t socket, uint8_t type);

	//! Switches OFF the module and closes the UART.
	/*!
    \param void
	\return void  
    */
    void OFF(void);  
	 
   	//! The goal of this command is to detect as many targets (maximum MaxTg) 
   	//! as possible in passive mode. 
    /*! 	
    \param uint8_t CardID: pointer to CardID. A vector where stores the card identifier
    \param uint8_t ATQ: pointer to ATQ. A vector where stores the answer to request
    \return Number of targets detected
    */   
    uint8_t init(uint8_t *CardID , uint8_t *ATQ);
 
	//!A block must be authenticated before read and write operations 
    /*! 	
    \param  uint8_t blockAddress: The block to be authenticated.
	\param  uint8_t keyAcces: A vector pointer with the keyAcces needed to autenticate
    \return true on success, false otherwise.
    */	 
    uint8_t authenticate(uint8_t *CardID, uint8_t blockAddress, uint8_t *keyAccess); 
	
 
 	//!Write 16 bytes in address 
    /*! 	
    \param  uint8_t address: the address where write  blockData. 
	\param  uint8_t blockData: A vector pointer with the information to write.
    \return true on success, false otherwise.
    */ 	
    uint8_t writeData(uint8_t address, uint8_t *blockData);  
 
  	//!Read 16 bytes from  address 
    /*! 	
    \param  uint8_t address: the address to read 
    \return true on success, false otherwise.
    */ 
    uint8_t readData(uint8_t address, uint8_t *data); 
  
  	//!Authenticates one card's sector and reads one block.
    /*! 	
    \param uint8_t CardID: pointer to CardID. A vector with the card identifier. 
	\param  uint8_t keyAcces: A vector pointer with the keyAcces needed to autenticate.
    \param  uint8_t _data : a vector where stores the read data
    \param  uint8_t address: the address to read.
    \return true on success, false otherwise.
    */ 
    uint8_t readWithAuth(	uint8_t *CardID , 
							uint8_t *keyAccess, 
							uint8_t *data, 
							uint8_t address);
	
  	//!Authenticates one card's sector and writes one block.
    /*! 	
    \param uint8_t CardID: pointer to CardID. A vector with the card identifier. 
	\param  uint8_t keyAcces: A vector pointer with the keyAcces needed to autenticate.
    \param  uint8_t _data : data to write.
    \param  uint8_t address: the address where write  _data. 
    \return true on success, false otherwise.
    */ 
    uint8_t writeWithAuth(	uint8_t *CardID , 
							uint8_t *keyAccess , 
							uint8_t *_data , 
							uint8_t address);
	 
	//!Writes one card's block and checks that.
    /*! 
	\param  uint8_t _data : data to write.
    \param  uint8_t address: the address where write  _data. 
    */	
    uint8_t writeAndCheck(uint8_t *data,uint8_t address);	
	
	//!Authenticates one card's sector, writes one block and checks that.
    /*! 	
    \param uint8_t CardID: pointer to CardID. A vector with the card identifier. 
	\param  uint8_t keyAcces: A vector pointer with the keyAcces needed to autenticate.
    \param  uint8_t _data : data to write.
    \param  uint8_t address: the address where write  _data. 
    */	
    uint8_t writeAndCheckWithAuth(	uint8_t *CardID, 
									uint8_t *keyAccess, 
									uint8_t *data, 
									uint8_t address);
	
	//!Put the module into Power Down mode in order to save power consumption.
    /*! 	
    \return true on success, false otherwise.
    */ 	
    uint8_t powerDown(void); 
	
	//!Wake up from power dowm mode.
    /*! 	
    \return true on success, false otherwise.
    */
    void wakeUp(void);
	
	//!Print data stored in vectors .
    /*! 	
    \param  uint8_t _data: data to print
    \param  uint8_t length: length of the _data to print.
    */	
    void print(uint8_t *_data, uint8_t length);

 	//!It compares 2 UIDs
    /*! 	
    \param  uint8_t CardID1
    \param  uint8_t CardID2
    */ 
    uint8_t equalUIDs(uint8_t *UID1,uint8_t *UID2);
	
	//!Searches one UID inside one group of UIDs
    /*! 	
    \param  uint8_t vCards: a vector with several Card identifiers
    \param  uint8_t CardID: the Card identifier to search. 
	\param  uint8_t nCards: number of Cards. 
    */
    int searchUID(uint8_t *vCards,uint8_t  *CardID,uint8_t nCards);
	
	//! Convert from a string to a pointer to uint8_t's
    /*! 	
    \param  char inp: the string we want to convert 
    \param  uint8_t outp: the transformed pointer to unit8_t's 
    */	
    int string2vector(char *inp, uint8_t *outp);
	
	//! Convert from a pointer to uint8_t's to an integer 
    /*! 	
    \param  char inp: the pointer to uint8_t's we want to convert
    \return int : the converted integer.
    */	
    int vector2int(uint8_t *inp);
	
	//! Changes both keys and access conditions to one card's sector 
    /*! 	
    \param uint8_t CardID: pointer to CardID. A vector with the card identifier. 
	\param  uint8_t kelOld: the old keyAccess
	\param  uint8_t keyA: the new keyAcces A
	\param  uint8_t keyB: the new keyAccess B
	\param  uint8_t config: the configuration of the target 
	\param  uint8_t data: data to write
	\param  uint8_t add: address where change the keyAccess.
 	\return uint8_t 0 success, 1 otherwise.
    */
    uint8_t setKeys(uint8_t *CardID,
					uint8_t *keyOld,
					uint8_t *keyA,
					uint8_t *keyB,
					uint8_t *config,
					uint8_t *data,
					uint8_t add);
		
private: 
 
 //***********************************************************
 // Private variables									     *
 //***********************************************************
	//!Receive Buffer 
 	uint8_t dataRX[35];
		
	//!Transmit Buffer
	uint8_t dataTX[35];
	
	//!Key access
	KeyAccess keyAccess;
		
	//!UART port 0 or 1
	uint8_t _uart;
	
	//!The card identification number
	CardIdentifier _CardID; 
	
	//!Answer To Request
	uint8_t _ATQ[2]; 
	
	uint8_t _type; 
	
	
 //***********************************************************
 // Private Methods 									     *
 //*********************************************************** 	
 
  	//!This command is used to set internal parameters of the PN532, 
    /*! 	and then to configure its behavior regarding different cases.
    \param void.
    \return true on success, false otherwise.
    */   
    bool  configureSAM(void);  //! Configure the SAM 
	
	//!Send data stored in dataTX
    /*! 	
    \param  uint8_t dataTX : pointer to dataTX vector. 
    */ 	
	void sendTX(uint8_t *dataTX, uint8_t length, uint8_t outLength);
	
	//!Wait for ACK response and stores it in the dataRX buffer
    /*! 	
    \param  void
    \return void
    */ 	
	void getACK(void);
	
	//!Wait the response of the module
    /*! 	
    \param  void
    \return void
    */
	void waitResponse(void);
	
	//!Get data from the module 
    /*! 	
    \param  uint8_t outLength: amount of bytes to read
    \return void
    */
	void  getData(uint8_t outLength);
	
    //!Calculates the checksum and stores it in dataTX buffer
    /*! 	
    \param  uint8_t *dataTX: 
    \return void
    */
	void checkSum(uint8_t *dataTX);
	
	//!Calculates the length checksum and sotres it in the buffer. 
    /*! 	
    \param  uint8_t *dataTX: pointer to buffer
    \return void
    */
	uint8_t lengthCheckSum(uint8_t *dataTX);
	
	//!Changes the module baud rate from 115200 to 38400. 
    /*! 	
    */    
	void changeBaudRate(void);	
	
	
	//!The PN532 sends back the version of the embedded firmware.
    /*! It is needed to launch a simple command to sycnchronize
    \param void.
    \return true on success, false otherwise.
    */ 
    void getFirmware(void); 

};

extern WaspRFID13 RFID13;

#endif
