#include <Servo.h> 
#include <Arduino.h>
#include <Time.h>
#include <SoftwareSerial.h>
#include <Streaming.h>
#include <PString.h>
#include <WiFlySerial.h>
#include <String.h>
//#include <TimeAlarms.h>
#include "MemoryFree.h"
#include "Credentials.h"

// Arduino       _wiflySerial
//  2 - receive  TX   (Send from _wiflySerial, Receive to Arduino)
//  3 - send     RX   (Send from Arduino, Receive to _wiflySerial) 
WiFlySerial _wiflySerial(2,3); 

char chOut; 

#define REQUEST_BUFFER_SIZE 180
#define HEADER_BUFFER_SIZE 150 
#define BODY_BUFFER_SIZE 100
#define HTTP_RESPONSE_BUFFER_SIZE 200

#define DAYS 7
#define SCHEDULEBLOCKS 24
#define HOURFALSE 255
#define SHEDULE_DOWNLOAD_SCHEDULE 30
#define SHEDULE_CHECK_SCHEDULE 5

char bufRequest[REQUEST_BUFFER_SIZE];
char bufHeader[HEADER_BUFFER_SIZE];
char bufBody[BODY_BUFFER_SIZE];
char bufResponse[HTTP_RESPONSE_BUFFER_SIZE];

bool _daysSchedule[DAYS*SCHEDULEBLOCKS] = {false};
bool _servoState = false;
byte _onHour = HOURFALSE;
String _mac; 
unsigned long _nextScheduleCheck = 0;
unsigned long _nextDownload = 0;


#define SettingsFullUrl "https://dl.dropbox.com/u/1343111/ServerTimer/autoSwitchSettings.txt"
//#define SETTINGS_HOST "dl.dropbox.com"
#define SETTINGS_HOST "107.20.162.164"
#define SettingsUrl "/u/1343111/ServerTimer/autoSwitchSettings.txt"

// the setup routine runs once when you press reset:
void setup() {
	Serial.begin(9600);
	Serial << "connectToWiFly " <<  F("Free memory:") << freeMemory() << endl;
	_wiflySerial.begin();
	_mac = String(_wiflySerial.getMAC(bufRequest, 20)) ;
	Serial << F("Starting _wiflySerial ") <<  _wiflySerial.getLibraryVersion(bufRequest, REQUEST_BUFFER_SIZE) << " on " << _mac  << endl;
	
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
		
	}
	setNTPAndSetTheCurrentTime();
	
	_wiflySerial.closeConnection();
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
  
  
  if (millis() > _nextDownload) {
	   Serial << "DownloadNewSchedule " <<  F("Free memory:") << freeMemory() << endl;
	   char* page = getHttp(SETTINGS_HOST,SettingsUrl);
	   Serial << "page" << page << endl;
	   parseSchedule(page);
	   _nextDownload = millis() + (SHEDULE_DOWNLOAD_SCHEDULE * 1000);
	   checkSchedule();
  }

  if (millis() > _nextScheduleCheck) {
	  bool isCurrentlyOn = _onHour == hour() || getSchedule(weekday()-1,hour());
	  Serial << "Day:" << (weekday()-1) << "[" << weekday()-1 << "] Hour:" << hour() << " IsOn:" << isCurrentlyOn  << endl;
	  switchServo(isCurrentlyOn);
	  _nextScheduleCheck = millis() + (SHEDULE_CHECK_SCHEDULE * 1000);
  }

  
  delay(100);
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

void switchServo(bool value) {
	if (_servoState != value) {
		_servoState = value;
		Serial << "Servo is now " << _servoState << endl;
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

void  setNTPAndSetTheCurrentTime() {
  Serial << "setNTPAndSetTheCurrentTime " << ntp_server <<  F("Free memory:") << freeMemory() << endl;
  
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



char* getHttp(char* host, char* url) {
  
  PString strRequest(bufRequest, REQUEST_BUFFER_SIZE);
  PString response(bufResponse, HTTP_RESPONSE_BUFFER_SIZE);

  // Build GET expression
  strRequest << F("GET ") << url << "?mac=" <<  _mac
     << F(" HTTP/1.1") << "\n"
     << F("Host: ") <<  host << "\n"
     << F("Connection: close") << "\n"
     << "\n\n";

  // send data via request
  // close connection
  
  Serial << F("GET request:")  << strRequest <<  endl << F("RAM: ") << freeMemory() << endl;

  // Open connection, then sent GET Request, and display response.
  if (_wiflySerial.openConnection( host ) ) {
    
    _wiflySerial <<  (const char*) strRequest << endl; 
    
    // Show server response
    unsigned long TimeOut = millis() + 3000;

    char currentChar = '2';
    byte counter = 0;
    boolean headerDone = false;
    while (  TimeOut > millis() && _wiflySerial.isConnectionOpen() ) {
      if (  _wiflySerial.available() > 0 ) {
		 currentChar = _wiflySerial.read();
		 headerDone = (headerDone || currentChar == '{') && currentChar != '}'; 
         if (headerDone) {
            response.write(currentChar);
         }
      }
	}
    if (TimeOut < millis()) {
      Serial << F("Timed out!") << host << endl;
    }
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