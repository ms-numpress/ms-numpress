MS Numpress
===========

Implementations of two compression schemes for numeric data from mass spectrometers.

The library provides implementations of 3 different algorithms, 
1 designed to compress first order smooth data like retention 
time or M/Z arrays, and 2 for compressing non smooth data with
lower requirements on precision like ion count arrays.

Implementations and unit test are provided in c++ and java.


MS Numpress positive integer compression - Numpress Pic 
-------------------------------------------------------

Intended for ion count data, this compression simply rounds values 
to the nearest integer, and stores these integers in a truncated 
form which is effective for values relatively close to zero. 


MS Numpress short logged float compression - Numpress Slof 
----------------------------------------------------------

Also targeting ion count data, this compression takes the natural
logarithm of values, multiplies by 3000 and rounds to the nearest 
integer. For typical ion count values ( < e10) this value fits into
a two byte integer, so only the two least significant bytes of the 
integer are stored.


MS Numpress linear prediction compression - Numpress Lin 
--------------------------------------------------------

This compression uses a fixed point repressentation, achieve by 
multiplication by 100000 and rounding to the nearest integer. To 
exploit the assumed linearity of the data, linear prediction is 
then used in the following way. 

The first to values are stored without compression as 4 byte integers.
For each following value a linear predicion is made from the two previous
values:

`Xpred = (X(n) - X(n-1)) + X(n)`
`Xres = Xpred - X(n+1)`

The residual Xres is then stored, using the same truncated integer repressentation 
as in Numpress Pic.  


Truncated integer repressentation 
---------------------------------

This encoding works on a 4 byte integer, by truncating initial zeros or ones.
If the initial (most significant) half byte is 0x0 or 0xf, the number of such 
halfbytes starting from the most significant is stored in a halfbyte. This initial 
count is then followed by the rest of the ints halfbytes, in little-endian order. 
A count halfbyte c of

	`0 <= c <= 8 		is interpreted as an initial c 		0x0 halfbytes`
	`9 <= c <= 15		is interpreted as an initial (c-8) 	0xf halfbytes`

Examples:
`int		c		rest`
`0 	=> 	0x8`
`-1	=>	0xf		0xf`
`23	=>	0x6 	0x7	0x1`



License 
-------

This code is open source, under the Apache 2.0 license.
