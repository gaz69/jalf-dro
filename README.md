# JALF-DRO

1- Description
==============

I have aftermarket digital scales mounted on the two horizontal axes of my chinese mill and have played for some time with the idea of combining them into a single display. I know there are solutions out there both commercial and DIY that solve this or similar problems but found none that worked for me. The commercial products come at a significant cost and they are by nature not as rewarding as a DIY solution. 

I started searching the web looking a suitable DIY solution or at least for information on the protocol used by the scales. What I found was that there is not such thing as THE protocol. Instead there are several protocols used by different manufacturers. Since my scales are typical OEM (no name) products it was clear that i had to reverse engineer the protocol they used to tranfer the data. If I was going to have to do that much, I thought, I could as well implement the whole thing. So I connected my scales to the oscilloscope and started making notes.

I came with following basic requirements for the device:
* it should use affordable and easy to use hardware, if possible no Assembly. Since Arduinos are hard to beat in price, are well documented and easy to program in C/C++, it was not a difficult choice. A chinese-made Arduino UNO was the selected plattform.
* the device should support a minimum of 2 scales directly, with the prospect of supporting 3 scales and may be some sort of angle-finder in the future.
* it should support the scales I already have.
* the first version would use an LCD module as display. I had some large LCD modules (HD44780 interface) in my junk pile which have the added advantage to be directly supported by the LiquidCrystal arduino library so and one of these would have to do. 
* The board (shield) for connecting the scales and the display should be simple and cheap. I decided on a hand soldered solution on a protoboard.

2- Protocol types 1 and 2
=========================

As it had to happen i found out that i had two different protocols in my scales, the good news was that they were compatible with each other. The protocols are:

* 24 Bits in 6 nibbles of 4 bits each
* LSB first
* mm/inches: This is the only difference I have found beteween type 1 and type 2 scales. Type 1 sends an integer value containing thousandths of an inch (mils) even when the scale display is set for and showing millimeters. The resolution is 1/2 thousandth: when bit 0 is set it indicates 1/2 thousand of an inch (0.0005'). This means that to display millimeters with a type 1 scale, the software must convert this value from inches to millimeters. 
Type 2 scales send an integer value containing hundredths of a millimeter when the scale is set to show millimeters, the resolution is 0.01 mm. When the scale is showing inches it sends an integer containing thousandths in the same manner as type 1 scales do.  

3- Timing
=========

The data is sent in bursts about 142ms appart (~7.04Hz). There is an idle time of ~140ms between data bursts. The start of the data is signaled by the clock line going low. The data is sampled when the clock is high.
A data nibble consists of three short clock pulses of 100us(t1) followed by a long clock pulse of 400us(t3). Between the pulses the clock goes low for 250us(t2). The length of a data nibble is about 1.050ms (4*t1+3*t2+t3) and the complete length of a data burst ist around 1.7 ms (6*tdn).

4- Sampling
===========

I decided not to use harware interrupts and instead to poll the clock line at a rate of 25us by using Timer2. A prescaler of 1/8 gives a resolution of 0.5us on the Arduino Uno (16MHz).

TODO
====
* Support for other protocols. This basically depends on me finding other scales and reverse engineering the protocols used.
* Scale/Protocol recognition. This one depends on the previous one.
* Separating the display API from the main program.
* Support for other display types (7-Segment, I2C ...)
* Some configuration over the serial port.


