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
using std::max;


double ENC_LINEAR_FIXED_POINT = 100000.0;
double ENC_TWO_BYTE_FIXED_POINT = 3000.0;



void encodeLinear1() {

	double mzs[1];
	
	mzs[0] = 100.0;
	
	size_t nMzs = 1;
	unsigned char encoded[4];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], nMzs, &encoded[0], ENC_LINEAR_FIXED_POINT);
	
	assert(4 == encodedBytes);
	assert(0x80 == encoded[0]);
	assert(0x96 == encoded[1]);
	assert(0x98 == encoded[2]);
	assert(0x00 == encoded[3]);
	
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
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], nMzs, &encoded[0], ENC_LINEAR_FIXED_POINT);
	
	assert(10 == encodedBytes);
	assert(0x80 == encoded[0]);
	assert(0x96 == encoded[1]);
	assert(0x98 == encoded[2]);
	assert(0x00 == encoded[3]);
	assert(0x75 == encoded[8]);
	assert(0x80 == encoded[9]);
	
	cout << "+ pass    encodeLinear " << endl << endl;
}


void decodeLinearNice() {
	
	double mzs[4];
	
	mzs[0] = 100.0;
	mzs[1] = 200.0;
	mzs[2] = 300.00005;
	mzs[3] = 400.00010;
	
	size_t nMzs = 4;
	unsigned char encoded[20];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], nMzs, &encoded[0], ENC_LINEAR_FIXED_POINT);
	
	double decoded[4];
	size_t numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0], ENC_LINEAR_FIXED_POINT);
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
	unsigned char encoded[20];
	size_t encodedBytes = ms::numpress::MSNumpress::encodeLinear(&mzs[0], nMzs, &encoded[0], ENC_LINEAR_FIXED_POINT);
	
	double decoded[4];
	size_t numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0], ENC_LINEAR_FIXED_POINT);
	assert(4 == numDecoded);
	assert(abs(100.0 - decoded[0]) < 0.000005);
	assert(abs(200.0 - decoded[1]) < 0.000005);
	assert(abs(300.00005 - decoded[2]) < 0.000005);
	assert(abs(0.00010 - decoded[3]) < 0.000005);
	
	cout << "+ pass    decodeLinearWierd " << endl << endl;
}

void decodeLinearFaulty() {
	unsigned char encoded[20];
	double decoded[4];
	
	size_t numDecoded;
	numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], 3, &decoded[0], ENC_LINEAR_FIXED_POINT);
	assert(-1 == numDecoded);
	
	numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], 6, &decoded[0], ENC_LINEAR_FIXED_POINT);
	assert(-1 == numDecoded);
	
	cout << "+ pass    decodeLinearFaulty " << endl << endl;
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
	size_t numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0], fixedPoint);
	
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
	size_t numDecoded = ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0], fixedPoint);
	
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
	numDecoded 		= ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0], fixedPoint);
	
	for (int i=0; i<n; i++) 
		firstDecoded[i] = decoded[i];
	
	for (int i=0; i<5; i++) {
		encodedBytes 	= ms::numpress::MSNumpress::encodeLinear(&decoded[0], n, &encoded[0], fixedPoint);
		numDecoded 		= ms::numpress::MSNumpress::decodeLinear(&encoded[0], encodedBytes, &decoded[0], fixedPoint);
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
	size_t numDecoded = ms::numpress::MSNumpress::decodeSlof(&encoded[0], encodedBytes, &decoded[0], fixedPoint);
	
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
	numDecoded 		= ms::numpress::MSNumpress::decodeSlof(&encoded[0], encodedBytes, &decoded[0], fixedPoint);
	
	for (int i=0; i<n; i++) 
		firstDecoded[i] = decoded[i];
	
	for (int i=0; i<5; i++) {
		encodedBytes 	= ms::numpress::MSNumpress::encodeSlof(&decoded[0], n, &encoded[0], fixedPoint);
		numDecoded 		= ms::numpress::MSNumpress::decodeSlof(&encoded[0], encodedBytes, &decoded[0], fixedPoint);
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


int main(int argc, const char* argv[]) {
	optimalLinearFixedPoint();
	encodeLinear1();
	encodeLinear();
	decodeLinearFaulty();
	decodeLinearNice();
	decodeLinearWierd();
	encodeDecodeLinearStraight();
	encodeDecodeLinear();
	encodeDecodePic();
	optimalSlofFixedPoint();
	encodeDecodeSlof();
	encodeDecodeLinear5();
	encodeDecodePic5();
	encodeDecodeSlof5();
	
	cout << "=== all tests succeeded! ===" << endl;
	return 0;
}
