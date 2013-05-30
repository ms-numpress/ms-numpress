//
// MSNumpress.hpp
// johan.teleman@immun.lth.se
//
// Some of the encodings described below use a integer compression refered to simply as 
//
// 	encodeInt()
// 
// This encoding works on a 4 byte integer, by truncating initial zeros or ones.
// If the initial (most significant) half byte is 0x0 or 0xf, the number of such 
// halfbytes starting from the most significant is stored in a halfbyte. This initial 
// count is then followed by the rest of the ints halfbytes, in little-endian order. 
// A count halfbyte c of
// 
// 	0 <= c <= 8 		is interpreted as an initial c 		0x0 halfbytes 
// 	9 <= c <= 15		is interpreted as an initial (c-8) 	0xf halfbytes
//
// Ex:
// int		c		rest
// 0 	=> 	0x8
// -1	=>	0xf		0xf
// 23	=>	0x6 	0x7	0x1

#ifndef _MSNUMPRESS_HPP_
#define _MSNUMPRESS_HPP_

#include <vector>

namespace ms {
namespace numpress {

class MSNumpress {
	public:

	/**
	 * Encodes the doubles in data by first using a 
	 *   - lossy conversion to a 4 byte 5 decimal fixed point repressentation
	 *   - storing the residuals from a linear prediction after first to values
	 *   - encoding by encodeInt (see above) 
	 * 
	 * The resulting binary is maximally dataSize * 5 bytes, but much less if the 
	 * data is reasonably smooth on the first order.
	 *
	 * This encoding is suitable for typical m/z or retention time binary arrays. 
	 * For masses above 100 m/z the encoding is accurate to at least 0.1 ppm.
	 */
	static void encodeLinear(
		const double *data, 
		size_t dataSize, 
		unsigned char *result, 
		size_t *resultByteCount);

	/**
	 * Decodes data encoded by encodeLinear. Note that the compression 
	 * discard any information < 1e-5, so data is only guaranteed 
	 * to be within +- 5e-6 of the original value.
	 *
	 * Further, values > ~42000 will also be truncated because of the
	 * fixed point representation, so this scheme is stronly discouraged 
	 * if values above might be above this size.
	 *
	 * result vector guaranteedly shorter than twice the data length (in nbr of values)
	 */
	static void decodeLinear(
		std::vector<unsigned char> &data,
		std::vector<double> &result);

	/**
	 * Encodes ion counts by simply rounding to the nearest 4 byte integer, 
	 * and compressing each integer with encodeInt. 
	 *
	 * The handleable range is therefore 0 -> 4294967294.
	 * The resulting binary is maximally dataSize * 5 bytes, but much less if the 
	 * data is close to 0 on average.
	 */
	static void encodeCount(
		const double *data, 
		size_t dataSize, 
		unsigned char *result, 
		size_t *resultByteCount);

	/**
	 * Decodes data encoded by encodeCount
	 */
	static void decodeCount(
		std::vector<unsigned char> &data,
		std::vector<double> &result);

	/**
	 * Encodes ion counts by taking the natural logarithm, and storing a
	 * fixed point representation of this. This is calculated as
	 * 
	 * unsigned short fp = log(d) * 3000.0 + 0.5
	 */
	static void encode2ByteFloat(
		const double *data, 
		size_t dataSize, 
		unsigned char *result, 
		size_t *resultByteCount);

	/**
	 * Decodes data encoded by encode2ByteFloat
	 */
	static void decode2ByteFloat(
		std::vector<unsigned char> &data,
		std::vector<double> &result);
};

} // namespace msdata
} // namespace pwiz

#endif // _MSNUMPRESS_HPP_
