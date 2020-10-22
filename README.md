# JALF-DRO

1- Description
==============
Simple DRO (Digital Read Out) for aftermarket digital scales.

Basic requirements:
* Affordable and easy to use hardware: Arduino UNO.
* should support a minimum of 2 scales.
* support for the reverse engineered scales I already have.
* LCD module as display (HD44780 interface). 


Protocol
=========================

* 24 Bits in 6 nibbles of 4 bits each
* LSB first
* Nibbles 1 to 5 contain the measurement shown on the scale as an integer  
* Resolution (inches): when in inches Nibbles 1 to 5 contain an integer value containing thousandths of an inch (mils). The resolution is 1/2 thousandth by using bit 0: when bit 0 is set it indicates 1/2 thousand of an inch (0.0005'). 
* Resolution (mm): 0.01 mm.
* Nibble 6 contains flags indicating inches and the negative sign.

3- Timing
=========

The data is sent in bursts about 142ms appart (~7.04Hz). There is an idle time of ~140ms between data bursts. The start of the data is signaled by the clock line going low. The data is sampled when the clock is high.
A data nibble consists of three short clock pulses of 100us(t1) followed by a long clock pulse of 400us(t3). Between the pulses the clock goes low for 250us(t2). The length of a data nibble is about 1.050ms (4 x t1 + 3 x t2 + t3) and the complete length of a data burst ist around 1.7 ms (6 x tdn).

For timing diagrams (ASCII) see timing.txt.


4- Sampling
===========

Done by sampling the clock line at a rate of 25us by using Timer2. A prescaler of 1/8 gives a resolution of 0.5us on the Arduino Uno (16MHz).

TODOs and "nice to have"
========================
* Support for other protocols. This basically depends on me finding other scales and reverse engineering the protocols used.
* Scale/Protocol recognition. This one depends on the previous one.
* Separating the display API from the main program.
* Support for other display types (7-Segment, I2C ...)
* Configuration over the serial port.


