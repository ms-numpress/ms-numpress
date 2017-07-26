#cython: embedsignature=True
#distutils: language = c++
"""
A Python wrapper around the C numpress library

The following library abstracts the C numpress library and allows encoding and
decoding of raw data string.

Example:

    >>> data = [100, 101, 102, 103]
    >>> encoded = []; decoded = []
    >>> PyMSNumpress.encodeLinear(data, encoded, 500.0)
    >>> encoded
    [64, 127, 64, 0, 0, 0, 0, 0, 80, 195, 0, 0, 68, 197, 0, 0, 136]
    >>> PyMSNumpress.decodeLinear(encoded, decoded)
    >>> decoded
    [100.0, 101.0, 102.0, 103.0]

The interface expects Python lists of ordinal numbers, these can be converted
to byte strings with "ord" and "chr" if desired:

    >>> bstr = "".join([chr(e) for e in encoded])
    >>> blist = [ord(b) for b in bstr]

        PyMSNumpress.pyx
        roest@imsb.biol.ethz.ch
 
        Copyright 2013 Hannes Roest

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
"""


from libcpp.vector cimport vector as libcpp_vector
from cython.operator cimport dereference as deref, preincrement as inc, address as address
from MSNumpress cimport encodeLinear as _encodeLinear
from MSNumpress cimport decodeLinear as _decodeLinear
from MSNumpress cimport optimalLinearFixedPoint as _optimalLinearFixedPoint
from MSNumpress cimport optimalLinearFixedPointMass as _optimalLinearFixedPointMass
from MSNumpress cimport encodeSlof as _encodeSlof
from MSNumpress cimport decodeSlof as _decodeSlof
from MSNumpress cimport optimalSlofFixedPoint as _optimalSlofFixedPoint
from MSNumpress cimport encodePic as _encodePic
from MSNumpress cimport decodePic as _decodePic

def optimalLinearFixedPointMass(data, mz):
    """
        
        Compute the optimal linear fixed point with a desired m/z accuracy.
        
        @note If the desired accuracy cannot be reached without overflowing 64
        bit integers, then a negative value is returned. You need to check for
        this and in that case abandon numpress or use optimalLinearFixedPoint
        which returns the largest safe value.
        
        @data            pointer to array of double to be encoded (need memorycont. repr.)
        @mass_acc        desired m/z accuracy in Th
        @return          the linear fixed point that satisfies the accuracy requirement (or -1 in case of failure).
        
    """
    dataSize = len(data)
    cdef libcpp_vector[double] c_data = data

    cdef double result = _optimalLinearFixedPointMass( &c_data[0], dataSize, mz)

    return result

def optimalLinearFixedPoint(data):
    """

        Compute the maximal linear fixed point that prevents integer overflow.
        
        @data                pointer to array of double to be encoded (need memorycont. repr.)
        @return              the linear fixed point safe to use
        
    """
    dataSize = len(data)
    cdef libcpp_vector[double] c_data = data

    cdef double result = _optimalLinearFixedPoint( &c_data[0], dataSize)

    return result

def optimalSlofFixedPoint(data):
    dataSize = len(data)
    cdef libcpp_vector[double] c_data = data

    cdef double result = _optimalSlofFixedPoint( &c_data[0], dataSize)

    return result

def decodeLinear(data, result):
    """
        
        Decodes data encoded by encodeLinear. 
        
        result vector guaranteed to be shorter or equal to (|data| - 8) * 2
        
        Note that this method may throw a const char* if it deems the input data to be corrupt, i.e.
        that the last encoded int does not use the last byte in the data. In addition the last encoded 
        int need to use either the last halfbyte, or the second last followed by a 0x0 halfbyte. 
        
        @data                pointer to array of bytes to be decoded (need memorycont. repr.)
        @result              pointer to were resulting doubles should be stored
        @return              the number of decoded doubles, or -1 if dataSize < 4 or 4 < dataSize < 8
        
    """
    cdef libcpp_vector[unsigned char] c_data = data
    cdef libcpp_vector[double] c_result

    _decodeLinear(c_data, c_result)

    cdef libcpp_vector[double].iterator it_result = c_result.begin()
    while it_result != c_result.end():
        result.append( deref(it_result) )
        inc(it_result)

def encodeLinear(data, result, fixedPoint):
    """
        
        Encodes the doubles in data by first using a 
          - lossy conversion to a 4 byte 5 decimal fixed point representation
          - storing the residuals from a linear prediction after first two values
          - encoding by encodeInt (see above) 
        
        The resulting binary is maximally 8 + dataSize * 5 bytes, but much less if the 
        data is reasonably smooth on the first order.
        
        This encoding is suitable for typical m/z or retention time binary arrays. 
        On a test set, the encoding was empirically show to be accurate to at least 0.002 ppm.
        
        @data              pointer to array of double to be encoded (need memorycont. repr.)
        @result            pointer to where resulting bytes should be stored
        @fixedPoint        the scaling factor used for getting the fixed point repr. 
                           This is stored in the binary and automatically extracted
                           on decoding (see optimalLinearFixedPoint or optimalLinearFixedPointMass)
        @return            the number of encoded bytes
        
    """
    cdef double c_fixedPoint = fixedPoint
    cdef libcpp_vector[double] c_data = data
    cdef libcpp_vector[unsigned char] c_result

    _encodeLinear(c_data, c_result, c_fixedPoint)

    cdef libcpp_vector[unsigned char].iterator it_result = c_result.begin()
    while it_result != c_result.end():
        result.append( deref(it_result) )
        inc(it_result)

def decodeSlof(data, result):
    """
        
        Decodes data encoded by encodeSlof
        
        The return will include exactly (|data| - 8) / 2 doubles.
        
        Note that this method may throw a const char* if it deems the input data to be corrupt.
        
        @data                pointer to array of bytes to be decoded (need memorycont. repr.)
        @result                pointer to were resulting doubles should be stored
        @return                the number of decoded doubles
    """
    cdef libcpp_vector[unsigned char] c_data = data
    cdef libcpp_vector[double] c_result

    _decodeSlof(c_data, c_result)

    cdef libcpp_vector[double].iterator it_result = c_result.begin()
    while it_result != c_result.end():
        result.append( deref(it_result) )
        inc(it_result)

def encodeSlof(data, result, fixedPoint):
    """
        
        Encodes ion counts by taking the natural logarithm, and storing a
        fixed point representation of this. This is calculated as
        
        unsigned short fp = log(d + 1) * fixedPoint + 0.5
        
        the result vector is exactly |data| * 2 + 8 bytes long
        
        @data            pointer to array of double to be encoded (need memorycont. repr.)
        @result          pointer to were resulting bytes should be stored
        @fixedPoint      fixed point to use for encoding (see optimalSlofFixedPoint)
        @return          the number of encoded bytes
    """
    cdef double c_fixedPoint = fixedPoint
    cdef libcpp_vector[double] c_data = data
    cdef libcpp_vector[unsigned char] c_result

    _encodeSlof(c_data, c_result, c_fixedPoint)

    cdef libcpp_vector[unsigned char].iterator it_result = c_result.begin()
    while it_result != c_result.end():
        result.append( deref(it_result) )
        inc(it_result)

def decodePic(data, result):
    """
        
        Decodes data encoded by encodePic
        
        result vector guaranteed to be shorter of equal to |data| * 2
        
        Note that this method may throw a const char* if it deems the input data to be corrupt, i.e.
        that the last encoded int does not use the last byte in the data. In addition the last encoded 
        int need to use either the last halfbyte, or the second last followed by a 0x0 halfbyte. 
        
        @data                pointer to array of bytes to be decoded (need memorycont. repr.)
        @result              pointer to were resulting doubles should be stored
        @return              the number of decoded doubles
    """
    cdef libcpp_vector[unsigned char] c_data = data
    cdef libcpp_vector[double] c_result

    _decodePic(c_data, c_result)

    cdef libcpp_vector[double].iterator it_result = c_result.begin()
    while it_result != c_result.end():
        result.append( deref(it_result) )
        inc(it_result)

def encodePic(data, result):
    """
        
        Encodes ion counts by simply rounding to the nearest 4 byte integer, 
        and compressing each integer with encodeInt. 
        
        The handleable range is therefore 0 -> 4294967294.
        The resulting binary is maximally dataSize * 5 bytes, but much less if the 
        data is close to 0 on average.
        
        @data                pointer to array of double to be encoded (need memorycont. repr.)
        @result              pointer to were resulting bytes should be stored
        @return              the number of encoded bytes
        
    """
    cdef libcpp_vector[double] c_data = data
    cdef libcpp_vector[unsigned char] c_result

    _encodePic(c_data, c_result)

    cdef libcpp_vector[unsigned char].iterator it_result = c_result.begin()
    while it_result != c_result.end():
        result.append( deref(it_result) )
        inc(it_result)

