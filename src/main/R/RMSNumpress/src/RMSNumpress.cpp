/*
  RMSNumpress.cpp
  justincsing@gmail.com
  
  Copyright 2020 Justin Sing
  
  Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
  limitations under the License.
*/
#include <Rcpp.h>
using namespace Rcpp;
#include "include/MSNumpress.hpp"

//' optimalLinearFixedPointMass
//' 
//' Compute the optimal linear fixed point with a desired m/z accuracy.
//' 
//' @note If the desired accuracy cannot be reached without overflowing 64
//' bit integers, then a negative value is returned. You need to check for
//'   this and in that case abandon numpress or use optimalLinearFixedPoint
//'   which returns the largest safe value.
//' 
//' @param data            pointer to array of double to be encoded (need memorycont. repr.)
//' @param mass_acc        desired m/z accuracy in Th
//' @return          the linear fixed point that satisfies the accuracy requirement (or -1 in case of failure).
// [[Rcpp::export]]
double optimalLinearFixedPointMass(
    const std::vector<double> &data, 
    double mass_acc
) {
  size_t dataSize = data.size();
  double result = ms::numpress::MSNumpress::optimalLinearFixedPointMass(&data[0], dataSize, mass_acc);
  return result;
}

//' optimalLinearFixedPoint
//' 
//' Compute the maximal linear fixed point that prevents integer overflow.
//' 
//' @param data          pointer to array of double to be encoded (need memorycont. repr.)
//' @return              the linear fixed point safe to use
//'   
// [[Rcpp::export]]
double optimalLinearFixedPoint(const std::vector<double> &data){
  size_t dataSize = data.size();
  double result = ms::numpress::MSNumpress::optimalLinearFixedPoint( &data[0],  dataSize );
  return result;
}

//' optimalSlofFixedPoint
//' 
//' Compute the maximal natural logarithm fixed point that prevents integer overflow.
//' 
//' @param data          pointer to array of double to be encoded (need memorycont. repr.)
//' @return              the slof fixed point safe to use
//'  
// [[Rcpp::export]]
double optimalSlofFixedPoint(const std::vector<double> &data){
  size_t dataSize = data.size();
  double result = ms::numpress::MSNumpress::optimalSlofFixedPoint( &data[0],  dataSize );
  return result;
}

//' encodeLinear
//' 
//' Encodes the doubles in data by first using a \cr
//'  - lossy conversion to a 4 byte 5 decimal fixed point representation \cr
//'  - storing the residuals from a linear prediction after first two values \cr
//'  - encoding by encodeInt (see above) \cr
//'  
//'  The resulting binary is maximally 8 + dataSize * 5 bytes, but much less if the 
//'    data is reasonably smooth on the first order.
//'  
//'  This encoding is suitable for typical m/z or retention time binary arrays. 
//'  On a test set, the encoding was empirically show to be accurate to at least 0.002 ppm.
//'  
//' @param data        pointer to array of double to be encoded (need memorycont. repr.)
//' @param fixedPoint  the scaling factor used for getting the fixed point repr. 
//'                     This is stored in the binary and automatically extracted
//'                     on decoding (see optimalLinearFixedPoint or optimalLinearFixedPointMass)
//' @return            the number of encoded bytes
//' 
//' @seealso [\code{\link{decodeLinear}}]
//' 
//' @examples
//' \dontrun{
//' ## Retention time array
//' rt_array <- c(4313.0, 4316.4, 4319.8, 4323.2, 4326.6, 4330.1)
//' ## encode retention time array
//' rt_encoded <- encodeLinear(rt_array, 500)
//' #> [1] 40 7f 40 00 00 00 00 00 d4 e7 20 00 78 ee 20 00 88 86 23
//' }
// [[Rcpp::export]]
std::vector<unsigned char> encodeLinear(const std::vector<double> &data, 
                            double fixedPoint) {
    std::vector<unsigned char> result;
    ms::numpress::MSNumpress::encodeLinear(data, result, fixedPoint);  
 return result;
  }

//' decodeLinear
//' 
//' Decodes data encoded by encodeLinear. 
//' 
//' result vector guaranteed to be shorter or equal to (|data| - 8) * 2
//' 
//' Note that this method may throw a const char* if it deems the input data to be corrupt, i.e.
//' that the last encoded int does not use the last byte in the data. In addition the last encoded 
//' int need to use either the last halfbyte, or the second last followed by a 0x0 halfbyte. 
//' 
//' @param data                pointer to array of bytes to be decoded (need memorycont. repr.)
//' @return                    the number of decoded doubles, or -1 if dataSize < 4 or 4 < dataSize < 8
//' 
//' @seealso [\code{\link{encodeLinear}}]
//' 
//' @examples
//' \dontrun{
//' ## Retention time data that is encoded with encodeLinear and is zlib compressed
//' ### NOTE: For the sake of this example, I have broken the raw vector into several parts
//' ###       to avoid Rd line widths (>100 characters) issues with CRAN build checks.
//' rt_raw1 <- c("78", "9c", "73", "50", "61", "00", "83", "aa", "15", "0c", "0c", "73", "80")
//' rt_raw2 <- c("b8", "a3", "5d", "fe", "47", "07", "84", "28", "fc", "8f", "c4", "40", "e5")
//' rt_raw3 <- c("61", "51", "84", "a9", "85", "08", "e1", "06", "00", "06", "be", "41", "cf")
//' ## Add all character representation of raw data back together and convert back to hex raw vector
//' rt_blob <- as.raw(as.hexmode(c(rt_raw1, rt_raw2, rt_raw3 )))
//' ## Decompress blob
//' rt_blob_uncompressed <- as.raw(Rcompression::uncompress( rt_blob, asText = FALSE ))
//' ## Decode to rentention time double values
//' rt_array <- decodeLinear(rt_blob_uncompressed)
//' }
// [[Rcpp::export]]
std::vector<double> decodeLinear(const std::vector<unsigned char> &data) {
  std::vector<double> result;
  ms::numpress::MSNumpress::decodeLinear(data, result);
  return result;
}

//' encodeSlof
//' 
//' Encodes ion counts by taking the natural logarithm, and storing a
//'   fixed point representation of this. This is calculated as
//'   
//'   unsigned short fp = log(d + 1) * fixedPoint + 0.5
//' 
//' the result vector is exactly |data| * 2 + 8 bytes long
//'   
//' @param data            pointer to array of double to be encoded (need memorycont. repr.)
//' @param fixedPoint      fixed point to use for encoding (see optimalSlofFixedPoint)
//' @return                the number of encoded bytes
//' 
//' @seealso [\code{\link{decodeSlof}}]
// [[Rcpp::export]]
std::vector<unsigned char> encodeSlof(const std::vector<double> &data, 
                                        double fixedPoint) {
  std::vector<unsigned char> result;
  ms::numpress::MSNumpress::encodeSlof(data, result, fixedPoint);  
  return result;
}

//' decodeSlof
//' 
//' Decodes data encoded by encodeSlof
//'   
//'   The return will include exactly (|data| - 8) / 2 doubles.
//' 
//' Note that this method may throw a const char* if it deems the input data to be corrupt.
//' 
//' @param data                pointer to array of bytes to be decoded (need memorycont. repr.)
//' @return                    the number of decoded doubles
//' 
//' @seealso [\code{\link{encodeSlof}}]
//' @examples
//' \dontrun{
//' ## Intensity array to encode
//' ### NOTE: For the sake of this example, I have broken the intensity vector into several parts
//' ###       to avoid Rd line widths (>100 characters) issues with CRAN build checks.
//' int_array1 <- c(0.71773432,  0.43443741,  1.71883610, 0.13220307,  0.90664242)  
//' int_array2 <- c(0.00000000, 0.00000000,  0.64213755,  0.43443741, 0.47221479)
//' ## Comcatenate into one intensity array
//' int_array <- c(int_array1, int_array2)
//' ## Encode intensity array using encodeSlof
//' int_encode <- encodeSlof( int_array, 16 )
//' }
// [[Rcpp::export]]
std::vector<double> decodeSlof(const std::vector<unsigned char> &data) {
  std::vector<double> result;
  ms::numpress::MSNumpress::decodeSlof(data, result);
  return result;
}

//' encodePic
//' 
//' Encodes ion counts by simply rounding to the nearest 4 byte integer, 
//' and compressing each integer with encodeInt. 
//' 
//' The handleable range is therefore 0 -> 4294967294.
//' The resulting binary is maximally dataSize * 5 bytes, but much less if the 
//'   data is close to 0 on average.
//' 
//' @param data                pointer to array of double to be encoded (need memorycont. repr.)
//' @return                    the number of encoded bytes
//' 
//' @seealso [\code{\link{decodePic}}]
// [[Rcpp::export]]
std::vector<unsigned char> encodePic(const std::vector<double> &data) {
  std::vector<unsigned char> result;
  ms::numpress::MSNumpress::encodePic(data, result);  
  return result;
}

//' decodePic
//' 
//' Decodes data encoded by encodePic
//'   
//'   result vector guaranteed to be shorter of equal to |data| * 2
//' 
//' Note that this method may throw a const char* if it deems the input data to be corrupt, i.e.
//' that the last encoded int does not use the last byte in the data. In addition the last encoded 
//'   int need to use either the last halfbyte, or the second last followed by a 0x0 halfbyte. 
//' 
//' @param data                pointer to array of bytes to be decoded (need memorycont. repr.)
//' @return                    the number of decoded doubles
//' 
//' @seealso [\code{\link{encodePic}}]
// [[Rcpp::export]]
std::vector<double> decodePic(const std::vector<unsigned char> &data) {
  std::vector<double> result;
  ms::numpress::MSNumpress::decodePic(data, result);
  return result;
}
