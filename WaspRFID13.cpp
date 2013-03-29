/*! \file WaspRFID.cpp
 *  \brief Library for managing WIFI modules
 *
 *  Copyright (C) 2012 Libelium Comunicaciones Distribuidas S.L.
 *  http://www.libelium.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 2.1 of the License, or
 *  (at your option) any later version.
   
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
  
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Version:			0.1
 *  Design:				David Gascón
 *  Implementation:		Ahmad Saad, Javier Solobera
 */

#ifndef __WPROGRAM_H__
	#include "WaspClasses.h"
#endif

//***********************************************************
//Constructor												*
//***********************************************************
	WaspRFID13::WaspRFID13(){/*void constructor*/}
	//SoftwareSerial mySerial;		
	 SoftwareSerial mySerial = SoftwareSerial(0,1);;
	 SoftwareSerial mySerial2 = SoftwareSerial(43,44);;

//***********************************************************
//Public Methods											*
//***********************************************************
	
	//! Switches ON the modelue and assigns the UART
	/*!
	\param uint8_t socket: The uart por used
	*/	
	void WaspRFID13::ON(uint8_t socket, uint8_t type )
	{		
			_uart = socket;
		
			if (socket == 0) {
			
				pinMode(0, INPUT);
				pinMode(1, OUTPUT);
			}			
			else {
				Utils.setMuxGPRS();
				pinMode(43, INPUT);
				pinMode(44, OUTPUT);
			}
			
			mySerial.begin(115200);			
			mySerial2.begin(115200);			

			_type = type;			

			delay(100);

			getFirmware();
			configureSAM();
			changeBaudRate();
			
			getFirmware();
			configureSAM();
			changeBaudRate();
			
			delay(100);			


		if (socket == 1) 
		{			
			beginSerial(RFID_RATE, 1);
			pinMode(DIGITAL6, OUTPUT);
			digitalWrite(DIGITAL6, HIGH);
			Utils.setMuxGPRS();
			_uart = 1;
		} 
		else 
		{
			beginSerial(RFID_RATE, 0);
			pinMode(XBEE_PW,OUTPUT);
			digitalWrite(XBEE_PW,HIGH);
			_uart = 0;
		}
		
		if (_type == B) {

		} else {

		}		
	}

	//! Switches OFF the module and closes the UART.
	/*!
	*/
	void WaspRFID13::OFF(void)
	{
		closeSerial(_uart);

		if (_uart == 1) {
			digitalWrite(DIGITAL6, LOW);
		} else {
			digitalWrite(XBEE_PW,LOW);
		}
	}

	//!The goal of this command is to detect as many targets (maximum MaxTg) as possible in passive mode. 
	/*!
	\param uint8_t *CardID : Card identifier pointer 
	\param uint8_t *ATQ : Answer to Request pointer. 
	\return Number of targets detected
	*/	
	uint8_t WaspRFID13::init(uint8_t *CardID , uint8_t *ATQ)//! Request InListPassive 
	{
	
		if (_type == B) {
			USB.flush();

			delay(_delay);
			digitalWrite(DIGITAL9, HIGH);
			delay(_delay);
			dataTX[0] = 0x04; //Length
			lengthCheckSum(dataTX); // Length Checksum
			dataTX[2] = HOSTTOPN532;
			dataTX[3] = INLISTPASSIVETARGET;//Code
			dataTX[4] = 0x01;//MaxTarget
			dataTX[5] = 0x00;//BaudRate = 106Kbps
			dataTX[6] = 0x00;//Clear checkSum position
			checkSum(dataTX);

			sendTX(dataTX , 7 ,23);

			for (int i = 17; i < (21) ; i++){
				_CardID[i-17] = dataRX[i];
				CardID[i-17] = _CardID[i-17];
			}

			ATQ[0] = dataRX[13];
			ATQ[1] = dataRX[14];

			if ((dataRX[9]== 0xD5) & (dataRX[10] == 0x4B) & (dataRX[11] == 0x01)) { 
				return 1;
			} else {
				return 0;
			}
			
		} else {
		
		}
		
	}

	//!A block must be authenticated before read and write operations 
	/*!
	\param uint8_t blockAddress: The block to be authenticated.
	\param uint8_t keyAcces: A vector pointer with the keyAcces needed to authenticate.
	\return 0 on success, 1 otherwise.
	*/	
	uint8_t WaspRFID13::authenticate(uint8_t *CardID, uint8_t blockAddress, uint8_t *keyAccess)
	{
		if (_type == B) {
			delay(_delay);
			digitalWrite(DIGITAL9, HIGH);
			delay(_delay);
		
			dataTX[0] = 0x0F;
			lengthCheckSum(dataTX);
			dataTX[2] = HOSTTOPN532;
			dataTX[3] = INDATAEXCHANGE; // inDataEchange
			dataTX[4] = 0x01; //Number of targets
			dataTX[5] = AUTH_WITH_KEYA; // Authentication code
			dataTX[6] = blockAddress;

			for (int i = 0; i<6 ; i++) {
				dataTX[i + 7] = keyAccess[i];
			}

			dataTX[13] = CardID[0];  dataTX[14] = CardID[1];
			dataTX[15] = CardID[2];  dataTX[16] = CardID[3]; 

			dataTX[17] = 0x00;
			checkSum(dataTX);
		
			sendTX(dataTX , 18 ,14);
			
			if ((dataRX[9]== 0xD5) & (dataRX[10] == 0x41) & (dataRX[11] == 0x00)) {
				return 1;
			} else {
				return 0;
			}
		} else {
		
		}
			
	}	

	//!The PN532 sends back the version of the embedded firmware.
	/*!
	 *  It is needed to launch a simple command to sycnchronize
	\return 0 on success, 1 otherwise.
	*/	
/*	void WaspRFID13::getFirmware(void) 
	{
		USB.flush();
		mySerial.print("                ");
		//mySerial.flush();

		memset(dataTX, 0x00, 35);
		dataTX[0] = 0x02; // Length
		lengthCheckSum(dataTX); // Length Checksum
		dataTX[2] = 0xD4; // CODE
		dataTX[3] = 0x02; //TFI
		checkSum(dataTX); //0x2A; //Checksum   

		// sendTX(dataTX , 5 , 17);
		mySerial.print(char(0x00));
		mySerial.print(char(0x00));
		mySerial.print(char(0xFF)); 

		for (int i = 0; i < 5; i++) {
			mySerial.print(char(dataTX[i]));
		}	

		mySerial.print(char(0x00));	
		 
		delay(100);
		//while(mySerial.read() != 0xFF) {}//Wait for 0x00 response
		//mySerial.flush();

}*/


	//!Write 16 bytes in address .
	/*!
	\param uint8_t address: the address where write  blockData. 
	\param uint8_t blockData: A vector pointer with the information to write.
	\return 0 on success, 1 otherwise.
	*/	
	uint8_t WaspRFID13::writeData(uint8_t address, uint8_t *blockData) 
	{
		if (_type == B) {
			delay(_delay);
			digitalWrite(DIGITAL9, HIGH);
			delay(2);
			Serial.print("                ", _uart);//Send some dummy data to clear the module buffer.

			dataTX[0] = 0x15;
			lengthCheckSum(dataTX); //Length Checksum
			dataTX[2] = HOSTTOPN532;
			dataTX[3] = 0x40;//inDataEchange CODE
			dataTX[4] = 0x01;//Number of targets
			dataTX[5] = MIFARE_WRITE; //Write Command
			dataTX[6] = address; //Address		

			for (int i = 0; i<16; i++) {
				dataTX[i+7] = blockData[i];
			}

			dataTX[23] = 0x00;
			checkSum(dataTX);
			sendTX(dataTX , 24 ,14);


			if ((dataRX[9]== 0xD5) & (dataRX[10] == 0x41) & (dataRX[11] == 0x00)) {
				return 1;
			} else {
				return 0;
			} 
		} else {

		}		
	}

	//!Read 16 bytes from address .
	/*!
	\param uint8_t address: the address for reading. 
	\return 0 on success, 1 otherwise.
	*/	
	uint8_t WaspRFID13::readData(uint8_t address, uint8_t *readData)  
	{
		if (_type == B) {
			delay(2);
			digitalWrite(DIGITAL9, HIGH);
			delay(2);
			Serial.print("                ", _uart);//Send some dummy data to clear the module buffer.		
		
			dataTX[0] = 0x05;
			lengthCheckSum(dataTX); // Length Checksum
			dataTX[2] = HOSTTOPN532; // Code
			dataTX[3] = INDATAEXCHANGE; // Code
			dataTX[4] = 0x01; // Number of targets
			dataTX[5] = MIFARE_READ; //ReadCode
			dataTX[6] = address; //Read address
			dataTX[7] = 0x00;
			checkSum(dataTX);

			sendTX(dataTX , 8, 30);
			memset(readData, 0x00, 16);

			if ((dataRX[9]== 0xD5) & (dataRX[10] == 0x41) & (dataRX[11] == 0x00)){
				for (int i = 12; i < 28; i++) {
					readData[i-12] = dataRX[i];
				}			
				return 1;
			} else {
				return 0;
			}
		} else {
		
		}
	}
	
	//!Authenticates one card's sector and reads one block.
	/*!
	\param uint8_t CardID: pointer to CardID. A vector with the card identifier. 
	\param uint8_t keyAcces: A vector pointer with the keyAcces needed to autenticate.
	\param uint8_t _data : a vector where stores the read data
	\param uint8_t address: the address to read.
	\return 0 on success, 1 otherwise.
	*/
	uint8_t WaspRFID13::readWithAuth(	uint8_t *CardID, 
										uint8_t *keyAccess, 
										uint8_t *data, 
										uint8_t address)
	{
		if (_type == B) {
			_CardID[0] = CardID[0];		_CardID[2] = CardID[2];
			_CardID[1] = CardID[1];		_CardID[3] = CardID[3];

			authenticate(CardID, address , keyAccess);
		
			return readData(address, data);
		} else { 
		
		}
	}

	//!Authenticates one card's sector and writes one block.
	/*!
	\param uint8_t CardID: pointer to CardID. A vector with the card identifier. 
	\param uint8_t keyAcces: A vector pointer with the keyAcces needed to autenticate.
	\param uint8_t _data : data to write.
	\param uint8_t address: the address where write _data. 
	\return 0 on success, 1 otherwise.

	*/ 
	uint8_t WaspRFID13::writeWithAuth(	uint8_t *CardID, 
										uint8_t *keyAccess, 
										uint8_t *data, 
										uint8_t address)
	{
		if (_type == B) {
			_CardID[0] = CardID[0]; 	_CardID[2] = CardID[2];
			_CardID[1] = CardID[1];		_CardID[3] = CardID[3];
		
			authenticate(CardID, address , keyAccess);
			return writeData(address, data);
		} else { 
		
		}
	}

	//!Writes one card's block and checks that.
	/*! 
	\param uint8_t _data : data to write.
	\param uint8_t address: the address where write _data. 
	*/	
	uint8_t WaspRFID13::writeAndCheck(uint8_t *data, uint8_t address)
	{
		if (_type == B) {
			uint8_t _readData[16]; 
		
			writeData(address, data);
			readData(address, _readData);		 
			return !memcmp(_readData, data, 16);
		} else {

		}	
	}

	//!Authenticates one card's sector, writes one block and checks that.
	/*! 	
	\param uint8_t CardID: pointer to CardID. A vector with the card identifier. 
	\param uint8_t keyAcces: A vector pointer with the keyAcces needed to autenticate.
	\param uint8_t _data : data to write.
	\param uint8_t address: the address where write _data. 
	*/	
	uint8_t WaspRFID13::writeAndCheckWithAuth(	uint8_t *CardID, 
												uint8_t *keyAccess, 
												uint8_t *data, 
												uint8_t address)
	{
		if (_type == B) {
			_CardID[0] = CardID[0]; 	_CardID[2] = CardID[2];
			_CardID[1] = CardID[1];		_CardID[3] = CardID[3];
		
			authenticate(CardID, address , keyAccess);
			return writeAndCheck(data, address);
		} else {

		}	
	}

	//!Put the module into Power Down mode in order to save power consumption.
	/*!
	\return true on success, false otherwise.
	*/	
	uint8_t WaspRFID13::powerDown(void)
	{
		if (_type == B) { 
			delay(2);
			digitalWrite(DIGITAL9, HIGH); 
			delay(2);		
			Serial.print("                ", _uart);//Send some dummy data to clear the module buffer.	
		
			dataTX[0] = 0x03;
			lengthCheckSum(dataTX);// Length Checksum
			dataTX[2] = HOSTTOPN532; // Code
			dataTX[3] = POWERDOWN; // Code
			dataTX[4] = 0x10; // Wake UP mode HFU.
			dataTX[5] = 0x00; // Clear checkSum byte. 

			checkSum(dataTX);

			sendTX(dataTX , 6, 12); 

			if ((dataRX[9]== 0xD5) & (dataRX[10] == 0x17) & (dataRX[11] == 0x00)) {		 
				return 1;
			} else {
				return 0;
			}
		} else {

		}	
	}
	
	//!Wake up from power dowm mode.
	/*!
	*/ 	
	void WaspRFID13::wakeUp(void)
	{
		if (_type == B) {
			delay(2);
			digitalWrite(DIGITAL9, HIGH); 
			delay(2);

			Serial.print(PN532_WAKEUP, _uart);	
			delay(1000);
			Serial.flush(_uart);
			digitalWrite(DIGITAL9, LOW); 
			delay(2);		
		} else {

		}	
		
	}

	//!Print data stored in vectors .
	/*!
	\param uint8_t _data: data to print
	\param uint8_t length: length of the _data to print.
	*/	
	void WaspRFID13::print(uint8_t * _data, uint8_t length) 
	{
		for (int i = 0; i < length ; i++)
		{
			USB.print(_data[i], HEX);
			USB.print(" ");
		}
			USB.print("\n");
	}

	//!It compares 2 UIDs
	/*!
	\param uint8_t CardID1
	\param uint8_t CardID2
	*/ 
	uint8_t WaspRFID13::equalUIDs(uint8_t *CardID1 , uint8_t *CardID2)
	{
		if (memcmp(CardID1, CardID2, 4) == 0) return 0; else return 1;
	}

	//!Searches one UID inside one group of UIDs
	/*! 	
	\param uint8_t vCards: a vector with several Card identifiers
	\param uint8_t CardID: the Card identifier to search. 
	\param uint8_t nCards: number of Cards. 
	*/ 
	int WaspRFID13::searchUID(uint8_t *vCards , uint8_t *CardID, uint8_t nCards)
	{
			int card = -1;
			bool ok = false;

		for (int i=0; i<nCards; i++) // will chek all the 4-byte groups
		{
			for (int j=0; j<4; j++) // will check each of the 4-bye groups
			{
				if (vCards[i*4 + j] == CardID[j]) // check if that byte is equal to the reference
					ok = true;
				else {
					ok = false;
					break;
				}
			}
			if (ok == true) // when the 4-byte analysis ended, check if the state is still ok
			{
				card = i;
				break;
			}
		}
			return card; 
	}

	//! Convert from a string to a pointer to uint8_t's
	/*!
	\param char inp: the string we want to convert 
	\param uint8_t outp: the transformed pointer to unit8_t's 
	\return int : returns 0 on success, -1 on failure.
	*/	
	int WaspRFID13::string2vector(char *inp, uint8_t *outp)
	{
		for (int i=0; i<16; i++) {
			outp[i] = '\0'; // write 0's first
		}
		if (sizeof(inp) > 16) {
			return -1; // this is an error
		} else {
			for (int i=0; i<16; i++){
				outp[i] = inp[i]; // write the content of inp
				if (inp[i] == '\0')
					break;
			}
			return 0;
		}
	}

	//! Convert from a pointer to uint8_t's to an integer 
	/*!
	\param char inp: the pointer to uint8_t's we want to convert
	\return int : the converted integer.
	*/	
	int WaspRFID13::vector2int(uint8_t *inp) 
	{
		int length = 0; // stores the number of digits
		int outp   = 0; // stores the output

		for (int i=0; i<16; i++) {// firs, calculate the number of digits
			if (inp[i] == '\0')
				break;
			else
				length++;
		}

		for (int i=0; i < length; i++){ // iterate to add the digits
			outp = outp + (inp[i] - 48) * pow(10, (length-i-1));
		}
		
		if (length > 2)
			outp = outp + 1; // mandatory
		return outp; 
	}

	//! Changes both keys and access conditions to one card's sector 
	/*!
	\param uint8_t CardID: pointer to CardID. A vector with the card identifier. 
	\param uint8_t kelOld: the old keyAccess
	\param uint8_t keyA: the new keyAcces A
	\param uint8_t keyB: the new keyAccess B
	\param uint8_t config: the configuration of the target 
	\param uint8_t data: data to write
	\param uint8_t add: address where change the keyAccess
	\return uint8_t 0 success, 1 otherwise.
	*/	
	uint8_t WaspRFID13::setKeys(uint8_t *CardID,
								uint8_t *keyOld,
								uint8_t *keyA,
								uint8_t *keyB,
								uint8_t *config,
								uint8_t *data,
								uint8_t add)
	{
		if (_type == B) {
			uint8_t state = 1; 

			if (((add+1) % 4) == 0){
				state = authenticate(CardID , add, keyOld);
					if (state == 0) {
						for (int i = 0; i<6; i++) {
							data[i] = keyA[i];
							data[i+ 10] = keyB[i];
						}

						for (int i = 0; i<4 ; i++) {
							data[i + 6] = config[i];
						}
						state = writeData(add, data);
					}
			}
			return state;
		} else {

		}	
	}

//***********************************************************
//Private Methods											*
//***********************************************************
 
	//!This command is used to set internal parameters of the PN532, 
	/*!	and then to configure its behavior regarding different cases.
	\return true on success, false otherwise.
	*/	
	bool WaspRFID13::configureSAM(void)//! Configure the SAM 
	{
		
		if (_uart == 0) {
			  mySerial.print("               ");

			  dataTX[0] = 0x05; //Length
			  lengthCheckSum(dataTX); // Length Checksum
			  dataTX[2] = 0xD4;
			  dataTX[3] = 0x14;
			  dataTX[4] = 0x01; // Normal mode
			  dataTX[5] = 0x14; // TimeOUT
			  dataTX[6] = 0x00; // IRQ
			  dataTX[7] = 0x00; // Clean checkSum position
			  checkSum(dataTX);

			  //  sendTX(dataTX , 8, 13);

			  mySerial.print(char(0x00));
			  mySerial.print(char(0x00));
			  mySerial.print(char(0xFF)); 

			  for (int i = 0; i < 8; i++) {
				mySerial.print(char(dataTX[i]));
			  }

			  mySerial.print(char(0x00));
			  delay(100);
			  
		  } else {
			  
			  mySerial2.print("               ");

			  dataTX[0] = 0x05; //Length
			  lengthCheckSum(dataTX); // Length Checksum
			  dataTX[2] = 0xD4;
			  dataTX[3] = 0x14;
			  dataTX[4] = 0x01; // Normal mode
			  dataTX[5] = 0x14; // TimeOUT
			  dataTX[6] = 0x00; // IRQ
			  dataTX[7] = 0x00; // Clean checkSum position
			  checkSum(dataTX);
			  
			  mySerial2.print(char(0x00));
			  mySerial2.print(char(0x00));
			  mySerial2.print(char(0xFF)); 

			  for (int i = 0; i < 8; i++) {
				mySerial2.print(char(dataTX[i]));
			  }

			  mySerial2.print(char(0x00));
			  delay(100);
		  }
	}

	//!Send data stored in dataTX
	/*!
	\param uint8_t dataTX : pointer to dataTX vector. 
	*/
	void WaspRFID13::sendTX(uint8_t *dataTX, uint8_t length, uint8_t outLength)
	{

		Serial.print(PREAMBLE, BYTE, _uart);
		Serial.print(PREAMBLE, BYTE, _uart);
		Serial.print(STARTCODE2, BYTE, _uart); 

		for (int i = 0; i<length; i++) {
			Serial.print(dataTX[i], BYTE, _uart);
		}

		Serial.print(POSTAMBLE, BYTE, _uart);			
		getACK(); 
		waitResponse();// 1C - receive response
		getData(outLength); 

		digitalWrite(DIGITAL9, LOW); 
		delay(_delay);
	}

	//!Wait for ACK response and stores it in the dataRX buffer
	void WaspRFID13::getACK(void)
	{
		delay(5);
		waitResponse();

		for (int i = 0; i<5 ; i++) {		
			dataRX[i] = Serial.read(_uart);
		}
	}

	//!Wait the response of the module
	void WaspRFID13::waitResponse(void)
	{	
		int val = 0xFF;
		int cont = 0x00;

		while(val != PREAMBLE) {
			val = Serial.read(_uart);
			delay(5);
			cont ++;
		}
	}

	//!Get data from the module 
	void WaspRFID13::getData(uint8_t outLength)
	{
		for (int i=5; i < outLength; i++) {
			dataRX[i] = Serial.read(_uart); // read data from the module.
		} 
	}

	//!Calculates the checksum and stores it in dataTX buffer
	void WaspRFID13::checkSum(uint8_t *dataTX)
	{
		for (int i = 0; i< dataTX[0] ; i++) {
			dataTX[dataTX[0] + 2] += dataTX[i + 2];
		}

		byte(dataTX[dataTX[0] + 2]= - dataTX[dataTX[0] + 2]);
	}

	//!Calculates the length checksum and stores it in the buffer. 
	uint8_t WaspRFID13::lengthCheckSum(uint8_t *dataTX)
	{
		dataTX[1] = byte(0x100 - dataTX[0]); 
	}



	void WaspRFID13::changeBaudRate() 
	{  
		 if (_uart == 0 ) {
			 delay(5);
			 mySerial.print("                   ");//Send some dummy data to clear the module buffer. 
			 memset(dataTX, 0 , 16);	           

			 dataTX[0] = 0x03;
			 lengthCheckSum(dataTX);
			 dataTX[2] = 0xD4;
			 dataTX[3] = 0x10; //SetSerialBaudRate.
			 dataTX[4] = 0x02; //Baud Rate 38400.
			 dataTX[5] = 0;
			 checkSum(dataTX);    
			 mySerial.print(char(0x00));
			 mySerial.print(char(0x00));
			 mySerial.print(char(0xFF)); 

			  for (int i = 0; i<6; i++) {
					mySerial.print(char(dataTX[i]));
			  }

			  mySerial.print(char(0x00));
			 
			  mySerial.print(char(0x00));
			  mySerial.print(char(0x00));
			  mySerial.print(char(0x00));
			  mySerial.print(char(0xFF)); 
			  mySerial.print(char(0x00));
			  mySerial.print(char(0xFF)); 
			  mySerial.print(char(0x00));	

			  delay(50);
			  
		  } else {
			 delay(5);
			 mySerial2.print("                   ");//Send some dummy data to clear the module buffer. 
			 memset(dataTX, 0 , 16);	           

			 dataTX[0] = 0x03;
			 lengthCheckSum(dataTX);
			 dataTX[2] = 0xD4;
			 dataTX[3] = 0x10; //SetSerialBaudRate.
			 dataTX[4] = 0x02; //Baud Rate 38400.
			 dataTX[5] = 0;
			 checkSum(dataTX);    
			 mySerial2.print(char(0x00));
			 mySerial2.print(char(0x00));
			 mySerial2.print(char(0xFF)); 

			  for (int i = 0; i<6; i++) {
					mySerial2.print(char(dataTX[i]));
			  }

			  mySerial2.print(char(0x00));	;
			 
			  mySerial2.print(char(0x00));
			  mySerial2.print(char(0x00));
			  mySerial2.print(char(0x00));
			  mySerial2.print(char(0xFF)); 
			  mySerial2.print(char(0x00));
			  mySerial2.print(char(0xFF)); 
			  mySerial2.print(char(0x00));
			  
			  delay(50);
		  }
	}


	//!The PN532 sends back the version of the embedded firmware.
	/*!
	 *  It is needed to launch a simple command to sycnchronize
	\return 0 on success, 1 otherwise.
	*/	
	void WaspRFID13::getFirmware(void) 
	{
		if (_uart == 0) {
		
			USB.flush();
			mySerial.print("                ");
			//mySerial.flush();

			memset(dataTX, 0x00, 35);
			dataTX[0] = 0x02; // Length
			lengthCheckSum(dataTX); // Length Checksum
			dataTX[2] = 0xD4; // CODE
			dataTX[3] = 0x02; //TFI
			checkSum(dataTX); //0x2A; //Checksum   

			// sendTX(dataTX , 5 , 17);
			mySerial.print(char(0x00));
			mySerial.print(char(0x00));
			mySerial.print(char(0xFF)); 

			for (int i = 0; i < 5; i++) {
				mySerial.print(char(dataTX[i]));
			}	

			mySerial.print(char(0x00));	
			 
			delay(100);
			//while(mySerial.read() != 0xFF) {}//Wait for 0x00 response
			//mySerial.flush();
			
		} else {			
		
			USB.flush();
			mySerial2.print("                ");
			//mySerial.flush();

			memset(dataTX, 0x00, 35);
			dataTX[0] = 0x02; // Length
			lengthCheckSum(dataTX); // Length Checksum
			dataTX[2] = 0xD4; // CODE
			dataTX[3] = 0x02; //TFI
			checkSum(dataTX); //0x2A; //Checksum   

			// sendTX(dataTX , 5 , 17);
			mySerial2.print(char(0x00));
			mySerial2.print(char(0x00));
			mySerial2.print(char(0xFF)); 

			for (int i = 0; i < 5; i++) {
				mySerial2.print(char(dataTX[i]));
			}	

			mySerial2.print(char(0x00));	
			 
			delay(100);
		}

}

	
	
WaspRFID13 RFID13=WaspRFID13();

