/*
	MSNumpressTest.cpp
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

/*
	Compile and run tests (on LINUX) with
	
	> g++ MSNumpress.cpp MSNumpressTest.cpp -o test && ./test

 */

#include "MSNumpress.hpp"
#include <assert.h>
#include <iostream>
#include <cmath>
#include <cstdlib>

using std::cout;
using std::endl;
using std::abs;

void encodeLinear() {
	cout << "test encodeLinear: ";

	double mzs[4];
	
	mzs[0] = 100.0;
	mzs[1] = 200.0;
	mzs[2] = 300.00005;
	mzs[3] = 400.00010;
	
	size_t nMzs = 4;
	unsigned char encoded[20];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], nMzs, &encoded[0]);
	
	assert(10 == encodedBytes);
	assert(0x80 == encoded[0]);
	assert(0x96 == encoded[1]);
	assert(0x98 == encoded[2]);
	assert(0x00 == encoded[3]);
	assert(0x75 == encoded[8]);
	assert(0x80 == encoded[9]);
	
	cout << "Passed" << endl;
}


void decodeLinearNice() {
	cout << "test decodeLinearNice: ";
	
	double mzs[4];
	
	mzs[0] = 100.0;
	mzs[1] = 200.0;
	mzs[2] = 300.00005;
	mzs[3] = 400.00010;
	
	size_t nMzs = 4;
	unsigned char encoded[20];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], nMzs, &encoded[0]);
	
	double decoded[4];
	size_t numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0]);
	assert(4 == numDecoded);
	assert(abs(100.0 - decoded[0]) < 0.000005);
	assert(abs(200.0 - decoded[1]) < 0.000005);
	assert(abs(300.00005 - decoded[2]) < 0.000005);
	assert(abs(400.00010 - decoded[3]) < 0.000005);
	
	cout << "Passed" << endl;
}

void decodeLinearWierd() {
	cout << "test decodeLinearWierd: ";
	
	double mzs[4];
	
	mzs[0] = 100.0;
	mzs[1] = 200.0;
	mzs[2] = 300.00005;
	mzs[3] = 0.00010;
	
	size_t nMzs = 4;
	unsigned char encoded[20];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], nMzs, &encoded[0]);
	
	double decoded[4];
	size_t numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0]);
	assert(4 == numDecoded);
	assert(abs(100.0 - decoded[0]) < 0.000005);
	assert(abs(200.0 - decoded[1]) < 0.000005);
	assert(abs(300.00005 - decoded[2]) < 0.000005);
	assert(abs(0.00010 - decoded[3]) < 0.000005);
	
	cout << "Passed" << endl;
}


void encodeDecodeLinear() {
	cout << "test encodeDecodeLinear: ";
	
	srand(123459);
	
	size_t n = 1000;
	double mzs[1000];
	mzs[0] = 300 + (rand() % 1000) / 1000.0;
	for (int i=1; i<n; i++) 
		mzs[i] = mzs[i-1] + (rand() % 1000) / 1000.0;
	
	unsigned char encoded[5000];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], n, &encoded[0]);
	
	double decoded[1000];
	size_t numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0]);
	
	assert(n == numDecoded);
	
	for (int i=0; i<n; i++) 
		assert(abs(mzs[i] - decoded[i]) < 0.000005);
	
	cout << "Passed" << endl;
}



void encodeDecodeLinear5() {
	cout << "test encodeDecodeLinear5: ";
	
	srand(123459);
	
	size_t n = 1000;
	double mzs[1000];
	mzs[0] = 100 + (rand() % 1000) / 1000.0;
	for (int i=1; i<n; i++) 
		mzs[i] = mzs[i-1] + (rand() % 1000) / 1000.0;
		
	size_t encodedBytes;
	size_t numDecoded;
	unsigned char encoded[5000];
	double firstDecoded[1000];
	double decoded[1000];
	
	encodedBytes 	= ms::numpress::MSNumpress::encodeLinear(&mzs[0], n, &encoded[0]);
	numDecoded 		= ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0]);
	
	for (int i=0; i<n; i++) 
		firstDecoded[i] = decoded[i];
	
	for (int i=0; i<5; i++) {
		encodedBytes 	= ms::numpress::MSNumpress::encodeLinear(&decoded[0], n, &encoded[0]);
		numDecoded 		= ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0]);
	}
	
	assert(n == numDecoded);
		
	for (int i=0; i<n; i++)
		if (firstDecoded[i] != decoded[i]) {
			cout << endl << firstDecoded[i] << " " << decoded[i] << endl;
			assert(firstDecoded[i] == decoded[i]);
		}
	
	cout << "Passed" << endl;
}



void encodeDecodePic() {
	cout << "test encodeDecodePic: ";
	
	srand(123459);
	
	size_t n = 1000;
	double ics[1000];
	for (int i=0; i<n; i++) 
		ics[i] = rand() % 1000000;
	
	unsigned char encoded[5000];
	size_t encodedBytes = ms::numpress::MSNumpress::encodePic(&ics[0], n, &encoded[0]);
	
	double decoded[1000];
	size_t numDecoded = ms::numpress::MSNumpress::decodePic(&encoded[0], encodedBytes, &decoded[0]);
	
	assert(n == numDecoded);
	
	for (int i=0; i<n; i++) 
		assert(abs(ics[i] - decoded[i]) < 0.5);
	
	cout << "Passed" << endl;
}



void encodeDecodePic5() {
	cout << "test encodeDecodePic5: ";
	
	srand(123459);
	
	size_t n = 1000;
	double ics[1000];
	for (int i=0; i<n; i++) 
		ics[i] = rand() % 1000000;
	
	ics[1] = 0.0;
	ics[2] = 0.0001;
	ics[3] = 0.0002;
	
	size_t encodedBytes;
	size_t numDecoded;
	unsigned char encoded[5000];
	double firstDecoded[1000];
	double decoded[1000];
	
	encodedBytes 	= ms::numpress::MSNumpress::encodePic(&ics[0], n, &encoded[0]);
	numDecoded 		= ms::numpress::MSNumpress::decodePic(&encoded[0], encodedBytes, &decoded[0]);
	
	for (int i=0; i<n; i++) 
		firstDecoded[i] = decoded[i];
	
	for (int i=0; i<5; i++) {
		encodedBytes 	= ms::numpress::MSNumpress::encodePic(&decoded[0], n, &encoded[0]);
		numDecoded 		= ms::numpress::MSNumpress::decodePic(&encoded[0], encodedBytes, &decoded[0]);
	}
	
	assert(n == numDecoded);
		
	for (int i=0; i<n; i++)
		if (firstDecoded[i] != decoded[i]) {
			cout << endl << firstDecoded[i] << " " << decoded[i] << endl;
			assert(firstDecoded[i] == decoded[i]);
		}
	
	cout << "Passed" << endl;
}



void encodeDecodeSlof() {
	cout << "test encodeDecodeSlof: ";
	
	srand(123459);
	
	size_t n = 1000;
	double ics[1000];
	for (int i=0; i<n; i++) 
		ics[i] = rand() % 1000000;
	
	ics[1] = 0.0;
	ics[2] = 0.0001;
	ics[3] = 0.0002;
	
	unsigned char encoded[5000];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeSlof(&ics[0], n, &encoded[0]);
	
	double decoded[1000];
	size_t numDecoded = ms::numpress::MSNumpress::decodeSlof(&encoded[0], encodedBytes, &decoded[0]);
	
	assert(n == numDecoded);
		
	for (int i=0; i<n; i++)
		if (ics[i] < 1.0) {
			if (abs(ics[i] - decoded[i]) >= 0.0005) {
				cout << endl << ics[i] << " " << decoded[i] << endl;
				assert(abs(ics[i] - decoded[i]) < 0.0005);
			}
		} else {
			if (abs((ics[i] - decoded[i]) / ((ics[i] + decoded[i])/2)) >= 0.0005) {
				cout << endl << ics[i] << " " << decoded[i] << endl;
				assert(abs((ics[i] - decoded[i]) / ((ics[i] + decoded[i])/2)) < 0.0005);
			}
		}
	cout << "Passed" << endl;
}



void encodeDecodeSlof5() {
	cout << "test encodeDecodeSlof5: ";
	
	srand(123459);
	
	size_t n = 1000;
	double ics[1000];
	for (int i=0; i<n; i++) 
		ics[i] = rand() % 1000000;
	
	ics[1] = 0.0;
	ics[2] = 0.0001;
	ics[3] = 0.0002;
	
	size_t encodedBytes;
	size_t numDecoded;
	unsigned char encoded[5000];
	double firstDecoded[1000];
	double decoded[1000];
	
	encodedBytes 	= ms::numpress::MSNumpress::encodeSlof(&ics[0], n, &encoded[0]);
	numDecoded 		= ms::numpress::MSNumpress::decodeSlof(&encoded[0], encodedBytes, &decoded[0]);
	
	for (int i=0; i<n; i++) 
		firstDecoded[i] = decoded[i];
	
	for (int i=0; i<5; i++) {
		encodedBytes 	= ms::numpress::MSNumpress::encodeSlof(&decoded[0], n, &encoded[0]);
		numDecoded 		= ms::numpress::MSNumpress::decodeSlof(&encoded[0], encodedBytes, &decoded[0]);
	}
	
	assert(n == numDecoded);
		
	for (int i=0; i<n; i++)
		if (firstDecoded[i] != decoded[i]) {
			cout << endl << firstDecoded[i] << " " << decoded[i] << endl;
			assert(firstDecoded[i] == decoded[i]);
		}
	
	cout << "Passed" << endl;
}


int main(int argc, const char* argv[]) {
	encodeLinear();
	decodeLinearNice();
	decodeLinearWierd();
	encodeDecodeLinear();
	encodeDecodePic();
	encodeDecodeSlof();
	encodeDecodeLinear5();
	encodeDecodePic5();
	encodeDecodeSlof5();
	cout << "=== all tests succeeded! ===" << endl;
	return 0;
}
