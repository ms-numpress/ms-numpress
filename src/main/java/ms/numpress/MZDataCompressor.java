package ms.numpress;

class MZDataCompressor {


	protected static double FIXED_POINT = 100000.0;
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
			for (i=l; i<8; i++) {
				res[resOffset+1+i-l] = (byte)(0xf & (x >> (4*(i-l))));
			}
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
			for (i=l; i<8; i++) {
				res[resOffset+1+i-l] = (byte)(0xf & (x >> (4*(i-l))));
			}
			return 1+8-l;

		} else {
			res[resOffset] = 0;
			for (i=0; i<8; i++) {
				res[resOffset+1+i] = (byte)(0xf & (x >> (4*i)));
			}
			return 9;

		}
	}
	
	
/**
 * Encodes the doubles in data by first using a 
 *   - lossy conversion to a 4 byte 5 decimal fixed point repressentation
 *   - storing the residuals from a linear prediction after first to values
 *   - encoding by encode_int (see above) 
 * 
 * The resulting binary is maximally dataSize * 5 bytes, but much less if the 
 * data is reasonably smooth on the first order.
 */
	public static int encode(
			double[] data, 
			int dataSize, 
			byte[] result
	) {
		long[] ints = new long[3];
		int i, j, ri;
		byte halfBytes[] = new byte[10];
		int halfByteCount;
		int hbi;
		long extrapol;
		long diff;

		//printf("Encoding %d doubles\n", (int)dataSize);

		ints[1] = (long)(data[0] * FIXED_POINT + 0.5);
		ints[2] = (long)(data[1] * FIXED_POINT + 0.5);

		for (i=0; i<4; i++) {
			result[i] 	= (byte)((ints[1] >> (i*8)) & 0xff);
			result[4+i] = (byte)((ints[2] >> (i*8)) & 0xff);
		}

		halfByteCount = 0;
		ri = 8;

		for (i=2; i<dataSize; i++) {
			ints[0] = ints[1];
			ints[1] = ints[2];
			ints[2] = (long)(data[i] * FIXED_POINT + 0.5);
			extrapol = ints[1] + (ints[1] - ints[0]);
			diff = ints[2] - extrapol;
			//printf("%d %d %d,   extrapol: %d    diff: %d \n", ints[0], ints[1], ints[2], extrapol, diff);
			halfByteCount += encodeInt(diff, halfBytes, halfByteCount);
			/*
			printf("%d (%d):  ", diff, (int)halfByteCount);
			for (j=0; j<halfByteCount; j++) {
				printf("%x ", halfBytes[j] & 0xf);
			}
			printf("\n");
			*/
		
		
			for (hbi=1; hbi < halfByteCount; hbi+=2) {
				result[ri] = (byte)((halfBytes[hbi-1] << 4) | (halfBytes[hbi] & 0xf));
				//printf("%x \n", result[ri]);
				ri++;
			}
			if (halfByteCount % 2 != 0) {
				halfBytes[0] = halfBytes[halfByteCount-1];
				halfByteCount = 1;
			} else {
				halfByteCount = 0;
			}
		}
		if (halfByteCount == 1) {
			result[ri] = (byte)(halfBytes[0] << 4);
			ri++;
		}
		return ri;
	}
	
	
	
/**
 * Decodes data encoded by encode. Note that the compression 
 * discard any information < 10^-5, so data is only guaranteed 
 * to be within +- 0.5*10^-5 of the original value.
 *
 * Further, values > ~42000 will also be truncated because of the
 * fixed point repressentation, so this scheme is stronly discouraged 
 * if values above might be above this size.
 */
	public static void decode(
			byte[] data, 
			int dataSize, 
			double[] result, 
			int resultDoubleCount
	) {
		int i;
		long[] ints = new long[3];
		long extrapol;
		long y;
		IntDecoder dec = new IntDecoder(data, 8);

		ints[1] = 0;
		ints[2] = 0;

		for (i=0; i<4; i++) {
			ints[1] = ints[1] | ( (0xFF & data[i]) << (i*8));
			ints[2] = ints[2] | ( (0xFF & data[4+i]) << (i*8));
		}
		
		result[0] = ((double)ints[1]) / FIXED_POINT;
		result[1] = ((double)ints[2]) / FIXED_POINT;
	
		for (i=2; i<resultDoubleCount; i++) {
			ints[0] = ints[1];
			ints[1] = ints[2];
			ints[2] = dec.next();
		
			extrapol = ints[1] + (ints[1] - ints[0]);
			y = extrapol + ints[2];
			result[i] = ((double)y) / FIXED_POINT;
			ints[2] = y;
		}
	}
}
