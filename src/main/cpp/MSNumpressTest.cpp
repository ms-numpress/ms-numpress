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
#include <stdio.h>

using std::cout;
using std::endl;
using std::abs;
using std::max;


double ENC_TWO_BYTE_FIXED_POINT = 3000.0;



void encodeLinear1() {

	double mzs[1];
	
	mzs[0] = 100.0;
	
	size_t nMzs = 1;
	unsigned char encoded[12];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], nMzs, &encoded[0], 100000.0);
	
	assert(12 == encodedBytes);
	assert(0x80 == encoded[8]);
	assert(0x96 == encoded[9]);
	assert(0x98 == encoded[10]);
	assert(0x00 == encoded[11]);
	
	cout << "+ pass    encodeLinear1 " << endl << endl;
}

void encodeLinear() {
	double mzs[4];
	
	mzs[0] = 100.0;
	mzs[1] = 200.0;
	mzs[2] = 300.00005;
	mzs[3] = 400.00010;
	
	size_t nMzs = 4;
	unsigned char encoded[20];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], nMzs, &encoded[0], 100000.0);
	
	assert(18 == encodedBytes);
	assert(0x80 == encoded[8]);
	assert(0x96 == encoded[9]);
	assert(0x98 == encoded[10]);
	assert(0x00 == encoded[11]);
	assert(0x75 == encoded[16]);
	assert(0x80 == encoded[17]);
	
	cout << "+ pass    encodeLinear " << endl << endl;
}


void decodeLinearNice() {
	
	double mzs[4];
	
	mzs[0] = 100.0;
	mzs[1] = 200.0;
	mzs[2] = 300.00005;
	mzs[3] = 400.00010;
	
	size_t nMzs = 4;
	unsigned char encoded[28];
	double fixedPoint = ms::numpress::MSNumpress::optimalLinearFixedPoint(&mzs[0], nMzs);
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], nMzs, &encoded[0], fixedPoint);
	
	double decoded[4];
	size_t numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0]);
	assert(4 == numDecoded);
	assert(abs(100.0 - decoded[0]) < 0.000005);
	assert(abs(200.0 - decoded[1]) < 0.000005);
	assert(abs(300.00005 - decoded[2]) < 0.000005);
	assert(abs(400.00010 - decoded[3]) < 0.000005);
	
	cout << "+ pass    decodeLinearNice " << endl << endl;
}

void decodeLinearWierd() {
	double mzs[4];
	
	mzs[0] = 100.0;
	mzs[1] = 200.0;
	mzs[2] = 300.00005;
	mzs[3] = 0.00010;
	
	size_t nMzs = 4;
	unsigned char encoded[28];
	double fixedPoint = ms::numpress::MSNumpress::optimalLinearFixedPoint(&mzs[0], nMzs);
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], nMzs, &encoded[0], fixedPoint);
	
	double decoded[4];
	size_t numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0]);
	assert(4 == numDecoded);
	assert(abs(100.0 - decoded[0]) < 0.000005);
	assert(abs(200.0 - decoded[1]) < 0.000005);
	assert(abs(300.00005 - decoded[2]) < 0.000005);
	assert(abs(0.00010 - decoded[3]) < 0.000005);
	
	cout << "+ pass    decodeLinearWierd " << endl << endl;
}


void decodeLinearCorrupt1() {
	unsigned char encoded[20];
	double decoded[4];
	
	size_t numDecoded;
	try {
		numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], 11, &decoded[0]);
		cout << "- fail    decodeLinearCorrupt1: didn't throw exception for corrupt input " << endl << endl;
		assert(0 == 1);
	} catch (const char *err) {
		
	}
	
	try {
		numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], 14, &decoded[0]);
		cout << "- fail    decodeLinearCorrupt1: didn't throw exception for corrupt input " << endl << endl;
		assert(0 == 1);
	} catch (const char *err) {
		
	}
	
	cout << "+ pass    decodeLinearCorrupt 1 " << endl << endl;
}


void decodeLinearCorrupt2() {
	
	double mzs[4];
	
	mzs[0] = 100.0;
	mzs[1] = 200.0;
	mzs[2] = 300.00005;
	mzs[3] = 0.00010;
	
	size_t nMzs = 4;
	unsigned char encoded[28];
	double fixedPoint = ms::numpress::MSNumpress::optimalLinearFixedPoint(&mzs[0], nMzs);
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], nMzs, &encoded[0], fixedPoint);
	
	double decoded[4];
	try {
		size_t numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes-1, &decoded[0]);
		cout << "- fail    decodeLinearCorrupt2: didn't throw exception for corrupt input " << endl << endl;
		assert(0 == 1);
	} catch (const char *err) {
		
	}
	
	cout << "+ pass    decodeLinearCorrupt 2 " << endl << endl;
}



void optimalLinearFixedPoint() {
	srand(123459);
	
	size_t n = 1000;
	double mzs[1000];
	mzs[0] = 300 + (rand() % 1000) / 1000.0;
	for (int i=1; i<n; i++) 
		mzs[i] = mzs[i-1] + (rand() % 1000) / 1000.0;
	
	cout << "+                    max val: " << mzs[n-1] << endl;
	cout << "+          optimal linear fp: " << ms::numpress::MSNumpress::optimalLinearFixedPoint(&mzs[0], n) << endl;
	cout << "+ pass    optimalLinearFixedPoint " << endl << endl;
}


void encodeDecodeLinearStraight() {
	size_t n = 15;
	double mzs[15];
	for (int i=0; i<n; i++) 
		mzs[i] = i;
	
	double fixedPoint = ms::numpress::MSNumpress::optimalLinearFixedPoint(&mzs[0], n);
	
	unsigned char encoded[75];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], n, &encoded[0], fixedPoint);
	
	double decoded[15];
	size_t numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0]);
	
	assert(n == numDecoded);
	
	double m = 0;
	double error;
	double mLim = 0.000005;
	
	for (int i=0; i<n; i++) { 
		error = abs(mzs[i] - decoded[i]);
		m = max(m, error);
		if (error >= mLim) {
			cout << "error   " << error << " above limit " << mLim << endl;
			assert(error < mLim);
		}
	}
	cout << "+         compression: " << encodedBytes / double(n*8) * 100 << "% " << endl;
	cout << "+           max error: " << m << "  limit: " << mLim << endl;
	cout << "+ pass    encodeDecodeLinearStraight " << endl << endl;
}



void encodeDecodeSafeStraight() {
	double error;
	double eLim = 1.0e-300;
	size_t n = 15;
	double mzs[15];
	for (int i=0; i<n; i++) 
		mzs[i] = i+1;
	
	unsigned char encoded[8000];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeSafe(&mzs[0], n, &encoded[0]);
	
	/*
	printf("\n");
	assert(encodedBytes == n*8);
	for (size_t i=0; i<n; i++) { 
		for (size_t j=0; j<8; j++) {
			printf("%x ", encoded[i*8 + j]);
		}
		printf("\n");
	}
	*/
	
	double decoded[1000];
	size_t numDecoded = ms::numpress::MSNumpress::decodeSafe(&encoded[0], encodedBytes, &decoded[0]);
	
	assert(numDecoded == n);
	for (int i=0; i<n; i++) { 
		error = abs(mzs[i] - decoded[i]);
		if (error >= eLim) {
			cout << "error   " << error << " is non-zero ( >= " << eLim << " )" << endl;
			assert(error == 0);
		}
	}
	cout << "+ pass    encodeDecodeSafeStraight " << endl << endl;
}



void encodeDecodeSafe() {
	srand(123459);
	
	double error;
	double eLim = 1.0e-300;
	size_t n = 1000;
	double mzs[1000];
	mzs[0] = 300 + rand() / double(RAND_MAX);
	for (int i=1; i<n; i++) 
		mzs[i] = mzs[i-1] + rand() / double(RAND_MAX);
	
	unsigned char encoded[8000];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeSafe(&mzs[0], n, &encoded[0]);
	
	assert(encodedBytes == n*8);
	
	double decoded[1000];
	size_t numDecoded = ms::numpress::MSNumpress::decodeSafe(&encoded[0], encodedBytes, &decoded[0]);
	
	assert(numDecoded == n);
	for (int i=0; i<n; i++) { 
		error = abs(mzs[i] - decoded[i]);
		if (error >= eLim) {
			cout << "error   " << error << " is non-zero ( >= " << eLim << " )" << endl;
			assert(error == 0);
		}
	}
	cout << "+ pass    encodeDecodeSafe " << endl << endl;
}
		


void encodeDecodeLinear() {
	srand(123459);
	
	size_t n = 1000;
	double mzs[1000];
	mzs[0] = 300 + rand() / double(RAND_MAX);
	for (int i=1; i<n; i++) 
		mzs[i] = mzs[i-1] + rand() / double(RAND_MAX);
	
	double fixedPoint = ms::numpress::MSNumpress::optimalLinearFixedPoint(&mzs[0], n);
	
	unsigned char encoded[5000];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], n, &encoded[0], fixedPoint);
	
	double decoded[1000];
	size_t numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0]);
	
	assert(n == numDecoded);
	
	double m = 0;
	double error;
	double mLim = 0.000005;
	
	for (int i=0; i<n; i++) { 
		error = abs(mzs[i] - decoded[i]);
		m = max(m, error);
		if (error >= mLim) {
			cout << "error   " << error << " above limit " << mLim << endl;
			assert(error < mLim);
		}
	}
	cout << "+         compression: " << encodedBytes / double(n*8) * 100 << "% " << endl;
	cout << "+           max error: " << m << "  limit: " << mLim << endl;
	cout << "+ pass    encodeDecodeLinear " << endl << endl;
}



void encodeDecodeLinear5() {
	srand(123662);
	
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
	
	double fixedPoint = ms::numpress::MSNumpress::optimalLinearFixedPoint(&mzs[0], n);
	
	encodedBytes 	= ms::numpress::MSNumpress::encodeLinear(&mzs[0], n, &encoded[0], fixedPoint);
	numDecoded 		= ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0]);
	
	for (int i=0; i<n; i++) 
		firstDecoded[i] = decoded[i];
	
	for (int i=0; i<5; i++) {
		encodedBytes 	= ms::numpress::MSNumpress::encodeLinear(&decoded[0], n, &encoded[0], fixedPoint);
		numDecoded 		= ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0]);
	}
	
	double afterFixedPoint = ms::numpress::MSNumpress::optimalLinearFixedPoint(&decoded[0], n);
	
	assert(fixedPoint == afterFixedPoint);
	assert(n == numDecoded);
		
	for (int i=0; i<n; i++)
		if (firstDecoded[i] != decoded[i]) {
			cout << endl << firstDecoded[i] << " " << decoded[i] << endl;
			assert(firstDecoded[i] == decoded[i]);
		}
	
	cout << "+ pass    encodeDecodeLinear5 " << endl << endl;
}



void encodeDecodePic() {
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
	
	cout << "+ pass    encodeDecodePic " << endl << endl;
}



void encodeDecodePic5() {
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
	
	cout << "+ pass    encodeDecodePic5 " << endl << endl;
}



void optimalSlofFixedPoint() {

	srand(123459);
	
	size_t n = 1000;
	double ics[1000];
	for (int i=0; i<n; i++) 
		ics[i] = rand() % 1000000;
		
	cout << "+           optimal slof fp: " << ms::numpress::MSNumpress::optimalSlofFixedPoint(&ics[0], n) << endl;
	cout << "+ pass    optimalSlofFixedPoint " << endl << endl;
}



void encodeDecodeSlof() {
	srand(123459);
	
	size_t n = 1000;
	double ics[1000];
	for (int i=0; i<n; i++) 
		ics[i] = rand() % 1000000;
	
	ics[1] = 0.0;
	ics[2] = 0.0001;
	ics[3] = 0.0002;
	
	double fixedPoint = ms::numpress::MSNumpress::optimalSlofFixedPoint(&ics[0], n);
	
	unsigned char encoded[5000];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeSlof(&ics[0], n, &encoded[0], fixedPoint);
	
	double decoded[1000];
	size_t numDecoded = ms::numpress::MSNumpress::decodeSlof(&encoded[0], encodedBytes, &decoded[0]);
	
	assert(n == numDecoded);
	
	double m = 0;
	double rm = 0;
	double mLim = 0.0005;
	double rmLim = 0.0005;
	double error;
	
	for (int i=0; i<n; i++)
		if (ics[i] < 1.0) {
			error = abs(ics[i] - decoded[i]);
			m = max(m, error);
			if (error >= mLim) {
				cout << endl << ics[i] << " " << decoded[i] << endl;
				assert(error < mLim);
			}
		} else {
			error = abs((ics[i] - decoded[i]) / ((ics[i] + decoded[i])/2));
			rm = max(rm, error);
			if (error >= rmLim) {
				cout << endl << ics[i] << " " << decoded[i] << endl;
				assert(error < rmLim);
			}
		}
	cout << "+               max error: " << m << "  limit: " << mLim << endl;
	cout << "+           max rel error: " << rm << "  limit: " << rmLim << endl;
	cout << "+ pass    encodeDecodeSlof " << endl << endl;
}



void encodeDecodeSlof5() {
	srand(123459);
	
	size_t n = 1000;
	double ics[1000];
	for (int i=0; i<n; i++) 
		ics[i] = rand() % 1000000;
	
	ics[1] = 0.0;
	ics[2] = 0.0001;
	ics[3] = 0.0002;
	
	double fixedPoint = ms::numpress::MSNumpress::optimalSlofFixedPoint(&ics[0], n);
	
	size_t encodedBytes;
	size_t numDecoded;
	unsigned char encoded[5000];
	double firstDecoded[1000];
	double decoded[1000];
	
	encodedBytes 	= ms::numpress::MSNumpress::encodeSlof(&ics[0], n, &encoded[0], fixedPoint);
	numDecoded 		= ms::numpress::MSNumpress::decodeSlof(&encoded[0], encodedBytes, &decoded[0]);
	
	for (int i=0; i<n; i++) 
		firstDecoded[i] = decoded[i];
	
	for (int i=0; i<5; i++) {
		encodedBytes 	= ms::numpress::MSNumpress::encodeSlof(&decoded[0], n, &encoded[0], fixedPoint);
		numDecoded 		= ms::numpress::MSNumpress::decodeSlof(&encoded[0], encodedBytes, &decoded[0]);
	}
	
	double afterFixedPoint = ms::numpress::MSNumpress::optimalSlofFixedPoint(&decoded[0], n);
	
	assert(n == numDecoded);
	assert(fixedPoint == afterFixedPoint);
	
	for (int i=0; i<n; i++)
		if (firstDecoded[i] != decoded[i]) {
			cout << endl << firstDecoded[i] << " " << decoded[i] << endl;
			assert(firstDecoded[i] == decoded[i]);
		}
	
	cout << "+ pass    encodeDecodeSlof5 " << endl << endl;
}



void testErroneousDecodePic() {
	std::vector<double> result;

	// set data to [  100, 102, 140, 92, 33, 80, 145  ]; // Base64 is "ZGaMXCFQkQ=="
	std::vector<unsigned char> data;
	data.resize(32);
	data[0] = 100;
	data[1] = 102;
	data[2] = 140;
	data[3] = 92;
	data[4] = 33;
	data[5] = 80;
	data[6] = 145;

	try {
		ms::numpress::MSNumpress::decodePic(data, result);
		cout << "- fail    testErroneousDecodePic: didn't throw exception for corrupt input " << endl << endl;
		assert(0 == 1);
	} catch (const char *err) {
		
	}
	
	cout << "+ pass    testErroneousDecodePic " << endl << endl;
}


int main(int argc, const char* argv[]) {
	optimalLinearFixedPoint();
	encodeLinear1();
	encodeLinear();
	decodeLinearNice();
	decodeLinearWierd();
	decodeLinearCorrupt1();
	decodeLinearCorrupt2();
	encodeDecodeLinearStraight();
	encodeDecodeLinear();
	encodeDecodePic();
	encodeDecodeSafeStraight();
	encodeDecodeSafe();
	optimalSlofFixedPoint();
	encodeDecodeSlof();
	encodeDecodeLinear5();
	encodeDecodePic5();
	encodeDecodeSlof5();
	testErroneousDecodePic();
	
	cout << "=== all tests succeeded! ===" << endl;
	return 0;
}
