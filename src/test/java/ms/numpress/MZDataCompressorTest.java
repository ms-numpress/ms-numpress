package ms.numpress;

import static org.junit.Assert.*;

import org.junit.Test;

public class MZDataCompressorTest {

	@Test
	public void encodeInt() {
		byte[] res = new byte[10];
		int l;
		l = MZDataCompressor.encodeInt(0l, res, 0);
		assertEquals(1, l);
		assertEquals(8, res[0] & 0xf);
		
		l = MZDataCompressor.encodeInt(-1l, res, 0);
		assertEquals(2, l);
		assertEquals(0xf, res[0] & 0xf);
		assertEquals(0xf, res[1] & 0xf);
		
		l = MZDataCompressor.encodeInt(35l, res, 0);
		assertEquals(3, l);
		assertEquals(6, res[0] & 0xf);
		assertEquals(0x3, res[1] & 0xf);
		assertEquals(0x2, res[2] & 0xf);
		
		l = MZDataCompressor.encodeInt(370000000l, res, 0);
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
	public void encode() {
		double[] mzs = {100.0, 200.0, 300.00005, 400.00010};
		byte[] encoded 		= new byte[16];
		int encodedBytes 	= MZDataCompressor.encode(mzs, 4, encoded);
		assertEquals(10, encodedBytes);
		assertEquals(0x80, 0xff & encoded[0]);
		assertEquals(0x96, 0xff & encoded[1]);
		assertEquals(0x98, 0xff & encoded[2]);
		assertEquals(0x00, 0xff & encoded[3]);
		assertEquals(0x75, 0xff & encoded[8]);
		assertEquals(0x80, 0xf0 & encoded[9]);
	}
	
	
	@Test
	public void decodeNice() {
		double[] mzs = {100.0, 200.0, 300.00005, 400.00010};
		byte[] encoded 		= new byte[20];
		int encodedBytes 	= MZDataCompressor.encode(mzs, 4, encoded);
		double[] decoded 	= new double[4];
		MZDataCompressor.decode(encoded, encodedBytes, decoded, 4);
		assertEquals(100.0, decoded[0], 0.000005);
		assertEquals(200.0, decoded[1], 0.000005);
		assertEquals(300.00005, decoded[2], 0.000005);
		assertEquals(400.00010, decoded[3], 0.000005);
	}
	
	
	@Test
	public void decodeWierd() {
		double[] mzs = {100.0, 200.0, 4000.00005, 0.00010};
		byte[] encoded 		= new byte[20];
		int encodedBytes 	= MZDataCompressor.encode(mzs, 4, encoded);
		double[] decoded 	= new double[4];
		MZDataCompressor.decode(encoded, encodedBytes, decoded, 4);
		assertEquals(100.0, decoded[0], 0.000005);
		assertEquals(200.0, decoded[1], 0.000005);
		assertEquals(4000.00005, decoded[2], 0.000005);
		assertEquals(0.00010, decoded[3], 0.000005);
	}


	@Test
	public void encodeDecode() {
		
		int n = 1000;
		double[] mzs = new double[n];
		mzs[0] = 300 + Math.random();
		for (int i=1; i<n; i++) 
			mzs[i] = mzs[i-1] + Math.random();
		
		byte[] encoded 		= new byte[n * 5];
		int encodedBytes 	= MZDataCompressor.encode(mzs, n, encoded);
		double[] decoded 	= new double[n];
		MZDataCompressor.decode(encoded, encodedBytes, decoded, n);
		
		int count = 0;
		for (int i=0; i<n; i++) 
			assertEquals(mzs[i], decoded[i], 0.000005);
	}
}
