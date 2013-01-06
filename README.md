Switch Scheduler
===============

Remotely schedule a switch to go on and off

The Idea
============
The idea is to have a web server host some scheduling setting. Then I would have an arduino with a wifly connected poll for new setting every 10 minutes. Based on the schedule I want to switch 1 or more switches on and off.

The story
==============
Initially I wanted this schedule to run for my geyser. Obviously a geyser pulls a lot of current and I was scared to tinker with a relay, so the plan was to switch it on and off using a servo and a little printed part. This would work on a normal switch but unfortunately this switch requires too much force for my servo. 

![Alt text](https://github.com/rolfwessels/switchscheduler/raw/master/Photos/2013-01-06 08.45.46.jpg "3d printed part to toggle the switch")

Next would be to do it the right way using a relay. I went to my local electronic shop and picked up a hf18ff relay. It is a 12v relay and the arduino pins only push out 5v so I need to up the voltage somehow.

![Alt text](https://github.com/rolfwessels/switchscheduler/raw/master/Photos/2013-01-02 19.42.32.jpg "The back of the switch that will be hacked")
![Alt text](https://github.com/rolfwessels/switchscheduler/raw/master/Photos/2013-01-06 08.44.46.jpg "The relay and the wifly")
![Alt text](https://github.com/rolfwessels/switchscheduler/raw/master/Photos/2013-01-02 20.30.57.jpg "The suggested way of getting 5v to 12v")

Missing bits
=============

- Develop the scheduling server
- Updating fritzing file
- Togle the 12v relay
- Installing the relay
- Create instructable


Component list
===============

- arduino uno
- wifly
- Relay [hf18ff]
- diode [1n4007]
- 1k resistor
- 10k resistor
- transistor [pn2222A]
- usb power supply


Required Arduino libraries
==============================

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
==================================================
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
