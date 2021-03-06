﻿# XBee Timing Box

## Goal.

The goal of this timing box is to have low latency, high reliability, and usability in a clinical setting. It must also have sub-millisecond variability in the latency of the pulses.

The XBee (IEEE 802.15.4) protocol fits the bill. It has low latency at about 10.0(1) ms and seems to be reliable in the clinical setting. XBee also has a thriving community, largely driven by its usage in RC drones. A breakout board from SparkFun (Fio V3) already takes care of most of the work. It is mounted on a board (or even a proto-board) and connected to a shift register to give a TTL-type timing pulse. 
