/*
	IntDecoder.java
	johan.teleman@immun.lth.se
	
	This distribution goes under the BSD 3-clause license. If you prefer to use Apache
	version 2.0, that is also available at https://github.com/fickludd/ms-numpress
 
	Copyright (c) 2013, Johan Teleman
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, 
	are permitted provided that the following conditions are met:

    * 	Redistributions of source code must retain the above copyright notice, this list 
    	of conditions and the following disclaimer.
    *	Redistributions in binary form must reproduce the above copyright notice, this 
    	list of conditions and the following disclaimer in the documentation and/or other 
    	materials provided with the distribution.
    *	Neither the name of the Lund University nor the names of its contributors may be 
    	used to endorse or promote products derived from this software without specific 
    	prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
	SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
	OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 package ms.numpress;

/**
 * Decodes ints from the half bytes in bytes. Lossless reverse of encodeInt 
 */
class IntDecoder {
	
	int pos = 0;
	boolean half = false;
	byte[] bytes;
	
	public IntDecoder(byte[] _bytes, int _pos) {
		bytes 	= _bytes;
		pos 	= _pos;
	}
	
	public long next() {
		int head;
		int i, n;
		long res = 0;
		long mask, m;
		int hb;
		
		if (!half)
			head = (0xff & bytes[pos]) >> 4;
		else
			head = 0xf & bytes[pos++];

		half = !half;
		
		if (head <= 8)
			n = head;
		else { 
			// leading ones, fill in res
			n = head - 8;
			mask = 0xf0000000;
			for (i=0; i<n; i++) {
				m = mask >> (4*i);
				res = res | m;
			}
		}

		if (n == 8) return 0;
			
		for (i=n; i<8; i++) {
			if (!half)
				hb = (0xff & bytes[pos]) >> 4;
			else
				hb = 0xf & bytes[pos++];

			res = res | (hb << ((i-n)*4));
			half = !half;
		}
		
		return res;
	}
}
