Switch Scheduler
===============

Remotely schedule a switch to go on and off

Required libraries
===============

The schedular makes use of the following libraries. These libraries are included in the source code.
```AJson
NewSoftSerial
PString
SoftwareServo
Streaming
Time
TimeAlarms
WiFlySerial
```



How to test your wifly connection using USB
====================
Connect to the wifly to USD
Telnet to the wifly:
Send commands:
```
$$$  //Responds with CMD
scan 30 // scans for networks
join # 3        // Join the third network

// Below sets the auth to | WPA1 | channel 11 | password sogaanditmaar | join on connection | ssid | joins pings and saves connection
set wlan auth 3
set wlan channel 11 
set wlan pass sogaanditmaar     
set wlan join 1
set wlan ssid CC902
join CC902
ping 74.125.233.5 2
save
reboot

set time address 129.6.15.28 
set time zone 22
set time enable 1 
time
show time
save
reboot
```
