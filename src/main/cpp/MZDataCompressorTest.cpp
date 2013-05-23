

#include "MZDataCompressor.hpp"
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <vector>

using ms::numpress::MZDataCompressor;



void testEncodeInt() {
	unsigned char res[10];
	size_t l;
	l = 0;
	MZDataCompressor::encode_int(0, &res[0], &l);
	assert(1 == l);
	assert(8 == (res[0] & 0xf));
	
	l = 0;
	MZDataCompressor::encode_int(-1,  &res[0], &l);
	assert(2 == l);
	assert(0xf == (res[0] & 0xf));
	assert(0xf == (res[1] & 0xf));
	
	l = 0;
	MZDataCompressor::encode_int(35,  &res[0], &l);
	assert(3 == l);
	assert(6 == (res[0] & 0xf));
	assert(0x3 == (res[1] & 0xf));
	assert(0x2 == (res[2] & 0xf));
	
	l = 0;
	MZDataCompressor::encode_int(370000000,  &res[0], &l);
	assert(9 == l);
	assert(0 == (res[0] & 0xf));
	assert(0x0 == (res[1] & 0xf));
	assert(0x8 == (res[2] & 0xf));
	assert(0x0 == (res[3] & 0xf));
	assert(0xc == (res[4] & 0xf));
	assert(0xd == (res[5] & 0xf));
	assert(0x0 == (res[6] & 0xf));
	assert(0x6 == (res[7] & 0xf));
	assert(0x1 == (res[8] & 0xf));
}




void testDecodeInt() {
	std::vector<unsigned char> bytes;
	bytes.resize(10);
	bytes[0] = (unsigned char)0x75;
	bytes[1] = (unsigned char)0x87;
	bytes[2] = (unsigned char)0x11;
	bytes[3] = (unsigned char)0x00;
	bytes[4] = (unsigned char)0x1e;
	bytes[5] = (unsigned char)0x5f;
	bytes[6] = (unsigned char)0x50;
	
	size_t bi = 0;
	int half = 0;
	int res = 0;
	
	MZDataCompressor::decode_int(bytes, &bi, &half, &res);
	assert(5 == res);
	
	MZDataCompressor::decode_int(bytes, &bi, &half, &res);
	assert(0 == res);
		
	MZDataCompressor::decode_int(bytes, &bi, &half, &res);
	assert(1 == res);
	
	MZDataCompressor::decode_int(bytes, &bi,  &half, &res);
	assert(100000000 == res);
}
	


int main(int argc, char **args) {
	testEncodeInt();
	testDecodeInt();
	return 0;
}
