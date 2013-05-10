//
// MZDataCompressor.hpp
// johan.teleman@immun.lth.se
//


#ifndef _MZDATACOMPRESSOR_HPP_
#define _MZDATACOMPRESSOR_HPP_

#include <stdlib.h>

namespace ms {
namespace numpress {

class MZDataCompressor {
	public:

	static void encode_int(
		int x,
		unsigned char* res,
		size_t *res_length);
	
	static void decode_int(
		const unsigned char **bp,
		int *half,
		int *res);

/**
 * Encodes the doubles in data by first using a 
 *   - lossy conversion to a 4 byte 5 decimal fixed point repressentation
 *   - storing the residuals from a linear prediction after first to values
 *   - encoding by encode_int (see above) 
 * 
 * The resulting binary is maximally dataSize * 5 bytes, but much less if the 
 * data is reasonably smooth on the first order.
 */
 	static void encode(
		const double *data, 
		size_t dataSize, 
		unsigned char *result, 
		size_t *resultByteCount);

/**
 * Decodes data encoded by encode. Note that the compression 
 * discard any information < 10^-5, so data is only guaranteed 
 * to be within +- 0.5*10^-5 of the original value.
 *
 * Further, values > ~42000 will also be truncated because of the
 * fixed point repressentation, so this scheme is stronly discouraged 
 * if values above might be above this size.
 */
	static void decode(
		const unsigned char *dat, 
		size_t dataSize, 
		double *result, 
		size_t resultDoubleCount);
};

} // namespace numpress
} // namespace ms

#endif // _MZDATACOMPRESSOR_HPP_
