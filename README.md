MS Numpress
===========

Implementations of two compression schemes for numeric data from mass spectrometers.

The library provides implementations of 3 different algorithms, 
1 designed to compress first order smooth data like retention 
time or M/Z arrays, and 2 for compressing non smooth data with
lower requirements on precision like ion count arrays.

Implementations and unit test are provided in `c++` and `java`.

For `c++`, run tests by  

Numpress Pic
------------
### MS Numpress positive integer compression

Intended for ion count data, this compression simply rounds values 
to the nearest integer, and stores these integers in a truncated 
form which is effective for values relatively close to zero. 


Numpress Slof
-------------
### MS Numpress short logged float compression

Also targeting ion count data, this compression takes the natural
logarithm of values, multiplies by a scaling factor and rounds to 
the nearest integer. For typical ion count dynamic range these values 
fits into two byte integers, so only the two least significant bytes 
of the integer are stored.

The scaling factor can be chosen manually, but the library also contains
a function for retrieving the optimal Slof scaling factor for a given data array.
Since the scaling factor is variable, it is stored as a regular double 
precision float first in the encoding, and automatically parsed during decoding.

Numpress Lin
------------
### MS Numpress linear prediction compression

This compression uses a fixed point repressentation, achieve by 
multiplication by a scaling factor and rounding to the nearest integer. 
To exploit the assumed linearity of the data, linear prediction is 
then used in the following way. 

The first to values are stored without compression as 4 byte integers.
For each following value a linear predicion is made from the two previous
values:

	Xpred 	= (X(n) - X(n-1)) + X(n)
	Xres 	= Xpred - X(n+1)

The residual `Xres` is then stored, using the same truncated integer 
representation as in Numpress Pic.  

The scaling factor can be chosen manually, but the library also contains
a function for retrieving the optimal Lin scaling factor for a given data array.
Since the scaling factor is variable, it is stored as a regular double 
precision float first in the encoding, and automatically parsed during decoding.

Truncated integer representation 
---------------------------------

This encoding works on a 4 byte integer, by truncating initial zeros or ones.
If the initial (most significant) half byte is 0x0 or 0xf, the number of such 
halfbytes starting from the most significant is stored in a halfbyte. This initial 
count is then followed by the rest of the ints halfbytes, in little-endian order. 
A count halfbyte c of

	0 <= c <= 8 		is interpreted as an initial c 		0x0 halfbytes
	9 <= c <= 15		is interpreted as an initial (c-8) 	0xf halfbytes

Examples:

	int		c		rest
	0 	=> 	0x8
	-1	=>	0xf		0xf
	23	=>	0x6 	0x7	0x1



License 
-------

This code is open source. By default code is licenses with the Apache 2.0 license. 
If you for some reason prefer BSD, there is a version which is BSD 3-clause licensed 
in the src/bsd directory.  
