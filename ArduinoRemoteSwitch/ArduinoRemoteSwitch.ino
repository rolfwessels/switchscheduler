#include <Servo.h> 
#include <Arduino.h> 
#include <Time.h>
#include <SoftwareSerial.h>
#include <Streaming.h>
#include <PString.h>
#include <WiFlySerial.h>
#include <String.h>
#include "MemoryFree.h"
#include "Credentials.h"

#define REQUEST_BUFFER_SIZE 150
#define HTTP_RESPONSE_BUFFER_SIZE 200

#define DAYS 7
#define SCHEDULEBLOCKS 24
#define HOURFALSE 255
#define SHEDULE_DOWNLOAD_SCHEDULE 20//300
#define SHEDULE_CHECK_SCHEDULE 5

#define SettingsFullUrl "https://dl.dropbox.com/u/1343111/ServerTimer/autoSwitchSettings.txt"
#define SETTINGS_HOST "dl.dropbox.com"
//#define SETTINGS_HOST "23.21.195.136"
#define SettingsUrl "/u/1343111/ServerTimer/autoSwitchSettings.txt"
#define SERVO_LOW 100
#define SERVO_HIGH 150

char bufRequest[REQUEST_BUFFER_SIZE];
char bufResponse[HTTP_RESPONSE_BUFFER_SIZE];

bool _daysSchedule[DAYS*SCHEDULEBLOCKS] = {false};
bool _servoState = false;
byte _onHour = HOURFALSE;
String _mac; 
unsigned long _nextScheduleCheck = 0;
unsigned long _nextDownload = 0;
int pinLed = 13;
int pinServo = 12;
// Arduino       _wiflySerial
//  2 - receive  TX   (Send from _wiflySerial, Receive to Arduino)
//  3 - send     RX   (Send from Arduino, Receive to _wiflySerial) 
WiFlySerial _wiflySerial(2,3); 
Servo _servo; 
char chOut; 



// the setup routine runs once when you press reset:
void setup() {
	Serial.begin(9600);
	pinMode(pinLed, OUTPUT);
	_servo.attach(pinServo); 
	switchServo(false);
}


// the loop routine runs over and over again forever:
void loop() {
  while(_wiflySerial.available() > 0) {
    Serial.write(_wiflySerial.read());
  }  
  if(Serial.available()) { // Outgoing data
    _wiflySerial.write( (chOut = Serial.read()) );
    Serial.write (chOut);
  }

  // download a schedule every 15 minutes
  if (millis() > _nextDownload) {
	   connectToWiFly();
	   downloadNewSchedule();
	   _nextDownload = millis() + (SHEDULE_DOWNLOAD_SCHEDULE * 1000);
  }

  // check the schedule every 5 seconds
  if (millis() > _nextScheduleCheck) {
	  checkScheduleAndUpdateSwitch();
	  _nextScheduleCheck = millis() + (SHEDULE_CHECK_SCHEDULE * 1000);
  }
    
  delay(100);
} 


void downloadNewSchedule() {
	Serial << "downloadNewSchedule " <<  F("Free memory:") << freeMemory() << endl;
	char* page = getHttp(SETTINGS_HOST,SettingsUrl);
	parseSchedule(page);
	checkSchedule();
	checkScheduleAndUpdateSwitch();
}

void switchServo(bool value) {
	if (_servoState != value) {
		_servoState = value;
		
		if (_servoState) {
			digitalWrite(pinLed, HIGH);
			_servo.write(SERVO_LOW); 
		}
		else {
			digitalWrite(pinLed, LOW);
			_servo.write(SERVO_HIGH); 
		}
		
		Serial << "Servo is now " << _servoState << endl;
	}
}

void checkScheduleAndUpdateSwitch() {
  bool isCurrentlyOn = _onHour == hour() || getSchedule(weekday()-1,hour());
  Serial << "Day:" <<weekday()-1 << "[" << weekday()-1 << "] Hour:" << hour() << ":" << minute() << ":" << second()  << " IsOn:" << isCurrentlyOn  << endl;
  switchServo(isCurrentlyOn);
}

void checkSchedule() {
	Serial << "checkSchedule " <<  F("Free memory:") << freeMemory() << endl;
	bool hOn;
	for (byte day = 0; day < DAYS ; day++) {
		for (byte hour = 0; hour < SCHEDULEBLOCKS ; hour++) {
			hOn = getSchedule(day,hour);
			if (hOn) {
				Serial << "Day:" << day  << " Hour:" << hour << " On:" << hOn << endl;
			}
		}	
	}
}

void parseSchedule(char* scheduleString) {
	Serial << "parseSchedule " <<  F("Free memory:") << freeMemory() << endl;
	byte pointer = 0; // ship first char
	if (scheduleString[pointer++] != '{') {
		Serial << "Invalid scheduleString" << endl;
		return;
	}
	bool hOn;
	if (parseScheduleChar(scheduleString,pointer++)) {
		_onHour =  hour();
		Serial << "Enable the switch for hour of :" << _onHour << endl;
	}
	else {
		_onHour =  HOURFALSE;
	}
	
	for (byte day = 0; day < DAYS ; day++) {
		for (byte hour = 0; hour < SCHEDULEBLOCKS ; hour++) {
			hOn = parseScheduleChar(scheduleString,pointer++);
			//if (hOn) { Serial << "Day:" << day  << " Hour:" << hour << " On:" << hOn << endl; }
			putSchedule(day, hour ,hOn);
		}	
	}
}


bool parseScheduleChar(char* scheduleString,byte pointer) {
	return scheduleString[pointer] == '1';
}

void putSchedule(byte day, byte hour ,bool value) {
	byte pos = ((day) * SCHEDULEBLOCKS)  + hour;
	//Serial << "putSchedule " << day << " " << hour << " " << value << " " << pos << endl;
	_daysSchedule[pos]  = value;
}

bool getSchedule(byte day, byte hour ) {
	byte pos = ((day) * SCHEDULEBLOCKS)  + hour;
	return _daysSchedule[pos];
}

void connectToWiFly() {
	Serial << "connectToWiFly " <<  F("Free memory:") << freeMemory() << endl;
	_wiflySerial.begin();
	_mac = String(_wiflySerial.getMAC(bufRequest, REQUEST_BUFFER_SIZE)) ;
	Serial << F("Starting _wiflySerial ") <<  _wiflySerial.getLibraryVersion(bufRequest, REQUEST_BUFFER_SIZE) << " on " << _mac  << endl;
	
	joinWiflyIfNotUp();
	setNTPAndSetTheCurrentTime();
	setWelcomeText();
	_wiflySerial.closeConnection();
  
	// clear out prior requests.
	clearOutPriorRequests();
}
void fullSetup() {
	Serial << "Leave:" <<  ssid << _wiflySerial.leave() << endl;
	_wiflySerial.setAuthMode( WIFLY_AUTH_WPA1_2);
	_wiflySerial.setJoinMode( WIFLY_JOIN_AUTO );
	_wiflySerial.setChannel("11");
	_wiflySerial.setDHCPMode( WIFLY_DHCP_ON );
	// join
	if (_wiflySerial.setSSID(ssid) ) {    
		Serial << "SSID Set :" << ssid << endl;
	}
	if (_wiflySerial.setPassphrase(passphrase)) {
		Serial << "Passphrase Set :" << passphrase << endl;
	}
}

void joinWiflyIfNotUp() {
	_wiflySerial.getDeviceStatus();
	if (!_wiflySerial.isifUp()) 
	{
		Serial << "Joining... :"<< ssid << endl;
		if ( _wiflySerial.join() ) 
		{
			Serial << F("Joined ") << ssid << F(" successfully.") << endl;
		} 
		else 
		{
			Serial << F("Join to ") << ssid << F(" failed.") << endl;
		}
	}
	else {
		Serial << "Connection is up... :"<< ssid << endl;
		getConnectionInfo();
	}
	
}

void  setNTPAndSetTheCurrentTime() {
  Serial << "setNTPAndSetTheCurrentTime " << ntp_server <<  F(" Free memory:") << freeMemory() << endl;
  
  _wiflySerial.setNTP(ntp_server); 
  _wiflySerial.setNTP_Update_Frequency("15");
  _wiflySerial.setNTP_UTC_Offset(2);
  setTime(_wiflySerial.getTime());
  Serial << F("DateTime: ") << year() << "-" << month() << "-" << day() << " " << _DEC(hour()) << ":" << minute() << ":" << second() << endl;
  setSyncProvider( getSyncProvider );
  getSyncProvider();
  _wiflySerial.exitCommandMode();
}

time_t getSyncProvider() {
  time_t tCurrent = (time_t) _wiflySerial.getTime();
  return tCurrent;
}

void setWelcomeText() {
	//Set the welcom string to ""
	_wiflySerial.SendCommand("set comm remote 0",">", bufResponse, HTTP_RESPONSE_BUFFER_SIZE);
	memset(bufResponse,'\0',HTTP_RESPONSE_BUFFER_SIZE);
}

void clearOutPriorRequests() {
	
	_wiflySerial.flush();
	while (_wiflySerial.available() )
	_wiflySerial.read();
}

char* getHttp(char* host, char* url) {
  
  PString strRequest(bufRequest, REQUEST_BUFFER_SIZE);
  PString response(bufResponse, HTTP_RESPONSE_BUFFER_SIZE);

  strRequest << F("GET ") << url << "?mac=" <<  _mac;
  
  Serial << F("GET request: http://") << host << strRequest << F(" Free memory:") << freeMemory() << endl;
  // Build GET expression
  strRequest << F(" HTTP/1.1") << "\n"
     << F("Host: ") <<  host << "\n"
     << F("Connection: close") << "\n"
     << "\n\n";
  
  // Open connection, then sent GET Request, and display response.
  if (_wiflySerial.openConnection( host ) ) {
    
    _wiflySerial <<  (const char*) strRequest << endl; 
    
    // Show server response
    unsigned long TimeOut = millis() + 10000;

    char currentChar = '2';
    int counter = 0;
    boolean headerDone = false;
    while (  TimeOut > millis() && _wiflySerial.isConnectionOpen() ) {
      if (  _wiflySerial.available() > 0 ) {
		 currentChar = _wiflySerial.read();
		 headerDone = (headerDone || currentChar == '{') && currentChar != '}'; 
         if (headerDone) {
            response.write(currentChar);
			counter++;
         }
      }
	}
    if (TimeOut < millis()) {
      Serial << F("Timed out!") << host << endl;
    }
	Serial << "Done loading from " << host << " bytes read:"<< counter << " into buffer " << HTTP_RESPONSE_BUFFER_SIZE << endl; 
	_wiflySerial.closeConnection();
  }
  else 
  {
    // Failed to open connection
    Serial << F("Failed to connect to:") << host << endl;
  }  
  _wiflySerial.setDebugChannel( NULL );
  return bufResponse;
}
void getConnectionInfo() {
	Serial << 
		F("IP: ") << _wiflySerial.getIP(bufRequest, REQUEST_BUFFER_SIZE) << endl <<
		F("Netmask: ") << _wiflySerial.getNetMask(bufRequest, REQUEST_BUFFER_SIZE) << endl <<
		F("Gateway: ") << _wiflySerial.getGateway(bufRequest, REQUEST_BUFFER_SIZE) << endl <<
		F("DNS: ") << _wiflySerial.getDNS(bufRequest, REQUEST_BUFFER_SIZE) << endl  /*<<
		F("_wiflySerial Sensors: ") << bufBody <<  _wiflySerial.SendCommand("show q 0x177 ",">", bufBody, BODY_BUFFER_SIZE) << endl << 
		F("_wiflySerial Temp: ") <<  _wiflySerial.SendCommand("show q t ",">", bufBody, BODY_BUFFER_SIZE) << endl  << 
		F("_wiflySerial battery: ") << _wiflySerial.getBattery(bufBody, BODY_BUFFER_SIZE) << endl*/
		;
}
