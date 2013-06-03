/*
	MSNumpressTest.java
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

import static org.junit.Assert.*;

import org.junit.Test;

public class MSNumpressTest {

	@Test
	public void encodeInt() {
		byte[] res = new byte[10];
		int l;
		l = MSNumpress.encodeInt(0l, res, 0);
		assertEquals(1, l);
		assertEquals(8, res[0] & 0xf);
		
		l = MSNumpress.encodeInt(-1l, res, 0);
		assertEquals(2, l);
		assertEquals(0xf, res[0] & 0xf);
		assertEquals(0xf, res[1] & 0xf);
		
		l = MSNumpress.encodeInt(35l, res, 0);
		assertEquals(3, l);
		assertEquals(6, res[0] & 0xf);
		assertEquals(0x3, res[1] & 0xf);
		assertEquals(0x2, res[2] & 0xf);
		
		l = MSNumpress.encodeInt(370000000l, res, 0);
		assertEquals(9, l);
		assertEquals(0, res[0] & 0xf);
		assertEquals(0x0, res[1] & 0xf);
		assertEquals(0x8, res[2] & 0xf);
		assertEquals(0x0, res[3] & 0xf);
		assertEquals(0xc, res[4] & 0xf);
		assertEquals(0xd, res[5] & 0xf);
		assertEquals(0x0, res[6] & 0xf);
		assertEquals(0x6, res[7] & 0xf);
		assertEquals(0x1, res[8] & 0xf);
	}
	
	
	@Test
	public void decodeInt() {
		byte[] res = new byte[10];
		res[0] = (byte)0x75;
		res[1] = (byte)0x87;
		res[2] = (byte)0x10;
		res[3] = (byte)0x08;
		res[4] = (byte)0x0c;
		res[5] = (byte)0xd0;
		res[6] = (byte)0x61;
		IntDecoder dec = new IntDecoder(res, 0);
		
		long l;
		l = dec.next();
		assertEquals(5, l);
		
		l = dec.next();
		assertEquals(0, l);
		
		l = dec.next();
		assertEquals(1, l);
		
		l = dec.next();
		assertEquals(370000000L, l);
	}

	@Test
	public void encodeLinear() {
		double[] mzs = {100.0, 200.0, 300.00005, 400.00010};
		byte[] encoded 		= new byte[16];
		int encodedBytes 	= MSNumpress.encodeLinear(mzs, 4, encoded);
		assertEquals(10, encodedBytes);
		assertEquals(0x80, 0xff & encoded[0]);
		assertEquals(0x96, 0xff & encoded[1]);
		assertEquals(0x98, 0xff & encoded[2]);
		assertEquals(0x00, 0xff & encoded[3]);
		assertEquals(0x75, 0xff & encoded[8]);
		assertEquals(0x80, 0xf0 & encoded[9]);
	}
	
	
	@Test
	public void decodeLinearNice() {
		double[] mzs = {100.0, 200.0, 300.00005, 400.00010};
		byte[] encoded 		= new byte[20];
		int encodedBytes 	= MSNumpress.encodeLinear(mzs, 4, encoded);
		double[] decoded 	= new double[4];
		int numDecoded 		= MSNumpress.decodeLinear(encoded, encodedBytes, decoded);
		assertEquals(4, numDecoded);
		assertEquals(100.0, decoded[0], 0.000005);
		assertEquals(200.0, decoded[1], 0.000005);
		assertEquals(300.00005, decoded[2], 0.000005);
		assertEquals(400.00010, decoded[3], 0.000005);
	}
	
	
	@Test
	public void decodeLinearWierd() {
		double[] mzs = {100.0, 200.0, 4000.00005, 0.00010};
		byte[] encoded 		= new byte[20];
		int encodedBytes 	= MSNumpress.encodeLinear(mzs, 4, encoded);
		double[] decoded 	= new double[4];
		int numDecoded 		= MSNumpress.decodeLinear(encoded, encodedBytes, decoded);
		assertEquals(4, numDecoded);
		assertEquals(100.0, decoded[0], 0.000005);
		assertEquals(200.0, decoded[1], 0.000005);
		assertEquals(4000.00005, decoded[2], 0.000005);
		assertEquals(0.00010, decoded[3], 0.000005);
	}


	@Test
	public void encodeDecodeLinear() {
		
		int n = 1000;
		double[] mzs = new double[n];
		mzs[0] = 300 + Math.random();
		for (int i=1; i<n; i++) 
			mzs[i] = mzs[i-1] + Math.random();
		
		byte[] encoded 		= new byte[n * 5];
		int encodedBytes 	= MSNumpress.encodeLinear(mzs, n, encoded);
		double[] decoded 	= new double[n];
		int decodedDoubles 	= MSNumpress.decodeLinear(encoded, encodedBytes, decoded);
		
		assertEquals(n, decodedDoubles);
		
		for (int i=0; i<n; i++) 
			assertEquals(mzs[i], decoded[i], 0.000005);
	}


	@Test
	public void encodeDecodeCount() {
		
		int n = 1000;
		double[] ics = new double[n];
		for (int i=0; i<n; i++) 
			ics[i] = Math.pow(10, 6*Math.random());
		
		byte[] encoded 		= new byte[n * 5];
		int encodedBytes 	= MSNumpress.encodeCount(ics, n, encoded);
		double[] decoded 	= new double[n];
		int decodedDoubles 	= MSNumpress.decodeCount(encoded, encodedBytes, decoded);
		
		assertEquals(n, decodedDoubles);
		
		for (int i=0; i<n; i++) 
			assertEquals(ics[i], decoded[i], 0.5);
	}


	@Test
	public void encodeDecode2ByteFloat() {
		
		int n = 1000;
		double[] ics = new double[n];
		for (int i=0; i<n; i++)
			ics[i] = Math.pow(10, 6*Math.random());
		
		byte[] encoded 		= new byte[n * 5];
		int encodedBytes 	= MSNumpress.encode2ByteFloat(ics, n, encoded);
		double[] decoded 	= new double[n];
		int decodedDoubles 	= MSNumpress.decode2ByteFloat(encoded, encodedBytes, decoded);
		
		assertEquals(n, decodedDoubles);
		
		for (int i=0; i<n; i++)
			assertEquals(0.0, (ics[i] - decoded[i]) / ((ics[i] + decoded[i])/2), 0.0005);
	}
}
