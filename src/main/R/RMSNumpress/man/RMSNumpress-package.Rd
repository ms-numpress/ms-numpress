\name{RMSNumpress-package}
\alias{RMSNumpress-package}
\alias{RMSNumpress}
\docType{package}
\title{
  Rcpp bindings to native C++ implementation of MS Numpress
}
\description{
MS Numpress \cr
=========== \cr

Implementations of two compression schemes for numeric data from mass spectrometers.

The library provides implementations of 3 different algorithms, 
1 designed to compress first order smooth data like retention 
time or M/Z arrays, and 2 for compressing non smooth data with
lower requirements on precision like ion count arrays.

}
\author{
Maintainer: Justin Sing <justincsing@gmail.com>
}
\references{
  See: https://github.com/ms-numpress/ms-numpress
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
    rt_blob <- as.raw(as.hexmode(c("78", "9c", "73", "50", "61", "00", "83", "aa", "15", "0c", "0c", "73", "80", "b8", "a3", "5d", "fe", "47", "07", "84", "28", "fc", "8f", "c4", "40", "e5", "61", "51", "84", "a9", "85", "08", "e1", "06", "00", "06", "be", "41", "cf")))
    ## Decompress blob
    rt_blob_uncompressed <- as.raw(Rcompression::uncompress( rt_blob, asText = FALSE ))
    ## Decode to rentention time double values
    rt_array <- decodeLinear(rt_blob_uncompressed)
  }
}
