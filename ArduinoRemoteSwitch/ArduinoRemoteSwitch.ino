#include <aJSON.h>
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


// Arduino       WiFly
//  2 - receive  TX   (Send from Wifly, Receive to Arduino)
//  3 - send     RX   (Send from Arduino, Receive to WiFly) 
WiFlySerial WiFly(2,3); 

char chOut; 

#define REQUEST_BUFFER_SIZE 180
#define HEADER_BUFFER_SIZE 150 
#define BODY_BUFFER_SIZE 100
#define HTTP_RESPONSE_BUFFER_SIZE 400
#define DAYS 7
#define SCHEDULEBLOCKS 24
#define HOURFALSE 255

char bufRequest[REQUEST_BUFFER_SIZE];
char bufHeader[HEADER_BUFFER_SIZE];
char bufBody[BODY_BUFFER_SIZE];
char bufResponse[HTTP_RESPONSE_BUFFER_SIZE];

bool* _daysSchedule[DAYS];
bool _servoState = false;
byte _onHour = HOURFALSE;
String _mac; 


#define SettingsFullUrl "https://dl.dropbox.com/u/1343111/ServerTimer/autoSwitchSettings.txt"
//#define SETTINGS_HOST "dl.dropbox.com"
#define SETTINGS_HOST "107.20.162.164"
#define SettingsUrl "/u/1343111/ServerTimer/autoSwitchSettings.txt"
char* dayNames[DAYS] = {"Sunday",    "Monday",    "Tuesday",    "Wednesday",    "Thursday",    "Friday",    "Saturday"};

// the setup routine runs once when you press reset:
void setup() {
	Serial.begin(9600);
	InitSchedule();
	//connectToWiFly();
	
	//char* page = getHttp(SETTINGS_HOST,SettingsUrl);
	char* page = "{00001010100000000000000000000000100000000000000000000000100000000000000000000000100000000000000000000000100000000000000000000000100000000000000000000000100000000000000000";
	Serial << "page" << page << endl;
	ParseSchedule(page);
	
	Serial << F("Free memory:") << freeMemory() << endl;
	CheckSchedule();
	connectToWiFly();
}

// the loop routine runs over and over again forever:
void loop() {
  
  while(WiFly.available() > 0) {
    Serial.write(WiFly.read());
  }
  
  if(Serial.available()) { // Outgoing data
    WiFly.write( (chOut = Serial.read()) );
    Serial.write (chOut);
  }
  // check the date and set the _servoState
  Serial << "hour():" << hour() << endl;
  Serial << "weekday()-1:" << weekday()-1 << endl;
  Serial << "_onHour:" << _onHour << " - " << (_onHour == hour())  << endl;
  bool* daySched = _daysSchedule[weekday()-1];
 // Serial << "_daysSchedule[weekday()-1]:" <<  daySched << endl;
  Serial << "daySched[hour()]:" << daySched[hour()]  << endl;
  bool isCurrentlyOn = _onHour == hour() || _daysSchedule[weekday()-1][hour()];

  Serial << "Day:" <<dayNames[weekday()-1] << "[" << weekday()-1 << "] Hour:" << hour() << " IsOn:" << isCurrentlyOn  << endl;
  
  
  delay(15000);
} 

void InitSchedule() {
	for (byte day = 0; day < DAYS ; day++) {
		bool blocks[SCHEDULEBLOCKS] = {false};
		_daysSchedule[day] = blocks;
	}
}

void CheckSchedule() {
	
	bool* list;
	bool hOn;
	for (byte day = 0; day < 1 ; day++) {
		for (byte houra = 0; houra < SCHEDULEBLOCKS ; houra++) {
			list = _daysSchedule[day];
			hOn = list[houra];
			Serial << "Day:" << dayNames[day]  << " Hour:" << houra << " On:" << hOn << endl;
		}	
	}
}

void ParseSchedule(char* scheduleString) {
	byte pointer = 0; // ship first char
	if (scheduleString[pointer++] != '{') {
		Serial << "Invalid scheduleString" << endl;
		return;
	}
	bool hOn;
	if (ParseScheduleChar(scheduleString,pointer++)) {
		_onHour =  hour();
		Serial << "Enable the switch for hour of :" << _onHour << endl;
	}
	else {
		_onHour =  HOURFALSE;
	}
	
	for (byte day = 0; day < DAYS ; day++) {
		for (byte hour = 0; hour < SCHEDULEBLOCKS ; hour++) {
			hOn = ParseScheduleChar(scheduleString,pointer++);
			if (hOn) {
				Serial << "Day:" << day  << " Hour:" << hour << " On:" << hOn << endl;
			}
			_daysSchedule[day][hour] = hOn;
		}	
	}
}

bool ParseScheduleChar(char* scheduleString,byte pointer) {
	return scheduleString[pointer] == '1';
}

void connectToWiFly() {
	Serial.println(F("Connecting to WiFly" ));
	WiFly.begin();
	_mac = String(WiFly.getMAC(bufRequest, REQUEST_BUFFER_SIZE)) ;
	Serial << F("Starting WiFly ") <<  WiFly.getLibraryVersion(bufRequest, REQUEST_BUFFER_SIZE) << " on " << _mac  << endl;

	ConnectToWiFlyAndGetTime();
	WiFly.closeConnection();
  
	// clear out prior requests.
	setWelcomeText();
	clearOutPriorRequests();
	Serial << F("Free memory:") << freeMemory() << endl;
}
void fullSetup() {
	Serial << "Leave:" <<  ssid << WiFly.leave() << endl;
	WiFly.setAuthMode( WIFLY_AUTH_WPA1_2);
	WiFly.setJoinMode( WIFLY_JOIN_AUTO );
	WiFly.setChannel("11");
	WiFly.setDHCPMode( WIFLY_DHCP_ON );
	// join
	if (WiFly.setSSID(ssid) ) {    
		Serial << "SSID Set :" << ssid << endl;
	}
	if (WiFly.setPassphrase(passphrase)) {
		Serial << "Passphrase Set :" << passphrase << endl;
	}
}

void ConnectToWiFlyAndGetTime() {
	WiFly.getDeviceStatus();
	if (!WiFly.isifUp()) 
	{
		Serial << "Joining... :"<< ssid << endl;
		if ( WiFly.join() ) 
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
	setNTPAndSetTheCurrentTime();
}

void  setNTPAndSetTheCurrentTime() {
  Serial << F("Connect to ntp server: ") << ntp_server << endl;
  WiFly.setNTP(ntp_server); 
  WiFly.setNTP_Update_Frequency("15");
  WiFly.setNTP_UTC_Offset(2);
  setTime(WiFly.getTime());
  Serial << F("DateTime: ") << year() << "-" << month() << "-" << day() << " " << _DEC(hour()) << ":" << minute() << ":" << second() << endl;
  setSyncProvider( getSyncProvider );
  getSyncProvider();
  WiFly.exitCommandMode();
}

time_t getSyncProvider() {
  time_t tCurrent = (time_t) WiFly.getTime();
  return tCurrent;
}

void setWelcomeText() {
	//Set the welcom string to ""
	WiFly.SendCommand("set comm remote 0",">", bufBody, BODY_BUFFER_SIZE);
	memset (bufBody,'\0',BODY_BUFFER_SIZE);
}

void clearOutPriorRequests() {
	
	WiFly.flush();
	while (WiFly.available() )
	WiFly.read();
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
  if (WiFly.openConnection( host ) ) {
    
    WiFly <<  (const char*) strRequest << endl; 
    
    // Show server response
    unsigned long TimeOut = millis() + 3000;

    char currentChar = '2';
    byte counter = 0;
    boolean headerDone = false;
    while (  TimeOut > millis() && WiFly.isConnectionOpen() ) {
      if (  WiFly.available() > 0 ) {
		 currentChar = WiFly.read();
		 headerDone = (headerDone || currentChar == '{') && currentChar != '}'; 
         if (headerDone) {
            response.write(currentChar);
         }
      }
	}
    if (TimeOut < millis()) {
      Serial << F("Timed out!") << host << endl;
    }
	WiFly.closeConnection();
  }
  else 
  {
    // Failed to open connection
    Serial << F("Failed to connect to:") << host << endl;
  }  
  WiFly.setDebugChannel( NULL );
  return bufResponse;
}
void getConnectionInfo() {
	Serial << 
		F("IP: ") << WiFly.getIP(bufRequest, REQUEST_BUFFER_SIZE) << endl <<
		F("Netmask: ") << WiFly.getNetMask(bufRequest, REQUEST_BUFFER_SIZE) << endl <<
		F("Gateway: ") << WiFly.getGateway(bufRequest, REQUEST_BUFFER_SIZE) << endl <<
		F("DNS: ") << WiFly.getDNS(bufRequest, REQUEST_BUFFER_SIZE) << endl  /*<<
		F("WiFly Sensors: ") << bufBody <<  WiFly.SendCommand("show q 0x177 ",">", bufBody, BODY_BUFFER_SIZE) << endl << 
		F("WiFly Temp: ") <<  WiFly.SendCommand("show q t ",">", bufBody, BODY_BUFFER_SIZE) << endl  << 
		F("WiFly battery: ") << WiFly.getBattery(bufBody, BODY_BUFFER_SIZE) << endl*/
		;
}
