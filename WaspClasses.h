/*
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
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Version:		0.7
 *  Design:			David Gascón
 *  Implementation:	David Cuartielles, Alberto Bielsa, Yuri Carmona
 */
   
  
// strings and math
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// interrupts and sleep
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/sleep.h>

// interrupt core (must be called before Wire)
//#include "WInterrupts.h"

#include <alloca.h>

// I2C libraries
#include "Wire.h"
#include "twi.h"

#include "pins_waspmote.h"

#include "WaspConstants.h"

#ifndef __WASPXBEECONSTANTS_H__
	#include "WaspXBeeConstants.h"
#endif

#include "wiring.h"

// SD
/*
#include "sd_raw_config.h"
#include "sd_raw.h"
#include "partition.h"
#include "fat_config.h"
#include "fat.h"

#include "Sd2Card.h"
#include "Sd2PinMap.h"
#include "SdFat.h"
#include "SdFatUtil.h"
#include "SdInfo.h"
*/


#ifdef __cplusplus

#include "WaspUtils.h"
#include "HardwareSerial.h"
#include "WaspUSB.h"

#include "WaspXBee.h"

	#ifdef USE_WASP_GPS
		#include "WaspGPS.h"
	#endif	

	#ifdef USE_WASP_GPRS
		#include "WaspGPRS.h"
		#include "WaspGPRSconstants.h"
	#endif	

#include "Wire.h"
#include "WaspRTC.h"

	#ifdef USE_WASP_ACC
		#include "WaspACC.h"
	#endif
	
	#ifdef USE_WASP_SD
		#include "WaspSD.h"
	#endif

	#include "WaspPWR.h"
	#include "WaspXBeeCore.h"

	#ifdef USE_WASP_XBEE802
		#include "WaspXBee802.h"
	#endif
	
#include "WaspXBeeZB.h"

	#ifdef USE_WASP_XBEE_DM
		#include "WaspXBeeDM.h"
	#endif	

	#ifdef USE_WASP_XBEE868
		#include "WaspXBee868.h"
	#endif	

	#ifdef USE_WASP_XBEE_XSC
		#include "WaspXBeeXSC.h"
	#endif	
	
	#ifdef USE_WASP_SENSOR_AGR	
		#include "WaspSensorAgr.h"
	#endif
	
	#ifdef USE_WASP_SENSOR_GAS	
		#include "WaspSensorGas.h"
	#endif
	
	#ifdef USE_WASP_SENSOR_EVENT
		#include "WaspSensorEvent.h"
	#endif
	
	#ifdef USE_WASP_SENSOR_PROTOTYPING
		#include "WaspSensorPrototyping.h"
	#endif

	#ifdef USE_WASP_BT
		#include "WaspBT.h"
		#include "WaspBTConstants.h"
	#endif
	
	#ifdef USE_WASP_SENSOR_SMART	
		#include "WaspSensorSmart.h"
	#endif	
	
	
#include "MemoryFree.h"


	#ifdef USE_WASP_SENSOR_CITIES
		#include "WaspSensorCities.h"
	#endif

	#ifdef USE_WASP_SENSOR_PARKING
		#include "WaspSensorParking.h"
	#endif

	#ifdef WASPBT_PRO
		#include "WaspBT_Pro.h"
	#endif

	#ifdef USE_WIFI
		#include "WaspWIFI.h"
	#endif

	#ifdef USE_WASP_GPRS_PRO	
		#include "WaspGPRS_Pro.h"
		#include "WaspGPRS_Proconstants.h"
	#endif
		
	#ifdef USE_WASP_SENSOR_SMART_V20
		#include "WaspSensorSmart_v20.h"
	#endif

	#ifdef WASP_SENSOR_RADIATION	
		#include "WaspSensorRadiation.h"
	#endif	

#include "WaspSensorAgr_v20.h"

	#ifdef USE_WASP_RFID
		#include "WaspRFID13.h"
	#endif
	
	#ifdef USE_SOFTWARE_SERIAL
		#include "SoftwareSerial.h"
	#endif
	
#include "WaspSensorGas_v20.h"

	#ifdef USE_WASP_SENSOR_PROTOTYPING_V20 
		#include "WaspSensorPrototyping_v20.h"
	#endif

// random prototypes
long random(long);
long random(long, long);
void randomSeed(unsigned int);

#endif
