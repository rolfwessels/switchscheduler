#include <aJSON.h> 
#include <Servo.h> 
#include <Arduino.h>
#include <Time.h>
#include <SoftwareSerial.h>
#include <Streaming.h>
#include <PString.h>
#include <WiFlySerial.h>

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
#define TEMP_BUFFER_SIZE 100

char bufRequest[REQUEST_BUFFER_SIZE];
char bufHeader[HEADER_BUFFER_SIZE];
char bufBody[BODY_BUFFER_SIZE];
char* mac;

#define SettingsFullUrl "https://dl.dropbox.com/u/1343111/ServerTimer/autoSwitchSettings.txt"
#define SETTINGS_HOST "dl.dropbox.com"
#define SettingsUrl "/u/1343111/ServerTimer/autoSwitchSettings.txt"


// the setup routine runs once when you press reset:
void setup() {                
    Serial.begin(9600);
  Serial.println(F("Starting WiFly Tester." ) );
  Serial << F("Free memory:") << freeMemory() << endl;  
  Serial << "Trying to connect using the Credentials :"<< ssid << " - "<< passphrase << endl;
  WiFly.begin();
  // get MAC
  Serial << "Connection made"<< endl;
  Serial << F("Starting WiFly...") <<  WiFly.getLibraryVersion(bufRequest, REQUEST_BUFFER_SIZE)  << endl;
  
  // get MAC
  mac = WiFly.getMAC(bufRequest, REQUEST_BUFFER_SIZE);

  Serial << "Mac:" << mac << endl;
  /*
  WiFly.setAuthMode( WIFLY_AUTH_WPA1_2);
  WiFly.setJoinMode( WIFLY_JOIN_AUTO );
  WiFly.setChannel("11");
  WiFly.setDHCPMode( WIFLY_DHCP_ON );
 */
  // if not connected restart link
  WiFly.getDeviceStatus();
  if (! WiFly.isifUp() ) {
    /*
	  Serial << "Leave:" <<  ssid << WiFly.leave() << endl;
    // join
    if (WiFly.setSSID(ssid) ) {    
      Serial << "SSID Set :" << ssid << endl;
    }
    if (WiFly.setPassphrase(passphrase)) {
      Serial << "Passphrase Set :" << passphrase << endl;
    }
	*/
    Serial << "Joining... :"<< ssid << endl;

    if ( WiFly.join() ) {
      Serial << F("Joined ") << ssid << F(" successfully.") << endl;
    //  WiFly.setNTP( ntp_server ); // use your favorite NTP server
    } else {
      Serial << F("Join to ") << ssid << F(" failed.") << endl;
    }
  } // if not connected

  //getConnectionInfo();
  setNTPAndSetTheCurrentTime();
  WiFly.SendCommand("set comm remote 0",">", bufBody, BODY_BUFFER_SIZE);
  memset (bufBody,'\0',BODY_BUFFER_SIZE);
  
  WiFly.closeConnection();
  
  // clear out prior requests.
  WiFly.flush();
  while (WiFly.available() )
    WiFly.read();

  
  downloadSettings();
  Serial << F("Free memory:") << freeMemory() << endl;
  delay(1123122);
}

// the loop routine runs over and over again forever:
void loop() {
  // Always display a response uninterrupted by typing
  // but note that this makes the terminal unresponsive
  // while a response is being received.
  
  while(WiFly.available() > 0) {
    Serial.write(WiFly.read());
  }
  
  if(Serial.available()) { // Outgoing data
    WiFly.write( (chOut = Serial.read()) );
    Serial.write (chOut);
  }
  
} 

void  setNTPAndSetTheCurrentTime() {
  Serial << F("Connect to ntp server: ") << ntp_server << endl;
  WiFly.setNTP(ntp_server); 
  WiFly.setNTP_Update_Frequency("15");
  setTime(WiFly.getTime());
  Serial << F("DateTime: ") << year() << "-" << month() << "-" << day() << " " << _DEC(hour()) << ":" << minute() << ":" << second() << endl;
  setSyncProvider( getSyncTime );
}

time_t getSyncTime() {
  time_t tCurrent = (time_t) WiFly.getTime();
  //Serial << F("SyncTime: ") << year() << "-" << month() << "-" << day() << " " << _DEC(hour()) << ":" << minute() << ":" << second() << endl;
  WiFly.exitCommandMode();
  return tCurrent;
}


int downloadSettings() {

  const int size = 600;
  char bufResponse[size];
  PString strRequest(bufRequest, REQUEST_BUFFER_SIZE);
  PString response(bufResponse, size);

  // Build GET expression
  strRequest << F("GET ") << SettingsUrl
     << F(" HTTP/1.1") << "\n"
     << F("Host: ") <<  SETTINGS_HOST << "\n"
     << F("Connection: close") << "\n"
     << "\n\n";

  // send data via request
  // close connection
  
  Serial << F("GET request:")  << strRequest <<  endl << F("RAM: ") << freeMemory() << endl;

  // Open connection, then sent GET Request, and display response.
  if (WiFly.openConnection( SETTINGS_HOST ) ) {
    
    WiFly <<  (const char*) strRequest << endl; 
    
    // Show server response
    unsigned long TimeOut = millis() + 30;

    char currentChar = '2';
    byte counter = 0;
    boolean headerDone = false;
    while (  TimeOut > millis() && WiFly.isConnectionOpen() ) {
      if (  WiFly.available() > 0 ) {
		 currentChar = WiFly.read();
         headerDone = (headerDone || currentChar == '{') && currentChar != '*'; 
         if (headerDone) {
            response.write(currentChar);
         }
         
      }
	}

    if (TimeOut < millis()) {
      Serial << F("Timed out!") << SETTINGS_HOST << endl;
    }

    Serial << "Response: " << response << endl;
	ParseString(response);
    // Force-close connection
    WiFly.closeConnection();
 
  } else {
    // Failed to open connection
    Serial << F("Failed to connect to:") << SETTINGS_HOST << endl;
  }
  
  WiFly.setDebugChannel( NULL );
  return 0;
}

void ParseString(PString json_string) {
	
	//aJsonObject* jsonObject = aJson.parse(new aJsonStringStream(json_string));

}

void getConnectionInfo() {
   Serial << F("IP: ") << WiFly.getIP(bufRequest, REQUEST_BUFFER_SIZE) << endl <<
    F("Netmask: ") << WiFly.getNetMask(bufRequest, REQUEST_BUFFER_SIZE) << endl <<
    F("Gateway: ") << WiFly.getGateway(bufRequest, REQUEST_BUFFER_SIZE) << endl <<
    F("DNS: ") << WiFly.getDNS(bufRequest, REQUEST_BUFFER_SIZE) << endl 
    << F("WiFly Sensors: ") << bufBody <<  WiFly.SendCommand("show q 0x177 ",">", bufBody, BODY_BUFFER_SIZE) << endl
    << F("WiFly Temp: ") <<  WiFly.SendCommand("show q t ",">", bufBody, BODY_BUFFER_SIZE)
    << F("WiFly battery: ") << WiFly.getBattery(bufBody, BODY_BUFFER_SIZE) << endl;
}
