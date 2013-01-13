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
- Store schedule file on sd card for offline scheduling



Component list
===============

- arduino uno
- wifly
- wifly breakout board
- Relay [hf18ff]
- diode [1n4007]
- 1k resistor (R1)
- 10k resistor (R2)
- transistor [pn2222A]
- power supply (12V)

![Alt text](https://github.com/rolfwessels/switchscheduler/raw/master/Photos/12vRelayBoard_schem.jpg "The circuit")


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
All the arduino code is stored in folder ArduinoRemoteSwitch

