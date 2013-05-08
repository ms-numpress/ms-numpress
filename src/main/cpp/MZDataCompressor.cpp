//
// MZDataCompressor.cpp
// johan.teleman@immun.lth.se
//

#include <stdio.h>
#include <stdint.h>
#include "MZDataCompressor.hpp"

namespace ms {
namespace numpress {

/**
 * Encodes the int x as a number of halfbytes in res. 
 * res_length is incremented by the number of halfbytes, 
 * which will be 1 <= n <= 9
 */
void encode_int(
		int x,
		unsigned char* res,
		size_t *res_length	
) {
	int i, l, m;
	int mask = 0xf0000000;
	int init = x & mask;

	if (init == 0) {
		l = 8;
		for (i=0; i<8; i++) {
			m = mask >> (4*i);
			if ((x & m) != 0) {
				l = i;
				break;
			}
		}
		res[0] = l;
		for (i=l; i<8; i++) {
			res[1+l-i] = x >> (4*(7-i));
		}
		*res_length += 1+8-l;

	} else if (init == mask) {
		l = 7;
		for (i=0; i<8; i++) {
			m = mask >> (4*i);
			if ((x & m) != m) {
				l = i;
				break;
			}
		}
		res[0] = l + 8;
		for (i=l; i<8; i++) {
			res[+1+l-i] = x >> (4*(7-i));
		}
		*res_length += 1+8-l;

	} else {
		res[0] = 0;
		for (i=0; i<8; i++) {
			res[1+i] = x >> (4*i);
		}
		*res_length += 9;

	}
}



void MZDataCompressor::encode(
		const double *data, 
		size_t dataSize, 
		unsigned char *result, 
		size_t *resultByteCount
) {
	int ints[3];
	size_t i, j, ri;
	unsigned char* init;
	unsigned char halfBytes[10];
	size_t halfByteCount;
	size_t hbi;
	int extrapol;
	int diff;

	//printf("Encoding %d doubles\n", (int)dataSize);

	ints[1] = data[0] * 100000 + 0.5;
	ints[2] = data[1] * 100000 + 0.5;

	init = (unsigned char*)&ints[1];
	
	for (ri=0; ri<8; ri++) {
		result[ri] = init[ri];
	}

	halfByteCount = 0;
	ri = 8;

	for (i=2; i<dataSize; i++) {
		ints[0] = ints[1];
		ints[1] = ints[2];
		ints[2] = data[i] * 100000 + 0.5;
		extrapol = ints[1] + (ints[1] - ints[0]);
		diff = ints[2] - extrapol;
		//printf("%d %d %d,   extrapol: %d    diff: %d \n", ints[0], ints[1], ints[2], extrapol, diff);
		encode_int(diff, &halfBytes[halfByteCount], &halfByteCount);
		/*
		printf("%d (%d):  ", diff, (int)halfByteCount);
		for (j=0; j<halfByteCount; j++) {
			printf("%x ", halfBytes[j] & 0xf);
		}
		printf("\n");
		*/
		
		
		for (hbi=1; hbi < halfByteCount; hbi+=2) {
			result[ri] = (halfBytes[hbi-1] << 4) | (halfBytes[hbi] & 0xf);
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
		result[ri] = halfBytes[0] << 4;
		ri++;
	}
	*resultByteCount = ri;
}


/**
 * Decodes an int from the half bytes in bp. Lossless reverse of encode_int 
 */
void decode_int(
		const unsigned char **bp,
		int *half,
		int *res
) {
	size_t n;
	size_t i;
	int mask, m;
	unsigned char head;
	unsigned char hb;

	if (*half == 0) {
		head = (**bp) >> 4;
	} else {
		head = (**bp) & 0xf;
		(*bp)++;
	}
	*res = 0;
	
	*half = 1- (*half);
	
	if (head <= 8) {
		n = head;
	} else {
		n = head - 8;
		mask = 0xf0000000;
		for (i=0; i<n; i++) {
			m = mask >> (4*i);
			*res = *res | m;
		}
	}
	
	if (n == 8) {
		return;
	}
	
	for (i=0; i<(8-n); i++) {
		if (*half == 0) {
			hb = (**bp) >> 4;
		} else {
			hb = (**bp) & 0xf;
			(*bp)++;
		}
		*res = *res | (hb << ((7-n)*4));
		*half = 1 - (*half);
	}
}



void MZDataCompressor::decode(
		const unsigned char *data, 
		size_t dataSize, 
		double *result, 
		size_t resultDoubleCount
) {
	size_t i;
	int *inits;
	int ints[3];
	double d;
	const unsigned char *di;
	int half;
	int extrapol;
	int y;

	inits = (int*)&data[0];
	result[0] = (d = inits[0]) / 100000;
	result[1] = (d = inits[1]) / 100000;
	
	ints[1] = inits[0];
	ints[2] = inits[1];
	
	di = &data[8];
	half = 0;
	
	for (i=2; i<resultDoubleCount; i++) {
		ints[0] = ints[1];
		ints[1] = ints[2];
		decode_int(&di, &half, &ints[2]);
		
		extrapol = ints[1] + (ints[1] - ints[0]);
		y = extrapol + ints[2];
		printf("%d %d,   extrapol: %d    diff: %d \n", ints[0], ints[1], extrapol, ints[2]);
		result[i] = (d = y) / 100000;
		ints[2] = y;
	}
}

} // namespace numpress
} // namespace ms
