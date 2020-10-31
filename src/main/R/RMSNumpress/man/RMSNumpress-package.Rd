\name{RMSNumpress-package}
\alias{RMSNumpress-package}
\alias{RMSNumpress}
\docType{package}
\title{
  Rcpp bindings to native C++ implementation of MS Numpress
}
\description{
MS Numpress 

=========== 


Implementations of two compression schemes for numeric data from mass spectrometers.

The library provides implementations of 3 different algorithms, 
1 designed to compress first order smooth data like retention 
time or M/Z arrays, and 2 for compressing non smooth data with
lower requirements on precision like ion count arrays.

Numpress Pic 

=========== 

MS Numpress positive integer compression

Intended for ion count data, this compression simply rounds values to the nearest integer, and stores these integers in a truncated form which is effective for values relatively close to zero.

Numpress Slof 

=========== 

MS Numpress short logged float compression

Also targeting ion count data, this compression takes the natural logarithm of values, multiplies by a scaling factor and rounds to the nearest integer. For typical ion count dynamic range these values fits into two byte integers, so only the two least significant bytes of the integer are stored.

The scaling factor can be chosen manually, but the library also contains a function for retrieving the optimal Slof scaling factor for a given data array. Since the scaling factor is variable, it is stored as a regular double precision float first in the encoding, and automatically parsed during decoding.

Numpress Lin 

=========== 

MS Numpress linear prediction compression

This compression uses a fixed point representation, achieve by multiplication by a scaling factor and rounding to the nearest integer. To exploit the assumed linearity of the data, linear prediction is then used in the following way.

The first two values are stored without compression as 4 byte integers. For each following value a linear prediction is made from the two previous values: 



Xpred 	= (X(n) - X(n-1)) + X(n) 

Xres 	= Xpred - X(n+1)




The residual Xres is then stored, using the same truncated integer representation as in Numpress Pic.

The scaling factor can be chosen manually, but the library also contains a function for retrieving the optimal Lin scaling factor for a given data array. Since the scaling factor is variable, it is stored as a regular double precision float first in the encoding, and automatically parsed during decoding.

Truncated integer representation 

===========

This encoding works on a 4 byte integer, by truncating initial zeros or ones. If the initial (most significant) half byte is 0x0 or 0xf, the number of such halfbytes starting from the most significant is stored in a halfbyte. This initial count is then followed by the rest of the ints halfbytes, in little-endian order. A count halfbyte c of 



0 <= c <= 8 		is interpreted as an initial c 		0x0 halfbytes 

9 <= c <= 15		is interpreted as an initial (c-8) 	0xf halfbytes




Examples:

int		c		rest 

0 	=> 	0x8 

-1	=>	0xf		0xf 

23	=>	0x6 	0x7	0x1



}
\author{
Maintainer: Justin Sing <justincsing@gmail.com>
}
\references{
  See: https://github.com/ms-numpress/ms-numpress
}
\keyword{ package }
\seealso{
  \code{\link{encodeLinear}},
  \code{\link{decodeLinear}},
  \code{\link{encodeSlof}},
  \code{\link{decodeSlof}},
  \code{\link{encodePic}},
  \code{\link{decodePic}},
  \code{\link{optimalLinearFixedPoint}},
  \code{\link{optimalSlofFixedPoint}},
  \code{\link{optimalLinearFixedPointMass}},
}
\examples{
  \dontrun{
    # Encode Numpress Linear
    ## Retention time array
    rt_array <- c(4313.0, 4316.4, 4319.8, 4323.2, 4326.6, 4330.1)
    ## encode retention time array
    rt_encoded <- encodeLinear(rt_array, 500)
    #> [1] 40 7f 40 00 00 00 00 00 d4 e7 20 00 78 ee 20 00 88 86 23   
    
    # Decode Numpress Linear
    ## Retention time data that is encoded with encodeLinear and is zlib compressed
    ### NOTE: For the sake of this example, I have broken the raw vector into several parts
    ###       to avoid Rd line widths (>100 characters) issues with CRAN build checks.
    rt_raw1 <- c("78", "9c", "73", "50", "61", "00", "83", "aa", "15", "0c", "0c", "73", "80")
    rt_raw2 <- c("b8", "a3", "5d", "fe", "47", "07", "84", "28", "fc", "8f", "c4", "40", "e5")
    rt_raw3 <- c("61", "51", "84", "a9", "85", "08", "e1", "06", "00", "06", "be", "41", "cf")
    ## Add all character representation of raw data back together and convert back to hex raw vector
    rt_blob <- as.raw(as.hexmode(c(rt_raw1, rt_raw2, rt_raw3 )))
    ## Decompress blob
    rt_blob_uncompressed <- as.raw(Rcompression::uncompress( rt_blob, asText = FALSE ))
    ## Decode to rentention time double values
    rt_array <- decodeLinear(rt_blob_uncompressed)
  }
}
