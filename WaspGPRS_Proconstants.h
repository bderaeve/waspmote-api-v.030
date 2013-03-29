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
 *  Version:		0.1
 *  Design:		David Gascón
 *  Implementation:	Alejandro Gállego
 */
  
 
#ifndef WaspGPRS_Proconstants_h
#define WaspGPRS_Proconstants_h

#include <inttypes.h> 

#define	AT_COMMAND	"AT"

// Calls Constants
#define AT_CALL      "D"  // start phonecall, needs phone number
#define AT_CALL_R    "OK"
#define AT_HANG      "H"  // hang phonecall, no parameters
#define AT_HANG_R    "OK"
#define AT_DTMF "+CLDTMF="
#define AT_DTMF_R    "OK"

// Voice Mode Constants
#define	AT_ID_INCALL	"+CLIP=1"
#define	AT_ID_INCALL_R	"OK"

// SMS Constants
#define AT_SMS       "+CMGS="    // set phone number to send SMS to, needs phone number
#define AT_SMS_R     ">"
#define	AT_SMS_TEXT_R	"OK"
#define AT_SMS_MODE  "+CMGF=1"   // select text mode for SMS
#define AT_SMS_MODE_R "OK"
#define	AT_SMS_INFO	"+CNMI=2,1,0,0,0"
#define	AT_SMS_INFO_R	"OK"
#define	AT_SMS_READ	"+CMGR=" // needs index of sms to read
#define	AT_SMS_READ_R	"OK"
#define	AT_SMS_DELETE	"+CMGD=" // needs index of sms to delete
#define	AT_SMS_DELETE_R	"OK"

// Sound Constants
#define AT_SOUND_INT  "#CAP=2"   // set internal audio path
#define AT_SOUND_INT_R  "OK"
#define AT_SOUND_EXT  "#CAP=1"   // set external audio path
#define AT_SOUND_EXT_R  "OK"
#define AT_VOLUME_SET  "+CLVL="  // set volume for selected audio path, needs number (min)0..10(MAX)
#define AT_VOLUME_SET_R  "OK"

// SIM Constants
#define AT_PIN	"+CPIN="
#define AT_PIN_R	"OK"
#define AT_CHANGE_PASSWORD "+CPWD="
#define AT_CHANGE_PASSWORD_R "OK"

// Power Modes Constants
#define POWER_FULL	"+CFUN=1"
#define	POWER_RF_OFF	"+CFUN=4"
#define	POWER_MIN	"+CFUN=0"
#define	POWER_OFF	"+CPWD=1"
#define POWER_SLEEP_PRO "+CSCLK=2"
#define	POWER_R		"OK"


#define	AT_GPRS_PRO_APN "internetmas"
#define	AT_GPRS_PRO_LOGIN ""
#define	AT_GPRS_PRO_PASSW ""

// FTP_HTTP Constants
#define	AT_GPRS_CFG "+SAPBR="
#define	AT_GPRS_CFG_R "OK"
#define	AT_GPRS "GPRS"
	
#define AT_FTP_ID "+FTPCID="
#define AT_FTP_ID_R "OK"	
#define AT_FTP_PORT "+FTPPORT="
#define AT_FTP_PORT_R "OK"
#define AT_FTP_MODE "+FTPMODE="
#define AT_FTP_MODE_R "OK"
#define AT_FTP_TYPE "+FTPTYPE="
#define AT_FTP_TYPE_R "OK"
#define AT_FTP_SERVER "+FTPSERV="
#define AT_FTP_SERVER_R "OK"
#define AT_FTP_UN "+FTPUN="
#define AT_FTP_UN_R "OK"
#define AT_FTP_PW "+FTPPW="
#define AT_FTP_PW_R "OK"
#define AT_FTP_PUT "+FTPPUT="
#define AT_FTP_PUT_R "OK"
#define AT_FTP_PUT_NAME "+FTPPUTNAME="
#define AT_FTP_PUT_NAME_R "OK"
#define AT_FTP_PUT_PATH "+FTPPUTPATH="
#define AT_FTP_PUT_PATH_R "OK"
#define AT_FTP_GET "+FTPGET="
#define AT_FTP_GET_R "OK"
#define AT_FTP_GET_NAME "+FTPGETNAME="
#define AT_FTP_GET_NAME_R "OK"
#define AT_FTP_GET_PATH "+FTPGETPATH="
#define AT_FTP_GET_PATH_R "OK"

#define AT_HTTP_INIT "+HTTPINIT"
#define AT_HTTP_INIT_R "OK"
#define AT_HTTP_PARAM	"+HTTPPARA="
#define AT_HTTP_PARAM_R "OK"
#define AT_HTTP_ACTION	"+HTTPACTION=" 
#define AT_HTTP_ACTION_R	"+HTTPACTION:0,200," 
#define AT_HTTP_READ "+HTTPREAD"
#define AT_HTTP_READ_R "+HTTPREAD: "
#define AT_HTTP_TERM "+HTTPTERM"
#define AT_HTTP_TERM_R "OK"

#define	AT_GPRS_PRO_IP "0.0.0.0"
#define	AT_GPRS_PRO_DNS1 "212.73.32.3"
#define	AT_GPRS_PRO_DNS2 "212.73.32.67"

// TCP/UDP  constants
#define	AT_IP "IP"
#define AT_SINGLE_CONN "+CIPMUX=0"
#define AT_SINGLE_CONN_R "OK"
#define AT_MULTI_CONN "+CIPMUX=1"
#define AT_MULTI_CONN_R "OK"
#define AT_IP_STATUS	"+CIPSTATUS"
#define AT_IP_STATUS_R	"STATE: "
#define AT_IP_SET_APN "+CSTT="
#define AT_IP_SET_APN_R "OK"
#define AT_IP_BRING "+CIICR"
#define AT_IP_BRING_R "OK"
#define AT_IP_GET_IP "+CIFSR"
#define AT_IP_APP_MODE "+CIPMODE="
#define AT_IP_APP_MODE_R "OK"
#define AT_IP_HEADER "AT+CIPHEAD=1"
#define AT_IP_HEADER_R "OK"
#define	AT_IP_CLIENT "+CIPSTART="
#define AT_TCP	"TCP"
#define AT_UDP	"UDP"
#define AT_CONNECTED_OK	"CONNECT OK"
#define AT_CONNECTED_FAIL	"CONNECT FAIL"
#define	AT_IP_SERVER "+CIPSERVER="
#define	AT_IP_CFG_R "OK"
#define AT_IP_SEND "+CIPSEND"
#define AT_IP_SEND_R "SEND OK"
#define AT_IP_SEND_FAIL "SEND FAIL"	
#define	AT_IP_CLOSE "+CIPCLOSE="
#define AT_IP_CLOSE_R "CLOSE OK"
#define	AT_IP_QCLOSE "+CIPQRCLOSE="
#define AT_IP_QCLOSE_R "OK"	
#define	AT_IP_SHUT "+CIPSHUT"
#define AT_IP_SHUT_R "OK"
#define AT_IP_SET_DNS "+CDNSCFG="
#define AT_IP_SET_DNS_R "OK"
#define AT_IP_QUERY_DNS "+CDNSGIP="
#define AT_IP_QUERY_DNS_R "+CDNSGIP:"
#define AT_IP_LOCAL_PORT "+CLPORT="
#define AT_IP_LOCAL_PORT_R "OK"
#define AT_IP_SAVE_CONF	"+CIPSCONT"
#define AT_IP_SAVE_CONF_R	"OK"
#define AT_IP_HEADER	"+CIPHEAD="
#define AT_IP_HEADER_R	"OK"
#define AT_IP_AUTOSENDING	"+CIPATS="
#define AT_IP_AUTOSENDING_R	"OK"
#define AT_IP_SHOW_REMOTE_IP	"+CIPSRIP="
#define AT_IP_SHOW_REMOTE_IP_R	"OK"
#define AT_IP_PROTOCOL_HEADER	"+CIPSHOWTP="
#define AT_IP_PROTOCOL_HEADER_R	"OK"
#define AT_IP_DISCARD_AT_DATA	"+CIPTXISS="
#define AT_IP_DISCARD_AT_DATA_R	"OK"
#define AT_IP_GET_MANUALLY "+CIPRXGET"
#define AT_IP_GET_MANUALLY_R	"OK"
#define AT_IP_UDP_EXTENDED	"+CIPUDPMODE="
#define AT_IP_UDP_EXTENDED_R	"OK"

#define AT_COMMAND_MODE	"+++"
#define AT_COMMAND_MODE_R	"OK"
#define AT_DATA_MODE	"O"
#define AT_DATA_MODE_R	"CONNECT"
#define AT_DATA_MODE_FAIL	"NO CARRIER"



#define	AT_GPRS_CHECK "+CGATT?"
#define	AT_GPRS_CHECK_ON "+CGATT: 1"
#define	AT_GPRS_CHECK_OFF "+CGATT: 0"	
#define	AT_GPRS_ATT_ON "+CGATT=1"
#define	AT_GPRS_ATT_OFF "+CGATT=0"
#define	AT_GPRS_ATT_R "OK"
#define	AT_GPRS_CELLID	"+CENG"

#define AT_EMAIL_ID "+EMAILCID=3"
#define AT_EMAIL_ID_R "OK"

//SMTP constants
#define	AT_SMTP_SRV "+SMTPSRV="
#define	AT_SMTP_SRV_R "OK"
#define	AT_SMTP_AUTH "+SMTPAUTH="
#define	AT_SMTP_AUTH_R "OK"
#define	AT_SMTP_FROM "+SMTPFROM="
#define	AT_SMTP_FROM_R "OK"
#define	AT_SMTP_RCPT "+SMTPRCPT="
#define	AT_SMTP_RCPT_R "OK"
#define	AT_SMTP_SUBJECT "+SMTPSUB="
#define	AT_SMTP_SUBJECT_R "OK"
#define	AT_SMTP_BODY "+SMTPBODY"
#define	AT_SMTP_BODY_R "OK"
#define	AT_SMTP_SEND "+SMTPSEND"
#define	AT_SMTP_SEND_R "+SMTPSEND: 1"

//POP3 constants
#define	AT_POP3_SRV "+POP3SRV="
#define	AT_POP3_SRV_R "OK"
#define	AT_POP3_IN "+POP3IN="
#define	AT_POP3_IN_R "OK"
#define	AT_POP3_NUM "+POP3NUM="
#define	AT_POP3_NUM_R "OK"
#define	AT_POP3_LIST "+POP3LIST="
#define	AT_POP3_LIST_R "OK"
#define	AT_POP3_UIDL "+POP3UIDL="
#define	AT_POP3_UIDL_R "OK"
#define	AT_POP3_CMD "+POP3CMD="
#define	AT_POP3_CMD_R "OK"
#define	AT_POP3_READ "+POP3READ="
#define	AT_POP3_READ_R "OK"
#define	AT_POP3_DEL "+POP3DEL="
#define	AT_POP3_DEL_R "OK"
#define	AT_POP3_RSET "+POP3RSET="
#define	AT_POP3_RSET_R "OK"
#define	AT_POP3_OUT "+POP3OUT="
#define	AT_POP3_OUT_R "OK"

// IMEI - IMSI Constants
#define AT_GPRS_IMEI "AT+GSN"
#define AT_GPRS_IMSI "AT+CIMI"

//Various
#define AT_SPEAKER_VOLUME "L"
#define AT_SPEAKER_VOLUME_R "OK"
#define AT_SPEAKER_MODE "M"
#define AT_SPEAKER_MODE_R "OK"
#define AT_CLIP_MODE "+CLIP="
#define AT_CLIP_MODE_R "OK"
#define AT_CLIR_MODE "+CLIR="
#define AT_CLIR_MODE_R "OK"
#define AT_PHONE_ACTIVITY "+CPAS"
#define AT_PHONE_ACTIVITY_R "+CPAS:"	
#define AT_ALERT_SOUND_MODE "+CALM="
#define AT_ALERT_SOUND_MODE_R "OK"
#define AT_ALERT_SOUND_LEVEL "+CALS="
#define AT_ALERT_SOUND_LEVEL_R "OK"
#define AT_RINGER_SOUND_LEVEL "+CRSL="
#define AT_RINGER_SOUND_LEVEL_R "OK"
#define AT_SPEAKER_LEVEL "+CLVL="
#define AT_SPEAKER_LEVEL_R "OK"
#define AT_MUTE "+CMUT="
#define AT_MUTE_R "OK"

#define AT_GET_OPERATOR	"+COPS?"
#define AT_GET_OPERATOR_R "+COPS:"
#define AT_SET_PREF_OPERATOR "+COPS="
#define AT_SET_PREF_OPERATOR_R "OK"
#define AT_OPERATOR_LIST "+CPOL?"
#define AT_OPERATOR_LIST_R "+CPOL:"

//Error Constants
#define ERROR_CME "+CME ERROR:"
#define ERROR_CMS "+CMS ERROR:"
#define ERROR	"ERROR"


#endif
