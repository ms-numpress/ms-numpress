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

class MSNumpress {


	protected static double ENC_LINEAR_FIXED_POINT = 100000.0;
	protected static double ENC_TWO_BYTE_FIXED_POINT = 3000.0;
	/**
	 * Encodes the int x as a number of halfbytes in res. 
	 * res_length is incremented by the number of halfbytes, 
	 * which will be 1 <= n <= 9
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
	public static int encodeLinear(
			double[] data, 
			int dataSize, 
			byte[] result
	) {
		long[] ints = new long[3];
		int i;
		int ri = 8;
		byte halfBytes[] = new byte[10];
		int halfByteCount = 0;
		int hbi;
		long extrapol;
		long diff;

		ints[1] = (long)(data[0] * ENC_LINEAR_FIXED_POINT + 0.5);
		ints[2] = (long)(data[1] * ENC_LINEAR_FIXED_POINT + 0.5);

		for (i=0; i<4; i++) {
			result[i] 	= (byte)((ints[1] >> (i*8)) & 0xff);
			result[4+i] = (byte)((ints[2] >> (i*8)) & 0xff);
		}

		for (i=2; i<dataSize; i++) {
			ints[0] = ints[1];
			ints[1] = ints[2];
			ints[2] = (long)(data[i] * ENC_LINEAR_FIXED_POINT + 0.5);
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
	 * Decodes data encoded by encodeLinear. Note that the compression 
	 * discard any information < 1e-5, so data is only guaranteed 
	 * to be within +- 5e-6 of the original value.
	 *
	 * Further, values > ~42000 will also be truncated because of the
	 * fixed point representation, so this scheme is stronly discouraged 
	 * if values above might be above this size.
	 *
	 * result vector guaranteedly shorter than twice the data length (in nbr of values)
	 * returns the number of doubles read
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
		IntDecoder dec = new IntDecoder(data, 8);

		ints[1] = 0;
		ints[2] = 0;

		for (int i=0; i<4; i++) {
			ints[1] = ints[1] | ( (0xFF & data[i]) << (i*8));
			ints[2] = ints[2] | ( (0xFF & data[4+i]) << (i*8));
		}
		
		result[0] = ints[1] / ENC_LINEAR_FIXED_POINT;
		result[1] = ints[2] / ENC_LINEAR_FIXED_POINT;
	
		while (dec.pos < dataSize) {
			if (dec.pos == (dataSize - 1) && dec.half)
				if ((data[dec.pos] & 0xf) != 0x8)
					break;
			
			ints[0] = ints[1];
			ints[1] = ints[2];
			ints[2] = dec.next();
			
			extrapol = ints[1] + (ints[1] - ints[0]);
			y = extrapol + ints[2];
			result[ri++] = y / ENC_LINEAR_FIXED_POINT;
			ints[2] = y;
		}
		
		return ri;
	}
	
	
	/**
	 * Encodes ion counts by simply rounding to the nearest 4 byte integer, 
	 * and compressing each integer with encodeInt. 
	 *
	 * The handleable range is therefore 0 -> 4294967294.
	 * The resulting binary is maximally dataSize * 5 bytes, but much less if the 
	 * data is close to 0 on average.
	 */
	public static int encodeCount(
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
	 * Decodes data encoded by encodeCount
	 *
	 * result vector guaranteedly shorter than twice the data length (in nbr of values)
	 * returns the number of doubles read
	 */
	public static int decodeCount(
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
	
	
	/**
	 * Encodes ion counts by taking the natural logarithm, and storing a
	 * fixed point representation of this. This is calculated as
	 * 
	 * unsigned short fp = log(d) * 3000.0 + 0.5
	 */
	public static int encode2ByteFloat(
			double[] data, 
			int dataSize, 
			byte[] result
	) {
		int fp;
		int ri = 0;
		
		for (int i=0; i<dataSize; i++) {
			fp = (int)(Math.log(data[i]) * ENC_TWO_BYTE_FIXED_POINT + 0.5);
		
			result[ri++] = (byte)(0xff & fp);
			result[ri++] = (byte)(fp >> 8);
		}
		return ri;
	}
	
	
	/**
	 * Decodes data encoded by encode2ByteFloat
	 *
	 * result vector length is twice the data length
	 * returns the number of doubles read
	 */
	public static int decode2ByteFloat(
			byte[] data, 
			int dataSize, 
			double[] result
	) {
		int fp;
		int ri = 0;
	
		for (int i=0; i<dataSize; i+=2) {
			fp = (0xff & data[i]) | ((0xff & data[i+1]) << 8);
			result[ri++] = Math.exp(((double)(0xffff & fp)) / ENC_TWO_BYTE_FIXED_POINT);
		}
		return ri;
	}
}
