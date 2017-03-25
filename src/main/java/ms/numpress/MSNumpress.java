/*
	MSNumpress.java
	johan.teleman@immun.lth.se
 
	Copyright 2013 Johan Teleman

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
package ms.numpress;

public class MSNumpress {

	///PSI-MS obo accession numbers.
	public static final String ACC_NUMPRESS_LINEAR 	= "MS:1002312";
	public static final String ACC_NUMPRESS_PIC 	= "MS:1002313";
	public static final String ACC_NUMPRESS_SLOF 	= "MS:1002314";
	
	
	/**
	 * Convenience function for decoding binary data encoded by MSNumpress. If
	 * the passed cvAccession is one of
	 * 
	 *    ACC_NUMPRESS_LINEAR = "MS:1002312"
	 *    ACC_NUMPRESS_PIC    = "MS:1002313"
	 *    ACC_NUMPRESS_SLOF   = "MS:1002314"
	 * 
	 * the corresponding decode function will be called.
	 * 
	 * @cvAccession		The PSI-MS obo CV accession of the encoded data.
	 * @data			array of double to be encoded
	 * @dataSize		number of doubles from data to encode
	 * @return			The decoded doubles
	 */
	public static double[] decode(
			String cvAccession, 
			byte[] data, 
			int dataSize
	) {		
		if (cvAccession.equals(ACC_NUMPRESS_LINEAR)) {
			if (dataSize < 8 || data.length < 8)
				throw new IllegalArgumentException("Cannot decode numLin data, need at least 8 initial bytes for fixed point.");

			double[] buffer 	= new double[dataSize * 2];
			int nbrOfDoubles 	= MSNumpress.decodeLinear(data, dataSize, buffer);
			if (nbrOfDoubles < 0)
				throw new IllegalArgumentException("Corrupt numLin data!");

			double[] result 	= new double[nbrOfDoubles];
			System.arraycopy(buffer, 0, result, 0, nbrOfDoubles);
			return result;
			
		} else if (cvAccession.equals(ACC_NUMPRESS_SLOF)) {
			double[] result 	= new double[(dataSize-8) / 2];
			MSNumpress.decodeSlof(data, dataSize, result);
			return result;
			
		} else if (cvAccession.equals(ACC_NUMPRESS_PIC)) {
			if (dataSize < 8 || data.length < 8)
				throw new IllegalArgumentException("Cannot decode numPic data, need at least 8 initial bytes for fixed point.");

			double[] buffer 	= new double[dataSize * 2];
			int nbrOfDoubles 	= MSNumpress.decodePic(data, dataSize, buffer);
			if (nbrOfDoubles < 0)
				throw new IllegalArgumentException("Corrupt numPic data!");

			double[] result 	= new double[nbrOfDoubles];
			System.arraycopy(buffer, 0, result, 0, nbrOfDoubles);
			return result;
			
		}
		
		throw new IllegalArgumentException("'"+cvAccession+"' is not a numpress compression term");
	}
	

	/**
	 * This encoding works on a 4 byte integer, by truncating initial zeros or ones.
	 * If the initial (most significant) half byte is 0x0 or 0xf, the number of such 
	 * halfbytes starting from the most significant is stored in a halfbyte. This initial 
	 * count is then followed by the rest of the ints halfbytes, in little-endian order. 
	 * A count halfbyte c of
	 * 
	 * 		0 <= c <= 8 		is interpreted as an initial c 		0x0 halfbytes 
	 * 		9 <= c <= 15		is interpreted as an initial (c-8) 	0xf halfbytes
	 * 
	 * Ex:
	 * 	int		c		rest
	 * 	0 	=> 	0x8
	 * 	-1	=>	0xf		0xf
	 * 	23	=>	0x6 	0x7	0x1
	 * 
	 * 	@x			the int to be encoded
	 *	@res		the byte array were halfbytes are stored
	 *	@resOffset	position in res were halfbytes are written
	 *	@return		the number of resulting halfbytes
	 */
	protected static int encodeInt(
			long x,
			byte[] res,
			int resOffset
	) {
		byte i, l;
		long m;
		long mask = 0xf0000000;
		long init = x & mask;

		if (init == 0) {
			l = 8;
			for (i=0; i<8; i++) {
				m = mask >> (4*i);
				if ((x & m) != 0) {
					l = i;
					break;
				}
			}
			res[resOffset] = l;
			for (i=l; i<8; i++) 
				res[resOffset+1+i-l] = (byte)(0xf & (x >> (4*(i-l))));
			
			return 1+8-l;

		} else if (init == mask) {
			l = 7;
			for (i=0; i<8; i++) {
				m = mask >> (4*i);
				if ((x & m) != m) {
					l = i;
					break;
				}
			}
			res[resOffset] = (byte)(l | 8);
			for (i=l; i<8; i++) 
				res[resOffset+1+i-l] = (byte)(0xf & (x >> (4*(i-l))));
			
			return 1+8-l;

		} else {
			res[resOffset] = 0;
			for (i=0; i<8; i++)
				res[resOffset+1+i] = (byte)(0xf & (x >> (4*i)));
			
			return 9;

		}
	}
	
	
	
	public static void encodeFixedPoint(
			double fixedPoint, 
			byte[] result
	) {
		long fp = Double.doubleToLongBits(fixedPoint);
		for (int i=0; i<8; i++) {
			result[7-i] = (byte)((fp >> (8*i)) & 0xff);
		}
	}
	
	
	
	public static double decodeFixedPoint(
			byte[] data
	) {
		long fp = 0;
		for (int i=0; i<8; i++) {
			fp = fp | ((0xFFl & data[7-i]) << (8*i));
		}
		return Double.longBitsToDouble(fp);
	}
	
	
	
	
	/////////////////////////////////////////////////////////////////////////////////

	/**
	 * Compute the maximal linear fixed point that prevents integer overflow.
	 *
	 * @data		array of doubles to be encoded
	 * @dataSize	number of doubles from data to encode
	 *
	 * @return		the largest linear fixed point safe to use
	 */
	public static double optimalLinearFixedPoint(
			double[] data,
			int dataSize
	) {
		if (dataSize == 0) return 0;
		if (dataSize == 1) return Math.floor(0xFFFFFFFFl / data[0]);
		double maxDouble = Math.max(data[0], data[1]);

		for (int i=2; i<dataSize; i++) {
			double extrapol = data[i-1] + (data[i-1] - data[i-2]);
			double diff 	= data[i] - extrapol;
			maxDouble 		= Math.max(maxDouble, Math.ceil(Math.abs(diff)+1));
		}

		return Math.floor(0x7FFFFFFFl / maxDouble);
	}


	/**
	 * Compute the linear fixed point the results in a minimal encoded size while guaranteeing a
	 * specified m/z accuracy.
	 *
	 * @note If the desired accuracy cannot be reached without overflowing 64
	 * bit integers, then a negative value is returned. You need to check for
	 * this and in that case abandon numpress or use optimalLinearFixedPoint
	 * which returns the largest safe value.
	 *
	 * @data		array of doubles to be encoded
	 * @dataSize	number of doubles from data to encode
	 * @mass_acc	desired m/z accuracy in Th
	 *
	 * @return		the linear fixed point that satisfies the accuracy requirement (or -1 in case of failure).
	 */

	public static double optimalLinearFixedPointMass(
			double[] data,
			int dataSize,
			double massAccuracy
	) {
		if (dataSize < 3) return 0;

		// We calculate the maximal fixedPoint we need to achieve a specific mass
		// accuracy. Note that the maximal error we will make by encoding as int is
		// 0.5 due to rounding errors.
		double maxFp = 0.5 / massAccuracy;

		// There is a maximal value for the FP given by the int length (32bit)
		// which means we cannot choose a value higher than that. In case we cannot
		// achieve the desired accuracy, return failure (-1).
		double maxFpOverflow = optimalLinearFixedPoint(data, dataSize);
		if (maxFp > maxFpOverflow) return -1;

		return maxFp;
	}


	/**
	 * Encodes the doubles in data by first using a 
	 *   - lossy conversion to a 4 byte 5 decimal fixed point repressentation
	 *   - storing the residuals from a linear prediction after first two values
	 *   - encoding by encodeInt (see above) 
	 * 
	 * The resulting binary is maximally 8 + dataSize * 5 bytes, but much less if the 
	 * data is reasonably smooth on the first order.
	 *
	 * This encoding is suitable for typical m/z or retention time binary arrays. 
	 * On a test set, the encoding was empirically show to be accurate to at least 0.002 ppm.
	 *
	 * @data		array of doubles to be encoded
	 * @dataSize	number of doubles from data to encode
	 * @result		array were resulting bytes should be stored
	 * @fixedPoint	the scaling factor used for getting the fixed point repr. 
	 * 				This is stored in the binary and automatically extracted
	 * 				on decoding.
	 * @return		the number of encoded bytes
	 */
	public static int encodeLinear(
			double[] data, 
			int dataSize, 
			byte[] result,
			double fixedPoint
	) {
		long[] ints = new long[3];
		int i;
		int ri = 16;
		byte halfBytes[] = new byte[10];
		int halfByteCount = 0;
		int hbi;
		long extrapol;
		long diff;
		
		encodeFixedPoint(fixedPoint, result);

		if (dataSize == 0) return 8;
		
		ints[1] = (long)(data[0] * fixedPoint + 0.5);
		for (i=0; i<4; i++) {
			result[8+i] = (byte)((ints[1] >> (i*8)) & 0xff);
		}
		 
		if (dataSize == 1) return 12;
		
		ints[2] = (long)(data[1] * fixedPoint + 0.5);
		for (i=0; i<4; i++) {
			result[12+i] = (byte)((ints[2] >> (i*8)) & 0xff);
		}

		halfByteCount = 0;
		ri = 16;
		
		for (i=2; i<dataSize; i++) {
			ints[0] = ints[1];
			ints[1] = ints[2];
			ints[2] = (long)(data[i] * fixedPoint + 0.5);
			extrapol = ints[1] + (ints[1] - ints[0]);
			diff = ints[2] - extrapol;
			halfByteCount += encodeInt(diff, halfBytes, halfByteCount);
					
			for (hbi=1; hbi < halfByteCount; hbi+=2)
				result[ri++] = (byte)((halfBytes[hbi-1] << 4) | (halfBytes[hbi] & 0xf));
			
			if (halfByteCount % 2 != 0) {
				halfBytes[0] = halfBytes[halfByteCount-1];
				halfByteCount = 1;
			} else 
				halfByteCount = 0;

		}
		if (halfByteCount == 1)
			result[ri++] = (byte)(halfBytes[0] << 4);

		return ri;
	}
	
	
	
	/**
     * Decodes data encoded by encodeLinear. 
	 *
	 * result vector guaranteed to be shorter or equal to (|data| - 8) * 2
	 *
	 * Note that this method may throw a ArrayIndexOutOfBoundsException if it deems the input data to 
	 * be corrupt, i.e. that the last encoded int does not use the last byte in the data. In addition 
	 * the last encoded int need to use either the last halfbyte, or the second last followed by a 
	 * 0x0 halfbyte. 
	 *
	 * @data		array of bytes to be decoded
	 * @dataSize	number of bytes from data to decode
	 * @result		array were resulting doubles should be stored
	 * @return		the number of decoded doubles, or -1 if dataSize < 4 or 4 < dataSize < 8
	 */
	public static int decodeLinear(
			byte[] data, 
			int dataSize, 
			double[] result
	) {
		int ri = 2;
		long[] ints = new long[3];
		long extrapol;
		long y;
		IntDecoder dec = new IntDecoder(data, 16);

		if (dataSize == 8) return 0;
		if (dataSize < 8) return -1;
		double fixedPoint = decodeFixedPoint(data);
		if (dataSize < 12) return -1;
		
		ints[1] = 0;
		for (int i=0; i<4; i++) {
			ints[1] = ints[1] | ( (0xFFl & data[8+i]) << (i*8));
		}
		result[0] = ints[1] / fixedPoint;
		
		if (dataSize == 12) return 1;
		if (dataSize < 16) return -1;
		
		ints[2] = 0;
		for (int i=0; i<4; i++) {
			ints[2] = ints[2] | ( (0xFFl & data[12+i]) << (i*8));
		}
		result[1] = ints[2] / fixedPoint;
	
		while (dec.pos < dataSize) {
			if (dec.pos == (dataSize - 1) && dec.half)
				if ((data[dec.pos] & 0xf) != 0x8)
					break;
			
			ints[0] = ints[1];
			ints[1] = ints[2];
			ints[2] = dec.next();
			
			extrapol = ints[1] + (ints[1] - ints[0]);
			y = extrapol + ints[2];
			result[ri++] = y / fixedPoint;
			ints[2] = y;
		}
		
		return ri;
	}
	
	
	
	/////////////////////////////////////////////////////////////////////////////////
	
	/**
	 * Encodes ion counts by simply rounding to the nearest 4 byte integer, 
	 * and compressing each integer with encodeInt. 
	 *
	 * The handleable range is therefore 0 -> 4294967294.
	 * The resulting binary is maximally dataSize * 5 bytes, but much less if the 
	 * data is close to 0 on average.
	 *
	 * @data		array of doubles to be encoded
	 * @dataSize	number of doubles from data to encode
	 * @result		array were resulting bytes should be stored
	 * @return		the number of encoded bytes
	 */
	public static int encodePic(
			double[] data, 
			int dataSize, 
			byte[] result
	) {
		long count;
		int ri = 0;
		int hbi = 0;
		byte halfBytes[] = new byte[10];
		int halfByteCount = 0;

		//printf("Encoding %d doubles\n", (int)dataSize);

		for (int i=0; i<dataSize; i++) {
			count = (long)(data[i] + 0.5);
			halfByteCount += encodeInt(count, halfBytes, halfByteCount);
					
			for (hbi=1; hbi < halfByteCount; hbi+=2)
				result[ri++] = (byte)((halfBytes[hbi-1] << 4) | (halfBytes[hbi] & 0xf));
			
			if (halfByteCount % 2 != 0) {
				halfBytes[0] = halfBytes[halfByteCount-1];
				halfByteCount = 1;
			} else
				halfByteCount = 0;

		}
		if (halfByteCount == 1)
			result[ri++] = (byte)(halfBytes[0] << 4);
		
		return ri;
	}
	
	
	/**
	 * Decodes data encoded by encodePic
	 *
	 * result vector guaranteed to be shorter of equal to |data| * 2
	 *
	 * Note that this method may throw a ArrayIndexOutOfBoundsException if it deems the input data to 
	 * be corrupt, i.e. that the last encoded int does not use the last byte in the data. In addition 
	 * the last encoded int need to use either the last halfbyte, or the second last followed by a 
	 * 0x0 halfbyte. 
	 *
	 * @data		array of bytes to be decoded (need memorycont. repr.)
	 * @dataSize	number of bytes from data to decode
	 * @result		array were resulting doubles should be stored
	 * @return		the number of decoded doubles
	 */
	public static int decodePic(
			byte[] data, 
			int dataSize, 
			double[] result
	) {
		int ri = 0;
		long count;
		IntDecoder dec = new IntDecoder(data, 0);

		while (dec.pos < dataSize) {
			if (dec.pos == (dataSize - 1) && dec.half)
				if ((data[dec.pos] & 0xf) != 0x8)
					break;
				
			count = dec.next();
			result[ri++] = count;
		}
		return ri;
	}
	
	
	
	
	/////////////////////////////////////////////////////////////////////////////////
	
	public static double optimalSlofFixedPoint(
			double[] data, 
			int dataSize
	) {
		if (dataSize == 0) return 0;
	
		double maxDouble = 1;
		double x;
		double fp;

		for (int i=0; i<dataSize; i++) {
			x = Math.log(data[i]+1);
			maxDouble = Math.max(maxDouble, x);
		}

		fp = Math.floor(0xFFFF / maxDouble);

		return fp;
	}

	/**
	 * Encodes ion counts by taking the natural logarithm, and storing a
	 * fixed point representation of this. This is calculated as
	 * 
	 * unsigned short fp = log(d+1) * fixedPoint + 0.5
	 *
	 * the result vector is exactly |data| * 2 + 8 bytes long
	 *
	 * @data		array of doubles to be encoded
	 * @dataSize	number of doubles from data to encode
	 * @result		array were resulting bytes should be stored
	 * @fixedPoint	the scaling factor used for getting the fixed point repr. 
	 * 				This is stored in the binary and automatically extracted
	 * 				on decoding.
	 * @return		the number of encoded bytes
	 */
	public static int encodeSlof(
			double[] data, 
			int dataSize, 
			byte[] result,
			double fixedPoint
	) {
		int x;
		int ri = 8;
		
		encodeFixedPoint(fixedPoint, result);
		
		for (int i=0; i<dataSize; i++) {
			x = (int)(Math.log(data[i]+1) * fixedPoint + 0.5);
		
			result[ri++] = (byte)(0xff & x);
			result[ri++] = (byte)(x >> 8);
		}
		return ri;
	}
	
	
	/**
	 * Decodes data encoded by encodeSlof
	 *
	 * The result vector will be exactly (|data| - 8) / 2 doubles.
	 * returns the number of doubles read, or -1 is there is a problem decoding.
	 *
	 * @data		array of bytes to be decoded (need memorycont. repr.)
	 * @dataSize	number of bytes from data to decode
	 * @result		array were resulting doubles should be stored
	 * @return		the number of decoded doubles
	 */
	public static int decodeSlof(
			byte[] data, 
			int dataSize, 
			double[] result
	) {
		int x;
		int ri = 0;
		
		if (dataSize < 8) return -1;
		double fixedPoint = decodeFixedPoint(data);	
		
		if (dataSize % 2 != 0) return -1;
		
		for (int i=8; i<dataSize; i+=2) {
			x = (0xff & data[i]) | ((0xff & data[i+1]) << 8);
			result[ri++] = Math.exp(((double)(0xffff & x)) / fixedPoint) - 1;
		}
		return ri;
	}
}
