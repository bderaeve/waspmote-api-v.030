/*
 *  Copyright (C) 2011 Libelium Comunicaciones Distribuidas S.L.
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
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Version:		0.032
 *  Design:		David Gascón
 *  Implementation:	Javier Siscart
 */

#ifndef __WPROGRAM_H__
#include "WaspClasses.h"
#endif

// Private Methods //

/*
 Function: Reads discovered devices from UART and saves them into specific array.
 Returns: 
 Parameters: 
	inquiryTime: wait for module answer during this time.
	MAX_DEVICES: Maximum number of deviced to discover.
	name: Enables friendly name scan.
	limit: Enables limited scan.
 Values: 
*/
void  WaspBT_Pro::waitInquiryAnswer(long inquiryTime, uint16_t MAX_DEVICES, bool name, bool limited) 
{
	delay(100);
	char dummy[4];						// Keyword
	char block[BLOCK_SIZE+1];				// Block with MAC, CoD y RSSI
	char number[4];
	bool totalFound=false;
	char total[4];	
	total[3]='\0';
	int a;
		
	Utils.setLED(LED1, LED_ON);  // Inquiry while led on
    	
	long previous=millis();
	while( (millis()-previous<inquiryTime) && numberOfDevices < MAX_DEVICES ) 
	{
		for(uint8_t zz=0;zz<BLOCK_SIZE;zz++) block[zz]=' ';	// Clear array
	
		// Looks for keyword and save device.		
		if (serialAvailable(1))	{
			dummy[0]=serialRead(1);	
			if (dummy[0]=='I'){
				while(serialAvailable(1)<2);
				dummy[1]=serialRead(1);	
				if (dummy[1]=='A'){
					dummy[2]=serialRead(1);	
					if (dummy[2]=='L'){
						while(serialAvailable(1)<BLOCK_SIZE);
						for(uint8_t x=0;x<BLOCK_SIZE;x++) block[x]= serialRead(1);	
						// Saves device
						parseBlock(block);						
						numberOfDevices++;
						#ifdef DEBUG_MODE
						printBuffer2();
						#endif
					}
				}
			}
			// Search inquiry end
			if ((dummy[0]=='Y')&&(!totalFound)){ 
				while(serialAvailable(1)<2);
				dummy[1]=serialRead(1);	
				if (dummy[1]==' '){
					// If here, total found.
					totalFound=true;
					while(serialAvailable(1)<3);
					total[0]=serialRead(1);
					total[1]=serialRead(1);
					total[2]=serialRead(1);
				}
			}
		}

	// Condition to avoid an overflow (DO NOT REMOVE)
    	if( millis()-previous < 0 ) previous=millis();
	}
	#ifdef ENABLE_FRIENDLY_NAME
	// If name, parse names
	if (name) parseNames();
	#endif

	if (!limited){
		sprintf(number, "%02u", numberOfDevices);
		#ifdef DEBUG_MODE
		// Compare total of devices found and total of devices saved. 
		a = Utils.array2long(total);
		if (a!=numberOfDevices){
			USB.println("+Maybe dev lost+");
		}
			USB.print("inquiried:");
			USB.print(a);
			USB.print("; saved:");
			USB.println(number);
		#endif

		if(!(SD.append(INQFILE,TOTAL))){
			#ifdef DEBUG_MODE
			USB.print(ERRORSD1);
			#endif
		}
		if(!(SD.appendln(INQFILE,number))){
			#ifdef DEBUG_MODE
			USB.println(ERRORSD1);
			#endif
		}
	}
    	Utils.setLED(LED1, LED_OFF);  // Inquiry while led on
   	
}

/*
 Function: Parses module answer trying to find specified MAC, stops inquiry when mac is found. if dewvice is found, saved on position 0
 Returns: Returs 1 if found. 0 if not found.
 Parameters: 
	inquiryTime: wait for module answer during this time.
	mac: mac of desired device.
 Values: 
*/
bool  WaspBT_Pro::waitScanDeviceAnswer(long inquiryTime, char* mac) 
{
	delay(100);
	char dummy[4];						
	bool found = false;
	char * block;
	block = (char*)calloc(BLOCK_SIZE+1,sizeof(char));

	Utils.setLED(LED1, LED_ON);  // Inquiry while led on
    	
	long previous=millis();
	while( (millis()-previous<inquiryTime) && (!found) ) 
	{
		// Looks for keyword and save device.		
		if (serialAvailable(1))	{
			dummy[0]=serialRead(1);
			delay(50);
			if (dummy[0]=='I'){
				dummy[1]=serialRead(1);	
				delay(50);
				if (dummy[1]=='A'){
					dummy[2]=serialRead(1);	
					if (dummy[2]=='L'){
						while(serialAvailable(1)<BLOCK_SIZE);
						for(uint8_t x=0;x<BLOCK_SIZE;x++) block[x]= serialRead(1);
						if (lookForAnswer(block,mac)>=1) {
							found=true;
							parseBlock(block);
							sendCommand("IC");
							numberOfDevices = 1;
							delay(200);
						}
					}
				}
			}
		}
	#ifdef DEBUG_MODE
	printBuffer();
	#endif	

	// Condition to avoid an overflow (DO NOT REMOVE)
    	if( millis()-previous < 0 ) previous=millis();
	}
	free(block);
	block=NULL;
    	Utils.setLED(LED1, LED_OFF);  // Inquiry while led on
 	return found;
}

/*
 Function: Read nodeID from EEPROM and Date from RTC. Writes them into INQFILE.
 Returns: 
 Parameters: 
 Values: 
*/
#ifdef ENABLE_DATE_AND_TIME
void WaspBT_Pro::getSetDateID(){

 	// Get ID
	mem_addr=200;
	// Read node identifier from EEPROM
	for (int i=0; i<8; i++) {
	identifier[i]=Utils.readEEPROM(mem_addr); 
    	mem_addr++; 
  	}
  	RTC.getTime();  // Get date and time 

	// Read line number before start a new inquiry
	numLinesBefore = SD.numln(INQFILE);

	#ifdef DEBUG_MODE
	USB.print("numlines before:");		
	USB.println(numLinesBefore);
	#endif

	// Build data and time string
	char * dateTime;
	dateTime = (char*) calloc(28,sizeof(char));
	sprintf(dateTime,"%02u-%02u-%02u;%02u:%02u; %s; ",RTC.date,RTC.month,RTC.year,RTC.hour,RTC.minute, identifier);

	if(!(SD.append(INQFILE,dateTime))){
		#ifdef DEBUG_MODE		
		USB.println(ERRORSD1);
		#endif
	}
	// free variables
	free(dateTime);
	dateTime=NULL;
}
#endif

/*
 Function: Looks for friendly names and stores them.
	NOTE: Inquiring with friendly names is avery slow process. 
	      Setting timeout (in milliseconds) the process can be aborted.
 Returns: 
 Parameters: 	
 Values: 
*/
#ifdef ENABLE_FRIENDLY_NAME
void WaspBT_Pro::parseNames(){

int namesFound =0;
char dummy[4];	
int dummies = 0;					
for (i = 0; i < 40; i++) theCommand[i] = ' ';	// Clear variable
#ifdef DEBUG_MODE
USB.println("Scanning names...");
#endif

if(!(SD.appendln(INQFILE,"Friendly names: "))){
	#ifdef DEBUG_MODE
	USB.print(ERRORSD1);
	#endif
}	
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
long timeout = 60000;	// Timeout to wait for name responses.			
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
long previous=millis();
while( (millis()-previous<timeout) && (namesFound<numberOfDevices)) {

	dummies=0;
	for (i = 0; i < COMMAND_SIZE; i++) theCommand[i] = ' ';	// Clear variable
	if (serialAvailable(1))	{
		dummy[0]=serialRead(1);
		if (dummy[0]=='A'){
			while(serialAvailable(1)<4);
			dummy[1]=serialRead(1);	
			if (dummy[1]=='M'){
				dummy[2]=serialRead(1);	
				if (dummy[2]=='E'){
					dummy[0]=serialRead(1);	//read space and miss it.			

					//if here, name sentence found. Now distinguish between ERROR and mac
					theCommand[0]=serialRead(1);
					if (theCommand[0]=='E'){
						//It is an error
						
						// read 11 dummy bytes
						while(serialAvailable(1)<11);
						for(uint8_t z=0;z<11;z++) dummy[0]=serialRead(1);
						
						// read and save corresponding mac
						while(serialAvailable(1)<BLOCK_MAC_SIZE);
						for(uint8_t x=0;x<BLOCK_MAC_SIZE;x++) theCommand[x]= serialRead(1);
						theCommand[BLOCK_MAC_SIZE+2]='N';
						theCommand[BLOCK_MAC_SIZE+3]='O';
						theCommand[BLOCK_MAC_SIZE+4]='N';
						theCommand[BLOCK_MAC_SIZE+5]='A';
						theCommand[BLOCK_MAC_SIZE+6]='M';
						theCommand[BLOCK_MAC_SIZE+7]='E';
						//save on SD
						if(!(SD.appendln(INQFILE,theCommand))){
							#ifdef DEBUG_MODE
							USB.print(ERRORSD1);
							#endif
						}	
													
						namesFound++;
						#ifdef DEBUG_MODE
						printBuffer();
						#endif

					}
					else {
						// It is a mac. Read mac (16bytes + one already read) + name
						while(serialAvailable(1)<20);		//!!
						for(uint8_t x=1;x<COMMAND_SIZE;x++) {
							if(serialAvailable(1)){
							theCommand[x]= serialRead(1);
							if (theCommand[x]=='"') dummies++;
							if (dummies>=2) x = COMMAND_SIZE;
							}
						}
							
						// NAME read. Now save into SD
						if(!(SD.appendln(INQFILE,theCommand))){
							#ifdef DEBUG_MODE
							USB.print(ERRORSD1);
							#endif
						}		
						
						namesFound++;
						#ifdef DEBUG_MODE
						printBuffer();
						#endif

					}
					
				}
			}
		}
	}
	
	// Condition to avoid an overflow (DO NOT REMOVE)
    	if( millis()-previous < 0 ) previous=millis();
}

	// When lefting loop, If timeout is over
	if (millis()-previous<timeout) {
		//scanNetworkCancel();	//NO HACE NADA, hay que hacer un reset.
		reset();
		#ifdef DEBUG_MODE
		USB.println("Timeout");
		#endif
	}

#ifdef DEBUG_MODE
printBuffer2();
USB.println("");
#endif

}
#endif

/*
 Function: Saves inquiry data into specific array.
 Returns: 
	'1' on success, '0' otherwise
 Parameters: 
	block: array which contains inquiry data.
	device: index used in data array.
 Values: 
*/
uint8_t WaspBT_Pro::parseBlock(char* block)
{
	uint8_t flag=1;
	char * dev;
	dev = (char*) calloc(60, sizeof(char));
	uint8_t x=1;
	for(uint8_t z=0;z<17;z++){
	mac_address[z]=block[x];	// Saves mac
	x++;
	}
	x++;	
	for(uint8_t z=0;z<6;z++){
	CoD[z]=block[x];		// Saves CoD
	x++;
	}		
	x= x+4;	
	for(uint8_t z=0;z<3;z++){
	RSSI[z]=block[x];		// Saves RSSI
	x++;
	}
	//save in file 
	sprintf(dev, "%s; %s; %s;",mac_address,CoD,RSSI);
	// Inside data, Check inquiry file
	if ((SD.isFile(INQFILE))!=1) {
		#ifdef DEBUG_MODE		
		USB.println("No file1");
		#endif
		SD.create(INQFILE);
	}
	if(!(SD.appendln(INQFILE,dev))){
		#ifdef DEBUG_MODE		
		USB.println(ERRORSD1);
		#endif
		flag = 0;
	}
	free(dev);
	dev=NULL;

	return flag;
}

/*
 Function: Reads and stores module answer.
 Returns: 
 Parameters: 
 Values: 
*/
void WaspBT_Pro::readCommandAnswer() {
	for (i = 0; i < RX_BUFFER; i++) commandAnswer[i] = ' ';	// Clears variable	
	i=0;
	while(serialAvailable(1)) {		
		commandAnswer[i]=serialRead(1);
		i++;
		delay(10);	
	}
}

/*
 Function: Searches a string in a text. 
 Returns: returns '0' if the string is not found or position of the expected answer.
 Parameters: 
	data: data where string is contained.
	expectedAnswer: string to be found.
 Values: 
*/
uint16_t WaspBT_Pro::lookForAnswer(uint8_t* data, const char* expectedAnswer) {
	
	char * received;
	received = (char*) calloc(RX_BUFFER, sizeof(char));

	uint8_t theLength = 0;
	uint8_t it=0;
	bool theSame=false;
	uint8_t first=1;
	uint8_t match=0;
	i=0;
    
	while( expectedAnswer[theLength]!='\0' ) theLength++;
	while( !match && data[i]!='\0' ){
		if( first )	{
			for(it=0;it<theLength;it++){
				received[it]=data[i];
				i++;
			}
			first=0;
		}
		it=0;
		theSame=true;
		for(it=0; it<theLength ; it++)	{
			if(received[it]!=expectedAnswer[it]){
				theSame= false;
				break;
			}
		}
		if( theSame ) match=1;
		else	{
			for(it=0; it<theLength-1 ; it++){
				received[it]=received[it+1];
			}
			received[it]=data[i];
			i++;
		}
	}
	
	if( !match ) i=0;

	free(received);
	received=NULL;
	return i;
}

/*
 Function: Searches a string in a text. 
 Returns: returns '0' if the string is not found or position of the expected answer
 Parameters: 
	data: data where string is contained.
	expectedAnswer: string to be found.
 Values: 
*/

uint16_t WaspBT_Pro::lookForAnswer(char* data, const char* expectedAnswer) {
	
	char * received;
	received = (char*) calloc(RX_BUFFER, sizeof(char));	

	uint8_t theLength = 0;
	uint8_t it=0;
	bool theSame=false;
	uint8_t first=1;
	uint8_t match=0;
	i=0;
    
	while( expectedAnswer[theLength]!='\0' ) theLength++;
	
	while( !match && data[i]!='\0' ){
		if( first ){
			for(it=0;it<theLength;it++){
				received[it]=data[i];
				i++;
			}
			first=0;
		}
		it=0;
		theSame=true;
		for(it=0; it<theLength ; it++){
			if(received[it]!=expectedAnswer[it]){
				theSame= false;
				break;
			}
		}
		if( theSame ) match=1;
		else{
			for(it=0; it<theLength-1 ; it++){
				received[it]=received[it+1];
			}
			received[it]=data[i];
			i++;
		}
	}
	
	if( !match ) i=0;

	free(received);
	received=NULL;
	return i;
}

/*				
 Function: Sends a given command to bluetooth module, adding CR+LF.
 Returns: 
 Parameters: 
	theText: Command to send.
 Values: 
*/
void WaspBT_Pro::sendCommand(const char* theText) {
    
    	sprintf(theCommand, "%s\r\n", theText);		// Adds CR+LF
  	serialFlush(1);	
       	printString(theCommand,PORT_USED);
        delay(100);
}

/*				
 Function: Change TX power
 Returns: 
 Parameters: 
	power: TX power value.
 Values: 
*/
void WaspBT_Pro::changeInquiryPower(int8_t power) {

	// Sets first two values to maximum by default
	sprintf(theCommand, "SET BT POWER %d %d %d", TX_POWER_DEFAULT_WT12, TX_POWER_MAX_WT12, power);
	sendCommand(theCommand);
	for (i = 0; i < COMMAND_SIZE; i++) theCommand[i] = ' ';	// Clear variable
	txPower = power;
}

/*				
 Function: Selects the active power mode among: ON and OFF.
 Returns: Returns '1' on success and '0' if error
 Parameters: 
	theText: Command to send.
 Values: 
*/
uint8_t WaspBT_Pro::setMode(uint8_t pwrMode) {
	_pwrMode=pwrMode;
	uint8_t answer=0;
    	switch(_pwrMode){
		case    BT_ON  	:		pinMode(DIGITAL6,OUTPUT);
                                                digitalWrite(DIGITAL6,HIGH);
                                                pinMode(BT_PW, OUTPUT);
						digitalWrite(BT_PW, HIGH);	
						delay(2000);
						digitalWrite(BT_PW, LOW);	
						delay(500);
						answer=1;
						break;

		/*case	BT_SLEEP	:	answer=sendCommand(POWER_SLEEP,POWER_R);
						switch(answer)
						{
							case	0	:	flag |= ERROR_POWER;
										break;
							case	2	:	flag |= ERROR_POWER;
										break;
						}
						break;*/

                case    BT_OFF  	:	pinMode(DIGITAL6,OUTPUT);
                                                digitalWrite(DIGITAL6,LOW);
						delay(500);
						answer=1;
						break;
	}
	return answer;
}

/*				
 Function: Sets time to wait inquiry answer
 Returns: Returns wait time in milliseconds
 Parameters: 
	time: time of inquiry in seconds. It is multiplied by 
 Values: 
*/
uint16_t WaspBT_Pro::setInquiryTime(uint16_t time)
{
	uint16_t waitTime = 0;

	switch (time) {					
		case 0	:	waitTime = 1000;break;
		case 1  :	waitTime = 2000;break;
		case 2	:	waitTime = 2560;break;
		case 3  :	waitTime = 3840;break;
		case 4	:	waitTime = 5120;break;
		case 5  :	waitTime = 6400;break;
		case 6	:	waitTime = 7680;break;
		case 7  :	waitTime = 8960;break;
		case 8	:	waitTime = 10240;break;
		case 9  :	waitTime = 11520;break;
		case 10	:	waitTime = 12800;break;
		case 11 :	waitTime = 14080;break;
		case 12	:	waitTime = 15360;break;
		case 13 :	waitTime = 16640;break;
		case 14	:	waitTime = 17920;break;
		case 15 :	waitTime = 19200;break;
		case 16	:	waitTime = 20480;break;
		case 17 :	waitTime = 21760;break;
		case 18	:	waitTime = 23040;break;
		case 19 :	waitTime = 24320;break;
		case 20	:	waitTime = 25600;break;
		case 21 :	waitTime = 26880;break;
		case 22	:	waitTime = 28100;break;
		case 23 :	waitTime = 29500;break;
		case 24 :	waitTime = 30700;break;
		case 25	:	waitTime = 32000;break;
		case 26 :	waitTime = 33200;break;
		case 27	:	waitTime = 34500;break;
		case 28 :	waitTime = 35800;break;
		case 29	:	waitTime = 37000;break;
		case 30 :	waitTime = 38400;break;
		case 31	:	waitTime = 39600;break;
		case 32 :	waitTime = 40900;break;
		case 33 :	waitTime = 42200;break;
		case 34	:	waitTime = 43500;break;
		case 35 :	waitTime = 44800;break;
		case 36	:	waitTime = 46000;break;
		case 37 :	waitTime = 47300;break;
		case 38	:	waitTime = 48640;break;
		case 39 :	waitTime = 50000;break;
		case 40 :	waitTime = 51200;break;
		case 41	:	waitTime = 52500;break;
		case 42 :	waitTime = 53700;break;
		case 43	:	waitTime = 55000;break;
		case 44 :	waitTime = 56300;break;
		case 45	:	waitTime = 57600;break;
		case 46 :	waitTime = 59000;break;
		case 47	:	waitTime = 60000;break;
		case 48 :	waitTime = 61500;break;
		}
	return waitTime;
}

/*
 Function: Cancels an inquiry 
 Returns: 
 Parameters: 
 Values: 
*/
void WaspBT_Pro::scanNetworkCancel() {	
	for (i = 0; i < COMMAND_SIZE; i++) theCommand[i] = ' ';	// Clear variable
	sendCommand("IC");
}

/*
 Function: Erases files created by bluetooth module on SD card.
 Returns: 
 Parameters: 
 Values: 
*/
void WaspBT_Pro::eraseSDFiles(){

	// erase inqfile if exist.
	if ((SD.isFile(INQFILE))==1){
		SD.del(INQFILE);
		delay(100);
		#ifdef ENABLE_DEBUG_MODE
		USB.println("delscan");
		#endif
	}

	// erase HANDSFREEFILE if exist.
	if ((SD.isFile(HANDSFREEFILE))==1){
		SD.del(HANDSFREEFILE);
		delay(100);
		#ifdef ENABLE_DEBUG_MODE
		USB.println("delhands");
		#endif
	}

	// erase MOBILEFILE if exist.
	if ((SD.isFile(MOBILEFILE))==1){
		SD.del(MOBILEFILE);
		delay(100);
		#ifdef ENABLE_DEBUG_MODE
		USB.println("delmob");
		#endif
	}

	// erase MOBILEFILE if exist.
	if ((SD.isFile(OTHERSFILE))==1){
		SD.del(OTHERSFILE);
		delay(100);
		#ifdef ENABLE_DEBUG_MODE
		USB.println("delother");
		#endif
	}
}

/*
 Function: Creates required files by bluetooth module into Sd card.
 Returns: 
	'1' on success, '0' otherwise
 Parameters: 
 Values: 
*/
uint8_t WaspBT_Pro::createSDFiles(){

	uint8_t flag=1;

	if ((SD.isFile(INQFILE))!=1) {

		if(!(SD.create(INQFILE))){
		#ifdef ENABLE_DEBUG_MODE
		USB.println("error creating");
		#endif
		flag = 0;
		}
		else SD.appendln(INQFILE,INQFILEHEAD);
	}

	if ((SD.isFile(HANDSFREEFILE))!=1) {
		
		if(!(SD.create(HANDSFREEFILE))){
		#ifdef ENABLE_DEBUG_MODE
		USB.println("error creating");
		#endif
		flag = 0;
		}
		else SD.appendln(HANDSFREEFILE,HANDSFREEFILEHEAD);
	}

	if ((SD.isFile(MOBILEFILE))!=1) {

		if(!(SD.create(MOBILEFILE))){
		#ifdef ENABLE_DEBUG_MODE
		USB.println("error creating");
		#endif
		flag = 0;
		}
		else SD.appendln(MOBILEFILE,MOBILEFILEHEAD);
	}
	
	if ((SD.isFile(OTHERSFILE))!=1) {
		
		if(!(SD.create(OTHERSFILE))){
		#ifdef ENABLE_DEBUG_MODE
		USB.println("error creating");
		#endif
		flag = 0;
		}
		else SD.appendln(OTHERSFILE,OTHERSFILEHEAD);
	}

	return flag;
}

// Constructors //

/*				
 Function: Constructor
 Returns: 
 Parameters: 	
 Values: 
*/
WaspBT_Pro::WaspBT_Pro() {

	i=0;
	_baudRate=BT_BLUEGIGA_RATE;
	_pwrMode=BT_ON;
	_uart=1;
	
}

// Public Methods //

/*
 Function: Turns on bluetooth module. 
 Returns: 
 Parameters: 
 Values: 
*/
void WaspBT_Pro::ON() {
		
	Utils.setMuxGPRS();
	beginSerial(_baudRate,_uart);
	setMode(BT_ON);						// Power saving
	serialFlush(1);
	reset();
	
}

/*
 Function: Turns off bluetooth module. 
 Returns: 
 Parameters: 
 Values:
*/ 
void WaspBT_Pro::OFF() {

	setMode(BT_OFF);
	closeSerial(_uart);
	Utils.setMux(MUX_TO_LOW,MUX_TO_LOW);
	
}

/*
 Function: Initialize some variables of the module
 Returns: 
 Parameters: 
 Values:
*/ 
void WaspBT_Pro::init(char * ID) {

	sendCommand("SET CONTROL CONFIG 0000 0101");			// Enable RSSI for inquiry
	delay(1000);

	//check if configuration is OK
	
	sendCommand("SET CONTROL CONFIG LIST");
	readCommandAnswer();
	
	#ifdef DEBUG_MODE
	//if(lookForAnswer(commandAnswer,"CONTROL CONFIG 0000 0000 0101")){
	if(lookForAnswer(commandAnswer,"CONTROL CONFIG 0000 0101")){
		USB.println("Configuration OK");
	}
	else{
		
		USB.println("Configuration failed");

	}
	#endif
	//SET defaul tx power to TX_POWER_5
	sendCommand("SET BT POWER 3 3 -2");			
	txPower = -2;	


	// Write waspmote ID in EEPROM
	// set identifier
  	mem_addr=200;
	// Write node identifier to EEPROM
	for (int i=0; i<8; i++)
	{
	  Utils.writeEEPROM(mem_addr,ID[i]);
	  mem_addr++; 
	}   


	// Prepare SD
	SD.cd("..");

	// erase files
	eraseSDFiles();

	// create necessary files to ensure their existence.
	createSDFiles();


}

/*
 Function: Resets bluetooth module.
 Returns: 
 Parameters: 
 Values: 
*/
uint8_t WaspBT_Pro::reset() { 	
	sendCommand("RESET");
	delay(2000);					// Reboot time
	readCommandAnswer();
	if (lookForAnswer(commandAnswer,"Bluegiga")){	// Searches keyword on answer. 
		#ifdef DEBUG_MODE
		USB.println("Ready");
		#endif
	}
	else{	
		#ifdef DEBUG_MODE
		USB.println("Reset failed");
		#endif	
	}
}

/*
 Function: Reads internal temperature sensor of bluetooth module.
 Returns: Temperature
 Parameters: 
 Values: 
*/
long WaspBT_Pro::getTemp(){
	long a;
	char dummy[3];
	char temperature[4];
	for (uint8_t i = 0; i < COMMAND_SIZE; i++) theCommand[i] = ' ';	// Clear variable
	for (uint8_t i = 0; i < 3; i++) temperature[i] = ' ';
	sendCommand("temp");
	delay(100);
	while(serialAvailable(1)){
		dummy[0]=serialRead(1);	
		if (dummy[0]=='M'){
			dummy[1]=serialRead(1);	
			if (dummy[1]=='P'){
				dummy[0]=serialRead(1);
				temperature[0]=serialRead(1);
				temperature[1]=serialRead(1);
				temperature[2]=serialRead(1);
				a = Utils.array2long(temperature);
				}
			}
	}
return a;
}

/*
 Function: Makes an inquiry to discover new devices.
 Returns: returns number of devices found.
 Parameters: 
	time: Inquiry time.
	power: Allowed TX power levels
 Values: 
*/
int8_t WaspBT_Pro::scanNetwork(uint8_t time, int8_t power) {	
	
	long inquiryTime= 0;
	bool name=false;
	bool limited=false;
	uint16_t maxDevices = DEFAULT_MAX_INQUIRY_RESULTS;	

	for (i = 0; i < COMMAND_SIZE; i++) theCommand[i] = ' ';		// Clears variable
	if (txPower != power)changeInquiryPower(power);		// checks previous value. Change only if different.

	if (time==0) time =1;					// Min inquiry time
	if (time>48) time = 48;					// Max inquiry time
	
			
	inquiryTime = setInquiryTime(time);
	
	// write header for inquiry file
	SD.cd("..");
	#ifdef ENABLE_DATE_AND_TIME
	getSetDateID();
	#endif
	if(!(SD.appendln(INQFILE,"ScanNetwork"))){
		#ifdef DEBUG_MODE		
		USB.println("err writting");
		#endif
	}

	sprintf(theCommand, "inquiry %u", time);		
	sendCommand(theCommand);
	
	numberOfDevices = 0;
	waitInquiryAnswer(inquiryTime, maxDevices, name, limited);
	
	// write line
	if(!(SD.appendln(INQFILE,ENDSTRING))){
		#ifdef DEBUG_MODE		
		USB.println(ERRORSD1);
		#endif
	}

	numLinesAfter = SD.numln(INQFILE);
	
	delay(3000);
	return numberOfDevices;
}

/*
 Function: It scans network and stops when finds "MAX_DEVICES". If max not reaches it scans max time (60s).
 Returns: Returns number of devices found.
 Parameters: 
	MAX_DEVICES: Maximum number of devices to find.
	power: Allowed TX power levels
 Values: 
*/
int8_t WaspBT_Pro::scanNetworkLimited(uint16_t MAX_DEVICES, int8_t power) {	
	
	long inquiryTime = 0;
	bool name=false;
	bool limited=true;
	for (i = 0; i < COMMAND_SIZE; i++) theCommand[i] = ' ';		// Clears variable
	if (txPower != power)changeInquiryPower(power);		// Checks previous value. Change only if different.
	
	
	inquiryTime = setInquiryTime(48);
	
	// write header for inquiry file
	SD.cd("..");
	#ifdef ENABLE_DATE_AND_TIME
	getSetDateID();
	#endif
	if(!(SD.appendln(INQFILE,"ScanNetworkLimited. "))){
		#ifdef DEBUG_MODE		
		USB.println(ERRORSD1);
		#endif
	}
	
	sprintf(theCommand, "inquiry 48");			// Inquiry command for max time
	sendCommand(theCommand);
	
	numberOfDevices = 0;
	waitInquiryAnswer(inquiryTime, MAX_DEVICES, name,limited);

	if (numberOfDevices>=MAX_DEVICES){
	scanNetworkCancel();				
	}
	else {
	#ifdef DEBUG_MODE
	USB.println("Maximum not reached");
	#endif
	}

	// write end line
	if(!(SD.appendln(INQFILE,";"))){
		#ifdef DEBUG_MODE		
		USB.println(ERRORSD1);
		#endif
	}

	numLinesAfter = SD.numln(INQFILE);
	
	delay(3000);
	return numberOfDevices;
}

/*
 Function: Makes an inquiry to discover specific device by its Mac.
 Returns: '0' if not found. Position in array otherwise.
 Parameters: 
	Mac: Mac of device to discover
	maxTime: Maximum time searching device
	power: Allowed TX power levels
 Values: 
*/
int16_t WaspBT_Pro::scanDevice(char* Mac, uint8_t maxTime, int8_t power) {	
	
	long inquiryTime= 0;
	uint16_t maxDevices = DEFAULT_MAX_INQUIRY_RESULTS;					

	for (i = 0; i < COMMAND_SIZE; i++) theCommand[i] = ' ';		// Clears variable
	if (txPower != power)changeInquiryPower(power);		// Checks previous value. Change only if different.
	
	if (maxTime==0) maxTime =1;				// Min inquiry time
	if (maxTime>48) maxTime = 48;				// Max inquiry time

	
	inquiryTime = setInquiryTime(maxTime);

	// write header for inquiry file
	SD.cd("..");
	#ifdef ENABLE_DATE_AND_TIME
	getSetDateID();
	#endif
	if(!(SD.append(INQFILE,"ScanDevice: "))){
		#ifdef DEBUG_MODE		
		USB.println(ERRORSD1);
		#endif
	}
	if(!(SD.appendln(INQFILE,Mac))){
		#ifdef DEBUG_MODE		
		USB.println(ERRORSD1);
		#endif
	}

	sprintf(theCommand, "inquiry %u", maxTime);			
	sendCommand(theCommand);

	numberOfDevices = 0;

	if(waitScanDeviceAnswer(inquiryTime,Mac)) {
		// write end line
		if(!(SD.appendln(INQFILE,";"))){
			#ifdef DEBUG_MODE		
			USB.println(ERRORSD1);
			#endif
		}
		numLinesAfter = SD.numln(INQFILE);
		return 1;
	}
	else {
		// write end line
		if(!(SD.appendln(INQFILE,"Not found."))){
			#ifdef DEBUG_MODE		
			USB.println(ERRORSD1);
			#endif
		}
		if(!(SD.appendln(INQFILE,";"))){
			#ifdef DEBUG_MODE		
			USB.println(ERRORSD1);
			#endif
		}
		numLinesAfter = SD.numln(INQFILE);
		return 0;
	}	
}

/*
 Function: Makes an inquiry to discover new devices
 Returns: returns number of devices found.
 Parameters: 
	time: Inquiry time.
	power: Allowed TX power levels
 Values: 
*/
#ifdef ENABLE_FRIENDLY_NAME
int8_t WaspBT_Pro::scanNetworkName(uint8_t time, int8_t power) {	
	
	long inquiryTime= 0;
	bool name=true;
	bool limited=false;
	uint16_t maxDevices = DEFAULT_MAX_INQUIRY_RESULTS;	

	for (i = 0; i < COMMAND_SIZE; i++) theCommand[i] = ' ';		// Clears variable
	if (txPower != power)changeInquiryPower(power);		// checks previous value. Change only if different.

	if (time==0) time =1;					// Min inquiry time
	if (time>48) time = 48;					// Max inquiry time
	
	
	inquiryTime = setInquiryTime(time);
	
	// write header for inquiry file
	SD.cd("..");
	#ifdef ENABLE_DATE_AND_TIME
	getSetDateID();
	#endif
	if(!(SD.appendln(INQFILE,"ScanNetworkName"))){
		#ifdef DEBUG_MODE		
		USB.println(ERRORSD1);
		#endif
	}

	sprintf(theCommand, "inquiry %u name", time);		
	sendCommand(theCommand);
	
	numberOfDevices = 0;
	
	waitInquiryAnswer(inquiryTime, maxDevices, name, limited);

	// write end line
	if(!(SD.appendln(INQFILE,";"))){
		#ifdef DEBUG_MODE		
		USB.println(ERRORSD1);
		#endif
	}

	numLinesAfter = SD.numln(INQFILE);

	delay(3000);
	return numberOfDevices;
}
#endif

/*
 Function: Prints data of last inquiry
 Returns: 
	'0' if no devices to print, '1' otherwise.
 Parameters: 
 Values: 
*/
uint8_t WaspBT_Pro::printInquiry(){

	uint8_t flag=1;
	USB.println("");
	i=0;
	SD.cd("..");

	// check if file exist
	if ((SD.isFile(INQFILE))!=1){
		flag=0;
		#ifdef DEBUG_MODE		
		USB.println("no print");
		#endif
	}
	else {
		for(i=numLinesBefore+1;i<numLinesAfter-1;i++){
			USB.print(SD.catln(INQFILE,i,1));
		}
	}
	return flag;
}

/*
 Function: Prints data of last inquiry
 Returns: 
	'0' if no devices to print, '1' otherwise.
 Parameters: 
 Values: 
*/
uint8_t WaspBT_Pro::printMobiles(){

	uint8_t flag=1;
	USB.println("");
	i=0;

	if (mobileCounter!=0){
	SD.cd("..");

	// check if file exist
	if ((SD.isFile(MOBILEFILE))!=1){
		flag=0;
		#ifdef DEBUG_MODE		
		USB.println("no print");
		#endif
	}
	else {
		for(i=mobNumLinesBefore+2;i<mobNumLinesAfter-1;i++){
			USB.print(SD.catln(MOBILEFILE,i,1));
		}
	}
	
	}
	else USB.println("0 Mobile phones");

	return flag;
}

/*
 Function: Prints data of last inquiry
 Returns: 
 Parameters: 
 Values: 
*/
uint8_t WaspBT_Pro::printHandsFree(){

	uint8_t flag=1;
	i=0;
	USB.println("");

	// if no new handsfree, print no handsfree
	if (handsCounter!=0){
	SD.cd("..");

	// check if file exist
	if ((SD.isFile(HANDSFREEFILE))!=1){
		flag=0;
		#ifdef DEBUG_MODE		
		USB.println("no print");
		#endif
	}
	else {
		for(i=handsNumLinesBefore+2;i<handsNumLinesAfter-1;i++){
			USB.print(SD.catln(HANDSFREEFILE,i,1));
		}
	}
	}
	else USB.println("0 handsfree");

	return flag;
}

/* ------------CoD structure------------------
CoD has 24 bits divided into 3 main fields.
 - Service Class: 11bits (from 23 to 13)
 - Major Device Class: 5 bits (from 12 to 8)
 - Minor Device Class: 6 bits (from 7 to 2)

SERVICE CLASS:
	bit 23: Information 
	bit 22: Telephony
	bit 21: Audio
	bit 20: Object transfer
	bit 19: Capturing
 	bit 18: Rendering
	bit 17: Networking
	bit 16: Positioning
	bit 15:
	bit 14:
	bit 13:

MAJOR DEVICE CLASS:
	00000: Miscellaneous
	00001: Computer, PDA, notebook
	00010: Phone
	00011: Lan/network acces point
	00100: Audio/video (headset,speaker,stereo...)
	00101: peripheral (mouse, joystick, keyboard...)
	00110: Imaging (printing, scanner, camera, display)
	00111: Wearable
	01000: Toy
	11111: Uncategorized
	xxxxx: All other values reserved

MINOR DEVICE CLASS:
	This field has its own values for each major device class value.	

*/

/*
 Function: Classify devices of last inquiry
 Returns: 
	'1' on success, '0' otherwise
 Parameters: 
 Values: 
*/
uint8_t WaspBT_Pro::classifyDevices(){

	// !!!!_!!!!
	// warning, used here global variables CoD and Mac. check if this has no relevance in other methods.	
	
	uint8_t flag=1;
	char * readLine;
	uint8_t deviceType;
	handsCounter=0;
	mobileCounter=0;
	othersCounter=0;
	char number[4];

	// check if an Sd is present
	if (SD.isSD()==0) USB.println("No SD");

	i=0;
	SD.cd("..");

	// set variables for printing
	mobNumLinesBefore = SD.numln(MOBILEFILE);
	handsNumLinesBefore = SD.numln(HANDSFREEFILE);
	
	// Write headers it into each file.
	readLine = SD.catln(INQFILE,numLinesBefore,1);	// also possible with strcpy
	SD.appendln(HANDSFREEFILE,readLine);
	SD.appendln(MOBILEFILE,readLine);
	SD.appendln(OTHERSFILE,readLine);
	// (...)
	
	// Loop to get each line of last inquiry and parse it.
	for(i=numLinesBefore+1;i<numLinesAfter-2;i++){

		readLine = SD.catln(INQFILE,i,1);	// also possible with strcpy
		
		// classify mac - develop this part
		// ----
		// read mac
		sprintf(mac_address,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",readLine[0],readLine[1],readLine[2],readLine[3],
		readLine[4],readLine[5],readLine[6],readLine[7],readLine[8],readLine[9],readLine[10],readLine[11],readLine[12],
		readLine[13],readLine[14],readLine[15],readLine[16]);

		//read CoD
		sprintf(CoD,"%c%c%c%c%c%c",readLine[19],readLine[20],readLine[21],readLine[22],readLine[23],readLine[24]);
		
		// Classify CoD 	//develop this part

		// Easy way. Redo for more accurate classification

		// Handsfree
		if (CoD[3]=='4'){
			handsCounter++;
			if(!(SD.append(HANDSFREEFILE,mac_address))){
				flag=0;
				#ifdef DEBUG_MODE		
				USB.println(ERRORSD1);
				#endif
			}
			#ifdef DEBUG_MODE		
				USB.println("hansfree");
			#endif

			SD.append(HANDSFREEFILE,";");
			SD.append(HANDSFREEFILE," ");
			SD.append(HANDSFREEFILE,CoD);
			SD.appendln(HANDSFREEFILE,";");

		}
		else {
			// Mobile
			if (CoD[3]=='2'){
				mobileCounter++;
				if(!(SD.append(MOBILEFILE,mac_address))){
					flag=0;
					#ifdef DEBUG_MODE		
					USB.println(ERRORSD1);
					#endif
				}
				#ifdef DEBUG_MODE		
				USB.println("Mobile");
				#endif
				SD.append(MOBILEFILE,";");
				SD.append(MOBILEFILE," ");
				SD.append(MOBILEFILE,CoD);
				SD.appendln(MOBILEFILE,";");
			}
			else {
				//others
				othersCounter++;
				if(!(SD.append(OTHERSFILE,mac_address))){
					flag=0;
					#ifdef DEBUG_MODE		
					USB.println(ERRORSD1);
					#endif
				}
				#ifdef DEBUG_MODE		
				USB.println("other");
				#endif

				SD.append(OTHERSFILE,";");
				SD.append(OTHERSFILE," ");
				SD.append(OTHERSFILE,CoD);
				SD.appendln(OTHERSFILE,";");
			}
		}
	}	//end loop

	// write totals
	sprintf(number, "%02u", mobileCounter);
	SD.append(MOBILEFILE,TOTAL);
	SD.appendln(MOBILEFILE,number);
		
	sprintf(number, "%02u", handsCounter);
	SD.append(HANDSFREEFILE,TOTAL);
	SD.appendln(HANDSFREEFILE,number);

	sprintf(number, "%02u", othersCounter);
	SD.append(OTHERSFILE,TOTAL);
	SD.appendln(OTHERSFILE,number);

	// write end of inquiry.
	SD.appendln(HANDSFREEFILE,ENDSTRING);
	SD.appendln(MOBILEFILE,ENDSTRING);
	SD.appendln(OTHERSFILE,ENDSTRING);
	
	// set variables for printing
	mobNumLinesAfter = SD.numln(MOBILEFILE);
	handsNumLinesAfter = SD.numln(HANDSFREEFILE);

	return flag;

}

#ifdef DEBUG_MODE
// Test purposes only
void WaspBT_Pro::printBuffer(){
	int flag;
	flag= serialAvailable(1);
	if (flag>900)	USB.println("_-OVERFLOW-_");
	
}

// Test purposes only
void WaspBT_Pro::printBuffer2(){
	int flag;
	flag= serialAvailable(1);
	//USB.println(flag);
	if (flag>900)	USB.println("_-*OVERFLOW in summary*-_");
	
}
#endif

WaspBT_Pro BT_Pro=WaspBT_Pro();		//object
